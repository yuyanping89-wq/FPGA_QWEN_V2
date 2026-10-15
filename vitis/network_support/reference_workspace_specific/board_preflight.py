#!/usr/bin/env python3
"""Read-only offline checks before board testing. No BSP rebuild or board access."""
import argparse
import hashlib
from pathlib import Path
import re
import struct
import sys

WORKSPACE = Path(__file__).resolve().parent.parent


def elf_symbols(path):
    raw = path.read_bytes()
    if raw[:6] != b"\x7fELF\x02\x01":
        raise ValueError("expected ELF64 little-endian")
    offset = struct.unpack_from("<Q", raw, 40)[0]
    size, count = struct.unpack_from("<HH", raw, 58)
    sections = [struct.unpack_from("<IIQQQQIIQQ", raw, offset + i * size) for i in range(count)]
    symbols = {}
    for section in sections:
        if section[1] != 2:
            continue
        strings = sections[section[6]]
        strings = raw[strings[4]:strings[4] + strings[5]]
        for at in range(section[4], section[4] + section[5], section[9]):
            name, info, other, index, value, length = struct.unpack_from("<IBBHQQ", raw, at)
            if index and name < len(strings):
                end = strings.find(b"\0", name)
                symbols[strings[name:end].decode("utf-8", errors="replace")] = value
    return symbols


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sd-root", type=Path, help="optionally compare SD BOOT.BIN with the project output")
    args = parser.parse_args()
    domain = "standalone_psu_cortexa53_0"
    bsp = WORKSPACE / "qwen/psu_cortexa53_0" / domain / "bsp/psu_cortexa53_0"
    export = WORKSPACE / "qwen/export/qwen/sw/qwen" / domain
    failures = []

    def check(label, ok):
        print(("PASS " if ok else "FAIL ") + label)
        if not ok:
            failures.append(label)

    source = bsp / "libsrc/lwip211_v1_5/src/contrib/ports/xilinx/netif/xemacpsif_physpeed.c"
    canonical = WORKSPACE / "network_support/lwip211_v1_5/xemacpsif_physpeed.c"
    check("BSP PHY patch matches project-local version", source.is_file() and source.read_bytes() == canonical.read_bytes())
    for name in ("liblwip4.a", "libxil.a", "libxilffs.a"):
        a, b = bsp / "lib" / name, export / "bsplib/lib" / name
        check(name + " current/export match", a.is_file() and b.is_file() and a.read_bytes() == b.read_bytes())
    for filename, values in {
        "lwipopts.h": {"MEM_SIZE": 524288, "MEMP_NUM_TCP_SEG": 512, "PBUF_POOL_SIZE": 512, "TCP_SND_BUF": 32768, "TCP_WND": 32768},
        "xparameters.h": {"FILE_SYSTEM_USE_LFN": 2},
    }.items():
        path = export / "bspinclude/include" / filename
        text = path.read_text() if path.exists() else ""
        for key, expected in values.items():
            match = re.search(r"#define\s+" + key + r"\s+(\d+)", text)
            check(key + "=" + str(expected), bool(match) and int(match[1]) == expected)
    app_main = WORKSPACE / "qwen_app/src/main.c"
    app_text = app_main.read_text(encoding="utf-8") if app_main.is_file() else ""
    for key, expected in {
        "QWEN_IPV4_ADDRESS": "192,168,8,190",
        "QWEN_IPV4_GATEWAY": "192,168,8,1",
    }.items():
        match = re.search(r"#define\s+" + key + r"\s+([0-9,]+)", app_text)
        check(key + "=" + expected, bool(match) and match[1] == expected)
    elf = WORKSPACE / "qwen_app/Debug/qwen_app.elf"
    try:
        symbols = elf_symbols(elf)
    except (OSError, ValueError, struct.error) as error:
        symbols = {}
        print("ELF read error:", error)
    for name in ("main", "qwen_debug_state", "qwen_phy_debug"):
        check("ELF defines " + name, name in symbols)
        if name in symbols:
            print(f"     {name} = 0x{symbols[name]:016x}")
            if name != "main":
                print(f"     XSCT read: mrd 0x{symbols[name]:x} 16")
    boot = WORKSPACE / "qwen_app_system/Debug/sd_card/BOOT.BIN"
    check("BOOT.BIN exists", boot.is_file() and boot.stat().st_size > 0)
    if boot.is_file():
        digest = hashlib.sha256(boot.read_bytes()).hexdigest()
        print("BOOT SHA256:", digest)
        print("BOOT bytes:", boot.stat().st_size)
        if args.sd_root:
            copied = args.sd_root / "BOOT.BIN"
            check("SD BOOT.BIN SHA256 matches project output", copied.is_file() and hashlib.sha256(copied.read_bytes()).hexdigest() == digest)
    if failures:
        print("OFFLINE CHECK FAILED. Restore network patch/build first; see network_support/rebuild_network.py.")
        return 1
    print("OFFLINE CHECK PASSED. Repackage BOOT after each ELF change. This is not a physical-board validation.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
