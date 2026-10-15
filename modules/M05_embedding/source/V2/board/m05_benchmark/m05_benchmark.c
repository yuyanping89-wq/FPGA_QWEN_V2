/* AXU9EGB / Vitis 2021.1 / standalone A53 64-bit.
 * Replace Hello World's main with this file; retain platform.c/platform.h.
 * Uses the full logical table layout, with reference data in selected rows.
 * No floating point computation: M05 must preserve each BF16 bit pattern.
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "platform.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xtime_l.h"
#include "sleep.h"
#include "xembedding_v2_hw.h"

#ifndef XPAR_M05_BASEADDR
#error "Use the validated m05_board_v22 or equivalent comparison hardware platform (XPAR_M05_BASEADDR missing)."
#endif

#define VOCAB 151936U
#define HIDDEN 896U
#define MAX_ROWS 64U
#define WORDS_PER_ROW 112U
#define GUARD_HALFWORDS 32U
#define SENTINEL 0xA55AU
#define DATAFLOW_BUILD 0x20010011U
#define SEQUENTIAL_BUILD 0x40010011U
#define WARMUPS 5U
#define REPEATS 100U
#define JOB_TIMEOUT_SECONDS 5U
#define REG(name) XEMBEDDING_V2_CTRL_ADDR_##name

/* Dedicated fixed DDR NOLOAD sections: linker reserves these buffers without storing 260 MiB in ELF.
 * Use the supplied benchmark lscript.ld; all buffers reside in DDR_LOW.
 * All three buffers are disjoint and cache-line aligned; no malloc required.
 */
static uint16_t table[VOCAB][HIDDEN] __attribute__((section(".m05_table"), aligned(64)));
static uint32_t token_ids[MAX_ROWS] __attribute__((section(".m05_ids"), aligned(64)));
static uint16_t output_storage[GUARD_HALFWORDS + MAX_ROWS * HIDDEN +
                               GUARD_HALFWORDS] __attribute__((section(".m05_output"), aligned(64)));
static const uint32_t selected_ids[] = {0U, 1U, 2U, 17U, 1024U,
                                      65535U, 65536U, VOCAB - 1U};
static uint32_t next_job = 1U;


typedef struct {
    uint32_t status, rows, crc, build;
    uint64_t total, active, reads, writes;
} result_t;

static void barrier(void) { __asm__ volatile("dsb sy" ::: "memory"); }
static uint32_t rd(uint32_t offset)
{
    return Xil_In32((UINTPTR)XPAR_M05_BASEADDR + offset);
}
static void wr(uint32_t offset, uint32_t value)
{
    Xil_Out32((UINTPTR)XPAR_M05_BASEADDR + offset, value);
    barrier();
}
static void wr64(uint32_t offset, uint64_t value)
{
    wr(offset, (uint32_t)value);
    wr(offset + 4U, (uint32_t)(value >> 32));
}
/* Read only after done, before another start: snapshot is stable. */
static uint64_t rd64(uint32_t offset)
{
    uint64_t low = rd(offset);
    return low | ((uint64_t)rd(offset + 4U) << 32);
}
static void print64(const char *label, uint64_t value)
{
    /* xil_printf does not reliably support %llu; print both 32-bit halves. */
    xil_printf(" %s=0x%08x%08x", label,
               (unsigned)(value >> 32), (unsigned)value);
}
static void print_addr(const char *label, const void *ptr)
{
    print64(label, (uint64_t)(UINTPTR)ptr);
}

static uint16_t reference(uint32_t id, uint32_t column)
{
    /* Unsigned wrap is intentional. Mix both halves of the token ID. */
    uint32_t x = id * 0x9E3779B9U + column * 0x85EBCA6BU + 0x1234567U;
    x ^= x >> 16;
    return (uint16_t)x;
}

static int valid_ddr_buffer(const void *ptr, size_t size)
{
    uint64_t start = (uint64_t)(UINTPTR)ptr;
    return (start & 63U) == 0U && size != 0U &&
           start < 0x7FF00000ULL && size <= 0x7FF00000ULL - start;
}

/* Board-test startup only; NOT part of M05's production interface.
 * Addresses/masks checked against this XSA's psu_init.c and FSBL xfsbl_hw.h.
 * psu_init() initializes PS/DDR, while psu_ps_pl_reset_config_data() is a
 * separate generated sequence. JTAG launch must also make PL ready.
 * Only the EMIO bank5 bit31 wired to pl_resetn0 is changed; preserve other GPIOs.
 * No full psu_init here: reinitializing DDR with an application running is unsafe.
 */
