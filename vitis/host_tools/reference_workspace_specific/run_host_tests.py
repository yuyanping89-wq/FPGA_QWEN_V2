#!/usr/bin/env python3
"""Run control-flow tests without a board/BSP. Requires a native MinGW gcc on Windows."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[3]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--gcc", default=shutil.which("gcc") or r"D:\Xilinx\Vitis_HLS\2021.1\tps\mingw\6.2.0\win64.o\nt\bin\gcc.exe")
    parser.add_argument("--out", type=Path, default=ROOT / "build-review/ps_debug_20261015")
    parser.add_argument("--tokenizer", default=None)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    env["PATH"] = str(Path(args.gcc).parent) + os.pathsep + env["PATH"]
    common = [args.gcc, "-std=c99", "-Wall", "-Wextra", "-Werror", "-Ivitis/vitis_ps_lwip/include", "-Ivitis/vitis_ps_lwip/src"]
    core = ["vitis/vitis_ps_lwip/src/" + f for f in ("qwen_hw_ops.c", "qwen_placeholders.c", "qwen_profile.c")]
    jobs = [
        ("ps_attention_mmio_test", ["tests/ps_attention_mmio_test.c"] + core),
        ("ps_inference_test", ["vitis/vitis_ps_lwip/tests/ps_inference_test.c"] + core +
         ["vitis/vitis_ps_lwip/src/qwen_cdma.c", "vitis/vitis_ps_lwip/src/qwen_infer.c"]),
        ("ps_server_test", ["-Ivitis/vitis_ps_lwip/tests/stubs", "vitis/vitis_ps_lwip/tests/ps_server_test.c"]),
    ]
    log = []

    def run(command):
        result = subprocess.run(command, cwd=ROOT, env=env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        print(result.stdout, end="")
        log.append("$ " + subprocess.list2cmdline([str(x) for x in command]) + "\n" + result.stdout)
        if result.returncode:
            raise RuntimeError(f"command failed: {result.returncode}")

    try:
        for name, sources in jobs:
            binary = args.out / (name + ".exe")
            # CDMA bounce buffers must remain below 2 GiB, including in this host emulator.
            extra = ["-Wl,--image-base,0x400000"] if name == "ps_inference_test" and os.name == "nt" else []
            run(common + extra + sources + ["-o", str(binary)])
            run([str(binary)] + ([str(args.out / "profile_fixture.bin")] if name == "ps_inference_test" else []))
        command = [sys.executable, "vitis/vitis_ps_lwip/tests/test_client.py", "--fixture", str(args.out / "profile_fixture.bin")]
        if args.tokenizer:
            command += ["--tokenizer", args.tokenizer]
        run(command)
    finally:
        (args.out / "host_tests.log").write_text("\n".join(log), encoding="utf-8")


if __name__ == "__main__":
    main()
