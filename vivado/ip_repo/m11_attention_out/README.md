# M11 V2 resource-fit implementation

VLNV `xilinx.com:hls:attention_out_v2_hls:3.1`, revision `2026101202`.

Use the maintained source project and build script recorded in `BUILD_INFO.json`. Flags accept CRC/PERF/STRICT (`0xB`); snapshot and reserved bits reject before writes. PS validates 64-byte alignment, PL DDR ranges and output aliases before start.
