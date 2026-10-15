#!/usr/bin/env python3
"""Execute the production Marvell/dispatch functions with a simulated MDIO bus."""
from pathlib import Path
import os
import subprocess

SUPPORT = Path(__file__).resolve().parent
ROOT = SUPPORT.parents[2]
OUT = ROOT / "build-review/network_marvell_20261015"
SOURCE = SUPPORT / "lwip211_v1_5/xemacpsif_physpeed.c"


def main():
    source = SOURCE.read_text()
    chunk = source[source.index("/* QWEN_MARVELL_BEGIN"):source.index("/* QWEN_MARVELL_END */")]
    start = source.rindex("static u32_t get_IEEE_phy_speed(XEmacPs *xemacpsp, u32_t phy_addr)\n{")
    dispatch = source[start:source.index("\n#endif", start)]
    preamble = r'''
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "qwen_phy_debug.h"
#include "xemac_ieee_reg.h"
typedef uint32_t u32_t;typedef uint16_t u16_t;typedef uintptr_t INTPTR;
typedef struct {int unused;} XEmacPs;
#define XST_SUCCESS 0
#define XST_FAILURE 1
#define PHY_IDENTIFIER_1_REG 2
#define PHY_IDENTIFIER_2_REG 3
#define PHY_MARVELL_IDENTIFIER 0x0141
#define PHY_TI_IDENTIFIER 0x2000
#define PHY_REALTEK_IDENTIFIER 0x001c
static u16_t regs[3][32];
static unsigned page,mode,resets,status_reads,writes,sleeps,errors,generic_calls;
static u16_t status_speed;
static int XEmacPs_PhyRead(XEmacPs *p,u32_t a,u32_t r,u16_t *v)
{
 (void)p;(void)a;
 if(mode==2) return -1;
 if(page==0&&r==0&&(regs[0][0]&IEEE_CTRL_RESET_MASK)) {
   ++resets;if(mode!=1&&resets>=3) regs[0][0]&=(u16_t)~IEEE_CTRL_RESET_MASK;
 }
 if(page==0&&r==1) {*v=++status_reads>=4&&mode!=4?(IEEE_STAT_AUTONEGOTIATE_COMPLETE|IEEE_STAT_LINK_STATUS):0;return 0;}
 if(page==0&&r==IEEE_SPECIFIC_STATUS_REG) {*v=status_speed;return 0;}
 *v=regs[page][r];return 0;
}
static int XEmacPs_PhyWrite(XEmacPs *p,u32_t a,u32_t r,u16_t v)
{
 (void)p;(void)a;++writes;if(mode==3) return -1;
 if(r==IEEE_PAGE_ADDRESS_REGISTER) {page=v;return 0;}
 regs[page][r]=v;return 0;
}
static void Xil_DCacheFlushRange(INTPTR p,unsigned n) {(void)p;(void)n;}
static void usleep(unsigned n) {(void)n;}
static void sleep(unsigned n) {sleeps+=n;}
static u32_t get_Generic_phy_speed(XEmacPs *p,u32_t a) {(void)p;(void)a;++generic_calls;return 1000;}
static u32_t get_TI_phy_speed(XEmacPs *p,u32_t a) {(void)p;(void)a;return 100;}
static u32_t get_Realtek_phy_speed(XEmacPs *p,u32_t a) {(void)p;(void)a;return 10;}
'''
    tests = r'''
static void check(int yes,const char *name) {if(!yes) {++errors;printf("FAIL %s\n",name);}}
static void setup(unsigned m,u16_t speed)
{mode=m;page=resets=status_reads=writes=sleeps=generic_calls=0;memset(regs,0,sizeof(regs));status_speed=speed;
 regs[0][2]=0x0141;regs[0][3]=0x0dd0;}
int main(void)
{
 XEmacPs mac;unsigned i;const unsigned speeds[]={1000,100,10};
 const u16_t bits[]={IEEE_SPEED_1000,IEEE_SPEED_100,0};
 for(i=0;i<3;++i) {
   setup(0,(u16_t)(0x400|bits[i]));
   check(get_IEEE_phy_speed(&mac,1)==speeds[i],"Marvell resolves 1000/100/10 from PHY status");
   check(qwen_phy_debug.phy_id1==0x0141&&qwen_phy_debug.stage==QWEN_PHY_UP,"ID and final debug state");
   check((regs[2][IEEE_CONTROL_REG_MAC]&IEEE_RGMII_TXRX_CLOCK_DELAYED_MASK)==IEEE_RGMII_TXRX_CLOCK_DELAYED_MASK,"reference RGMII delays retained");
   check((regs[0][IEEE_1000_ADVERTISE_REG_OFFSET]&ADVERTISE_1000)==ADVERTISE_1000,"gigabit advertisement");
   check(resets==3&&sleeps==1,"reset polled and AN waited");
 }
 setup(1,0x8400);check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_RESET_TIMEOUT&&resets==5000,"reset has bounded timeout");
 setup(2,0x8400);check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_MDIO_READ,"MDIO read error propagation");
 setup(3,0x8400);check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_MDIO_WRITE,"MDIO write error propagation");
 setup(4,0x8400);check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_AN_TIMEOUT&&sleeps==30,"AN has bounded timeout");
 setup(0,0xc400);check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_NO_LINK,"reserved speed rejected");
 setup(0,0x8000);check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_NO_LINK,"down link rejected");
 setup(0,0);regs[0][2]=0x1234;
 check(get_IEEE_phy_speed(&mac,1)==1000&&generic_calls==1&&writes==0,"unknown vendor avoids Marvell register writes");
 setup(0,0);regs[0][2]=regs[0][3]=0xffff;
 check(get_IEEE_phy_speed(&mac,1)==XST_FAILURE&&qwen_phy_debug.error==QWEN_PHY_BAD_ID&&writes==0,"absent PHY rejected");
 printf("MARVELL_MDIO_%s failures=%u\n",errors?"FAIL":"PASS",errors);return errors?1:0;
}
'''
    OUT.mkdir(parents=True, exist_ok=True)
    test = OUT / "marvell_mdio_test.c"
    test.write_text(preamble + chunk + dispatch + tests)
    compiler = Path("D:/Xilinx/Vitis_HLS/2021.1/tps/mingw/6.2.0/win64.o/nt/bin/gcc.exe")
    env = dict(os.environ)
    env["PATH"] = str(compiler.parent) + os.pathsep + env["PATH"]
    ieee = SUPPORT.parent / "qwen/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/psu_cortexa53_0/libsrc/lwip211_v1_5/src/contrib/ports/xilinx/netif"
    binary = OUT / "marvell_mdio_test.exe"
    log = []
    for command in [[str(compiler), "-std=c99", "-Wall", "-Wextra", "-Werror", "-I"+str(SUPPORT/"lwip211_v1_5"), "-I"+str(ieee), str(test), "-o", str(binary)], [str(binary)]]:
        result = subprocess.run(command, env=env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        log.append(result.stdout);print(result.stdout, end="")
        (OUT / "marvell_mdio_test.log").write_text("\n".join(log), encoding="utf-8")
        if result.returncode:
            raise SystemExit(result.returncode)


if __name__ == "__main__":
    main()
