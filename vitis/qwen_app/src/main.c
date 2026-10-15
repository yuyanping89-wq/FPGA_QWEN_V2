/* Empty Application: use with echo.c and the standard Xilinx ZynqMP platform
 * files. Static IPv4; no UART required. Keep carrier-specific PHY reset if needed. */
#include "qwen_app.h"
#include "qwen_debug.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#if LWIP_IPV6
#error "This entry point uses static IPv4; disable lwip211 IPv6 for this example"
#endif
#ifndef QWEN_IPV4_ADDRESS
#define QWEN_IPV4_ADDRESS 192,168,8,190
#endif
#ifndef QWEN_IPV4_NETMASK
#define QWEN_IPV4_NETMASK 255,255,255,0
#endif
#ifndef QWEN_IPV4_GATEWAY
#define QWEN_IPV4_GATEWAY 192,168,8,1
#endif
#define SET_IP(address,...) IP4_ADDR(address,__VA_ARGS__)
static struct netif interface;
struct netif *echo_netif=&interface;
extern volatile int TcpFastTmrFlag,TcpSlowTmrFlag;
void tcp_fasttmr(void);
void tcp_slowtmr(void);
int start_application(void);
int transfer_data(void);
static void pump(void)
{
    if(TcpFastTmrFlag) {tcp_fasttmr();TcpFastTmrFlag=0;}
    if(TcpSlowTmrFlag) {tcp_slowtmr();TcpSlowTmrFlag=0;}
    (void)xemacif_input(echo_netif);
}
int main(void)
{
    ip_addr_t ip,mask,gateway;
    unsigned char mac[]={0x02,0x00,0x00,0x00,0x01,0x10};
    qwen_debug_set_stage(QWEN_BOOT_NETWORK,0);
    init_platform();
    SET_IP(&ip,QWEN_IPV4_ADDRESS);SET_IP(&mask,QWEN_IPV4_NETMASK);SET_IP(&gateway,QWEN_IPV4_GATEWAY);
    qwen_debug_state.ipv4[0]=ip4_addr1(&ip);qwen_debug_state.ipv4[1]=ip4_addr2(&ip);
    qwen_debug_state.ipv4[2]=ip4_addr3(&ip);qwen_debug_state.ipv4[3]=ip4_addr4(&ip);
    lwip_init();
    if(!xemac_add(echo_netif,&ip,&mask,&gateway,mac,PLATFORM_EMAC_BASEADDR)) {
        qwen_debug_set_stage(QWEN_BOOT_ERROR,QWEN_E_INTERNAL);
        qwen_platform_log("Ethernet initialization failed\r\n");
        for(;;) {}
    }
    netif_set_default(echo_netif);netif_set_up(echo_netif);platform_enable_interrupts();
    qwen_platform_log("Ethernet initialized; loading SD model\r\n");
    if(start_application()!=0) for(;;) pump();
    for(;;) {pump();(void)transfer_data();}
}
