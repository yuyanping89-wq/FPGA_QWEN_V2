# M05 source bundle layout

The current HLS reproduction tree is kept in V2 because source/scripts/build_hls.tcl explicitly uses that directory. The main source, tests, specification, drivers, and original module IP package are also retained beside it.

For the integrated board design, use the active repository at vivado/ip_repo/m05_embedding. The source/ip_repo/m05_embedding_v2_2_1 export is preserved from the module's standalone delivery package and has a different packaged top HDL/component hash; it is not the repository referenced by the integrated qwen.xpr. Use the integrated IP repository when rebuilding this system.
