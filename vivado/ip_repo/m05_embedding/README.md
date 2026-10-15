# M05 Embedding V2 IP

This package is the fixed-width M05 embedding operator for ABI v2. Its Vivado boundary remains `m05_embedding_v2_ip` with the same AXI ports and 32-bit AXI-Lite register layout used by the integrated Qwen design.

- VLNV: `llm-fpga.local:user:m05_embedding_v2:2.1`
- HLS core: `embedding_v2`; build identity: `0x30010011`
- Target: `xczu9eg-ffvb1156-2-i`, 100 MHz target period 10 ns
- Control address in the integrated project: `0xB0040000`
- M05 AXI masters connect to PL DDR at `0x80000000`, range 1 GiB

## ABI behavior

Flags `0x0`, `0x1`, `0x2`, `0x3`, `0x8`, `0x9`, `0xA`, and `0xB` are accepted. Bit 0 requests the output CRC, bit 1 enables performance reporting, and bit 3 selects strict checking. Bit 2 (`DEBUG_SNAPSHOT`) and every other reserved bit return `BAD_FLAGS` before output writes.

When requested, `output_crc` is the reflected IEEE CRC-32 (polynomial `0xEDB88320`, initial/final XOR `0xFFFFFFFF`) over the produced BF16 output bytes in output order, low byte first. If token validation stops a job after complete rows have been written, the CRC covers that complete output prefix; an empty prefix produces zero. CRC is produced in the output write path. With the CRC flag clear, the result is zero.

Performance results are captured with the completion snapshot. `cycles_total` counts rising edges from accepted start through `ap_done`, inclusive. `read_beats` sums each `RVALID && RREADY` handshake across the ID and weight AXI masters. `write_beats` sums each `WVALID && WREADY` handshake on the output master. These are native AXI data beats, not bytes; a 32-bit ID beat and a 128-bit table beat each count as one. All counters are 64-bit modulo counters and report zero when PERF is disabled. `cycles_compute` is zero because this HLS core does not expose a valid compute-stage activity signal; no AXI traffic heuristic is substituted for that measurement.

Build identity and results are valid after a job completes. Auto-restart is not part of this interface contract; software should read results before starting another job. Error jobs may have written a valid output prefix, so the PS must pass the status to the next stage rather than consume failed output.

## Fixed interfaces and registers

`m_axi_ids` is a 64-bit-address, 32-bit-data AXI4 master. `m_axi_weight` and `m_axi_out_r` are 64-bit-address, 128-bit-data AXI4 masters. `s_axi_ctrl` is 9-bit-address, 32-bit-data AXI4-Lite. The package preserves the existing register offsets, including `AP_CTRL=0x000`, input arguments from `0x010` through `0x064`, status at `0x06C`, processed rows at `0x07C`, performance fields at `0x08C/0x0A4/0x0BC/0x0D4`, CRC at `0x0EC`, and build identity at `0x0FC`. The generated header `drivers/xembedding_v2_hw.h` is the bit-level reference for valid/clear behavior.

The PS remains responsible for validating physical address windows, alignment and buffer aliasing against the actual board DDR map, cache maintenance, and polling or interrupt handling. In the integrated project, completion is polled.

## Verification scope

The revised HLS source passes host ABI/CRC tests and Vitis HLS C simulation. The RTL counter block passes its AXI-handshake simulation, and all four revised HLS cores synthesize for the stated part. Vivado IP packaging and the project Block Design validation are checked separately. No full implementation, bitstream generation, or board test is claimed here.

The packaging script and manifests are retained with the IP. Run `package_m05_ip.tcl` only in Vivado 2021.1 with no project open; it creates the package from this directory's `hdl/` files and does not run system implementation.
