"""Connect ABI-v2 performance outputs to measured RTL AXI counters in HLS tops."""
from __future__ import annotations

import argparse
import re
from pathlib import Path


MARKER = "PSPL_V2_AXI_PERF_COUNTERS_INSTANCE"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("top", type=Path, help="generated HLS Verilog top")
    parser.add_argument("--ports", required=True,
                        help="comma-separated AXI master prefixes, e.g. m_axi_gmem0,m_axi_gmem1")
    parser.add_argument("--counter-rtl", type=Path, default=Path("hls/common/pspl_v2_axi_perf_counters.v"))
    args = parser.parse_args()

    top = args.top.resolve()
    counter = args.counter_rtl.resolve()
    if not top.is_file() or not counter.is_file():
        parser.error(f"Missing top or counter RTL: {top}; {counter}")
    source = top.read_text(encoding="utf-8-sig")
    if MARKER in source:
        print(f"Already instrumented: {top}")
        return 0

    ports = [name.strip() for name in args.ports.split(",") if name.strip()]
    if not ports or len(set(ports)) != len(ports):
        parser.error("--ports must contain unique AXI master prefixes")
    for port in ports:
        for channel in ("RVALID", "RREADY", "WVALID", "WREADY"):
            if not re.search(rf"\b{re.escape(port + '_' + channel)}\b", source):
                raise SystemExit(f"Missing AXI observation port {port}_{channel} in {top}")

    substitutions = {
        "perf_cycles_total": "pspl_perf_cycles_total",
        "perf_cycles_compute": "pspl_perf_cycles_compute",
        "perf_read_beats": "pspl_perf_read_beats",
        "perf_write_beats": "pspl_perf_write_beats",
    }
    for old, new in substitutions.items():
        # HLS tops expose these as either direct scalar ports (`.perf_*`) or
        # struct result ports (`.result_perf_*`).
        pattern = rf"(\.(?:result_)?{old}\s*\()([^)]*)(\))"
        source, count = re.subn(pattern, rf"\g<1>{new}\g<3>", source)
        if count != 1:
            raise SystemExit(f"Expected one HLS control connection for {old}, found {count} in {top}")
    source = re.sub(r"(\.(?:result_)?perf_[a-z_]+_ap_vld\s*\()([^)]*)(\))",
                    lambda m: m.group(1) + "pspl_perf_snapshot_valid" + m.group(3),
                    source)
    if len(re.findall(r"\.(?:result_)?perf_[a-z_]+_ap_vld\s*\(pspl_perf_snapshot_valid\)", source)) != 4:
        raise SystemExit(f"Expected four performance valid ports in {top}")

    declarations = """
wire pspl_perf_snapshot_valid;
wire [31:0] pspl_perf_snapshot_job_id;
wire [63:0] pspl_perf_cycles_total;
wire [63:0] pspl_perf_cycles_compute;
wire [63:0] pspl_perf_read_beats;
wire [63:0] pspl_perf_write_beats;
"""
    declaration_anchor = re.search(r"(?m)^\s*wire\s+ap_start\s*;", source)
    if not declaration_anchor:
        raise SystemExit(f"Could not locate the HLS internal declarations in {top}")
    source = source[:declaration_anchor.start()] + declarations + source[declaration_anchor.start():]

    rvalid = ",".join(f"{p}_RVALID" for p in reversed(ports))
    rready = ",".join(f"{p}_RREADY" for p in reversed(ports))
    wvalid = ",".join(f"{p}_WVALID" for p in reversed(ports))
    wready = ",".join(f"{p}_WREADY" for p in reversed(ports))
    instance = f"""
// {MARKER}: accepted-start to done cycles and native AXI data handshakes.
// No HLS compute-stage activity signal exists, so compute cycles stay zero.
pspl_v2_axi_perf_counters #(.NUM_PORTS({len(ports)})) pspl_v2_axi_perf_counters_inst (
    .ap_clk(ap_clk), .ap_rst_n(ap_rst_n),
    .start_event(ap_start && ap_CS_fsm_state1), .done_event(ap_done),
    .job_id(job_id), .flags(flags), .compute_active(1'b0),
    .rvalid({{{rvalid}}}), .rready({{{rready}}}),
    .wvalid({{{wvalid}}}), .wready({{{wready}}}),
    .snapshot_valid(pspl_perf_snapshot_valid),
    .snapshot_job_id(pspl_perf_snapshot_job_id),
    .perf_cycles_total(pspl_perf_cycles_total),
    .perf_cycles_compute(pspl_perf_cycles_compute),
    .perf_read_beats(pspl_perf_read_beats),
    .perf_write_beats(pspl_perf_write_beats)
);
"""
    endmodule = re.search(r"(?m)^endmodule\b", source)
    if not endmodule:
        raise SystemExit(f"Could not locate endmodule in {top}")
    source = source[:endmodule.start()] + instance + "\n" + source[endmodule.start():]
    source += "\n\n" + counter.read_text(encoding="utf-8-sig")
    top.write_text(source, encoding="utf-8", newline="")
    print(f"Instrumented {top} with {len(ports)} AXI ports")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
