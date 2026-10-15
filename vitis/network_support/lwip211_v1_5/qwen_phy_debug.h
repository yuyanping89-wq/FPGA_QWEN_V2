#ifndef QWEN_PHY_DEBUG_H
#define QWEN_PHY_DEBUG_H
#include <stdint.h>
enum {QWEN_PHY_IDENTIFY=1,QWEN_PHY_CONFIG=2,QWEN_PHY_RESET=3,
      QWEN_PHY_AUTONEG=4,QWEN_PHY_UP=5,QWEN_PHY_FAILED=6};
enum {QWEN_PHY_OK=0,QWEN_PHY_MDIO_READ=1,QWEN_PHY_MDIO_WRITE=2,
      QWEN_PHY_RESET_TIMEOUT=3,QWEN_PHY_AN_TIMEOUT=4,QWEN_PHY_NO_LINK=5,QWEN_PHY_BAD_ID=6};
typedef struct {
    uint32_t magic,stage,phy_address,phy_id1,phy_id2,last_reg,mdio_status,error;
    uint32_t bmcr,bmsr,mac_control,phy_status,link_mbps,reset_polls,autoneg_polls,version;
} QwenPhyDebug;
/* Read through Vitis Expressions/Memory. One complete, cache-cleaned 64B line. */
extern volatile QwenPhyDebug qwen_phy_debug;
#endif
