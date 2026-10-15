#include "qwen_internal.h"

#define REG32(base, offset) qwen_platform_read32((base) + (offset))
#define WR32(base, offset, value) qwen_platform_write32((base) + (offset), (value))

static void qwen_write_ptr(uint32_t base, uint32_t low_offset, uint32_t address)
{
    WR32(base, low_offset, address);
    WR32(base, QWEN_PTR_HI(low_offset), 0U);
}

uint32_t qwen_float_bits(float value)
{
    union { float f; uint32_t u; } bits;
    bits.f = value;
    return bits.u;
}

/* Read-only inputs may alias; outputs must not overlap any other buffer.
 * Check the complete tensor extents before writing an IP register. */
QwenStatus qwen_check_axi_buffers(const uint32_t *addresses, const uint32_t *bytes,
                                  uint32_t count, uint32_t first_output)
{
    uint32_t i, j;
    for (i = 0U; i < count; ++i) {
        QwenStatus status;
        if ((addresses[i] & (QWEN_CACHE_LINE_BYTES - 1U)) != 0U)
            return QWEN_E_ALIGNMENT;
        status = qwen_check_pl_range(addresses[i], bytes[i]);
        if (status != QWEN_OK) return status;
    }
    for (i = first_output; i < count; ++i) {
        for (j = 0U; j < i; ++j) {
            if ((uint64_t)addresses[i] < (uint64_t)addresses[j] + bytes[j] &&
                (uint64_t)addresses[j] < (uint64_t)addresses[i] + bytes[i])
                return QWEN_E_ARGUMENT;
        }
    }
    return QWEN_OK;
}

QwenStatus qwen_wait_hls(uint32_t base, uint32_t expected_rows,
                         uint32_t status_offset, uint32_t processed_rows_offset,
                         uint32_t build_config_offset, uint32_t expected_build,
                         uint32_t timeout_ms, uint32_t *device_status)
{
    QwenProfileEvent event;
    uint32_t ctrl;
    uint32_t status = UINT32_MAX;
    uint32_t rows;
    uint32_t build = 0U;
    QwenStatus result;

    if (qwen_debug_faulted()) return QWEN_E_NEEDS_RESET;
    event = qwen_profile_ip_begin(base, expected_rows);

    WR32(base, QWEN_HLS_AP_CTRL, QWEN_HLS_AP_START);
    do {
        ctrl = REG32(base, QWEN_HLS_AP_CTRL);
        if ((ctrl & QWEN_HLS_AP_DONE) != 0U) break;
        if (qwen_platform_time_us() - event.start_us >= (uint64_t)timeout_ms * 1000U) {
            event.duration_us = qwen_platform_time_us() - event.start_us;
            qwen_profile_ip_end(&event, base, QWEN_E_TIMEOUT, ctrl, status, build);
            return QWEN_E_TIMEOUT;
        }
        qwen_platform_service();
    } while (1);

    event.duration_us = qwen_platform_time_us() - event.start_us;
    status = REG32(base, status_offset);
    rows = REG32(base, processed_rows_offset);
    build = REG32(base, build_config_offset);
    if (device_status != 0) *device_status = status;
    result = build != expected_build ? QWEN_E_BAD_BUILD :
             (status != 0U || rows != expected_rows) ? QWEN_E_IP_STATUS : QWEN_OK;
    if (result == QWEN_OK && base == QWEN_M14_BASE && REG32(base, 0x0B0U) != REG32(base, 0x080U))
        result = QWEN_E_IP_STATUS;
    if (result == QWEN_OK && base == QWEN_M14_BASE && REG32(base, 0x090U) >= QWEN_VOCAB_SIZE)
        result = QWEN_E_IP_STATUS;
    if (result == QWEN_OK && base == QWEN_M09_BASE && REG32(base, 0x080U) != REG32(base, 0x050U) + expected_rows)
        result = QWEN_E_IP_STATUS;
    qwen_profile_ip_end(&event, base, result, ctrl, status, build);
    return result;
}

