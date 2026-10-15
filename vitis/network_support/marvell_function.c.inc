/* QWEN_MARVELL_BEGIN: production code also exercised by the MDIO host test. */
volatile QwenPhyDebug qwen_phy_debug __attribute__((aligned(64))) = {.magic=0x50485932U,.version=1U};
static void qwen_phy_publish(void)
{ Xil_DCacheFlushRange((INTPTR)&qwen_phy_debug,sizeof(qwen_phy_debug)); }
static u32_t qwen_phy_fail(u32_t error)
{
    qwen_phy_debug.error=error;qwen_phy_debug.stage=QWEN_PHY_FAILED;
    qwen_phy_publish();return XST_FAILURE;
}
static int qwen_phy_read(XEmacPs *mac,u32_t phy,u32_t reg,u16_t *value)
{
    int ret=XEmacPs_PhyRead(mac,phy,reg,value);
    qwen_phy_debug.last_reg=reg;qwen_phy_debug.mdio_status=(u32_t)ret;
    if(ret!=XST_SUCCESS) {qwen_phy_fail(QWEN_PHY_MDIO_READ);return XST_FAILURE;}
    return XST_SUCCESS;
}
static int qwen_phy_write(XEmacPs *mac,u32_t phy,u32_t reg,u16_t value)
{
    int ret=XEmacPs_PhyWrite(mac,phy,reg,value);
    qwen_phy_debug.last_reg=reg;qwen_phy_debug.mdio_status=(u32_t)ret;
    if(ret!=XST_SUCCESS) {qwen_phy_fail(QWEN_PHY_MDIO_WRITE);return XST_FAILURE;}
    return XST_SUCCESS;
}
static u32_t get_Marvell_phy_speed(XEmacPs *mac,u32_t phy)
{
    u16_t control=0,status=0,speed_status=0;
    u32_t i,speed;
#define PHY_R(reg,value) do {if(qwen_phy_read(mac,phy,reg,&(value))!=XST_SUCCESS) return XST_FAILURE;} while(0)
#define PHY_W(reg,value) do {if(qwen_phy_write(mac,phy,reg,value)!=XST_SUCCESS) return XST_FAILURE;} while(0)
    /* Keep the working reference's Marvell page/register programming sequence. */
    qwen_phy_debug.stage=QWEN_PHY_CONFIG;qwen_phy_publish();
    PHY_W(IEEE_PAGE_ADDRESS_REGISTER,2);
    PHY_R(IEEE_CONTROL_REG_MAC,control);
    control|=IEEE_RGMII_TXRX_CLOCK_DELAYED_MASK;
    PHY_W(IEEE_CONTROL_REG_MAC,control);qwen_phy_debug.mac_control=control;
    PHY_W(IEEE_PAGE_ADDRESS_REGISTER,0);
    PHY_R(IEEE_AUTONEGO_ADVERTISE_REG,control);
    control|=IEEE_ASYMMETRIC_PAUSE_MASK|IEEE_PAUSE_MASK|ADVERTISE_100|ADVERTISE_10;
    PHY_W(IEEE_AUTONEGO_ADVERTISE_REG,control);
    PHY_R(IEEE_1000_ADVERTISE_REG_OFFSET,control);control|=ADVERTISE_1000;
    PHY_W(IEEE_1000_ADVERTISE_REG_OFFSET,control);
    PHY_R(IEEE_COPPER_SPECIFIC_CONTROL_REG,control);
    control|=(7U<<12)|(1U<<11); /* Reference: seven gigabit attempts and downshift. */
    PHY_W(IEEE_COPPER_SPECIFIC_CONTROL_REG,control);
    PHY_R(IEEE_CONTROL_REG_OFFSET,control);
    control|=IEEE_CTRL_AUTONEGOTIATE_ENABLE|IEEE_STAT_AUTONEGOTIATE_RESTART;
    PHY_W(IEEE_CONTROL_REG_OFFSET,control);
    PHY_R(IEEE_CONTROL_REG_OFFSET,control);control|=IEEE_CTRL_RESET_MASK;
    PHY_W(IEEE_CONTROL_REG_OFFSET,control);
    qwen_phy_debug.stage=QWEN_PHY_RESET;qwen_phy_publish();
    for(i=0;i<5000U;++i) {
        PHY_R(IEEE_CONTROL_REG_OFFSET,control);
        qwen_phy_debug.bmcr=control;qwen_phy_debug.reset_polls=i+1U;
        if(!(control&IEEE_CTRL_RESET_MASK)) break;
        usleep(1000);
    }
    if(i==5000U) return qwen_phy_fail(QWEN_PHY_RESET_TIMEOUT);
    qwen_phy_debug.stage=QWEN_PHY_AUTONEG;qwen_phy_publish();
    for(i=0;i<=30U;++i) {
        /* BMSR contains latch-low bits; use the second read. */
        PHY_R(IEEE_STATUS_REG_OFFSET,status);PHY_R(IEEE_STATUS_REG_OFFSET,status);
        qwen_phy_debug.bmsr=status;qwen_phy_debug.autoneg_polls=i;
        if((status&IEEE_STAT_AUTONEGOTIATE_COMPLETE)&&(status&IEEE_STAT_LINK_STATUS)) break;
        if(i==30U) return qwen_phy_fail(QWEN_PHY_AN_TIMEOUT);
        qwen_phy_publish();sleep(1);
    }
    PHY_R(IEEE_SPECIFIC_STATUS_REG,speed_status);qwen_phy_debug.phy_status=speed_status;
    if(!(speed_status&0x0400U)) return qwen_phy_fail(QWEN_PHY_NO_LINK);
    speed=speed_status&IEEE_SPEED_MASK;
    if(speed==IEEE_SPEED_1000) speed=1000;
    else if(speed==IEEE_SPEED_100) speed=100;
    else if(speed==0U) speed=10;
    else return qwen_phy_fail(QWEN_PHY_NO_LINK);
    qwen_phy_debug.link_mbps=speed;qwen_phy_debug.stage=QWEN_PHY_UP;qwen_phy_publish();
#undef PHY_R
#undef PHY_W
    return speed;
}
/* QWEN_MARVELL_END */
