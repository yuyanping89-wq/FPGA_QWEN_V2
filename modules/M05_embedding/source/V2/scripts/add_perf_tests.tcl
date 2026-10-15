# Run through Vivado Tools > Run Tcl Script with m05_embedding_perf open.
# Adds simulation sources and sets the simulation top. Does not run simulation.
set m05_tb_root [file normalize [file join [file dirname [info script]] ..]]
if {[llength [get_projects -quiet]] != 1} {
    error "Open the existing m05_embedding_perf Vivado project first."
}
if {[get_property TOP [get_filesets sources_1]] ne "embedding_v2"} {
    error "Wrong design top: expected embedding_v2."
}
set m05_top_files [get_files -all -quiet *embedding_v2.v]
if {[llength $m05_top_files] != 1} {
    error "Expected one embedding_v2.v; check for duplicate or missing design sources."
}
set m05_top_handle [open [get_property NAME [lindex $m05_top_files 0]] r]
set m05_top_text [read $m05_top_handle]
close $m05_top_handle
if {[string first "m05_perf_counters_U" $m05_top_text] < 0 ||
    [string first "32'h20010011" $m05_top_text] < 0} {
    error "This is not the instrumented performance RTL (build 0x20010011)."
}

foreach m05_tb_name {m05_perf_tb_pkg.sv m05_axi_memory_models.sv m05_perf_unit_test.sv tb_m05_perf.sv} {
    set m05_tb_path [file join $m05_tb_root tb $m05_tb_name]
    if {![file isfile $m05_tb_path]} { error "Missing test source: $m05_tb_path" }
    if {[llength [get_files -quiet $m05_tb_path]] == 0} {
        add_files -fileset sim_1 -norecurse $m05_tb_path
    }
    set m05_tb_obj [get_files $m05_tb_path]
    set_property FILE_TYPE SystemVerilog $m05_tb_obj
    set_property USED_IN_SYNTHESIS false $m05_tb_obj
    set_property USED_IN_IMPLEMENTATION false $m05_tb_obj
    set_property USED_IN_SIMULATION true $m05_tb_obj
}
set_property TARGET_SIMULATOR XSim [current_project]
set_property TOP tb_m05_perf [get_filesets sim_1]
# Avoid the default 1000 ns stopping before even the first integrated job ends.
# The testbench normally finishes much earlier; its own timeout is 20 ms.
set_property xsim.simulate.runtime {21ms} [get_filesets sim_1]
update_compile_order -fileset sim_1
puts "M05: added 8 counter unit cases and 13 integrated M05 jobs."
puts "M05: simulation top = tb_m05_perf; design top remains embedding_v2."
puts "M05: no compile, simulation or synthesis has been started by this script."
puts "M05: now select Run Simulation > Run Behavioral Simulation."
