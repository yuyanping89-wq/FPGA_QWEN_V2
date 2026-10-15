"""Create a separate instrumented RTL tree. Does not compile or simulate.

Adapter for this project's Vitis HLS 2021.1 generated Verilog. Reject unexpected
control structures/ports instead of silently producing incorrectly timed counts.
The original solution and HLS generated files are never edited.
"""
import argparse
import hashlib
import json
import re
import shutil
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CORE_BUILD = 0x10010011
PERF_BUILD = 0x20010011
COUNTERS = ("cycles_total", "cycles_compute", "read_beats", "write_beats")


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def replace_once(text, pattern, replacement, description):
    require(len(re.findall(pattern, text, re.M)) == 1,
            "Expected exactly one " + description + "; regenerate/review the adapter")
    return re.sub(pattern, lambda _: replacement, text, count=1, flags=re.M)


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def instrument(top, control):
    require("hls_ip_2021_1" in top, "Only the reviewed HLS 2021.1 RTL is supported")
    require("m05_perf_counters" not in top, "Input is already instrumented")
    # Inspect the exact idle and completion equations used by this HLS version.
    compact = re.sub(r"\s+", "", top)
    require("if(((1'b1==ap_CS_fsm_state1)&(ap_start==1'b0)))beginap_idle=1'b1;"
            in compact, "Unexpected start/idle FSM; accepted-start tap must be reviewed")
    done = re.findall(r"if\(\(1'b1==(ap_CS_fsm_state\d+)\)\)beginap_done=1'b1;", compact)
    ready = re.findall(r"if\(\(1'b1==(ap_CS_fsm_state\d+)\)\)beginap_ready=1'b1;", compact)
    require(len(done) == 1 and done == ready,
            "Top may accept overlapping jobs; the single-job monitor is not applicable")
    # Returning to the idle state after completion is part of this adapter's contract.
    require(re.search(r"ap_ST_fsm_state6\s*:\s*begin\s*ap_NS_fsm\s*=\s*ap_ST_fsm_state1;", top),
            "Unexpected completion-to-idle transition")
    require(done[0] == "ap_CS_fsm_state6", "Completion FSM changed")
    for name, width in (("IDS", 32), ("WEIGHT", 128), ("OUT_R", 128)):
        for field, expected in (("DATA_WIDTH", width), ("ADDR_WIDTH", 64)):
            require(re.search(r"parameter\s+C_M_AXI_" + name + "_" + field +
                              r"\s*=\s*" + str(expected) + r"\s*;", top),
                    "Unexpected AXI width: " + name + " " + field)
    for suffix in COUNTERS:
        field = "result_perf_" + suffix
        require(re.search(r"if\s*\(" + field + r"_ap_vld\)\s*int_" + field +
                          r"\s*<=\s*" + field + r"\s*;", control),
                "Control block no longer captures " + field + " on valid")
    expected_offsets = {
        "CYCLES_TOTAL_DATA_0": 0x8c, "CYCLES_TOTAL_DATA_1": 0x90,
        "CYCLES_COMPUTE_DATA_0": 0xa4, "CYCLES_COMPUTE_DATA_1": 0xa8,
        "READ_BEATS_DATA_0": 0xbc, "READ_BEATS_DATA_1": 0xc0,
        "WRITE_BEATS_DATA_0": 0xd4, "WRITE_BEATS_DATA_1": 0xd8,
    }
    for field, address in expected_offsets.items():
        require(re.search(r"ADDR_RESULT_PERF_" + field +
                          r"\s*=\s*9'h0*" + format(address, "x") + r"\b", control, re.I),
                "Counter register offsets changed: " + field)

    # Match only the control instance, not similarly named submodule ports.
    instance = re.search(r"\bctrl_s_axi_U\s*\(.*?\n\);", top, re.S)
    require(instance is not None, "Missing ctrl_s_axi_U instance")
    block = instance.group()
    for suffix in COUNTERS:
        field = "result_perf_" + suffix
        block = replace_once(block, r"\." + field + r"\(64'd0\)",
                             "." + field + "(m05_measured_" + suffix + ")", field)
        block = replace_once(block, r"\." + field + r"_ap_vld\(" + field + r"_ap_vld\)",
                             "." + field + "_ap_vld(m05_perf_snapshot_valid)", field + " valid")
    block = replace_once(block, r"\.result_build_config\(32'd" + str(CORE_BUILD) + r"\)",
                         ".result_build_config(32'h%08x)" % PERF_BUILD, "core build constant")
    top = top[:instance.start()] + block + top[instance.end():]
    declarations = "\n// Added by prepare_perf_rtl.py; never edit the HLS solution in place.\n"
    declarations += "wire m05_perf_snapshot_valid;\nwire [31:0] m05_perf_snapshot_job_id;\n"
    for suffix in COUNTERS:
        declarations += "wire [63:0] m05_measured_" + suffix + ";\n"
    # Declare before the first module instance; all existing HLS nets stay intact.
    anchor = re.search(r"^embedding_v2_p_anonymous_namespace_run_embedding_dataflow\s", top, re.M)
    require(anchor is not None, "Missing DATAFLOW instance")
    top = top[:anchor.start()] + declarations + top[anchor.start():]
    monitor = """
m05_perf_counters m05_perf_counters_U (
    .ap_clk(ap_clk), .ap_rst_n(ap_rst_n),
    .start_event(ap_start && ap_CS_fsm_state1), .done_event(ap_done),
    .job_id(job_id),
    .ids_rvalid(m_axi_ids_RVALID), .ids_rready(m_axi_ids_RREADY),
    .weight_rvalid(m_axi_weight_RVALID), .weight_rready(m_axi_weight_RREADY),
    .out_wvalid(m_axi_out_r_WVALID), .out_wready(m_axi_out_r_WREADY),
    .snapshot_valid(m05_perf_snapshot_valid),
    .snapshot_job_id(m05_perf_snapshot_job_id),
    .perf_cycles_total(m05_measured_cycles_total),
    .perf_cycles_compute(m05_measured_cycles_compute),
    .perf_read_beats(m05_measured_read_beats),
    .perf_write_beats(m05_measured_write_beats)
);
"""
    require(len(re.findall(r"^endmodule\b", top, re.M)) == 1, "Unexpected top module count")
    return replace_once(top, r"^endmodule\b", monitor + "\nendmodule", "endmodule")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--solution", type=Path,
                        default=ROOT / "hls/embedding_v2_csim/solution1")
    # A fresh directory prevents stale modules from a previous generation.
    parser.add_argument("--output", type=Path, default=ROOT / "build/embedding_v2_perf")
    args = parser.parse_args()
    solution = args.solution.resolve()
    source = solution / "syn/verilog"
    output = args.output.resolve()
    require(not output.exists(), "Output already exists; choose a new --output directory")
    require(ROOT in output.parents, "Output must be inside V2")
    require(not (output == solution or solution in output.parents), "Do not write inside the HLS solution")
    require(not (ROOT / "rtl" == output or ROOT / "rtl" in output.parents), "Do not overwrite maintained RTL")
    files = sorted(source.glob("*.v"))
    require(files, "No synthesized Verilog found; run C Synthesis first")
    top_path = source / "embedding_v2.v"
    control_path = source / "embedding_v2_ctrl_s_axi.v"
    top = instrument(top_path.read_text(encoding="utf-8"), control_path.read_text(encoding="utf-8"))
    driver = solution / "impl/misc/drivers/embedding_v2_v1_0/src"
    require(driver.is_dir(), "Generated HLS driver directory is missing")
    expected_build = json.loads((ROOT / "build_profile.json").read_text(encoding="utf-8-sig"))
    require(expected_build["build_config_hex"].lower() == "0x10010011", "Review adapter for new source profile")

    rtl_out = output / "rtl"
    rtl_out.mkdir(parents=True)
    for path in files:
        shutil.copy2(path, rtl_out / path.name)
    # Keep any HLS memory initialization files with the copied RTL.
    for path in source.iterdir():
        if path.is_file() and path.suffix.lower() in (".dat", ".mem", ".coe"):
            shutil.copy2(path, rtl_out / path.name)
    (rtl_out / "embedding_v2.v").write_text(top, encoding="utf-8")
    shutil.copy2(ROOT / "rtl/m05_perf_counters.v", rtl_out / "m05_perf_counters.v")
    shutil.copytree(driver, output / "drivers")
    (output / "clock.xdc").write_text(
        "create_clock -name ap_clk -period 10.000 [get_ports ap_clk]\n", encoding="utf-8")
    profile = json.loads((ROOT / "perf_build_profile.json").read_text(encoding="utf-8-sig"))
    profile["prepared_at_utc"] = datetime.now(timezone.utc).isoformat()
    profile["input_rtl_sha256"] = {path.name: sha256(path) for path in files}
    profile["output_rtl_sha256"] = {path.name: sha256(path) for path in sorted(rtl_out.glob("*.v"))}
    profile["hls_source_sha256"] = {
        str(path.relative_to(ROOT)).replace("\\", "/"): sha256(path)
        for path in [ROOT / "embedding_v2.cpp"] + sorted((ROOT / "include").glob("*.h"))
    }
    (output / "manifest.json").write_text(json.dumps(profile, indent=2) + "\n", encoding="utf-8")
    print("Prepared instrumented RTL:", output)
    print("build_config = 0x20010011; existing AXI-Lite counter addresses retained.")
    print("No compilation, synthesis or simulation was run. This is not a validated IP package.")


if __name__ == "__main__":
    main()
