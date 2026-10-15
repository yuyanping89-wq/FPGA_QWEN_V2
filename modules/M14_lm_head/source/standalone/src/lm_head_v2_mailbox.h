#ifndef LM_HEAD_V2_MAILBOX_H
#define LM_HEAD_V2_MAILBOX_H

#include <stdint.h>

#define LMH_V2_MAILBOX_ADDR 0x51000000ull
#define LMH_V2_MAILBOX_MAGIC 0x4d313456u /* "M14V" */
#define LMH_V2_MAILBOX_VERSION 2u
#define LMH_V2_MAILBOX_READY 0x52454144u /* "READ" */
#define LMH_V2_MAILBOX_DONE 0x444f4e45u /* "DONE" */
#define LMH_V2_MAILBOX_FAILED 0x4641494cu /* "FAIL" */
#define LMH_V2_MAILBOX_BYPASS_HOST_VALIDATION 0x00000001u

/* Exactly 256 bytes; encoded little-endian by prepare_mailbox.py. */
struct lmh_v2_mailbox {
    uint32_t magic;
    uint32_t version;
    uint32_t state;
    uint32_t repeat_count;
    uint64_t hidden_addr;
    uint64_t weight_addr;
    uint64_t history_addr;
    uint64_t logits_addr;
    uint32_t run_mode;
    uint32_t write_logits;
    uint32_t enable_penalty;
    uint32_t repetition_penalty_bits;
    uint32_t flags;
    uint32_t job_id;
    uint32_t expected_token;
    uint32_t expected_crc;
    uint32_t command_flags;
    uint32_t hidden_size;
    uint32_t vocab_size;
    uint32_t valid_rows;
    uint32_t abi_version;
    uint32_t expected_status;
    uint32_t command_reserved[2];
    uint32_t driver_status;
    uint32_t selected_token;
    uint32_t selected_score_bits;
    uint32_t output_crc;
    uint64_t wall_ticks;
    uint64_t perf_cycles_total;
    uint64_t weight_wait_cycles;
    uint64_t dot_active_cycles;
    uint64_t perf_cycles_compute;
    uint64_t perf_read_beats;
    uint64_t perf_write_beats;
    uint64_t weight_read_beats_128;
    uint64_t logits_write_beats_128;
    uint64_t timer_counts_per_second;
    uint32_t vocab_rows_done;
    uint32_t history_read_words;
    uint32_t logits_written;
    uint32_t processed_rows;
    uint32_t build_config;
    uint32_t capabilities;
    uint32_t result_job_id;
    uint32_t status_code;
    uint32_t response_reserved[4];
};

_Static_assert(sizeof(struct lmh_v2_mailbox) == 256u,
               "lmh_v2_mailbox must remain exactly 256 bytes");

#endif
