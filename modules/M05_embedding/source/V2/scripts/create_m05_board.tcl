# Vivado 2021.1: construct and validate the M05 standalone AXU9EGB design.
# Does not launch synthesis/implementation, generate a bitstream, or access JTAG.
set m05_v2 [file dirname [file dirname [file normalize [info script]]]]
if {![string match "2021.1*" [version -short]]} { error "Use Vivado 2021.1." }
if {[llength [get_projects -quiet]]} { error "Save and close the current project first." }
set m05_root [file join $m05_v2 build m05_board_v22]
if {[file exists $m05_root]} { error "Output already exists: $m05_root. Preserve it and inspect the earlier error instead of overwriting." }
set m05_repo [file join $m05_v2 ip_repo]
set m05_vendor [file join $m05_v2 board alinx_axu9eg_reference ps_config.tcl]
if {![file isfile $m05_vendor] || ![file isfile [file join $m05_repo m05_embedding_v2_2_1 component.xml]]} {
    error "Vendor PS source or packaged M05 IP missing."
}
create_project m05_board [file join $m05_root project] -part xczu9eg-ffvb1156-2-i
set_property target_language Verilog [current_project]
set_property ip_repo_paths [list $m05_repo] [current_project]
update_ip_catalog
if {[llength [get_ipdefs -all llm-fpga.local:user:m05_embedding_v2:2.1]] != 1} {
    error "M05 IP definition is not uniquely available."
}
create_bd_design system
set ps [create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e:3.3 ps]
source $m05_vendor
set_ps_config ps
# Keep the manufacturer's DDR override, including the one in create_project.tcl.
set_property CONFIG.PSU__DDRC__CWL 16 $ps
# Remove unused peripheral blocks from this JTAG + UART test system.
# DDR parameters are preserved; no PL DDR/MIG is used.
foreach name {CAN0 CAN1 DISPLAYPORT PCIE ENET0 ENET1 ENET2 ENET3 USB3_0 USB0 USB1 SATA SD0 SD1 QSPI I2C0 I2C1 TTC0 TTC1 TTC2 TTC3 GPIO0_MIO GPIO1_MIO GPIO2_MIO GPIO_EMIO UART1} {
    set key CONFIG.PSU__${name}__PERIPHERAL__ENABLE
    if {[lsearch -exact [list_property $ps] $key] < 0} { error "Unexpected missing PS property: $key" }
    set_property $key 0 $ps
}
set_property -dict [list \
    CONFIG.PSU__UART0__PERIPHERAL__ENABLE {1} \
    CONFIG.PSU__UART0__PERIPHERAL__IO {MIO 42 .. 43} \
    CONFIG.PSU__USE__M_AXI_GP0 {1} \
    CONFIG.PSU__USE__M_AXI_GP1 {0} \
    CONFIG.PSU__USE__M_AXI_GP2 {0} \
    CONFIG.PSU__MAXIGP0__DATA_WIDTH {32} \
    CONFIG.PSU__USE__S_AXI_GP0 {0} \
    CONFIG.PSU__USE__S_AXI_GP1 {0} \
    CONFIG.PSU__USE__S_AXI_GP2 {1} \
    CONFIG.PSU__USE__S_AXI_GP3 {0} \
    CONFIG.PSU__USE__S_AXI_GP4 {0} \
    CONFIG.PSU__USE__S_AXI_GP5 {0} \
    CONFIG.PSU__USE__S_AXI_GP6 {0} \
    CONFIG.PSU__SAXIGP2__DATA_WIDTH {128} \
    CONFIG.PSU__FPGA_PL0_ENABLE {1} \
    CONFIG.PSU__CRL_APB__PL0_REF_CTRL__FREQMHZ {100} \
    CONFIG.PSU__USE__FABRIC__RST {1} \
    CONFIG.PSU__NUM_FABRIC_RESETS {1} \
    CONFIG.PSU__USE__IRQ0 {0} \
    CONFIG.PSU__USE__IRQ1 {0}] $ps

