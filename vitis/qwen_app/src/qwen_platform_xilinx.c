#include "qwen_infer.h"
#include "qwen_debug.h"

#include "xil_cache.h"
#include "xil_io.h"
#include "xtime_l.h"
#if QWEN_ENABLE_UART_LOG
#include "xil_printf.h"
#endif

static QwenServiceHook g_service_hook;
static void *g_service_context;

void qwen_platform_set_service_hook(QwenServiceHook hook, void *context)
{
    g_service_hook = hook;
    g_service_context = context;
}

uint64_t qwen_platform_time_ms(void)
{
    XTime ticks;
    XTime_GetTime(&ticks);
    return ((uint64_t)ticks / (uint64_t)COUNTS_PER_SECOND) * 1000ULL +
           (((uint64_t)ticks % (uint64_t)COUNTS_PER_SECOND) * 1000ULL) /
               (uint64_t)COUNTS_PER_SECOND;
}

uint64_t qwen_platform_time_us(void)
{
    XTime ticks;
    XTime_GetTime(&ticks);
    return ((uint64_t)ticks / (uint64_t)COUNTS_PER_SECOND) * 1000000ULL +
           (((uint64_t)ticks % (uint64_t)COUNTS_PER_SECOND) * 1000000ULL) /
               (uint64_t)COUNTS_PER_SECOND;
}

void qwen_platform_log(const char *text)
{
    uint32_t i=0U;
    if(!text) return;
    while(text[i] && i<sizeof(qwen_debug_state.last_log)-1U) {
        qwen_debug_state.last_log[i]=text[i];++i;
    }
    qwen_debug_state.last_log[i]='\0';++qwen_debug_state.log_sequence;
    qwen_platform_cache_flush((const void *)&qwen_debug_state,sizeof(qwen_debug_state));
#if QWEN_ENABLE_UART_LOG
    xil_printf("%s", text);
#endif
}

void qwen_platform_service(void)
{
    if (g_service_hook != 0) {
        g_service_hook(g_service_context);
    }
}

void qwen_platform_cache_flush(const void *address, uint32_t bytes)
{
    if (address != 0 && bytes != 0U) {
        Xil_DCacheFlushRange((INTPTR)address, bytes);
    }
}

void qwen_platform_cache_invalidate(void *address, uint32_t bytes)
{
    if (address != 0 && bytes != 0U) {
        Xil_DCacheInvalidateRange((INTPTR)address, bytes);
    }
}

uint32_t qwen_platform_read32(uint32_t address)
{
    return Xil_In32((UINTPTR)address);
}

void qwen_platform_write32(uint32_t address, uint32_t value)
{
    Xil_Out32((UINTPTR)address, value);
}
