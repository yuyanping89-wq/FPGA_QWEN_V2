#!/usr/bin/env python3
"""Synchronize the CURRENT generated BSP headers/libraries into the platform export."""
from pathlib import Path
import hashlib
import shutil

WORKSPACE = Path(__file__).resolve().parent.parent
DOMAIN = "standalone_psu_cortexa53_0"
BSP = WORKSPACE / "qwen/psu_cortexa53_0" / DOMAIN / "bsp"
EXPORT = WORKSPACE / "qwen/export/qwen/sw/qwen" / DOMAIN


def main():
    shutil.copytree(BSP / "psu_cortexa53_0/include", EXPORT / "bspinclude/include", dirs_exist_ok=True)
    shutil.copy2(BSP / "system.mss", EXPORT / "system.mss")
    for name in ("liblwip4.a", "libxil.a", "libxilffs.a"):
        src = BSP / "psu_cortexa53_0/lib" / name
        dst = EXPORT / "bsplib/lib" / name
        shutil.copy2(src, dst)
        assert hashlib.sha256(src.read_bytes()).digest() == hashlib.sha256(dst.read_bytes()).digest()
        print(name, "synchronized from current BSP")


if __name__ == "__main__":
    main()
