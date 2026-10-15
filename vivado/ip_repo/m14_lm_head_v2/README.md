# M14 LM Head V2 — flags/performance aligned candidate

VLNV remains `user.org:llm_fpga:lm_head_v2:2.0`; the control register map and
top-level ports are preserved. This IP was re-exported from the isolated M14
candidate source and includes the shared `pspl_v2_axi_perf_counters` RTL in the
top-level Verilog file.

The accepted common flags are `0x0/1/2/3/8/9/A/B`: bit 0 enables CRC, bit 1
enables performance outputs, bit 3 requests strict mode, and bit 2 plus all
other reserved bits return `BAD_FLAGS`. Strict mode does not replace M03's
software buffer-range and alias checks.

The four common performance fields follow M05–M12: `cycles_total` covers
accepted start through `ap_done`, inclusive; read/write beats sum native
`RVALID && RREADY` / `WVALID && WREADY` handshakes across the four M14 AXI
masters. Compute cycles are zero until a trusted activity signal is available.
All common fields are zero when PERF is disabled or flags are invalid. M14's
additional diagnostic counters remain local logical/modeled values and are
populated only when PERF is enabled.
