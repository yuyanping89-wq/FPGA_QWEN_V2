# AXU9EG PL DDR4 constraints for xczu9eg-ffvb1156-2-i.
# Source: ACU9EG core-board schematic, sheet 5 (FPGA Bank 64/65),
# with the PL DDR4 devices on sheets 16 and 17.
#
# Expected external interface names after selecting Make External in the BD:
#   C0_SYS_CLK_0 and C0_DDR4_0
# If Vivado assigned different external interface names, generate the HDL
# wrapper once and replace only the C0_SYS_CLK_0 / C0_DDR4_0 prefixes below.

# 200 MHz differential reference clock for the DDR4 MIG.
set_property PACKAGE_PIN AL8 [get_ports {C0_SYS_CLK_0_clk_p}]
set_property PACKAGE_PIN AL7 [get_ports {C0_SYS_CLK_0_clk_n}]
set_property IOSTANDARD DIFF_SSTL12 [get_ports {C0_SYS_CLK_0_clk_p C0_SYS_CLK_0_clk_n}]
# The DDR4 MIG supplies the C0_SYS_CLK clock constraint itself. Do not add a
# second create_clock here; two definitions make the MIG constraint override
# warning fatal to the synthesis cache.

# DDR4 address bus. DDR4 A14/A15/A16 are carried by WE_n/CAS_n/RAS_n.
set_property PACKAGE_PIN AN9  [get_ports {C0_DDR4_0_adr[0]}]
set_property PACKAGE_PIN AN6  [get_ports {C0_DDR4_0_adr[1]}]
set_property PACKAGE_PIN AN7  [get_ports {C0_DDR4_0_adr[2]}]
set_property PACKAGE_PIN AP5  [get_ports {C0_DDR4_0_adr[3]}]
set_property PACKAGE_PIN AK8  [get_ports {C0_DDR4_0_adr[4]}]
set_property PACKAGE_PIN AP7  [get_ports {C0_DDR4_0_adr[5]}]
set_property PACKAGE_PIN AM10 [get_ports {C0_DDR4_0_adr[6]}]
set_property PACKAGE_PIN AN8  [get_ports {C0_DDR4_0_adr[7]}]
set_property PACKAGE_PIN AK7  [get_ports {C0_DDR4_0_adr[8]}]
set_property PACKAGE_PIN AP10 [get_ports {C0_DDR4_0_adr[9]}]
set_property PACKAGE_PIN AM6  [get_ports {C0_DDR4_0_adr[10]}]
set_property PACKAGE_PIN AM8  [get_ports {C0_DDR4_0_adr[11]}]
set_property PACKAGE_PIN AP4  [get_ports {C0_DDR4_0_adr[12]}]
set_property PACKAGE_PIN AP8  [get_ports {C0_DDR4_0_adr[13]}]
set_property PACKAGE_PIN AJ9  [get_ports {C0_DDR4_0_adr[14]}]
set_property PACKAGE_PIN AP6  [get_ports {C0_DDR4_0_adr[15]}]
set_property PACKAGE_PIN AP11 [get_ports {C0_DDR4_0_adr[16]}]

set_property PACKAGE_PIN AJ10 [get_ports {C0_DDR4_0_ba[0]}]
set_property PACKAGE_PIN AP9  [get_ports {C0_DDR4_0_ba[1]}]
set_property PACKAGE_PIN AL10 [get_ports {C0_DDR4_0_bg[0]}]
set_property PACKAGE_PIN AM9  [get_ports {C0_DDR4_0_act_n}]
set_property PACKAGE_PIN AN4  [get_ports {C0_DDR4_0_cs_n[0]}]
set_property PACKAGE_PIN AK10 [get_ports {C0_DDR4_0_cke[0]}]
set_property PACKAGE_PIN AK9  [get_ports {C0_DDR4_0_odt[0]}]
set_property PACKAGE_PIN AM5  [get_ports {C0_DDR4_0_reset_n}]
set_property PACKAGE_PIN AL6  [get_ports {C0_DDR4_0_ck_t[0]}]
set_property PACKAGE_PIN AL5  [get_ports {C0_DDR4_0_ck_c[0]}]

