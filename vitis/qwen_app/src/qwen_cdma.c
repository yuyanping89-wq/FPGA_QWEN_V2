#include "qwen_internal.h"

#include <string.h>

#define CDMA_SR       0x04U
#define CDMA_SRC      0x18U
#define CDMA_DST      0x20U
#define CDMA_BTT      0x28U
#define CDMA_SR_IDLE  (1U << 1)
#define CDMA_SR_ERR   0x00000770U
#define CDMA_SR_IRQ_IOC 0x00001000U

static uint8_t g_asset_bounce[QWEN_CDMA_CHUNK_BYTES]
    __attribute__((aligned(QWEN_CACHE_LINE_BYTES)));

static uint32_t qwen_round_up_16(uint32_t value)
{
    return (value + 15U) & ~15U;
}

QwenStatus qwen_check_pl_range(uint32_t address, uint32_t bytes)
{
    uint64_t end;
    if (bytes == 0U) return QWEN_E_ARGUMENT;
    end = (uint64_t)address + (uint64_t)bytes;
    if (address < QWEN_PL_DDR_BASE || end > QWEN_PL_DDR_END_EXCLUSIVE || end <= address) {
        return QWEN_E_RANGE;
    }
    return QWEN_OK;
}

QwenStatus qwen_platform_cdma_copy(uint32_t source, uint32_t destination,
                                   uint32_t bytes, uint32_t timeout_ms)
{
    QwenProfileEvent event;
    QwenStatus result;
    uint32_t status;
    if (bytes == 0U || bytes > QWEN_CDMA_MAX_BTT) return QWEN_E_ARGUMENT;
    if ((source & 0xFU) != 0U || (destination & 0xFU) != 0U) return QWEN_E_ALIGNMENT;
    if ((source<QWEN_PL_DDR_BASE && (uint64_t)source+bytes>QWEN_PS_DDR_BYTES) ||
        (destination<QWEN_PL_DDR_BASE && (uint64_t)destination+bytes>QWEN_PS_DDR_BYTES)) return QWEN_E_RANGE;
    if(qwen_debug_faulted()) return QWEN_E_NEEDS_RESET;
    if (source >= QWEN_PL_DDR_BASE && qwen_check_pl_range(source, bytes) != QWEN_OK) {
        return QWEN_E_RANGE;
    }
    if (destination >= QWEN_PL_DDR_BASE &&
        qwen_check_pl_range(destination, bytes) != QWEN_OK) {
        return QWEN_E_RANGE;
    }

    event=qwen_profile_event_begin(QWEN_EVENT_CDMA,source>=QWEN_PL_DDR_BASE?1U:0U);
    event.source=source;event.destination=destination;event.bytes=bytes;
    do {
        status = qwen_platform_read32(QWEN_CDMA_BASE + CDMA_SR);
        if ((status & CDMA_SR_ERR) != 0U) {
            qwen_platform_write32(QWEN_CDMA_BASE + CDMA_SR, status & CDMA_SR_ERR);
            result=QWEN_E_CDMA;goto done;
        }
        if ((status & CDMA_SR_IDLE) != 0U) break;
        if (qwen_platform_time_us()-event.start_us >= (uint64_t)timeout_ms*1000U) {result=QWEN_E_TIMEOUT;goto done;}
        qwen_platform_service();
    } while (1);

    /* Match the active board test: clear latched completion/error bits first. */
    qwen_platform_write32(QWEN_CDMA_BASE + CDMA_SR, CDMA_SR_IRQ_IOC | CDMA_SR_ERR);
    qwen_platform_write32(QWEN_CDMA_BASE + CDMA_SRC, source);
    qwen_platform_write32(QWEN_CDMA_BASE + CDMA_DST, destination);
    qwen_platform_write32(QWEN_CDMA_BASE + CDMA_BTT, bytes);

    do {
        status = qwen_platform_read32(QWEN_CDMA_BASE + CDMA_SR);
        if ((status & CDMA_SR_ERR) != 0U) {
            qwen_platform_write32(QWEN_CDMA_BASE + CDMA_SR, status & CDMA_SR_ERR);
            result=QWEN_E_CDMA;goto done;
        }
        if ((status & CDMA_SR_IDLE) != 0U) {result=QWEN_OK;goto done;}
        if (qwen_platform_time_us()-event.start_us >= (uint64_t)timeout_ms*1000U) {result=QWEN_E_TIMEOUT;goto done;}
        qwen_platform_service();
    } while (1);
done:
    event.duration_us=qwen_platform_time_us()-event.start_us;event.hw_status=status;
    qwen_profile_cdma_snapshot(status,source,destination,bytes);
    if(result!=QWEN_OK) qwen_debug_latch_fault();
    qwen_profile_record(&event,result);
    return result;
}

