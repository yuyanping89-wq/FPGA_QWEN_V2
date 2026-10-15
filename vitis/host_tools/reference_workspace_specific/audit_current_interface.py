#!/usr/bin/env python3
"""Check the PS performance register map and addresses against the current local BD/IP."""
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[3]
PKG = Path(__file__).resolve().parents[1]


def main():
    config = (PKG / "include/qwen_board_config.h").read_text()
    profile = (PKG / "src/qwen_profile.c").read_text()
    bd_file = ROOT / "vivado/proj/qwen/qwen.srcs/sources_1/bd/system_wrapper/system_wrapper.bd"
    bd = json.loads(bd_file.read_text())["design"]
    segments = bd["addressing"]["/zynq_ultra_ps_e_0"]["address_spaces"]["Data"]["segments"]
    keys = ["PERF_CYCLES_TOTAL", "PERF_CYCLES_COMPUTE", "PERF_READ_BEATS", "PERF_WRITE_BEATS", "OUTPUT_CRC", "JOB_ID", "FLAGS"]
    audit = []
    for match in re.finditer(r"\{QWEN_M(\d+)_BASE,(\d+),([^}]+)\}", profile):
        module = int(match[1])
        values = [int(v.strip(), 0) for v in match[3].split(",")]
        folder = next(p for p in (ROOT / "vivado/ip_repo").glob(f"m{module:02}_*") if p.is_dir() and "pre_" not in p.name)
        headers = list(folder.rglob("*_hw.h"))
        assert len(headers) == 1, (module, headers)
        defs = re.findall(r"#define\s+\S+_ADDR_(\w+)_DATA\s+(0x[\da-fA-F]+)", headers[0].read_text())
        for key, expected in zip(keys, values[:7]):
            exact = [int(v, 16) for k, v in defs if k == key]
            found = exact or [int(v, 16) for k, v in defs if k.endswith("_" + key)]
            assert found == [expected], (module, key, expected, found)
        base = int(re.search(fr"#define\s+QWEN_M{module:02}_BASE\s+(0x[\da-fA-F]+)", config)[1], 16)
        segment = next(v for v in segments.values() if v["address_block"].startswith(f"/m{module:02}_"))
        assert int(segment["offset"], 16) == base
        instance = segment["address_block"].split("/")[1]
        for space in bd["addressing"]["/" + instance]["address_spaces"].values():
            seg = list(space["segments"].values())
            assert len(seg) == 1 and int(seg[0]["offset"], 16) == 0x80000000 and seg[0]["range"] == "1G"
        audit.append(dict(module=module, instance=instance, base=hex(base),
                          perf_offsets=dict(zip(keys, map(hex, values[:7]))), capabilities=values[7],
                          header=str(headers[0].relative_to(ROOT)), ddr_window="0x80000000..0xBFFFFFFF"))
    assert len(audit) == 10
    files = [bd_file, ROOT / "vivado/proj/qwen/qwen.xpr",
             ROOT / "vivado/proj/qwen/qwen.runs/impl_1/system_wrapper_wrapper.bit"]
    report = {"date": "2026-10-15", "checks": "10 AXI-Lite bases, 70 performance offsets, all operator PL DDR windows passed",
              "modules": audit, "hardware_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
              "notes": "This audit reads source interfaces. It does not validate FPGA runtime, numeric datapaths or timing closure."}
    output = ROOT / "build-review/ps_debug_20261015/interface_audit.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print("INTERFACE_AUDIT_PASS modules=10 perf_offsets=70 all_operator_ddr_windows=PL_DDR")


if __name__ == "__main__":
    main()
