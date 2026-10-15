#!/usr/bin/env python3
"""Install the packaged Marvell PHY patch into a generated lwIP 2.1.1 BSP."""
from __future__ import annotations
import argparse
from pathlib import Path
import shutil
import sys


def locate_port(bsp_root: Path):
    rel = Path("libsrc/lwip211_v1_5/src/contrib/ports/xilinx")
    for candidate in (bsp_root / "psu_cortexa53_0" / rel, bsp_root / rel):
        if (candidate / "netif/xemacpsif_physpeed.c").is_file():
            return candidate
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bsp-root", required=True, type=Path,
                        help="Generated BSP directory containing psu_cortexa53_0")
    args = parser.parse_args()
    support = Path(__file__).resolve().parent
    bsp = args.bsp_root.resolve()
    port = locate_port(bsp)
    if port is None:
        print(f"Could not find lwip211_v1_5 under BSP: {bsp}", file=sys.stderr)
        return 2
    src = support / "lwip211_v1_5/xemacpsif_physpeed.c"
    header = support / "lwip211_v1_5/qwen_phy_debug.h"
    if not src.is_file() or not header.is_file():
        print("Packaged PHY patch files are incomplete", file=sys.stderr)
        return 2
    target = port / "netif/xemacpsif_physpeed.c"
    include = port / "include/netif"
    include.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, target)
    shutil.copy2(header, include / "qwen_phy_debug.h")
    print(f"Installed Marvell PHY patch into {port}")
    print("Rebuild/export the BSP and relink the application before loading it.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
