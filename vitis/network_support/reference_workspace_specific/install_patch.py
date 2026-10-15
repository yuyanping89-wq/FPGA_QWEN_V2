#!/usr/bin/env python3
"""Reapply project-local lwIP PHY patch after BSP regeneration. No application edits."""
from pathlib import Path
import shutil

SUPPORT = Path(__file__).resolve().parent
WORKSPACE = SUPPORT.parent
BSP = WORKSPACE / "qwen/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp"
PORT = BSP / "psu_cortexa53_0/libsrc/lwip211_v1_5/src/contrib/ports/xilinx"


def main():
    target = PORT / "netif/xemacpsif_physpeed.c"
    if not target.is_file():
        raise SystemExit("Generate the current qwen application's lwip211_v1_5 BSP first")
    shutil.copy2(SUPPORT / "lwip211_v1_5/xemacpsif_physpeed.c", target)
    shutil.copy2(SUPPORT / "lwip211_v1_5/qwen_phy_debug.h", PORT / "include/netif/qwen_phy_debug.h")
    print("Network PHY patch installed in application BSP; application sources untouched")


if __name__ == "__main__":
    main()
