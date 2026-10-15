# Validation scope

## Board-level evidence included with this package

- The Vivado BD/address/original-design protection check passed all 45 recorded checks; the JSON record is in vivado/reports/integration_check_20261012.json.
- Offline PHY/BSP/ELF preflight passed for the matching Vitis application. The ELF contains main, qwen_debug_state, and qwen_phy_debug; configured static IP is 192.168.8.190.
- The host verified all 196 model weight files against the PC-side manifest.
- SD loading and PS-to-PL CDMA staging completed.
- 64 KiB and 1 MiB CDMA round-trip tests completed with Status OK.
- The integrated model returned the tested Chinese prompts with Status OK, including a one-token smoke test and two complete short answers.
- Per-module time and AXI read/write totals are recorded in docs/BOARD_TEST_RESULTS_20261015.md.

## Module-level evidence packaged

- M05: HLS C simulation/synthesis reports and testbench sources.
- M07/M11/M12: V2 HLS testbench sources and resource-fit records; M07 nonzero-bias coverage is included in its source tests.
- M08/M10: C simulation/synthesis, control co-simulation, and datapath co-simulation reports for the delivered V2 source package.
- M13 row4: source, interface testbench, HLS reports, and exact active packaged IP. The row8 candidate is separate and unintegrated.
- M14: source tests, numerical contract, flags/performance alignment record, and acceptance notes.
- M06/M09: exact packaged Vivado IP and drivers are included; their original HLS C++ project sources were not in the source workspace.

## Limits of the evidence

The included board tests cover short prompts and the current model-loading path. They are not a maximum-context campaign or a broad accuracy/stability qualification. The module timing counters include the module's DDR stalls and do not isolate pure arithmetic time. Any source or BSP rebuild creates a new artifact that needs its own board verification.