static void ps_mask_write(UINTPTR address, uint32_t mask, uint32_t value)
{
    uint32_t current = Xil_In32(address);
    Xil_Out32(address, (current & ~mask) | (value & mask));
    barrier();
}

static int prepare_pl_startup(void)
{
    const UINTPTR pl0_clock = 0xFF5E00C0U;
    const UINTPTR iou_reset = 0xFF5E0238U;
    const UINTPTR gpio_dir = 0xFF0A0344U;
    const UINTPTR gpio_enable = 0xFF0A0348U;
    const UINTPTR gpio_data = 0xFF0A0054U;
    const uint32_t reset0_bit = 0x80000000U;
    uint32_t clock, iou, pcap, direction, enable, data;
    clock = Xil_In32(pl0_clock);
    iou = Xil_In32(iou_reset);
    pcap = Xil_In32((UINTPTR)0xFFCA3010U);
    direction = Xil_In32(gpio_dir);
    enable = Xil_In32(gpio_enable);
    data = Xil_In32(gpio_data);
    xil_printf("[PS] PL0_REF_CTRL=0x%08x RST_LPD_IOU2=0x%08x PCAP_STATUS=0x%08x\r\n",
               (unsigned)clock, (unsigned)iou, (unsigned)pcap);
    xil_printf("[PS] GPIO_DIRM5=0x%08x OEN5=0x%08x DATA5=0x%08x\r\n",
               (unsigned)direction, (unsigned)enable, (unsigned)data);
    xil_printf("[PS] RST_FPD_TOP=0x%08x REQ_ISO_STATUS=0x%08x\r\n",
               (unsigned)Xil_In32((UINTPTR)0xFD1A0100U),
               (unsigned)Xil_In32((UINTPTR)0xFFD80310U));
    if ((clock & 0x01000000U) == 0U) {
        xil_printf("[FAIL] PS PL0 clock disabled. Fix platform initialization before PL access.\r\n");
        return 0;
    }
    if ((pcap & 8U) == 0U) {
        xil_printf("[FAIL] PL configuration DONE is low. Reload board bitstream before PL access.\r\n");
        return 0;
    }
    if (iou & 0x00040000U) {
        xil_printf("[FAIL] GPIO is held in reset. Fix platform initialization before PL access.\r\n");
        return 0;
    }
    /* The board test has issued no PL transactions yet. Pulse the generated
     * active-low reset once, then allow MMCM lock/reset synchronization time.
     * This resets M05, both SmartConnects and Clocking Wizard in this design.
     */
    ps_mask_write(gpio_data, reset0_bit, 0U);
    ps_mask_write(gpio_dir, reset0_bit, reset0_bit);
    ps_mask_write(gpio_enable, reset0_bit, reset0_bit);
    usleep(100U);
    ps_mask_write(gpio_data, reset0_bit, reset0_bit);
    usleep(10000U);
    direction = Xil_In32(gpio_dir);
    enable = Xil_In32(gpio_enable);
    data = Xil_In32(gpio_data);
    xil_printf("[PS] after reset: DIRM5=0x%08x OEN5=0x%08x DATA5=0x%08x\r\n",
               (unsigned)direction, (unsigned)enable, (unsigned)data);
    if ((direction & enable & data & reset0_bit) == 0U) {
        xil_printf("[FAIL] pl_resetn0 GPIO setup/readback failed\r\n");
        return 0;
    }
    return 1;
}

static void prepare_table(void)
{
    unsigned i;
    for (i = 0U; i < sizeof(selected_ids) / sizeof(selected_ids[0]); ++i) {
        uint32_t id = selected_ids[i], c;
        for (c = 0U; c < HIDDEN; ++c) table[id][c] = reference(id, c);
        Xil_DCacheFlushRange((INTPTR)&table[id][0], HIDDEN * sizeof(uint16_t));
    }
    barrier();
}

static void reset_output(void)
{
    unsigned i;
    for (i = 0; i < sizeof(output_storage) / sizeof(output_storage[0]); ++i)
        output_storage[i] = SENTINEL;
    /* Clean dirty sentinels BEFORE the device writes, so later invalidate
     * cannot write stale CPU cache lines over the device's output. */
    Xil_DCacheFlushRange((INTPTR)output_storage, sizeof(output_storage));
    barrier();
}