static void flush_measured(void *address,uint32_t bytes,int invalidate)
{
    QwenProfileEvent e=qwen_profile_event_begin(QWEN_EVENT_CACHE,invalidate?1U:0U);
    e.bytes=bytes;
    if(invalidate) qwen_platform_cache_invalidate(address,bytes);
    else qwen_platform_cache_flush(address,bytes);
    e.duration_us=qwen_platform_time_us()-e.start_us;
    qwen_profile_record(&e,QWEN_OK);
}

QwenStatus qwen_upload_bytes(uint32_t destination, const void *source, uint32_t bytes)
{
    const uint8_t *input = (const uint8_t *)source;
    uint32_t done = 0U;
    QwenStatus status;
    if (input == 0 || bytes == 0U) return QWEN_E_ARGUMENT;
    status = qwen_check_pl_range(destination, qwen_round_up_16(bytes));
    if (status != QWEN_OK) return status;
    while (done < bytes) {
        uint32_t part = bytes - done;
        uint32_t transfer;
        uintptr_t source_address;
        if (part > QWEN_CDMA_CHUNK_BYTES) part = QWEN_CDMA_CHUNK_BYTES;
        transfer = qwen_round_up_16(part);
        QwenProfileEvent e=qwen_profile_event_begin(QWEN_EVENT_CPU_COPY,0U);
        e.bytes=part;
        memcpy(g_asset_bounce, input + done, part);
        if (transfer > part) memset(g_asset_bounce + part, 0, transfer - part);
        e.duration_us=qwen_platform_time_us()-e.start_us;qwen_profile_record(&e,QWEN_OK);
        flush_measured(g_asset_bounce, transfer,0);
        source_address = (uintptr_t)g_asset_bounce;
        if (source_address+transfer > QWEN_PS_DDR_BYTES) return QWEN_E_RANGE;
        status = qwen_platform_cdma_copy((uint32_t)source_address,
                                         destination + done, transfer,
                                         QWEN_CDMA_TIMEOUT_MS);
        if (status != QWEN_OK) return status;
        done += part;
        qwen_platform_service();
    }
    return QWEN_OK;
}

QwenStatus qwen_stage_asset(const QwenAssetProvider *assets, QwenAssetId asset,
                            uint32_t layer, uint32_t destination, uint32_t expected_bytes)
{
    uint64_t source_size;
    uint32_t done = 0U;
    QwenStatus status;
    if (assets == 0 || assets->size == 0 || assets->read == 0 || expected_bytes == 0U) {
        return QWEN_E_ARGUMENT;
    }
    status = qwen_check_pl_range(destination, expected_bytes);
    if (status != QWEN_OK) return status;
    if ((destination & (QWEN_CACHE_LINE_BYTES - 1U)) != 0U ||
        (expected_bytes & (QWEN_CACHE_LINE_BYTES - 1U)) != 0U) {
        return QWEN_E_ALIGNMENT;
    }
    source_size = assets->size(assets->context, asset, layer);
    if (source_size == 0U) return QWEN_E_ASSET_MISSING;
    if (source_size != expected_bytes) return QWEN_E_ASSET_IO;

    while (done < expected_bytes) {
        uint32_t part = expected_bytes - done;
        uintptr_t source_address;
        if (part > QWEN_CDMA_CHUNK_BYTES) part = QWEN_CDMA_CHUNK_BYTES;
        QwenProfileEvent e=qwen_profile_event_begin(QWEN_EVENT_ASSET_READ,(uint32_t)asset);
        e.bytes=part;e.source=done;e.destination=destination+done;
        status = assets->read(assets->context, asset, layer, done, g_asset_bounce, part);
        e.duration_us=qwen_platform_time_us()-e.start_us;qwen_profile_record(&e,status);
        if (status != QWEN_OK) return status;
        flush_measured(g_asset_bounce, part,0);
        source_address = (uintptr_t)g_asset_bounce;
        if (source_address+part > QWEN_PS_DDR_BYTES) return QWEN_E_RANGE;
        status = qwen_platform_cdma_copy((uint32_t)source_address,
                                         destination + done, part,
                                         QWEN_CDMA_TIMEOUT_MS);
        if (status != QWEN_OK) return status;
        done += part;
        qwen_platform_service();
    }
    return QWEN_OK;
}

