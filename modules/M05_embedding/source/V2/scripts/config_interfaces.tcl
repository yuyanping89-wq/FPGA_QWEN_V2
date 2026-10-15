# Load this in the CURRENT open solution before C Synthesis (Vitis HLS 2021.1).
# This script only sets interface options; it does not start simulation or synthesis.

# The PS DDR high region starts above 4 GiB. Vivado IP flow otherwise defaults
# to 32-bit M_AXI addressing. Keep addresses 64-bit at the HLS IP boundary.
config_interface -m_axi_addr64

# Match the current hardware profile: uint32_t IDs and 128-bit table/output words.
# Disable automatic widening so IDs are not silently widened to a different bus.
config_interface -m_axi_max_bitwidth 128
config_interface -m_axi_max_widen_bitwidth 0

puts "M05: 64-bit M_AXI addressing enabled; max data width 128; automatic widening disabled."
puts "M05: keep Vivado IP flow and the default 32-bit AXI-Lite data width; inspect generated interfaces after synthesis."
