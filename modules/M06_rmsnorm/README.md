# M06 RMSNorm source status

The current delivery workspace contained the active Vivado packaged IP in vivado/ip_repo/m06_rmsnorm, its drivers, and generated Verilog/VHDL. It did not contain the original HLS C++ source or HLS project. The package therefore preserves the exact generated IP used by the board-tested design and does not invent an HLS source reconstruction.

The routed hierarchical report attributes 13,896 total LUTs, 20 BRAM tiles, and 84 DSPs to M06. End-to-end board inference exercised this IP. The PS profile records logical AXI amounts but no hardware-cycle compute field for M06.
