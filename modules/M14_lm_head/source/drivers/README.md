# Driver integration

`lm_head_v2_driver.h` defines the logical M14 V2 job/result contract and
`lm_head_v2_driver.c` performs checks that do not depend on the board memory
map. `lm_head_v2_mmio.c` maps that contract to the Vitis HLS generated driver,
waits with a finite polling budget, reads every result/counter, and checks the
production invariants. M03 remains responsible for DDR-region bounds,
capacity, alias, generation, and cache ownership.

After IP export, use Vitis HLS generated `xlm_head_v2.h` and
`xlm_head_v2_hw.h` as the only source of register offsets. The system wrapper
is included by `lm_head_v2_mmio.h`; numeric offsets are not copied into this
directory. Vitis HLS 2021.1 names the logical hidden pointer
`hidden_offset` and the hidden-size scalar `hidden_size_r`.

The common flags match M05–M12: `0x0/1/2/3/8/9/A/B` are accepted, bit 0
enables CRC, bit 1 enables performance results, bit 3 requests strict mode,
and bit 2 is reserved and rejected. Strict mode does not replace M03's range,
capacity, layout, ownership, or alias validation. The four common performance
fields count accepted-start through `ap_done` cycles (inclusive), and sum
native `RVALID && RREADY` / `WVALID && WREADY` handshakes across all four M14
AXI masters. Compute cycles are zero because no trusted activity signal is
available. M14-specific diagnostic counters are zero unless PERF is enabled.

Before start, the wrapper must:

1. validate descriptors and 64-byte alignment;
2. clean only CPU-dirty hidden/history ranges;
3. avoid re-cleaning the immutable 259.7 MiB weight on every job;
4. program all address high/low words before scalar controls;
5. write `ap_start` last;
6. read status/counters after completion;
7. invalidate logits only when `write_logits=1` and the CPU will read them.