QwenStatus qwen_ip_m05(uint32_t token_ids, uint32_t hidden_out, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id)
{
    uint32_t base = QWEN_M05_BASE;
    uint32_t addresses[3] = {token_ids, QWEN_PL_SHARED_WEIGHT_ADDR, hidden_out};
    uint32_t bytes[3] = {rows * 4U, (uint32_t)QWEN_SHARED_VOCAB_WEIGHT_BYTES, rows * QWEN_ROW_BYTES};
    QwenStatus check;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    check = qwen_check_axi_buffers(addresses, bytes, 3U, 2U);
    if (check != QWEN_OK) return check;
    qwen_write_ptr(base, 0x010U, token_ids);
    qwen_write_ptr(base, 0x01CU, QWEN_PL_SHARED_WEIGHT_ADDR);
    qwen_write_ptr(base, 0x028U, hidden_out);
    WR32(base, 0x034U, run_mode);
    WR32(base, 0x03CU, rows);
    WR32(base, 0x044U, QWEN_HIDDEN_SIZE);
    WR32(base, 0x04CU, QWEN_VOCAB_SIZE);
    WR32(base, 0x054U, QWEN_ABI_VERSION);
    WR32(base, 0x05CU, job_id);
    WR32(base, 0x064U, qwen_debug_ip_flags());
    return qwen_wait_hls(base, rows, 0x06CU, 0x07CU, 0x0FCU,
                         QWEN_M05_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
}

QwenStatus qwen_ip_m06(uint32_t x, uint32_t gamma, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t role, uint32_t job_id)
{
    uint32_t base = QWEN_M06_BASE;
    uint32_t addresses[3] = {x, gamma, y};
    uint32_t bytes[3] = {rows * QWEN_ROW_BYTES, QWEN_M06_GAMMA_BYTES, rows * QWEN_ROW_BYTES};
    QwenStatus check;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS || role > 2U ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    check = qwen_check_axi_buffers(addresses, bytes, 3U, 2U);
    if (check != QWEN_OK) return check;
    qwen_write_ptr(base, 0x010U, x);
    qwen_write_ptr(base, 0x01CU, gamma);
    qwen_write_ptr(base, 0x028U, y);
    WR32(base, 0x034U, run_mode);
    WR32(base, 0x03CU, rows);
    WR32(base, 0x044U, QWEN_HIDDEN_SIZE);
    WR32(base, 0x04CU, qwen_float_bits(QWEN_M06_EPSILON));
    WR32(base, 0x054U, role);
    WR32(base, 0x05CU, QWEN_ABI_VERSION);
    WR32(base, 0x064U, job_id);
    WR32(base, 0x06CU, qwen_debug_ip_flags());
    return qwen_wait_hls(base, rows, 0x074U, 0x084U, 0x104U,
                         QWEN_M06_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
}

QwenStatus qwen_ip_m07(uint32_t x, uint32_t wqkv, uint32_t bqkv,
                       uint32_t q, uint32_t k, uint32_t v, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id)
{
    uint32_t base = QWEN_M07_BASE;
    uint32_t addresses[6] = {x, wqkv, bqkv, q, k, v};
    uint32_t bytes[6];
    QwenStatus status;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS ||
        (qwen_debug_ip_flags() & ~0xBU) != 0U ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    bytes[0] = bytes[3] = rows * QWEN_ROW_BYTES;
    bytes[1] = QWEN_M07_WQKV_BYTES;
    bytes[2] = QWEN_M07_BQKV_BYTES;
    bytes[4] = bytes[5] = rows * QWEN_KV_HEADS * QWEN_KV_HEAD_ROW_BYTES;
    status = qwen_check_axi_buffers(addresses, bytes, 6U, 3U);
    if (status != QWEN_OK) return status;
    qwen_write_ptr(base, 0x010U, x);
    qwen_write_ptr(base, 0x01CU, wqkv);
    qwen_write_ptr(base, 0x028U, bqkv);
    qwen_write_ptr(base, 0x034U, q);
    qwen_write_ptr(base, 0x040U, k);
    qwen_write_ptr(base, 0x04CU, v);
    WR32(base, 0x058U, run_mode);
    WR32(base, 0x060U, rows);
    WR32(base, 0x068U, QWEN_HIDDEN_SIZE);
    WR32(base, 0x070U, QWEN_LAYOUT_W_OG32_I_L32);
    WR32(base, 0x078U, QWEN_ABI_VERSION);
    WR32(base, 0x080U, job_id);
    WR32(base, 0x088U, qwen_debug_ip_flags());
    return qwen_wait_hls(base, rows, 0x090U, 0x0A0U, 0x120U,
                         QWEN_M07_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
}

QwenStatus qwen_ip_m09(uint32_t k_new, uint32_t v_new, uint32_t layer,
                       uint32_t start_pos, uint32_t rows, uint32_t run_mode,
                       uint32_t job_id)
{
    uint32_t base = QWEN_M09_BASE;
    uint32_t k_cache = QWEN_PL_K_CACHE_BASE;
    uint32_t v_cache = QWEN_PL_V_CACHE_BASE;
    uint32_t addresses[4] = {k_new, v_new, k_cache, v_cache};
    uint32_t bytes[4] = {rows * QWEN_KV_HEADS * QWEN_KV_HEAD_ROW_BYTES,
                         rows * QWEN_KV_HEADS * QWEN_KV_HEAD_ROW_BYTES,
                         (uint32_t)QWEN_KV_CACHE_BYTES_PER_TENSOR,
                         (uint32_t)QWEN_KV_CACHE_BYTES_PER_TENSOR};
    QwenStatus status;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS || layer >= QWEN_NUM_LAYERS ||
        start_pos >= QWEN_MAX_SEQUENCE || rows > QWEN_MAX_SEQUENCE - start_pos ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    status = qwen_check_axi_buffers(addresses, bytes, 4U, 2U);
    if (status != QWEN_OK) return status;
    qwen_write_ptr(base, 0x010U, k_new);
    qwen_write_ptr(base, 0x01CU, v_new);
    qwen_write_ptr(base, 0x028U, k_cache);
    qwen_write_ptr(base, 0x034U, v_cache);
    WR32(base, 0x040U, run_mode);
    WR32(base, 0x048U, layer);
    WR32(base, 0x050U, start_pos);
    WR32(base, 0x058U, rows);
    WR32(base, 0x060U, QWEN_MAX_SEQUENCE);
    WR32(base, 0x068U, QWEN_ABI_VERSION);
    WR32(base, 0x070U, job_id);
    WR32(base, 0x078U, qwen_debug_ip_flags());
    status = qwen_wait_hls(base, rows, 0x090U, 0x0A0U, 0x120U,
                           QWEN_M09_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
    if (status != QWEN_OK) return status;
    if (REG32(base, 0x080U) != start_pos + rows) return QWEN_E_IP_STATUS;
    return QWEN_OK;
}

QwenStatus qwen_ip_m11(uint32_t x, uint32_t wo, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id)
{
    uint32_t base = QWEN_M11_BASE;
    uint32_t addresses[3] = {x, wo, y};
    uint32_t bytes[3];
    QwenStatus status;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS ||
        (qwen_debug_ip_flags() & ~0xBU) != 0U ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    bytes[0] = bytes[2] = rows * QWEN_ROW_BYTES;
    bytes[1] = QWEN_M11_WO_BYTES;
    status = qwen_check_axi_buffers(addresses, bytes, 3U, 2U);
    if (status != QWEN_OK) return status;
    qwen_write_ptr(base, 0x010U, x);
    qwen_write_ptr(base, 0x01CU, wo);
    qwen_write_ptr(base, 0x028U, y);
    WR32(base, 0x034U, run_mode);
    WR32(base, 0x03CU, rows);
    WR32(base, 0x044U, QWEN_HIDDEN_SIZE);
    WR32(base, 0x04CU, QWEN_LAYOUT_W_OG32_I_L32);
    WR32(base, 0x054U, QWEN_ABI_VERSION);
    WR32(base, 0x05CU, job_id);
    WR32(base, 0x064U, qwen_debug_ip_flags());
    return qwen_wait_hls(base, rows, 0x06CU, 0x07CU, 0x0FCU,
                         QWEN_M11_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
}

QwenStatus qwen_ip_m12(uint32_t a, uint32_t b, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t role, uint32_t job_id)
{
    uint32_t base = QWEN_M12_BASE;
    uint32_t addresses[3] = {a, b, y};
    uint32_t bytes[3] = {rows * QWEN_ROW_BYTES, rows * QWEN_ROW_BYTES, rows * QWEN_ROW_BYTES};
    QwenStatus check;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS || role > 1U ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    check = qwen_check_axi_buffers(addresses, bytes, 3U, 2U);
    if (check != QWEN_OK) return check;
    qwen_write_ptr(base, 0x010U, a);
    qwen_write_ptr(base, 0x01CU, b);
    qwen_write_ptr(base, 0x028U, y);
    WR32(base, 0x034U, run_mode);
    WR32(base, 0x03CU, rows);
    WR32(base, 0x044U, QWEN_HIDDEN_SIZE);
    WR32(base, 0x04CU, role);
    WR32(base, 0x054U, QWEN_ABI_VERSION);
    WR32(base, 0x05CU, job_id);
    WR32(base, 0x064U, qwen_debug_ip_flags());
    return qwen_wait_hls(base, rows, 0x06CU, 0x07CU, 0x0FCU,
                         QWEN_M12_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
}

QwenStatus qwen_ip_m13(uint32_t x, uint32_t w_gate, uint32_t w_up,
                       uint32_t w_down, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id)
{
    uint32_t base = QWEN_M13_BASE;
    uint32_t addresses[5] = {x, w_gate, w_up, w_down, y};
    uint32_t bytes[5];
    QwenStatus status;
    if (run_mode > 1U || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS ||
        (qwen_debug_ip_flags() & ~0xBU) != 0U ||
        (run_mode == 1U && rows != 1U)) return QWEN_E_ARGUMENT;
    bytes[0] = bytes[4] = rows * QWEN_ROW_BYTES;
    bytes[1] = bytes[2] = bytes[3] = QWEN_M13_WEIGHT_BYTES;
    status = qwen_check_axi_buffers(addresses, bytes, 5U, 4U);
    if (status != QWEN_OK) return status;
    qwen_write_ptr(base, 0x010U, x);
    qwen_write_ptr(base, 0x01CU, w_gate);
    qwen_write_ptr(base, 0x028U, w_up);
    qwen_write_ptr(base, 0x034U, w_down);
    qwen_write_ptr(base, 0x040U, y);
    WR32(base, 0x04CU, run_mode);
    WR32(base, 0x054U, rows);
    WR32(base, 0x05CU, QWEN_HIDDEN_SIZE);
    WR32(base, 0x064U, QWEN_INTERMEDIATE_SIZE);
    WR32(base, 0x06CU, QWEN_LAYOUT_W_OG32_I_L32);
    WR32(base, 0x074U, QWEN_ABI_VERSION);
    WR32(base, 0x07CU, job_id);
    WR32(base, 0x084U, qwen_debug_ip_flags());
    return qwen_wait_hls(base, rows, 0x08CU, 0x09CU, 0x11CU,
                         QWEN_M13_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
}

QwenStatus qwen_ip_m14(uint32_t hidden, uint32_t history, uint32_t job_id,
                       uint32_t run_mode, float repetition_penalty,
                       uint32_t *token)
{
    uint32_t base = QWEN_M14_BASE;
    uint32_t enable_penalty = repetition_penalty > 1.0f ? 1U : 0U;
    uint32_t result_job;
    QwenStatus status;
    uint32_t addresses[3] = {hidden, QWEN_PL_SHARED_WEIGHT_ADDR, history};
    uint32_t bytes[3] = {QWEN_ROW_BYTES, (uint32_t)QWEN_SHARED_VOCAB_WEIGHT_BYTES, QWEN_HISTORY_BITMAP_BYTES};
    if (token == 0 || run_mode > 1U || !(repetition_penalty >= 1.0f && repetition_penalty <= 16.0f)) {
        return QWEN_E_ARGUMENT;
    }
    status = qwen_check_axi_buffers(addresses, bytes, 3U, 3U);
    if (status != QWEN_OK) return status;
    qwen_write_ptr(base, 0x010U, hidden);
    qwen_write_ptr(base, 0x01CU, QWEN_PL_SHARED_WEIGHT_ADDR);
    qwen_write_ptr(base, 0x028U, history);
    qwen_write_ptr(base, 0x034U, QWEN_PL_BUF_FINAL_NORM + 0x10000U);
    WR32(base, 0x040U, QWEN_HIDDEN_SIZE);
    WR32(base, 0x048U, QWEN_VOCAB_SIZE);
    WR32(base, 0x050U, run_mode);
    WR32(base, 0x058U, 1U); /* M14 consumes one selected row. */
    WR32(base, 0x060U, 0U); /* Do not write the optional full logits vector. */
    WR32(base, 0x068U, enable_penalty);
    WR32(base, 0x070U, qwen_float_bits(repetition_penalty));
    WR32(base, 0x078U, QWEN_ABI_VERSION);
    WR32(base, 0x080U, job_id);
    WR32(base, 0x088U, qwen_debug_ip_flags());
    status = qwen_wait_hls(base, 1U, 0x0C0U, 0x0D0U, 0x150U,
                           QWEN_M14_BUILD_CONFIG, QWEN_POLL_TIMEOUT_MS, 0);
    if (status != QWEN_OK) return status;
    result_job = REG32(base, 0x0B0U);
    if (result_job != job_id) return QWEN_E_IP_STATUS;
    *token = REG32(base, 0x090U);
    if (*token >= QWEN_VOCAB_SIZE) return QWEN_E_IP_STATUS;
    return QWEN_OK;
}
