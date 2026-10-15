# Normal Tcl hook, NOT an XDC file. Run by Vivado just before opt_design.
# Fail early if a top-level buffered netlist was accidentally used.
set m05_ooc_pad_cells [get_cells -hierarchical -quiet -filter {REF_NAME =~ IBUF* || REF_NAME =~ OBUF* || REF_NAME =~ IOBUF* || REF_NAME == INBUF}]
if {[llength $m05_ooc_pad_cells] != 0} {
    error "M05 OOC: chip I/O buffers found. Reset and rerun synthesis with -mode out_of_context before implementation."
}
set m05_ooc_clk [get_ports -quiet ap_clk]
if {[llength $m05_ooc_clk] != 1 || [llength [get_clocks -quiet ap_clk]] != 1} {
    error "M05 OOC: expected ap_clk port and its 10 ns clock constraint."
}
# Deterministic exploratory external clock location, not a system assignment.
set m05_ooc_clock_sites [lsort -dictionary [get_sites -filter {SITE_TYPE == BUFGCE}]]
if {[llength $m05_ooc_clock_sites] == 0} {
    error "M05 OOC: no BUFGCE site available for clock context."
}
set m05_ooc_clock_site [lindex $m05_ooc_clock_sites 0]
set_property HD.CLK_SRC $m05_ooc_clock_site $m05_ooc_clk
puts "M05 OOC PRE-OPT: no chip I/O buffers; external clock context = $m05_ooc_clock_site"
