# M12 Residual Add V2 IP

- VLNV: `xilinx.com:hls:residual_add_v2_hls:3.1`; packaged core revision `2026101201`.
- Build identity: `0x30010001`; target part `xczu9eg-ffvb1156-2-i`, 10 ns HLS clock target.
- Integrated control window: `0xB0070000`–`0xB007FFFF`.
- Three AXI data masters (`m_axi_gmem0`–`m_axi_gmem2`) map to PL DDR `0x80000000`, 1 GiB.

## V2 behavior

Flags `0x0/0x1/0x2/0x3/0x8/0x9/0xA/0xB` are accepted: bit 0 enables output CRC, bit 1 enables performance reporting, and bit 3 selects strict checking. Bit 2 (`DEBUG_SNAPSHOT`) and other reserved bits return `BAD_FLAGS` before output writes.

The output CRC is reflected IEEE CRC-32 (polynomial `0xEDB88320`, initial/final XOR `0xFFFFFFFF`) over the logical BF16 output sequence, with low byte first. CRC is zero when disabled.

At completion, `cycles_total` reports accepted-start through `ap_done`, inclusive. `read_beats` and `write_beats` sum all three masters' AXI data-channel `VALID && READY` handshakes; they are native beats, not bytes. PERF-disabled/invalid-flag results are zero. `cycles_compute` is zero because the HLS core exposes no trustworthy compute-active signal.

Run host ABI/CRC C simulation and Vitis HLS synthesis with the verified sources in `hls/m07_m11_m12_v2_verified`. System implementation and board validation are outside this package check.
