# Hardware and IP map

The packaged project is Vivado 2021.1 for part xczu9eg-ffvb1156-2-i. The preserved source Block Design is vivado/proj/qwen/qwen.srcs/sources_1/bd/system_wrapper/system_wrapper.bd. The corresponding XPR, board constraints, and IP repository paths stay under the vivado/ tree, so the project can be opened without flattening or moving the original design structure.

M05 through M14 are individual HLS IPs. The PS controls each operator over AXI-Lite and schedules the operations in software. Their activation, weight, cache, and output data are exchanged through shared PL DDR buffers. The CDMA is used during model loading/staging to copy data from PS-side memory into PL DDR. M14's shared BF16 weights are resident in the PL DDR window while inference runs.

M08 RoPE and M10 GQA Attention are included in the active design. Their control bases are 0xB0090000 and 0xB00A0000. M10 reads the complete layer/head KV cache in PL DDR and writes context to a DDR buffer; M08 writes rotated Q/K data that M09 commits into the KV cache. The PS controls this sequence, so modules do not need direct operator-to-operator wires for tensor data.

The current design has 41 native operator AXI master channels connected through the DDR interconnect path. M08/M10 use the attention PL-DDR interconnect branch; M14 has a dedicated PL-DDR interconnect branch; the other HLS masters use their configured interconnect groups and merge. The existing board DDR4 interface, PS ports, clocks, resets, external pin assignments, and constraints are represented in the preserved Block Design and XDC.

The exact IP parameters, BD nets, AXI address map, and timing resources are represented by the checked-in Block Design, XPR, XSA, and reports. Do not infer address bases for other modules from the two M08/M10 values above; inspect their IP register headers and the Vivado address editor output.

## Bitstream and hardware handoff

The board-tested bitstream is vivado/artifacts/board_tested/system_wrapper_wrapper.bit. The matching exported platform file is vivado/proj/qwen/qwen.xsa. Their hashes are recorded in release_manifest.json. The bitstream is also contained in the XSA export. The packaged Vitis ELF is a convenience matching the current board test; rebuild it if the BSP or application changes.
