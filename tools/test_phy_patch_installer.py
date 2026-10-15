#!/usr/bin/env python3
"""Smoke-test the portable BSP PHY patch installer using a temporary fake BSP."""
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
INSTALLER = ROOT / "vitis/network_support/apply_phy_patch.py"
PATCH = ROOT / "vitis/network_support/lwip211_v1_5"
REL = Path("psu_cortexa53_0/libsrc/lwip211_v1_5/src/contrib/ports/xilinx")


def main() -> int:
    with tempfile.TemporaryDirectory(prefix="qwen_phy_patch_test_") as tmp:
        bsp = Path(tmp) / "bsp"
        port = bsp / REL
        (port / "netif").mkdir(parents=True)
        (port / "netif/xemacpsif_physpeed.c").write_text("old BSP file\n", encoding="utf-8")
        result = subprocess.run(
            [sys.executable, str(INSTALLER), "--bsp-root", str(bsp)],
            text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        )
        if result.returncode:
            print(result.stdout, end="")
            return result.returncode
        target = port / "netif/xemacpsif_physpeed.c"
        header = port / "include/netif/qwen_phy_debug.h"
        if target.read_bytes() != (PATCH / "xemacpsif_physpeed.c").read_bytes():
            raise SystemExit("PHY source copy mismatch")
        if header.read_bytes() != (PATCH / "qwen_phy_debug.h").read_bytes():
            raise SystemExit("PHY debug header copy mismatch")
        print("PHY_PATCH_INSTALLER_PASS")
        return 0


if __name__ == "__main__":
    raise SystemExit(main())