QwenStatus qwen_probe_pl_ddr(void)
{
    static uint8_t tx[64] __attribute__((aligned(QWEN_CACHE_LINE_BYTES)));
    static uint8_t rx[64] __attribute__((aligned(QWEN_CACHE_LINE_BYTES)));
    uintptr_t tx_address = (uintptr_t)tx;
    uintptr_t rx_address = (uintptr_t)rx;
    uint32_t i;
    QwenStatus status;
    for (i = 0U; i < sizeof(tx); ++i) tx[i] = (uint8_t)(0xA5U ^ (i * 37U));
    memset(rx, 0, sizeof(rx));
    if (tx_address+sizeof(tx) > QWEN_PS_DDR_BYTES || rx_address+sizeof(rx)>QWEN_PS_DDR_BYTES) return QWEN_E_RANGE;
    flush_measured(tx, sizeof(tx),0);
    /* Clean receive lines BEFORE DMA to avoid a later dirty eviction. */
    flush_measured(rx, sizeof(rx),0);
    status = qwen_platform_cdma_copy((uint32_t)tx_address, QWEN_PL_PROBE_ADDR,
                                     sizeof(tx), QWEN_CDMA_TIMEOUT_MS);
    if (status != QWEN_OK) return QWEN_E_MIG_PROBE;
    status = qwen_platform_cdma_copy(QWEN_PL_PROBE_ADDR, (uint32_t)rx_address,
                                     sizeof(rx), QWEN_CDMA_TIMEOUT_MS);
    if (status != QWEN_OK) return QWEN_E_MIG_PROBE;
    flush_measured(rx, sizeof(rx),1);
    return memcmp(tx, rx, sizeof(tx)) == 0 ? QWEN_OK : QWEN_E_MIG_PROBE;
}

QwenStatus qwen_debug_dma_benchmark(uint32_t bytes,uint32_t iterations)
{
    QwenStatus st=QWEN_OK;uint32_t i,j;
    uintptr_t address=(uintptr_t)g_asset_bounce;
    QwenDebugOptions opt={0U,64U,0U};
    qwen_profile_begin(&opt,0U);
    qwen_profile_context(QWEN_PHASE_DEBUG,UINT32_MAX,0U,0U,0U,0U,0U);
    if(!bytes || bytes>sizeof(g_asset_bounce) || (bytes&63U) || !iterations || iterations>1000U)
        st=QWEN_E_ARGUMENT;
    else if(address+bytes>QWEN_PS_DDR_BYTES) st=QWEN_E_RANGE;
    else if(qwen_debug_faulted()) st=QWEN_E_NEEDS_RESET;
    for(i=0;st==QWEN_OK && i<iterations;++i) {
        for(j=0;j<bytes;++j) g_asset_bounce[j]=(uint8_t)(j*37U+i);
        flush_measured(g_asset_bounce,bytes,0);
        st=qwen_platform_cdma_copy((uint32_t)address,QWEN_PL_DEBUG_DMA_ADDR,bytes,QWEN_CDMA_TIMEOUT_MS);
        if(st!=QWEN_OK) break;
        memset(g_asset_bounce,0,bytes);flush_measured(g_asset_bounce,bytes,0);
        st=qwen_platform_cdma_copy(QWEN_PL_DEBUG_DMA_ADDR,(uint32_t)address,bytes,QWEN_CDMA_TIMEOUT_MS);
        if(st!=QWEN_OK) break;
        flush_measured(g_asset_bounce,bytes,1);
        for(j=0;j<bytes;++j) if(g_asset_bounce[j]!=(uint8_t)(j*37U+i)) {st=QWEN_E_CDMA;break;}
        qwen_platform_service();
    }
    qwen_profile_finish(st,0U);return st;
}

const char *qwen_status_string(QwenStatus status)
{
    switch (status) {
    case QWEN_OK: return "OK";
    case QWEN_E_ARGUMENT: return "bad argument";
    case QWEN_E_RANGE: return "address or size out of range";
    case QWEN_E_ALIGNMENT: return "alignment violation";
    case QWEN_E_CDMA: return "CDMA error";
    case QWEN_E_TIMEOUT: return "timeout";
    case QWEN_E_IP_STATUS: return "accelerator status error";
    case QWEN_E_BAD_BUILD: return "accelerator build identity mismatch";
    case QWEN_E_ASSET_MISSING: return "model asset missing";
    case QWEN_E_ASSET_IO: return "model asset size or read error";
    case QWEN_E_M08_NOT_INSTALLED: return "M08 RoPE IP/driver not installed";
    case QWEN_E_M10_NOT_INSTALLED: return "M10 attention IP/driver not installed";
    case QWEN_E_MIG_PROBE: return "PL DDR/CDMA probe failed";
    case QWEN_E_NEEDS_RESET: return "hardware fault latched; reset PL/PS before retry";
    case QWEN_E_PROTOCOL: return "invalid network request";
    default: return "internal error";
    }
}
