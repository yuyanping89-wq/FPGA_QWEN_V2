#include "qwen_profile.h"
#include "qwen_debug.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    uint32_t kind, phase, module, layer, role, calls, failures, capabilities;
    uint64_t total_us, min_us, max_us, hw_cycles, compute_cycles, read_beats, write_beats, bytes;
    uint32_t hw_samples;
} QwenStat;
typedef struct {
    uint32_t base, module, total, compute, read, write, crc, job, flags, capabilities;
} QwenPerfMap;
/* Offsets verified against the active vivado/ip_repo driver register headers.
 * M06/M09 have constant-zero cycle outputs and logical beat counts. */
static const QwenPerfMap maps[] = {
    {QWEN_M05_BASE,5,0x8c,0xa4,0xbc,0xd4,0xec,0x5c,0x64,3},
    {QWEN_M06_BASE,6,0x94,0xac,0xc4,0xdc,0xf4,0x64,0x6c,4},
    {QWEN_M07_BASE,7,0xb0,0xc8,0xe0,0xf8,0x110,0x80,0x88,3},
    {QWEN_M08_BASE,8,0xb0,0xc8,0xe0,0xf8,0x110,0x80,0x88,3},
    {QWEN_M09_BASE,9,0xb0,0xc8,0xe0,0xf8,0x110,0x70,0x78,4},
    {QWEN_M10_BASE,10,0xb0,0xc8,0xe0,0xf8,0x110,0x80,0x88,3},
    {QWEN_M11_BASE,11,0x8c,0xa4,0xbc,0xd4,0xec,0x5c,0x64,3},
    {QWEN_M12_BASE,12,0x8c,0xa4,0xbc,0xd4,0xec,0x5c,0x64,3},
    {QWEN_M13_BASE,13,0xac,0xc4,0xdc,0xf4,0x10c,0x7c,0x84,3},
    {QWEN_M14_BASE,14,0xe0,0xf8,0x110,0x128,0x140,0x80,0x88,3}
};
static QwenStat stats[QWEN_PROFILE_MAX_STATS], boot[QWEN_PROFILE_MAX_BOOT_STATS];
static uint16_t hash_slots[2048];
static QwenProfileEvent trace[QWEN_PROFILE_MAX_TRACE], last;
static QwenProfileContext context;
static QwenDebugOptions options;
static uint64_t token_us[QWEN_MAX_SEQUENCE];
static uint64_t start_us, total_us, ttft_us, previous_token, boot_us, overhead_us;
static uint32_t stat_count, boot_count, trace_count, trace_dropped, stat_dropped;
static uint32_t prompt_count, generated_count, token_count, faulted, last_sr, last_src, last_dst, last_bytes;
static QwenStatus final_status;
volatile QwenDebugState qwen_debug_state __attribute__((aligned(64))) = {.magic=0x51444247U};
void qwen_debug_set_stage(uint32_t stage,int32_t status)
{
    qwen_debug_state.stage=stage;qwen_debug_state.status=status;
    qwen_platform_cache_flush((const void *)&qwen_debug_state,sizeof(qwen_debug_state));
}

