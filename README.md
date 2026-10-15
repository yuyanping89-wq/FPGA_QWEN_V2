# Qwen V2 FPGA board release

This folder is a Git-ready snapshot of the design that passed the current AXU9EG board checks on 2026-10-15. It contains the ten active M05–M14 Vivado IPs, the Vivado Block Design/project sources, the board-tested bitstream and hardware handoff, per-IP and whole-design resource reports, and the Vitis/host inference test code.

The integrated design uses the four-row M13 SwiGLU. The separate eight-row M13 package is preserved as an unintegrated candidate. This is a snapshot of the currently routed, board-tested design; it does not claim the previously reported congestion concern has been eliminated. The released board-tested bitstream is SHA-256 692BEBABB4D95664B8740492CBD5CB2CDDBDAAB602A2927F29123EC9EAC80A17. The matching exported qwen.xsa is SHA-256 1C98EE866642A592316815D2E602CE0B5C82A6AFE0BE9C2800790EA0945C4246. The Vitis-imported bit and the Vivado output bit were byte-identical at packaging time.

## Directory map

- modules: HLS/C++ sources, interface headers, tests, and module-level reports. Each IP repository used by Vivado is in vivado/ip_repo.
- vivado/proj/qwen: the qwen.xpr project, local Block Design source tree, build Tcl, and matching qwen.xsa.
- vivado/ip_repo: the ten active packaged HLS IP repositories. Backups and obsolete M13 packages are excluded.
- vivado/artifacts/board_tested: the exact bitstream used for the board test.
- vivado/reports: hierarchical routed resources, per-IP synthesis reports, routed timing/DRC/route reports, and the integration-check record.
- vitis/qwen_app: the PS application C sources and headers.
- vitis/network_support: the Marvell PHY patch and a portable script to install it into a generated lwIP BSP.
- vitis/host_tools: the PC-side tokenizer/inference client and model asset checks.
- vitis/prebuilt: the matching board-test ELF, supplied as a convenience; the application source remains authoritative.
- docs: hardware, resource, board-test, and Vitis setup notes.

## Current validation

The copied ELF passed the offline BSP/PHY preflight in the source workspace. The board replied at 192.168.8.190, the TCP inference client returned complete answers, and CDMA round-trip tests completed. The detailed measurements and limits are in docs/BOARD_TEST_RESULTS_20261015.md.

The final routed design uses 232,975 total LUTs, 204,414 logic LUTs, 325,372 FFs, 342 BRAM tiles, and 930 DSPs. The routed hierarchical report and per-IP table are in docs/RESOURCE_UTILIZATION.md. Timing and DRC evidence is in vivado/reports/implementation.

## Important delivery boundaries

Model weights are not included. They are large, board-specific input assets and remain outside Git. The SD card used for this test contains the 196 named weight files only; it does not need BOOT.BIN or manifest.json. The manifest is used on the PC to verify files before copying them.

M06 RMSNorm and M09 KV Cache Update are present as the generated packaged HDL/IP and drivers used by the project. The current workspace did not contain their original HLS C++ sources, so those sources are not represented as if they were available. Their exact Vivado IP repositories are included.

The resource reports are kept with their report stage and date. The per-IP routed numbers come from the hierarchical Routed report; individual *_utilization_synth.rpt files are synthesis-stage IP reports and should not be compared as if they were routed results.

No remote Git repository was configured in the workspace. This package is ready for review and commit; it has not been pushed to a remote.

