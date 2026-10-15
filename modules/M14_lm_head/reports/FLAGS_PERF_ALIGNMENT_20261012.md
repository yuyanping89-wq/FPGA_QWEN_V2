# M14 V2 flags and common performance alignment

This candidate keeps the raw r1 delivery at `module_raw/M14_V2_20261010_r1`
unchanged. The active M14 package was rebuilt from an isolated copy with
Vitis HLS 2021.1 and keeps VLNV `user.org:llm_fpga:lm_head_v2:2.0`, the same
top-level ports, and the same 68 control registers.

Flags now accept `0x0/1/2/3/8/9/A/B`. Bit 0 controls the existing CRC, bit 1
controls performance reporting, bit 3 is strict mode, and bit 2 plus unknown
bits return `BAD_FLAGS`. Strict mode still relies on M03 for software address,
capacity, range, ownership, and alias checks.

The common performance fields use the shared M05–M12 RTL counter. Total cycles
count accepted start through `ap_done`, inclusive. Read and write beats sum
native AXI `RVALID && RREADY` and `WVALID && WREADY` handshakes across the four
M14 masters. Counts are beats, not bytes. `cycles_compute` stays zero because
the core exposes no trusted activity signal. PERF-off and invalid-flag jobs
publish zero for the four common fields and M14-specific diagnostic counters.
The latter remain logical/modeled M14 values when PERF is enabled.

Validation completed:

- Vitis HLS C simulation: PASS, including all eight valid flag values, CRC and
  PERF gating, reserved bit rejection, and zero CSim common counters.
- Vitis HLS export / C synthesis: PASS. The 2021.1 `core_revision` overflow
  recovery ran successfully. The generated top was instrumented with the
  shared four-port counter RTL.
- Shared RTL counter XSim: PASS, 88 cycles and 12 snapshots, covering accepted
  start/done boundaries, parallel handshakes, PERF suppression, strict flags,
  stalls, reset, and 64-bit carry/wrap behavior.
- Offline package verification: PASS, 68 registers and 216 IP file references,
  11 Python tests, source contract, all eight assets, and 55-run archive review.

The historical r1 board campaign predates this counter/flags update. This
candidate has not been retested on the integrated board or used to generate a
new system bitstream. Public ABI enums, instance ID, timeout class, and M14's
software ABI remain pending system freeze.