set m05 [create_bd_cell -type ip -vlnv llm-fpga.local:user:m05_embedding_v2:2.1 m05]
set ctrl [create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 ctrl_axi]
set data [create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 ddr_axi]
set_property -dict [list CONFIG.NUM_SI 1 CONFIG.NUM_MI 1 CONFIG.NUM_CLKS 1] $ctrl
set_property -dict [list CONFIG.NUM_SI 3 CONFIG.NUM_MI 1 CONFIG.NUM_CLKS 1] $data
connect_bd_intf_net [get_bd_intf_pins ps/M_AXI_HPM0_FPD] [get_bd_intf_pins ctrl_axi/S00_AXI]
connect_bd_intf_net [get_bd_intf_pins ctrl_axi/M00_AXI] [get_bd_intf_pins m05/s_axi_ctrl]
foreach pair {{m_axi_ids S00_AXI} {m_axi_weight S01_AXI} {m_axi_out_r S02_AXI}} {
    lassign $pair src dst
    connect_bd_intf_net [get_bd_intf_pins m05/$src] [get_bd_intf_pins ddr_axi/$dst]
}
connect_bd_intf_net [get_bd_intf_pins ddr_axi/M00_AXI] [get_bd_intf_pins ps/S_AXI_HP0_FPD]

# Do not confuse requested PS clock with its computed frequency.
set m05_ps_mhz [get_property CONFIG.PSU__CRL_APB__PL0_REF_CTRL__ACT_FREQMHZ $ps]
if {![string is double -strict $m05_ps_mhz] || $m05_ps_mhz <= 0} {
    error "Invalid PS PL0 computed frequency: $m05_ps_mhz"
}
set clk [create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 m05_clock]
set_property -dict [list CONFIG.PRIM_SOURCE {No_buffer} \
    CONFIG.PRIM_IN_FREQ $m05_ps_mhz CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {100.000} \
    CONFIG.USE_RESET {true} CONFIG.RESET_TYPE {ACTIVE_LOW} CONFIG.USE_LOCKED {true}] $clk
connect_bd_net [get_bd_pins ps/pl_clk0] [get_bd_pins m05_clock/clk_in1]
connect_bd_net [get_bd_pins ps/pl_resetn0] [get_bd_pins m05_clock/resetn]
set rst [create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 reset_100m]
# AUX is tied to constant zero below. Explicit USER override is essential:
# Vivado 2021.1 can otherwise propagate ACTIVE_LOW onto the shared zero net,
# deriving C_AUX_RESET_HIGH=0 and holding the whole fabric in reset forever.
# Unlike C_EXT_RESET_HIGH, C_AUX_RESET_HIGH is overrideable (vendor bd.tcl).
set_property CONFIG.C_AUX_RESET_HIGH 1 $rst
# C_EXT_RESET_HIGH is propagation-only. The PS active-low reset connection
# supplies its polarity; check the derived value after validation below.
set zero [create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 zero]
set_property -dict [list CONFIG.CONST_WIDTH 1 CONFIG.CONST_VAL 0] $zero
connect_bd_net [get_bd_pins zero/dout] [get_bd_pins reset_100m/aux_reset_in] [get_bd_pins reset_100m/mb_debug_sys_rst]
connect_bd_net [get_bd_pins ps/pl_resetn0] [get_bd_pins reset_100m/ext_reset_in]
connect_bd_net [get_bd_pins m05_clock/locked] [get_bd_pins reset_100m/dcm_locked]
connect_bd_net [get_bd_pins m05_clock/clk_out1] \
    [get_bd_pins reset_100m/slowest_sync_clk] [get_bd_pins m05/ap_clk] \
    [get_bd_pins ctrl_axi/aclk] [get_bd_pins ddr_axi/aclk] \
    [get_bd_pins ps/maxihpm0_fpd_aclk] [get_bd_pins ps/saxihp0_fpd_aclk]
connect_bd_net [get_bd_pins reset_100m/interconnect_aresetn] \
    [get_bd_pins ctrl_axi/aresetn] [get_bd_pins ddr_axi/aresetn]
connect_bd_net [get_bd_pins reset_100m/peripheral_aresetn] [get_bd_pins m05/ap_rst_n]
# Interrupt output is intentionally unused: first bring-up polls AP_DONE.
# PS DDR/MIO are dedicated package pins on ZynqMP, not external PL IO ports.

