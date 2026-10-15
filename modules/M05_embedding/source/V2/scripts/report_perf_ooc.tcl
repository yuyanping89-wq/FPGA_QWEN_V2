# Run manually in the m05_embedding_perf_ooc project after implementation.
# Opens the existing implemented result and writes reports; does not run tools
# that change placement/routing, synthesize, simulate, or generate a bitstream.
if {[llength [get_projects -quiet]] != 1 ||
    [get_property NAME [current_project]] ne "m05_embedding_perf_ooc"} {
    error "Open m05_embedding_perf_ooc.xpr first."
}
set m05_impl [get_runs impl_1]
set m05_status [get_property STATUS $m05_impl]
if {![string match "*route_design Complete*" $m05_status]} {
    error "Finish Run Implementation through routing first. Current status: $m05_status"
}
if {[get_property NEEDS_REFRESH $m05_impl]} {
    error "Implementation is stale. Re-run with the current sources/constraints first."
}
# Open the run explicitly so a still-open synthesis view cannot be reported.
open_run impl_1
set m05_output [file dirname [get_property DIRECTORY [current_project]]]
set m05_stamp [clock format [clock seconds] -format %Y%m%d_%H%M%S]
set m05_reports [file join $m05_output reports $m05_stamp]
if {[file exists $m05_reports]} { error "Report directory already exists: $m05_reports" }
file mkdir $m05_reports
report_utilization -file [file join $m05_reports utilization.rpt]
report_utilization -hierarchical -file [file join $m05_reports utilization_hierarchical.rpt]
report_timing_summary -delay_type min_max -report_unconstrained -max_paths 10 -file [file join $m05_reports timing_summary.rpt]
report_timing -delay_type min -max_paths 10 -file [file join $m05_reports hold_paths.rpt]
report_timing -delay_type max -max_paths 10 -file [file join $m05_reports setup_paths.rpt]
check_timing -verbose -file [file join $m05_reports check_timing.rpt]
# Separate sequential-to-sequential paths from the provisional OOC boundary.
# These filters only select report paths; they do not change timing constraints.
# all_registers includes sequential resources such as BRAM/DSP as well as FFs.
set m05_seq_cells [all_registers -clock [get_clocks ap_clk]]
if {[llength $m05_seq_cells] == 0} {
    error "No sequential cells found on ap_clk; cannot report internal timing."
}
report_timing -delay_type min -from $m05_seq_cells -to $m05_seq_cells -max_paths 20 -file [file join $m05_reports internal_hold_paths.rpt]
report_timing -delay_type max -from $m05_seq_cells -to $m05_seq_cells -max_paths 20 -file [file join $m05_reports internal_setup_paths.rpt]
report_route_status -file [file join $m05_reports route_status.rpt]
report_drc -file [file join $m05_reports drc.rpt]
report_methodology -file [file join $m05_reports methodology.rpt]
report_clocks -file [file join $m05_reports clocks.rpt]
write_checkpoint [file join $m05_reports embedding_v2_ooc_routed.dcp]
set m05_handle [open [file join $m05_reports scope.txt] w]
puts $m05_handle "Report generated from completed impl_1: $m05_status"
puts $m05_handle "ap_clk HD.CLK_SRC = [get_property HD.CLK_SRC [get_ports ap_clk]]"
puts $m05_handle "Provisional I/O delays: min 0 ns, max 2 ns; period 10 ns."
puts $m05_handle "No boundary partition-pin locations. Review any unrouted nets by category."
puts $m05_handle "Internal unroute/hold/setup failures must be resolved; boundary timing is conditional."
puts $m05_handle "No automatic PASS assertion. Not board validation or integrated-system signoff."
puts $m05_handle "Internal timing reports select sequential-to-sequential paths on ap_clk; full reports retain boundary violations."
close $m05_handle
puts "M05 OOC REPORT COMPLETE"
puts "M05 OOC: reports saved to $m05_reports"
puts "M05 OOC: review timing, constraint coverage, routing and DRC before claiming success."
