#!/usr/bin/env python3
"""Regenerate BSP, install the local network patch, rebuild/export/link/package.

Run this again after regenerating the BSP in Vitis. No global SDK modifications.
"""
import argparse
import hashlib
import os
from pathlib import Path
import subprocess
import sys

SUPPORT = Path(__file__).resolve().parent
WORKSPACE = SUPPORT.parent
ROOT = SUPPORT.parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-config", action="store_true", help="BSP already regenerated with the five network settings")
    args = parser.parse_args()
    vitis = Path("D:/Xilinx/Vitis/2021.1")
    env = dict(os.environ)
    env["PATH"] = str(vitis / "gnu/aarch64/nt/aarch64-none/bin") + os.pathsep + str(vitis / "gnuwin/bin") + os.pathsep + env["PATH"]
    env["RDI_PLATFORM"] = "win64"
    out = ROOT / "build-review/network_marvell_20261015"
    out.mkdir(exist_ok=True)
    protected = {p: hashlib.sha256(p.read_bytes()).digest() for folder in ("src", "include")
                 for p in (WORKSPACE / "qwen_app" / folder).glob("*") if p.is_file()}

    def run(label, command, cwd=ROOT):
        result = subprocess.run([str(x) for x in command], cwd=cwd, env=env, text=True,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (out / (label + ".log")).write_text(result.stdout, encoding="utf-8")
        if result.returncode:
            print("\n".join(result.stdout.splitlines()[-25:]))
            raise SystemExit(f"{label} failed, exit={result.returncode}")
        print(label, "passed")

    if not args.skip_config:
        run("bsp_regenerate", [vitis / "bin/xsct.bat", SUPPORT / "configure_network.tcl"])
    run("patch_install", [sys.executable, SUPPORT / "install_patch.py"])
    bsp = WORKSPACE / "qwen/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp"
    run("bsp_build", [vitis / "gnuwin/bin/make.exe", "all"], bsp)
    run("export_sync", [sys.executable, SUPPORT / "sync_export.py"])
    run("app_build", [vitis / "gnuwin/bin/make.exe", "-B", "-j4", "all"], WORKSPACE / "qwen_app/Debug")
    run("boot_package", [sys.executable, SUPPORT / "package_boot.py"])
    changed = [str(p) for p, digest in protected.items() if hashlib.sha256(p.read_bytes()).digest() != digest]
    if changed:
        raise SystemExit("Unexpected application source changes: " + repr(changed))
    print("Application C/H files unchanged; ELF and BOOT.BIN rebuilt")


if __name__ == "__main__":
    main()
