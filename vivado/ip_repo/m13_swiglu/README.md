# M13 V2 resource-fit implementation

VLNV `user.org:hls:swiglu_v2:2.0`, revision `2026101203`.

Use the maintained source project and build script recorded in `BUILD_INFO.json`. Flags accept CRC/PERF/STRICT (`0xB`); snapshot and reserved bits reject before writes. PS validates 64-byte alignment, PL DDR ranges and output aliases before start.
