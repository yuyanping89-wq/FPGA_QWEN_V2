# M13 SwiGLU version selection

The active Vivado repository at vivado/ip_repo/m13_swiglu is byte-identical to the component.xml in the row4_packed package. This four-row implementation is the one integrated in the Block Design and used for the board-tested bitstream. Its source, interface package, tests, report, and manifest are under row4_board_tested.

The separate eight-row source and full packaged IP are under variants/row8_candidate_unintegrated. That folder is preserved for comparison and later experiments. It was not used to build this bitstream and has no end-to-end board result in this release.

The four-row version keeps at most a 4×64 FP32 Mid tile (1 KiB), consumes each tile in the Down projection, and overwrites the tile. It does not materialize the complete 64×4864 FP32 Mid matrix in PL DDR. Four rows reduce replicated compute and storage compared with eight rows, while a 64-row prefill requires more row blocks and weight passes.