static int invoke(uint32_t mode, uint32_t rows, uint32_t abi,
                  uint32_t flags, result_t *result)
{
    XTime begin, now;
    uint32_t control, polls = 0U;
    control = rd(REG(AP_CTRL)); /* Also clears stale done. */
    if ((control & 4U) == 0U || (control & 0x81U) != 0U) {
        xil_printf("[FAIL] M05 not idle / auto_restart enabled: ctrl=0x%08x\r\n",
                   (unsigned)control);
        return 0;
    }
    Xil_DCacheFlushRange((INTPTR)token_ids, sizeof(token_ids));
    barrier();
    wr(REG(GIE), 0U);
    wr(REG(IER), 0U);
    wr64(REG(TOKEN_IDS_DATA), (uint64_t)(UINTPTR)token_ids);
    wr64(REG(TABLE_R_DATA), (uint64_t)(UINTPTR)table);
    wr64(REG(HIDDEN_OUT_DATA),
         (uint64_t)(UINTPTR)&output_storage[GUARD_HALFWORDS]);
    wr(REG(RUN_MODE_DATA), mode);
    wr(REG(VALID_ROWS_DATA), rows);
    wr(REG(HIDDEN_SIZE_DATA), HIDDEN);
    wr(REG(VOCAB_SIZE_DATA), VOCAB);
    wr(REG(ABI_VERSION_DATA), abi);
    wr(REG(JOB_ID_DATA), next_job++);
    wr(REG(FLAGS_DATA), flags);
    barrier();
    XTime_GetTime(&begin);
    wr(REG(AP_CTRL), 1U); /* Single job, auto_restart=0. */
    for (;;) {
        control = rd(REG(AP_CTRL));
        if (control & 2U) break;
        XTime_GetTime(&now);
        ++polls;
        if (now - begin > (uint64_t)COUNTS_PER_SECOND * JOB_TIMEOUT_SECONDS) {
            xil_printf("[FAIL] M05 completion timeout, ctrl=0x%08x. Stop; reset/relaunch before retry.\r\n",
                       (unsigned)control);
            return 0;
        }
        if (polls == 1000000U) {
            xil_printf("[FAIL] polling limit reached: ctrl=0x%08x. Stop/reset before retry.\r\n",
                       (unsigned)control);
            return 0;
        }
    }
    barrier();
    result->status = rd(REG(RESULT_STATUS_CODE_DATA));
    result->rows = rd(REG(RESULT_PROCESSED_ROWS_DATA));
    result->total = rd64(REG(RESULT_PERF_CYCLES_TOTAL_DATA));
    result->active = rd64(REG(RESULT_PERF_CYCLES_COMPUTE_DATA));
    result->reads = rd64(REG(RESULT_PERF_READ_BEATS_DATA));
    result->writes = rd64(REG(RESULT_PERF_WRITE_BEATS_DATA));
    result->crc = rd(REG(RESULT_OUTPUT_CRC_DATA));
    result->build = rd(REG(RESULT_BUILD_CONFIG_DATA));
    Xil_DCacheInvalidateRange((INTPTR)output_storage, sizeof(output_storage));
    barrier();
    return 1;
}

static int check_output(uint32_t copied_rows)
{
    unsigned i;
    for (i = 0; i < sizeof(output_storage) / sizeof(output_storage[0]); ++i) {
        uint16_t expected = SENTINEL;
        if (i >= GUARD_HALFWORDS && i < GUARD_HALFWORDS + copied_rows * HIDDEN) {
            uint32_t n = i - GUARD_HALFWORDS;
            expected = reference(token_ids[n / HIDDEN], n % HIDDEN);
        }
        if (output_storage[i] != expected) {
            xil_printf("[FAIL] output halfword %u: actual=0x%04x expected=0x%04x\r\n",
                       i, (unsigned)output_storage[i], (unsigned)expected);
            return 0;
        }
    }
    return 1;
}


/* Decimal formatting independent of xil_printf's 64-bit format support. */
static void decimal(uint64_t value)
{
    char text[21];
    unsigned n = 20U;
    text[n] = 0;
    do { text[--n] = (char)('0' + value % 10U); value /= 10U; } while (value);
    xil_printf("%s", &text[n]);
}

static result_t samples[REPEATS];
static uint32_t observed_build;

