#ifndef QWEN_PROFILE_H
#define QWEN_PROFILE_H

#include "qwen_infer.h"

#define QWEN_DEBUG_TRACE 1U
#define QWEN_DEBUG_CRC   2U
#define QWEN_DEBUG_UART  4U
#define QWEN_PROFILE_HW_CYCLES       1U
#define QWEN_PROFILE_AXI_HANDSHAKES  2U
#define QWEN_PROFILE_LOGICAL_BEATS   4U
#define QWEN_PROFILE_COMPUTE_ACTIVE  8U
#define QWEN_PROFILE_CRC_ENABLED    16U

enum { QWEN_PHASE_LOAD=0, QWEN_PHASE_PREFILL=1, QWEN_PHASE_DECODE=2, QWEN_PHASE_DEBUG=3 };
enum { QWEN_EVENT_IP=0, QWEN_EVENT_CDMA=1, QWEN_EVENT_ASSET_READ=2,
       QWEN_EVENT_CPU_COPY=3, QWEN_EVENT_CACHE=4 };

typedef struct {
    uint32_t flags;
    uint32_t prefill_rows;
    uint32_t trace_limit;
} QwenDebugOptions;

typedef struct {
    uint32_t phase, layer, module, role, rows, position, job;
} QwenProfileContext;

typedef struct {
    uint32_t kind, phase, module, layer, role, job;
    int32_t status;
    uint32_t capabilities, rows, position, hw_status, build, crc;
    uint32_t source, destination, bytes;
    uint64_t start_us, duration_us, hw_cycles, compute_cycles, read_beats, write_beats;
} QwenProfileEvent;

void qwen_profile_begin(const QwenDebugOptions *options, uint32_t prompt_count);
void qwen_profile_finish(QwenStatus status, uint32_t generated);
void qwen_profile_save_boot(void);
void qwen_profile_first_token(void);
void qwen_profile_token(void);
void qwen_profile_context(uint32_t phase, uint32_t layer, uint32_t module,
                          uint32_t role, uint32_t rows, uint32_t position, uint32_t job);
const QwenProfileContext *qwen_profile_current_context(void);
QwenProfileEvent qwen_profile_event_begin(uint32_t kind, uint32_t role);
void qwen_profile_record(QwenProfileEvent *event, QwenStatus status);
QwenProfileEvent qwen_profile_ip_begin(uint32_t base, uint32_t rows);
void qwen_profile_ip_end(QwenProfileEvent *event, uint32_t base, QwenStatus status,
                         uint32_t ctrl, uint32_t hw_status, uint32_t build);
uint32_t qwen_debug_ip_flags(void);
uint32_t qwen_debug_prefill_rows(void);
int qwen_debug_faulted(void);
void qwen_debug_latch_fault(void);
void qwen_profile_cdma_snapshot(uint32_t sr, uint32_t source, uint32_t destination,
                                uint32_t bytes);
uint32_t qwen_profile_export(uint8_t *output, uint32_t capacity);
void qwen_profile_progress(const char *stage, uint32_t value);

QwenStatus qwen_generate_debug(const QwenModel *model, const QwenGenerateRequest *request,
                               const QwenDebugOptions *options, QwenGenerateResult *result);
QwenStatus qwen_debug_dma_benchmark(uint32_t bytes, uint32_t iterations);

#endif
