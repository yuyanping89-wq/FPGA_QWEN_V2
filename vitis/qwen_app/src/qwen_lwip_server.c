#include "qwen_lwip_server.h"
#include "qwen_profile.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include <string.h>

#define MAGIC 0x324e5751U
#define RX_CAPACITY (40U+QWEN_MAX_SEQUENCE*4U)
#define TX_CAPACITY (32U+QWEN_MAX_SEQUENCE*4U+QWEN_PROFILE_MAX_WIRE_BYTES)
typedef struct {
    struct tcp_pcb *pcb;
    const QwenModel *model;
    uint8_t rx[RX_CAPACITY],tx[TX_CAPACITY];
    uint32_t rx_used,rx_expected,header_bytes,tx_size,tx_sent,tx_acked;
    uint32_t request_id,prompt_count,max_new,eos,version,command;
    float penalty;
    QwenDebugOptions options;
    uint32_t prompt[QWEN_MAX_SEQUENCE];
    uint8_t pending,running;
} Connection;
static Connection state;
static struct tcp_pcb *listener;
static uint32_t get32(const uint8_t *p)
{return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint16_t get16(const uint8_t *p) {return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
static void put32(uint8_t *p,uint32_t x)
{p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);p[2]=(uint8_t)(x>>16);p[3]=(uint8_t)(x>>24);}
static void put16(uint8_t *p,uint16_t x) {p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static void reset_rx(Connection *s) {s->rx_used=0U;s->rx_expected=8U;s->header_bytes=0U;}
static void flush(Connection *s)
{
    if(!s->pcb) return;
    while(s->tx_sent<s->tx_size) {
        uint32_t part=s->tx_size-s->tx_sent;
        uint16_t available=tcp_sndbuf(s->pcb);
        err_t err;
        if(!available) break;
        if(part>available) part=available;
        if(part>1024U) part=1024U;
        err=tcp_write(s->pcb,s->tx+s->tx_sent,(u16_t)part,TCP_WRITE_FLAG_COPY);
        if(err!=ERR_OK) break;
        s->tx_sent+=part;
    }
    (void)tcp_output(s->pcb);
}
static void queue_response(Connection *s,QwenStatus status,const QwenGenerateResult *r)
{
    uint32_t i,n=r?r->generated_count:0U,h=s->version==2U?32U:28U;
    uint32_t end=h+n*4U,profile=0U;
    memset(s->tx,0,h);
    put32(s->tx,MAGIC);put16(s->tx+4,(uint16_t)s->version);
    put16(s->tx+6,(uint16_t)(status==QWEN_OK?0:-status));put32(s->tx+8,s->request_id);
    put32(s->tx+12,n);put32(s->tx+16,r?r->final_cache_length:0U);
    put32(s->tx+20,r?r->failed_module:0U);put32(s->tx+24,r?r->failed_layer:UINT32_MAX);
    for(i=0;i<n;++i) put32(s->tx+h+i*4U,r->generated[i]);
    if(s->version==2U) {profile=qwen_profile_export(s->tx+end,TX_CAPACITY-end);put32(s->tx+28,profile);}
    s->tx_size=end+profile;s->tx_sent=s->tx_acked=0U;
}
static void on_error(void *arg,err_t err)
{
    Connection *s=(Connection *)arg;(void)err;
    /* Keep running set until the foreground operation returns. */
    s->pcb=0;s->pending=0U;s->tx_size=s->tx_sent=s->tx_acked=0U;reset_rx(s);
}
static err_t abort_connection(Connection *s,struct tcp_pcb *pcb)
{
    tcp_arg(pcb,0);tcp_err(pcb,0);tcp_recv(pcb,0);tcp_sent(pcb,0);tcp_poll(pcb,0,0);
    s->pcb=0;s->pending=0U;s->tx_size=s->tx_sent=s->tx_acked=0U;reset_rx(s);
    tcp_abort(pcb);return ERR_ABRT;
}
static err_t on_sent(void *arg,struct tcp_pcb *pcb,u16_t len)
{
    Connection *s=(Connection *)arg;(void)pcb;
    s->tx_acked+=len;
    if(s->tx_acked>=s->tx_size) s->tx_size=s->tx_sent=s->tx_acked=0U;
    flush(s);return ERR_OK;
}
static err_t on_poll(void *arg,struct tcp_pcb *pcb)
{(void)pcb;flush((Connection *)arg);return ERR_OK;}
/* Exactly one request per response; reject pipelining rather than overwrite it. */
static int consume(Connection *s,const struct pbuf *p)
{
    const struct pbuf *node;
    for(node=p;node;node=node->next) {
        uint32_t at=0U;
        while(at<node->len) {
            uint32_t part;
            if(s->pending||s->running||s->tx_size) return 0;
            part=s->rx_expected-s->rx_used;
            if(part>(uint32_t)node->len-at) part=(uint32_t)node->len-at;
            memcpy(s->rx+s->rx_used,(const uint8_t *)node->payload+at,part);
            at+=part;s->rx_used+=part;
            if(s->rx_used!=s->rx_expected) continue;
            if(s->rx_expected==8U) {
                s->version=get16(s->rx+4);s->command=get16(s->rx+6);
                if(get32(s->rx)!=MAGIC || (s->version!=1U&&s->version!=2U) ||
                   !s->command || s->command>3U || (s->version==1U&&s->command!=1U)) return 0;
                s->header_bytes=s->version==2U?40U:28U;s->rx_expected=s->header_bytes;
                continue;
            }
            if(s->rx_expected==s->header_bytes) {
                s->prompt_count=get32(s->rx+12);
                if(s->prompt_count>QWEN_MAX_SEQUENCE ||
                   (s->command==1U&&!s->prompt_count) || (s->command!=1U&&s->prompt_count)) return 0;
                s->rx_expected=s->header_bytes+s->prompt_count*4U;
                if(s->prompt_count) continue;
            }
            {
                uint32_t i;union {uint32_t u;float f;} penalty;
                s->request_id=get32(s->rx+8);s->max_new=get32(s->rx+16);s->eos=get32(s->rx+20);
                penalty.u=get32(s->rx+24);s->penalty=penalty.f;
                s->options.flags=s->version==2U?get32(s->rx+28):0U;
                s->options.prefill_rows=s->version==2U?get32(s->rx+32):64U;
                s->options.trace_limit=s->version==2U?get32(s->rx+36):0U;
                for(i=0;i<s->prompt_count;++i) s->prompt[i]=get32(s->rx+s->header_bytes+i*4U);
                s->pending=1U;reset_rx(s);
            }
        }
    }
    return 1;
}
static err_t on_receive(void *arg,struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
    Connection *s=(Connection *)arg;int valid;
    if(!p) return abort_connection(s,pcb);
    if(err!=ERR_OK) {pbuf_free(p);return abort_connection(s,pcb);}
    tcp_recved(pcb,p->tot_len);valid=consume(s,p);pbuf_free(p);
    if(!valid) return abort_connection(s,pcb);
    flush(s);return ERR_OK;
}
static err_t on_accept(void *arg,struct tcp_pcb *pcb,err_t err)
{
    Connection *s=(Connection *)arg;
    if(err!=ERR_OK||s->pcb||s->running) {tcp_abort(pcb);return ERR_ABRT;}
    s->pcb=pcb;s->pending=0U;s->tx_size=s->tx_sent=s->tx_acked=0U;reset_rx(s);
    tcp_arg(pcb,s);tcp_recv(pcb,on_receive);tcp_sent(pcb,on_sent);tcp_err(pcb,on_error);
    tcp_poll(pcb,on_poll,2U);return ERR_OK;
}
QwenStatus qwen_lwip_server_start(struct netif *netif,uint16_t port,const QwenModel *model)
{
    struct tcp_pcb *listening;
    if(!netif||!port||!model||!model->loaded) return QWEN_E_ARGUMENT;
    if(listener) return QWEN_E_INTERNAL;
    memset(&state,0,sizeof(state));state.model=model;reset_rx(&state);
    listener=tcp_new();if(!listener) return QWEN_E_INTERNAL;
    if(tcp_bind(listener,IP_ADDR_ANY,port)!=ERR_OK) {tcp_close(listener);listener=0;return QWEN_E_INTERNAL;}
    listening=tcp_listen(listener);
    if(!listening) {tcp_close(listener);listener=0;return QWEN_E_INTERNAL;}
    listener=listening;tcp_arg(listener,&state);tcp_accept(listener,on_accept);return QWEN_OK;
}
void qwen_lwip_server_process(void)
{
    Connection *s=&state;
    if(s->pending&&!s->running&&s->pcb) {
        QwenGenerateResult result;QwenStatus status;
        s->pending=0U;s->running=1U;memset(&result,0,sizeof(result));result.failed_layer=UINT32_MAX;
        if(s->command==1U) {
            QwenGenerateRequest r;
            r.prompt_tokens=s->prompt;r.prompt_count=s->prompt_count;r.max_new_tokens=s->max_new;
            r.eos_token_id=s->eos;r.repetition_penalty=s->penalty;
            status=qwen_generate_debug(s->model,&r,&s->options,&result);
        } else if(s->command==3U) status=qwen_debug_dma_benchmark(s->max_new,s->eos);
        else status=QWEN_OK; /* command 2 reads the last report without resetting it */
        if(s->pcb) queue_response(s,status,s->command==1U?&result:0);
        s->running=0U;flush(s);
    } else flush(s);
}
