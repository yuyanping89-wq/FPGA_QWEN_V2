# M09 KV Cache Update source status

The current delivery workspace contained the active Vivado packaged IP in vivado/ip_repo/m09_kv_cache_update, its drivers, and generated Verilog/VHDL. It did not contain the original HLS C++ source or HLS project. The package therefore preserves the exact generated IP used by the board-tested design and does not invent an HLS source reconstruction.

The routed hierarchical report attributes 4,767 total LUTs and 8 BRAM tiles to M09. End-to-end board inference exercised this IP. The PS profile records logical AXI amounts but no hardware-cycle compute field for M09.