static const QwenPerfMap *map_for(uint32_t base)
{
    uint32_t i;
    for (i=0;i<sizeof(maps)/sizeof(maps[0]);++i) if (maps[i].base==base) return &maps[i];
    return 0;
}
static uint64_t read64(uint32_t address)
{
    uint32_t high, low, again;
    do {
        high=qwen_platform_read32(address+4U);
        low=qwen_platform_read32(address);
        again=qwen_platform_read32(address+4U);
    } while (high!=again);
    return ((uint64_t)high<<32)|low;
}
void qwen_profile_begin(const QwenDebugOptions *opt, uint32_t prompt)
{
    memset(stats,0,sizeof(stats)); memset(hash_slots,0,sizeof(hash_slots));
    memset(&last,0,sizeof(last)); memset(&context,0,sizeof(context));
    options.flags=opt?opt->flags:0U;
    options.prefill_rows=opt&&opt->prefill_rows?opt->prefill_rows:QWEN_MAX_PREFILL_ROWS;
    options.trace_limit=opt?opt->trace_limit:0U;
    if(options.trace_limit>QWEN_PROFILE_MAX_TRACE) options.trace_limit=QWEN_PROFILE_MAX_TRACE;
    context.layer=UINT32_MAX;
    stat_count=trace_count=trace_dropped=stat_dropped=token_count=generated_count=0U;
    prompt_count=prompt; total_us=ttft_us=overhead_us=0U; final_status=QWEN_OK;
    start_us=qwen_platform_time_us(); previous_token=start_us;
}
void qwen_profile_finish(QwenStatus status,uint32_t generated)
{ total_us=qwen_platform_time_us()-start_us; final_status=status; generated_count=generated; }
void qwen_profile_save_boot(void)
{
    boot_count=stat_count<QWEN_PROFILE_MAX_BOOT_STATS?stat_count:QWEN_PROFILE_MAX_BOOT_STATS;
    memcpy(boot,stats,boot_count*sizeof(boot[0])); boot_us=total_us;
    stat_count=0U; memset(stats,0,sizeof(stats)); memset(hash_slots,0,sizeof(hash_slots));
}
void qwen_profile_first_token(void) { ttft_us=qwen_platform_time_us()-start_us; }
void qwen_profile_token(void)
{
    uint64_t now=qwen_platform_time_us();
    if(token_count<QWEN_MAX_SEQUENCE) token_us[token_count++]=now-previous_token;
    previous_token=now;
}
void qwen_profile_context(uint32_t phase,uint32_t layer,uint32_t module,uint32_t role,
                          uint32_t rows,uint32_t position,uint32_t job)
{
    context.phase=phase;context.layer=layer;context.module=module;context.role=role;
    context.rows=rows;context.position=position;context.job=job;
    qwen_debug_state.phase=phase;qwen_debug_state.layer=layer;qwen_debug_state.module=module;
    qwen_debug_state.role=role;qwen_debug_state.rows=rows;qwen_debug_state.position=position;qwen_debug_state.job=job;
}
const QwenProfileContext *qwen_profile_current_context(void) { return &context; }
uint32_t qwen_debug_ip_flags(void) { return QWEN_IP_FLAGS|((options.flags&QWEN_DEBUG_CRC)?QWEN_IP_FLAG_CRC:0U); }
uint32_t qwen_debug_prefill_rows(void) { return options.prefill_rows; }
int qwen_debug_faulted(void) { return faulted!=0U; }
void qwen_debug_latch_fault(void) { faulted=1U; }
void qwen_profile_cdma_snapshot(uint32_t sr,uint32_t source,uint32_t destination,uint32_t bytes)
{last_sr=sr;last_src=source;last_dst=destination;last_bytes=bytes;}
QwenProfileEvent qwen_profile_event_begin(uint32_t kind,uint32_t role)
{
    QwenProfileEvent e;
    memset(&e,0,sizeof(e));
    e.kind=kind;e.phase=context.phase;e.layer=context.layer;e.module=context.module;
    e.role=role;e.job=context.job;e.rows=context.rows;e.position=context.position;
    e.start_us=qwen_platform_time_us();
    return e;
}
void qwen_profile_record(QwenProfileEvent *e,QwenStatus status)
{
    uint64_t begin=qwen_platform_time_us();
    uint32_t hash=(e->kind*3U+e->phase*17U+e->module*37U+e->layer*101U+e->role*7U)*2654435761U;
    uint32_t slot=hash&2047U,i;
    QwenStat *s=0;
    e->status=(int32_t)status;
    if(e->duration_us==0U) e->duration_us=begin-e->start_us;
    for(i=0;i<2048U;++i,slot=(slot+1U)&2047U) {
        if(hash_slots[slot]==0U) {
            if(stat_count>=QWEN_PROFILE_MAX_STATS) break;
            s=&stats[stat_count];hash_slots[slot]=(uint16_t)(++stat_count);
            s->kind=e->kind;s->phase=e->phase;s->module=e->module;s->layer=e->layer;s->role=e->role;
            s->min_us=UINT64_MAX;break;
        }
        s=&stats[hash_slots[slot]-1U];
        if(s->kind==e->kind&&s->phase==e->phase&&s->module==e->module&&s->layer==e->layer&&s->role==e->role) break;
        s=0;
    }
    if(s) {
        ++s->calls;if(status!=QWEN_OK) ++s->failures;
        s->capabilities|=e->capabilities;s->total_us+=e->duration_us;
        if(e->duration_us<s->min_us) s->min_us=e->duration_us;
        if(e->duration_us>s->max_us) s->max_us=e->duration_us;
        s->hw_cycles+=e->hw_cycles;s->compute_cycles+=e->compute_cycles;
        s->read_beats+=e->read_beats;s->write_beats+=e->write_beats;s->bytes+=e->bytes;
        if(e->kind==QWEN_EVENT_IP&&status==QWEN_OK) ++s->hw_samples;
    } else ++stat_dropped;
    if(options.flags&QWEN_DEBUG_TRACE) {
        if(trace_count<options.trace_limit) trace[trace_count++]=*e;else ++trace_dropped;
    }
    if(last.status==0 || status!=QWEN_OK) last=*e;
    overhead_us+=qwen_platform_time_us()-begin;
}
QwenProfileEvent qwen_profile_ip_begin(uint32_t base,uint32_t rows)
{
    const QwenPerfMap *m=map_for(base);
    QwenProfileEvent e=qwen_profile_event_begin(QWEN_EVENT_IP,context.role);
    if(m) {e.module=m->module;e.job=qwen_platform_read32(base+m->job);}
    e.rows=rows;
    /* Start timing after the diagnostic reads, directly before AP_START. */
    e.start_us=qwen_platform_time_us();
    return e;
}
void qwen_profile_ip_end(QwenProfileEvent *e,uint32_t base,QwenStatus status,
                         uint32_t ctrl,uint32_t hw_status,uint32_t build)
{
    const QwenPerfMap *m=map_for(base);
    uint64_t record_start=qwen_platform_time_us();
    e->hw_status=hw_status;e->build=build;e->source=base;e->destination=ctrl;
    if(m && status==QWEN_OK) {
        e->capabilities=m->capabilities;
        if(qwen_platform_read32(base+m->flags)&QWEN_IP_FLAG_CRC) e->capabilities|=QWEN_PROFILE_CRC_ENABLED;
        e->hw_cycles=read64(base+m->total);e->compute_cycles=read64(base+m->compute);
        e->read_beats=read64(base+m->read);e->write_beats=read64(base+m->write);
        e->crc=qwen_platform_read32(base+m->crc);
    }
    if(status==QWEN_E_TIMEOUT) qwen_debug_latch_fault();
    overhead_us+=qwen_platform_time_us()-record_start;
    qwen_profile_record(e,status);
}
void qwen_profile_progress(const char *stage,uint32_t value)
{
    char line[100];
    if(!(options.flags&QWEN_DEBUG_UART)) return;
    (void)snprintf(line,sizeof(line),"QWEN %s=%lu module=%lu layer=%lu\r\n",stage,
                   (unsigned long)value,(unsigned long)context.module,(unsigned long)context.layer);
    qwen_platform_log(line);
}
static void put32(uint8_t *p,uint32_t x)
{p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);p[2]=(uint8_t)(x>>16);p[3]=(uint8_t)(x>>24);}
static void put64(uint8_t *p,uint64_t x) {put32(p,(uint32_t)x);put32(p+4,(uint32_t)(x>>32));}
static void export_stat(uint8_t *p,const QwenStat *s)
{
    put32(p,s->kind);put32(p+4,s->phase);put32(p+8,s->module);put32(p+12,s->layer);
    put32(p+16,s->role);put32(p+20,s->calls);put32(p+24,s->failures);put32(p+28,s->capabilities);
    put64(p+32,s->total_us);put64(p+40,s->min_us);put64(p+48,s->max_us);put64(p+56,s->hw_cycles);
    put64(p+64,s->compute_cycles);put64(p+72,s->read_beats);put64(p+80,s->write_beats);put64(p+88,s->bytes);
    put32(p+96,s->hw_samples);put32(p+100,0U);put64(p+104,0U);
}
uint32_t qwen_profile_export(uint8_t *output,uint32_t capacity)
{
    uint32_t i,offset=QWEN_PROFILE_HEADER_BYTES;
    uint32_t needed=offset+(boot_count+stat_count+trace_count)*QWEN_PROFILE_RECORD_BYTES+token_count*8U;
    if(!output||capacity<needed) return 0U;
    memset(output,0,QWEN_PROFILE_HEADER_BYTES);
    put32(output,0x32465250U);put32(output+4,1U);put32(output+8,QWEN_PROFILE_HEADER_BYTES);
    put32(output+12,QWEN_PROFILE_RECORD_BYTES);put32(output+16,QWEN_PROFILE_RECORD_BYTES);
    put32(output+20,boot_count+stat_count);put32(output+24,trace_count);put32(output+28,token_count);
    put32(output+32,trace_dropped);put32(output+36,stat_dropped);put32(output+40,options.flags);
    put32(output+44,options.prefill_rows);put32(output+48,prompt_count);put32(output+52,generated_count);
    put32(output+56,(uint32_t)final_status);put32(output+60,QWEN_ACCELERATOR_CLOCK_HZ);
    put64(output+64,total_us);put64(output+72,ttft_us);put64(output+80,ttft_us?total_us-ttft_us:0U);
    put64(output+88,ttft_us);put64(output+96,boot_us);put64(output+104,overhead_us);
    put32(output+112,last.module);put32(output+116,last.layer);put32(output+120,last.hw_status);
    put32(output+124,last.build);put32(output+128,last.destination);put32(output+132,last.job);
    put32(output+136,last_sr);put32(output+140,last_src);put32(output+144,last_dst);put32(output+148,last_bytes);
    put32(output+152,faulted);
    for(i=0;i<boot_count;++i,offset+=112U) export_stat(output+offset,&boot[i]);
    for(i=0;i<stat_count;++i,offset+=112U) export_stat(output+offset,&stats[i]);
    for(i=0;i<trace_count;++i,offset+=112U) {
        const QwenProfileEvent *e=&trace[i];uint8_t *p=output+offset;
        put32(p,e->kind);put32(p+4,e->phase);put32(p+8,e->module);put32(p+12,e->layer);
        put32(p+16,e->role);put32(p+20,e->job);put32(p+24,(uint32_t)e->status);put32(p+28,e->capabilities);
        put32(p+32,e->rows);put32(p+36,e->position);put32(p+40,e->hw_status);put32(p+44,e->build);
        put32(p+48,e->crc);put32(p+52,e->source);put32(p+56,e->destination);put32(p+60,e->bytes);
        put64(p+64,e->start_us-start_us);put64(p+72,e->duration_us);put64(p+80,e->hw_cycles);
        put64(p+88,e->compute_cycles);put64(p+96,e->read_beats);put64(p+104,e->write_beats);
    }
    for(i=0;i<token_count;++i,offset+=8U) put64(output+offset,token_us[i]);
    return offset;
}
