# Module source index

All ten active packaged IP repositories are in vivado/ip_repo. The Vivado BD uses those repositories through the relative paths stored in qwen.xpr. Per-IP synthesis reports are in vivado/reports/per_ip_synthesis; the hierarchical routed report gives the integrated per-instance resource values.

| Module | Source location | Active Vivado IP | Notes |
|---|---|---|---|
| M05 Embedding | M05_embedding/source | m05_embedding | HLS source, headers, tests, and HLS reports included. |
| M06 RMSNorm | M06_rmsnorm/README.md | m06_rmsnorm | Generated packaged HDL/driver included; original HLS C++ was absent from the current source workspace. |
| M07 QKV Projection | M07_M11_M12/source/hls/M07_qkv_projection | m07_qkv_projection | Uses the four-row shared OG32 implementation. |
| M08 RoPE | M08_rope/source | m08_rope_v2 | V2 top-level C++ and test vectors included. |
| M09 KV Cache Update | M09_kv_cache_update/README.md | m09_kv_cache_update | Generated packaged HDL/driver included; original HLS C++ was absent from the current source workspace. |
| M10 GQA Attention | M10_attention/source | m10_attention_v2 | V2 top-level C++ and tests included. |
| M11 Attention Out | M07_M11_M12/source/hls/M11_attention_out_proj | m11_attention_out | Uses the four-row shared OG32 implementation. |
| M12 Residual Add | M07_M11_M12/source/hls/M12_residual_add | m12_residual_add | Existing V2 implementation retained. |
| M13 SwiGLU | M13_swiglu/row4_board_tested | m13_swiglu | Four-row implementation is integrated and board-tested. |
| M14 LM Head | M14_lm_head/source | m14_lm_head_v2 | Public flags/performance counters aligned with the M05–M12 ABI. |

M13's eight-row candidate source and packaged IP are in M13_swiglu/variants/row8_candidate_unintegrated. Do not substitute it into the active project without regenerating the BD, rebuilding, and testing a new bitstream.

Some module README/metadata files are preserved from their individual delivery packages and may describe earlier validation dates. For the consolidated 2026-10-15 board status, use docs/VALIDATION_SCOPE.md and docs/BOARD_TEST_RESULTS_20261015.md.

