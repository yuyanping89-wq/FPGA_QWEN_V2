#include <stdint.h>
#include <string.h>

#include "lm_head_v2_driver.h"
#include "lm_head_v2_mailbox.h"
#include "lm_head_v2_mmio.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xtime_l.h"

#if !defined(XPAR_LM_HEAD_V2_0_DEVICE_ID)
#error "The XSA/BSP does not expose XPAR_LM_HEAD_V2_0_DEVICE_ID"
#endif

#define LMH_V2_MAX_REPEATS 1000u
#define LMH_V2_POLL_BUDGET 1000000000ull

static uint32_t float_bits(float value) {
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static float bits_float(uint32_t bits) {
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static void publish_failure(volatile struct lmh_v2_mailbox *mailbox,
                            uint32_t status) {
    mailbox->driver_status = status;
    mailbox->state = LMH_V2_MAILBOX_FAILED;
    Xil_DCacheFlushRange((UINTPTR)mailbox, sizeof(*mailbox));
}

static void record_result(volatile struct lmh_v2_mailbox *mailbox,
                          const struct lmh_v2_result *result) {
    mailbox->selected_token = result->selected_token;
    mailbox->selected_score_bits = float_bits(result->selected_score);
    mailbox->output_crc = result->output_crc;
    mailbox->perf_cycles_total = result->perf_cycles_total;
    mailbox->weight_wait_cycles = result->weight_wait_cycles;
    mailbox->dot_active_cycles = result->dot_active_cycles;
    mailbox->perf_cycles_compute = result->perf_cycles_compute;
    mailbox->perf_read_beats = result->perf_read_beats;
    mailbox->perf_write_beats = result->perf_write_beats;
    mailbox->weight_read_beats_128 = result->weight_read_beats_128;
    mailbox->logits_write_beats_128 = result->logits_write_beats_128;
    mailbox->timer_counts_per_second = (uint64_t)COUNTS_PER_SECOND;
    mailbox->vocab_rows_done = result->vocab_rows_done;
    mailbox->history_read_words = result->history_read_words;
    mailbox->logits_written = result->logits_written;
    mailbox->processed_rows = result->processed_rows;
    mailbox->build_config = result->build_config;
    mailbox->capabilities = result->capabilities;
    mailbox->result_job_id = result->result_job_id;
    mailbox->status_code = result->status_code;
}

int main(void) {
    volatile struct lmh_v2_mailbox *mailbox =
        (volatile struct lmh_v2_mailbox *)(UINTPTR)LMH_V2_MAILBOX_ADDR;
    struct lmh_v2_job job;
    struct lmh_v2_result result;
    XLm_head_v2 instance;
    XTime start;
    XTime end;
    uint32_t repeat;
    int status;

    xil_printf("M14_V2_STANDALONE_BOOT\r\n");
    Xil_DCacheInvalidateRange((UINTPTR)mailbox, sizeof(*mailbox));
    if (mailbox->magic != LMH_V2_MAILBOX_MAGIC ||
        mailbox->version != LMH_V2_MAILBOX_VERSION ||
        mailbox->repeat_count == 0u ||
        mailbox->repeat_count > LMH_V2_MAX_REPEATS) {
        xil_printf("M14_V2_MAILBOX_INVALID addr=0x%08lx\r\n",
                   (unsigned long)LMH_V2_MAILBOX_ADDR);
        return 1;
    }

    status = XLm_head_v2_Initialize(&instance, XPAR_LM_HEAD_V2_0_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("M14_V2_INIT_FAIL status=%d\r\n", status);
        publish_failure(mailbox, LMH_DRV_INTERNAL_ERROR);
        return 2;
    }

    memset(&job, 0, sizeof(job));
    job.hidden_phys_addr = mailbox->hidden_addr;
    job.weight_phys_addr = mailbox->weight_addr;
    job.history_phys_addr = mailbox->history_addr;
    job.logits_phys_addr = mailbox->logits_addr;
    job.hidden_size = mailbox->hidden_size;
    job.vocab_size = mailbox->vocab_size;
    job.run_mode = mailbox->run_mode;
    job.valid_rows = mailbox->valid_rows;
    job.write_logits = mailbox->write_logits;
    job.enable_penalty = mailbox->enable_penalty;
    job.repetition_penalty = bits_float(mailbox->repetition_penalty_bits);
    job.abi_version = mailbox->abi_version;
    job.job_id = mailbox->job_id;
    job.flags = mailbox->flags;

    if ((mailbox->command_flags & LMH_V2_MAILBOX_BYPASS_HOST_VALIDATION) == 0u) {
        status = lmh_v2_validate_job(&job);
        if (status != LMH_DRV_OK) {
            xil_printf("M14_V2_JOB_INVALID status=%d\r\n", status);
            publish_failure(mailbox, (uint32_t)status);
            return 3;
        }
    }

    mailbox->state = LMH_V2_MAILBOX_READY;
    mailbox->driver_status = LMH_DRV_OK;
    Xil_DCacheFlushRange((UINTPTR)mailbox, sizeof(*mailbox));
    Xil_DCacheInvalidateRange((UINTPTR)job.hidden_phys_addr,
                              LMH_V2_HIDDEN_BYTES);
    if (job.enable_penalty != 0u) {
        Xil_DCacheInvalidateRange((UINTPTR)job.history_phys_addr,
                                  LMH_V2_HISTORY_BYTES);
    }

    XTime_GetTime(&start);
    for (repeat = 0u; repeat < mailbox->repeat_count; ++repeat) {
        job.job_id = mailbox->job_id + repeat;
        if ((mailbox->command_flags &
             LMH_V2_MAILBOX_BYPASS_HOST_VALIDATION) != 0u) {
            status = lmh_v2_submit_unchecked(&instance, &job);
        } else {
            status = lmh_v2_submit(&instance, &job);
        }
        if (status == LMH_DRV_OK) {
            status = lmh_v2_wait(&instance, LMH_V2_POLL_BUDGET);
        }
        if (status == LMH_DRV_OK) {
            status = lmh_v2_read_result(&instance, &result);
            if (status == LMH_DRV_OK) {
                record_result(mailbox, &result);
            }
        }
        if (status == LMH_DRV_OK && mailbox->expected_status == LMH_DRV_OK) {
            status = lmh_v2_verify_result(&job, &result);
        }
        if (status == LMH_DRV_OK && mailbox->expected_status != LMH_DRV_OK &&
            (result.status_code != mailbox->expected_status ||
             result.result_job_id != job.job_id)) {
            status = LMH_DRV_BAD_LAYOUT;
        }
        if (status != LMH_DRV_OK) {
            XTime_GetTime(&end);
            mailbox->wall_ticks = (uint64_t)(end - start);
            mailbox->timer_counts_per_second = (uint64_t)COUNTS_PER_SECOND;
            xil_printf("M14_V2_RUN_FAIL repeat=%lu status=%d\r\n",
                       (unsigned long)repeat, status);
            publish_failure(mailbox, (uint32_t)status);
            return 4;
        }
        if (mailbox->expected_status == LMH_DRV_OK &&
            (result.selected_token != mailbox->expected_token ||
            ((job.flags & 1u) != 0u &&
             result.output_crc != mailbox->expected_crc))) {
            XTime_GetTime(&end);
            mailbox->wall_ticks = (uint64_t)(end - start);
            xil_printf("M14_V2_GOLDEN_FAIL repeat=%lu token=%lu crc=0x%08lx\r\n",
                       (unsigned long)repeat,
                       (unsigned long)result.selected_token,
                       (unsigned long)result.output_crc);
            publish_failure(mailbox, LMH_DRV_BAD_TOKEN);
            return 5;
        }
    }
    XTime_GetTime(&end);

    if (job.write_logits != 0u) {
        Xil_DCacheInvalidateRange((UINTPTR)job.logits_phys_addr,
                                  LMH_V2_LOGITS_BYTES);
    }
    mailbox->driver_status = LMH_DRV_OK;
    mailbox->wall_ticks = (uint64_t)(end - start);
    record_result(mailbox, &result);
    mailbox->state = LMH_V2_MAILBOX_DONE;
    Xil_DCacheFlushRange((UINTPTR)mailbox, sizeof(*mailbox));

    xil_printf("M14_V2_RUN_PASS repeats=%lu token=%lu crc=0x%08lx cycles=0x%08lx%08lx\r\n",
               (unsigned long)mailbox->repeat_count,
               (unsigned long)result.selected_token,
               (unsigned long)result.output_crc,
               (unsigned long)(result.perf_cycles_total >> 32),
               (unsigned long)result.perf_cycles_total);
    return 0;
}
