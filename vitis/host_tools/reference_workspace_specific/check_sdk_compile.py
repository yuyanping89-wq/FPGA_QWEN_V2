#!/usr/bin/env python3
"""Compile ARM64 objects against local Vitis 2021.1 headers; no BSP link/board run."""
import argparse
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[3]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--vitis", type=Path, default=Path("D:/Xilinx/Vitis/2021.1"))
    parser.add_argument("--out", type=Path, default=ROOT / "build-review/ps_debug_20261015/arm64")
    args = parser.parse_args()
    sdk = args.vitis / "data/embeddedsw"
    bsp = sdk / "lib/bsp/standalone_v7_5/src"
    lwip = sdk / "ThirdParty/sw_services/lwip211_v1_5/src"
    includes = [bsp / "common", bsp / "arm/ARMv8/64bit", bsp / "arm/common", bsp / "arm/common/gcc",
                lwip / "lwip-2.1.1/src/include", lwip / "contrib/ports/xilinx/include",
                lwip / "contrib/ports/xilinx/include/arch", sdk / "lib/sw_services/xilffs_v4_5/src/include"]
    compiler = args.vitis / "gnu/aarch64/nt/aarch64-none/bin/aarch64-none-elf-gcc.exe"
    args.out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    env["PATH"] = str(compiler.parent) + os.pathsep + env["PATH"]
    command = [str(compiler), "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-DPROCESSOR_LITTLE_ENDIAN",
               "-Ivitis/vitis_ps_lwip/include", "-Ivitis/vitis_ps_lwip/src", "-Ivitis/vitis_ps_lwip/tests/sdk_shim"]
    for directory in includes:
        command += ["-isystem", str(directory)]
    log = []
    sources = sorted((ROOT / "vitis/vitis_ps_lwip/src").glob("*.c")) + [ROOT / "vitis/vitis_ps_lwip/examples/echo.c"]
    for source in sources:
        result = subprocess.run(command + ["-c", str(source), "-o", str(args.out / (source.stem + ".o"))],
                                cwd=ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        log.append(f"{source.name}: exit={result.returncode}\n{result.stdout}")
        print(log[-1], end="")
        (args.out / "compile.log").write_text("\n".join(log), encoding="utf-8")
        if result.returncode:
            raise SystemExit(result.returncode)
    print(f"ARM64_OBJECT_COMPILE_PASS sources={len(sources)}; BSP parameters are compile-only placeholders")


if __name__ == "__main__":
    main()