# Control address is local to this test design, not a frozen system ABI address.
assign_bd_address -offset 0xA0000000 -range 4K \
    -target_address_space [get_bd_addr_spaces ps/Data] [get_bd_addr_segs m05/s_axi_ctrl/Reg]
# Only DDR segments are exposed to the data masters, not arbitrary PS peripherals.
set ddr_segments {}
foreach seg [get_bd_addr_segs -of_objects [get_bd_intf_pins ps/S_AXI_HP0_FPD]] {
    if {[string match -nocase *DDR* [get_property NAME $seg]]} { lappend ddr_segments $seg }
}
if {[llength $ddr_segments] == 0} { error "No PS HP0 DDR address segments found." }
foreach master {m_axi_ids m_axi_weight m_axi_out_r} {
    set spaces [get_bd_addr_spaces -of_objects [get_bd_intf_pins m05/$master]]
    if {[llength $spaces] != 1} { error "Expected one address space for $master: $spaces" }
    foreach seg $ddr_segments {
        set seg_name [get_property NAME $seg]
        if {[string match *DDR_LOW $seg_name]} {
            assign_bd_address -offset 0x00000000 -range 2G -target_address_space $spaces $seg
        } elseif {[string match *DDR_HIGH $seg_name]} {
            assign_bd_address -offset 0x800000000 -range 2G -target_address_space $spaces $seg
        } else { error "Unexpected DDR segment: $seg" }
    }
}
save_bd_design
regenerate_bd_layout
validate_bd_design
if {[get_property CONFIG.C_EXT_RESET_HIGH $rst] != 0} {
    error "PS active-low reset polarity was not propagated to reset_100m."
}
if {[get_property CONFIG.C_AUX_RESET_HIGH $rst] != 1 ||
    [get_property CONFIG.POLARITY [get_bd_pins reset_100m/aux_reset_in]] ne "ACTIVE_HIGH"} {
    error "Constant-zero AUX reset must be ACTIVE_HIGH (C_AUX_RESET_HIGH=1)."
}
if {[get_property CONFIG.POLARITY [get_bd_pins reset_100m/mb_debug_sys_rst]] ne "ACTIVE_HIGH"} {
    error "Constant-zero debug reset must be ACTIVE_HIGH."
}
save_bd_design

set report [open [file join $m05_root board_config_report.txt] w]
puts $report "M05 AXU9EGB standalone candidate; Vivado [version -short]"
puts $report "Control base: 0xA0000000; first test uses polling, no interrupt."
puts $report "PS PL0 computed MHz: $m05_ps_mhz"
puts $report "M05 clock pin FREQ_HZ: [get_property CONFIG.FREQ_HZ [get_bd_pins m05_clock/clk_out1]]"
puts $report "External reset: ACTIVE_LOW, C_EXT_RESET_HIGH=[get_property CONFIG.C_EXT_RESET_HIGH $rst]"
puts $report "Constant-zero AUX reset: ACTIVE_HIGH, C_AUX_RESET_HIGH=[get_property CONFIG.C_AUX_RESET_HIGH $rst]"
puts $report "No synthesis/implementation/bitstream or hardware test run by this script."
foreach cell [list $ps $clk $rst] {
    puts $report "\nCELL $cell"
    foreach prop [lsort [list_property $cell]] {
        if {[string match CONFIG.* $prop]} { puts $report "$prop = [get_property $prop $cell]" }
    }
}
foreach space [get_bd_addr_spaces] {
    puts $report "\nADDRESS_SPACE $space"
    foreach seg [get_bd_addr_segs -of_objects $space] {
        puts $report "  $seg"
        foreach prop {OFFSET RANGE} {
            if {[lsearch -exact [list_property $seg] $prop] >= 0} { puts $report "    $prop = [get_property $prop $seg]" }
        }
    }
}
close $report
set bd [get_files */system.bd]
set wrappers [make_wrapper -files $bd -top]
add_files -norecurse $wrappers
set_property top system_wrapper [current_fileset]
update_compile_order -fileset sources_1
puts "M05 BOARD DESIGN CREATED"
puts "Report: [file join $m05_root board_config_report.txt]"
puts "Review validation warnings, clock and DDR mappings before running synthesis."