# DDR4 byte lanes: DQ[7:0], DQ[15:8], DQ[23:16], DQ[31:24].
set_property PACKAGE_PIN AE2  [get_ports {C0_DDR4_0_dq[0]}]
set_property PACKAGE_PIN AG3  [get_ports {C0_DDR4_0_dq[1]}]
set_property PACKAGE_PIN AD1  [get_ports {C0_DDR4_0_dq[2]}]
set_property PACKAGE_PIN AF2  [get_ports {C0_DDR4_0_dq[3]}]
set_property PACKAGE_PIN AD2  [get_ports {C0_DDR4_0_dq[4]}]
set_property PACKAGE_PIN AH3  [get_ports {C0_DDR4_0_dq[5]}]
set_property PACKAGE_PIN AE1  [get_ports {C0_DDR4_0_dq[6]}]
set_property PACKAGE_PIN AF1  [get_ports {C0_DDR4_0_dq[7]}]
set_property PACKAGE_PIN AE3  [get_ports {C0_DDR4_0_dq[8]}]
set_property PACKAGE_PIN AH4  [get_ports {C0_DDR4_0_dq[9]}]
set_property PACKAGE_PIN AD4  [get_ports {C0_DDR4_0_dq[10]}]
set_property PACKAGE_PIN AG4  [get_ports {C0_DDR4_0_dq[11]}]
set_property PACKAGE_PIN AE4  [get_ports {C0_DDR4_0_dq[12]}]
set_property PACKAGE_PIN AG5  [get_ports {C0_DDR4_0_dq[13]}]
set_property PACKAGE_PIN AF3  [get_ports {C0_DDR4_0_dq[14]}]
set_property PACKAGE_PIN AJ4  [get_ports {C0_DDR4_0_dq[15]}]
set_property PACKAGE_PIN AD6  [get_ports {C0_DDR4_0_dq[16]}]
set_property PACKAGE_PIN AG8  [get_ports {C0_DDR4_0_dq[17]}]
set_property PACKAGE_PIN AF6  [get_ports {C0_DDR4_0_dq[18]}]
set_property PACKAGE_PIN AF7  [get_ports {C0_DDR4_0_dq[19]}]
set_property PACKAGE_PIN AD7  [get_ports {C0_DDR4_0_dq[20]}]
set_property PACKAGE_PIN AH8  [get_ports {C0_DDR4_0_dq[21]}]
set_property PACKAGE_PIN AE7  [get_ports {C0_DDR4_0_dq[22]}]
set_property PACKAGE_PIN AG6  [get_ports {C0_DDR4_0_dq[23]}]
set_property PACKAGE_PIN AE12 [get_ports {C0_DDR4_0_dq[24]}]
set_property PACKAGE_PIN AG9  [get_ports {C0_DDR4_0_dq[25]}]
set_property PACKAGE_PIN AH11 [get_ports {C0_DDR4_0_dq[26]}]
set_property PACKAGE_PIN AE9  [get_ports {C0_DDR4_0_dq[27]}]
set_property PACKAGE_PIN AH12 [get_ports {C0_DDR4_0_dq[28]}]
set_property PACKAGE_PIN AG10 [get_ports {C0_DDR4_0_dq[29]}]
set_property PACKAGE_PIN AF12 [get_ports {C0_DDR4_0_dq[30]}]
set_property PACKAGE_PIN AD10 [get_ports {C0_DDR4_0_dq[31]}]

# The MIG is configured as "DM NO DBI", therefore the generated wrapper
# exposes dm_n rather than dm_dbi_n.
set_property PACKAGE_PIN AH2  [get_ports {C0_DDR4_0_dm_n[0]}]
set_property PACKAGE_PIN AE5  [get_ports {C0_DDR4_0_dm_n[1]}]
set_property PACKAGE_PIN AH7  [get_ports {C0_DDR4_0_dm_n[2]}]
set_property PACKAGE_PIN AE10 [get_ports {C0_DDR4_0_dm_n[3]}]

set_property PACKAGE_PIN AH1  [get_ports {C0_DDR4_0_dqs_t[0]}]
set_property PACKAGE_PIN AJ1  [get_ports {C0_DDR4_0_dqs_c[0]}]
set_property PACKAGE_PIN AJ6  [get_ports {C0_DDR4_0_dqs_t[1]}]
set_property PACKAGE_PIN AJ5  [get_ports {C0_DDR4_0_dqs_c[1]}]
set_property PACKAGE_PIN AE8  [get_ports {C0_DDR4_0_dqs_t[2]}]
set_property PACKAGE_PIN AF8  [get_ports {C0_DDR4_0_dqs_c[2]}]
set_property PACKAGE_PIN AF11 [get_ports {C0_DDR4_0_dqs_t[3]}]
set_property PACKAGE_PIN AG11 [get_ports {C0_DDR4_0_dqs_c[3]}]

# Electrical standards for the 1.2 V DDR4 interface.
# Match the four I/O groups required by this DDR4 MIG configuration.
# Command/address and CK use SSTL; DQ/DM and DQS use POD.
set_property IOSTANDARD SSTL12_DCI [get_ports {C0_DDR4_0_adr[*] C0_DDR4_0_ba[*] C0_DDR4_0_bg[*] C0_DDR4_0_act_n C0_DDR4_0_cs_n[*] C0_DDR4_0_cke[*] C0_DDR4_0_odt[*]}]
# DDR4 RESET_n is single-ended CMOS, not an SSTL address/control signal.
set_property IOSTANDARD LVCMOS12 [get_ports {C0_DDR4_0_reset_n}]
set_property DRIVE 8 [get_ports {C0_DDR4_0_reset_n}]
set_property IOSTANDARD DIFF_SSTL12_DCI [get_ports {C0_DDR4_0_ck_t[*] C0_DDR4_0_ck_c[*]}]
set_property IOSTANDARD POD12_DCI [get_ports {C0_DDR4_0_dq[*] C0_DDR4_0_dm_n[*]}]
set_property IOSTANDARD DIFF_POD12_DCI [get_ports {C0_DDR4_0_dqs_t[*] C0_DDR4_0_dqs_c[*]}]
