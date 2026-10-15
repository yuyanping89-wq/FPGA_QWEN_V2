/* Replace the generated lwIP echo server's echo.c with this file.
 * Keep its main.c, platform.c and board/PHY setup. No inference in callbacks. */
#include "qwen_app.h"
#include "qwen_fatfs_assets.h"
#include "qwen_debug.h"
#include "lwip/tcp.h"
#include "netif/xadapter.h"
#include "ff.h"
#include <stdio.h>

#ifndef QWEN_TCP_PORT
#define QWEN_TCP_PORT 5000U
#endif
extern struct netif *echo_netif;
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
/* These timer helpers are not declared by lwip/tcp.h in lwIP 2.1.1. */
void tcp_fasttmr(void);
void tcp_slowtmr(void);
static FATFS filesystem;
static QwenFatfsAssetContext assets_context;
static int ready;

/* Also called during SD loading, CDMA and accelerator polling. */
static void network_pump(void *unused)
{
    (void)unused;
    if(TcpFastTmrFlag) {tcp_fasttmr();TcpFastTmrFlag=0;}
    if(TcpSlowTmrFlag) {tcp_slowtmr();TcpSlowTmrFlag=0;}
    if(echo_netif) (void)xemacif_input(echo_netif);
}
void print_app_header(void)
{qwen_platform_log("Qwen V2 FPGA inference / profiling, TCP port 5000\r\n");}
int start_application(void)
{
    QwenAssetProvider assets;QwenStatus status;FRESULT fs_status;
    char message[160];
    ready=0;qwen_debug_set_stage(QWEN_BOOT_SD,0);
    fs_status=f_mount(&filesystem,"0:/",1);
    qwen_debug_state.fatfs_result=(uint32_t)fs_status;
    if(fs_status!=FR_OK) {
        qwen_debug_set_stage(QWEN_BOOT_ERROR,QWEN_E_ASSET_IO);
        qwen_platform_log("SD mount failed; inspect qwen_debug_state.fatfs_result\r\n");return -1;
    }
    status=qwen_fatfs_assets_init(&assets_context,"0:/qwen_model",&assets);
    if(status==QWEN_OK) {
        qwen_platform_log("Loading BF16 model into PL DDR; wait for stage=4 READY\r\n");
        status=qwen_app_boot(echo_netif,QWEN_TCP_PORT,&assets,network_pump,0);
    }
    if(status!=QWEN_OK) {
        (void)snprintf(message,sizeof(message),"QWEN BOOT FAILED: %d (%s)\r\n",(int)status,qwen_status_string(status));
        qwen_debug_set_stage(QWEN_BOOT_ERROR,status);qwen_platform_log(message);return -1;
    }
    ready=1;qwen_debug_set_stage(QWEN_BOOT_READY,0);
    qwen_platform_log("QWEN READY port=5000, M13 ROW_TILE=4, 100MHz\r\n");
    return 0;
}
int transfer_data(void)
{if(ready) qwen_app_process();return 0;}