static int checked_job(uint32_t rows, result_t *r)
{
    uint32_t saved[MAX_ROWS];
    memcpy(saved, token_ids, sizeof(saved));
    reset_output();
    if (!invoke(0U, rows, 2U, 0U, r)) return 0;
    if (r->build != DATAFLOW_BUILD && r->build != SEQUENTIAL_BUILD) {
        xil_printf("[FAIL] unrecognized counter build 0x%08x\r\n", (unsigned)r->build);
        return 0;
    }
    if (observed_build && observed_build != r->build) return 0;
    observed_build = r->build;
    if (r->status || r->rows != rows || r->crc || !r->total || !r->active ||
        r->active > r->total || r->reads != (uint64_t)rows * 113U ||
        r->writes != (uint64_t)rows * 112U) {
        xil_printf("[FAIL] result/counter mismatch, T=%u\r\n", (unsigned)rows);
        return 0;
    }
    if (!check_output(rows)) return 0;
    Xil_DCacheInvalidateRange((INTPTR)token_ids, sizeof(token_ids));
    barrier();
    if (memcmp(saved, token_ids, sizeof(saved))) {
        xil_printf("[FAIL] token_ids changed\r\n"); return 0;
    }
    return 1;
}

int main(void)
{
    static const uint32_t counts[] = {1U, 2U, 4U, 8U, 16U, 32U, 64U};
    unsigned group, i, c;
    result_t warmup;
    init_platform();
    XTime_StartTimer();
    xil_printf("\r\nM05 BENCHMARK V1: prefill, warmup=5, measured=100 per T\r\n");
    print_addr("table", table); print_addr("ids", token_ids);
    print_addr("out", &output_storage[GUARD_HALFWORDS]); xil_printf("\r\n");
    if ((UINTPTR)table != 0x10000000ULL || (UINTPTR)token_ids != 0x20400000ULL ||
        (UINTPTR)output_storage != 0x20400100ULL ||
        !valid_ddr_buffer(table, sizeof(table)) ||
        !valid_ddr_buffer(token_ids, sizeof(token_ids)) ||
        !valid_ddr_buffer(output_storage, sizeof(output_storage))) {
        xil_printf("[FAIL] use the supplied fixed-buffer linker script\r\n"); goto failed;
    }
    if (!prepare_pl_startup()) goto failed;
    prepare_table();
    for (i = 0U; i < MAX_ROWS; ++i) token_ids[i] = selected_ids[(i * 3U) % 8U];
    xil_printf("RAW,build,T,iteration,total_cycles,active_cycles,read_beats,write_beats\r\n");
    xil_printf("SUMMARY,build,T,repeats,min_cycles,sum_cycles,max_cycles\r\n");
    for (group = 0U; group < 7U; ++group) {
        uint32_t rows = counts[group];
        uint64_t min = ~(uint64_t)0, max = 0, sum = 0;
        xil_printf("[RUN] T=%u\r\n", (unsigned)rows);
        for (i = 0U; i < WARMUPS; ++i)
            if (!checked_job(rows, &warmup)) goto failed;
        /* Buffer measurements; UART printing occurs after all jobs in a group. */
        for (i = 0U; i < REPEATS; ++i) {
            uint64_t t;
            if (!checked_job(rows, &samples[i])) goto failed;
            t = samples[i].total;
            if (t < min) min = t;
            if (t > max) max = t;
            sum += t;
        }
        for (i = 0U; i < REPEATS; ++i) {
            const result_t *r = &samples[i];
            xil_printf("RAW,0x%08x,%u,%u,", (unsigned)r->build, (unsigned)rows, i + 1U);
            decimal(r->total); xil_printf(","); decimal(r->active);
            xil_printf(","); decimal(r->reads); xil_printf(","); decimal(r->writes);
            xil_printf("\r\n");
        }
        xil_printf("SUMMARY,0x%08x,%u,%u,", (unsigned)observed_build, (unsigned)rows, REPEATS);
        decimal(min); xil_printf(","); decimal(sum); xil_printf(","); decimal(max);
        xil_printf("\r\n[PASS] T=%u: 5 warmups and 100 measured jobs verified\r\n", (unsigned)rows);
    }
    for (i = 0U; i < 8U; ++i) {
        uint32_t id = selected_ids[i];
        Xil_DCacheInvalidateRange((INTPTR)&table[id][0], HIDDEN * sizeof(uint16_t));
        barrier();
        for (c = 0U; c < HIDDEN; ++c)
            if (table[id][c] != reference(id, c)) {
                xil_printf("[FAIL] table changed\r\n"); goto failed;
            }
    }
    xil_printf("ALL 7 BENCHMARK GROUPS PASSED; 700 measured jobs; table unchanged.\r\n");
    cleanup_platform(); return 0;
failed:
    xil_printf("M05 BENCHMARK FAILED; discard partial benchmark and save log.\r\n");
    cleanup_platform(); return 1;
}
