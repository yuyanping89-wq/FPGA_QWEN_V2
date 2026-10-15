# Configure and build the existing qwen.xpr project with the checked-in
# AXU9EG integrated V2 modules + AXI CDMA Block Design.
# Run with Vivado 2021.1 batch mode from any working directory:
#   vivado.bat -mode batch -source vivado/proj/qwen/build_qwen.tcl

set script_dir [file dirname [file normalize [info script]]]
set vivado_dir [file normalize [file join $script_dir .. ..]]
set workspace_dir [file normalize [file join $vivado_dir ..]]

set project_name qwen
set project_file [file join $script_dir ${project_name}.xpr]
set bd_template_file [file join $vivado_dir src bd system_wrapper.bd]
set bd_file [file join $script_dir ${project_name}.srcs sources_1 bd system_wrapper system_wrapper.bd]
set xdc_file [file join $vivado_dir src constraints acu9eg_plddr4.xdc]
set ip_repo_m06 [file join $vivado_dir ip_repo m06_rmsnorm]
set ip_repo_m09 [file join $vivado_dir ip_repo m09_kv_cache_update]
set ip_repo_m13 [file join $vivado_dir ip_repo m13_swiglu]
set xsa_file [file join $script_dir ${project_name}.xsa]
set timing_file [file join $script_dir ${project_name}_timing_summary_routed.rpt]
set drc_file [file join $script_dir ${project_name}_drc_routed.rpt]

# Check that a required design input exists before modifying the project.
# Arguments: path is the file to check; description is the readable item name.
proc require_file {path description} {
  if {![file exists $path] || [file isdirectory $path]} {
    error "Required $description is missing: $path"
  }
}

require_file $project_file "existing Vivado project"
require_file $bd_template_file "canonical legacy M06/M09 AXI CDMA Block Design"
require_file $bd_file "project-local M06/M09/M13 AXI CDMA Block Design"
require_file $xdc_file "AXU9EG PL DDR4 constraints"
require_file [file join $ip_repo_m06 component.xml] "M06 packaged HLS IP"
require_file [file join $ip_repo_m09 component.xml] "M09 packaged HLS IP"
require_file [file join $ip_repo_m13 component.xml] "M13 packaged HLS IP"

open_project $project_file

set expected_part xczu9eg-ffvb1156-2-i
set project_part [get_property PART [current_project]]
if {$project_part ne $expected_part} {
  error "Project part mismatch: expected $expected_part, found $project_part"
}

set_property target_language Verilog [current_project]
source [file join $vivado_dir scripts qwen_ip_repositories.tcl]
set_property ip_repo_paths $qwen_ip_repositories [current_project]
update_ip_catalog -rebuild

# Replace a prior reference to the shared template with the project-local BD.
# This keeps all generated BD/IP outputs inside qwen.gen, not in another
# Vivado project's output directory.
set template_obj [get_files -quiet -all $bd_template_file]
if {[llength $template_obj] > 0} {
  remove_files $template_obj
}

# Add the project-local sources only when they are not already referenced by
# the project, so this script can be rerun after a failed build without duplicates.
set bd_obj [get_files -quiet -all $bd_file]
if {[llength $bd_obj] == 0} {
  add_files -norecurse $bd_file
  set bd_obj [get_files -quiet -all $bd_file]
}
if {[llength $bd_obj] != 1} {
  error "Expected exactly one Block Design source, found [llength $bd_obj]"
}

set xdc_obj [get_files -quiet -all $xdc_file]
if {[llength $xdc_obj] == 0} {
  add_files -fileset constrs_1 -norecurse $xdc_file
}

open_bd_design $bd_obj
validate_bd_design
save_bd_design
generate_target all $bd_obj

set wrapper_obj [get_files -quiet -all *system_wrapper_wrapper.v]
if {[llength $wrapper_obj] == 0} {
  make_wrapper -files $bd_obj -top -import
}
set wrapper_obj [get_files -quiet -all *system_wrapper_wrapper.v]
if {[llength $wrapper_obj] != 1} {
  error "Expected exactly one Block Design wrapper source, found [llength $wrapper_obj]"
}
set_property top system_wrapper_wrapper [current_fileset]
update_compile_order -fileset sources_1

# Rebuild both runs from the current BD and constraints rather than reusing
# stale results from an earlier project state.
reset_run synth_1
launch_runs synth_1 -jobs 8
wait_on_run synth_1
set synth_status [get_property STATUS [get_runs synth_1]]
puts "SYNTH_STATUS=$synth_status"
if {![string match {*Complete*} $synth_status]} {
  error "Synthesis did not complete successfully: $synth_status"
}

reset_run impl_1
launch_runs impl_1 -to_step write_bitstream -jobs 8
wait_on_run impl_1
set impl_status [get_property STATUS [get_runs impl_1]]
puts "IMPL_STATUS=$impl_status"
if {![string match {*write_bitstream Complete*} $impl_status]} {
  error "Implementation/bitstream did not complete successfully: $impl_status"
}

open_run impl_1
report_timing_summary -file $timing_file -warn_on_violation
report_drc -file $drc_file

# Preserve an earlier hardware platform before replacing it, so the same
# build script remains usable for iterative development and the previous XSA
# is recoverable if the newly exported platform needs comparison.
if {[file exists $xsa_file]} {
  set stamp [clock format [clock seconds] -format %Y%m%d_%H%M%S]
  set backup_file [file join $script_dir ${project_name}_${stamp}.xsa]
  set backup_index 1
  while {[file exists $backup_file]} {
    set backup_file [file join $script_dir ${project_name}_${stamp}_${backup_index}.xsa]
    incr backup_index
  }
  file rename $xsa_file $backup_file
  puts "PREVIOUS_XSA_BACKUP=$backup_file"
}

write_hw_platform -fixed -include_bit -file $xsa_file

puts "PROJECT_FILE=$project_file"
puts "BLOCK_DESIGN=$bd_file"
puts "BITSTREAM=[file join $script_dir qwen.runs impl_1 system_wrapper_wrapper.bit]"
puts "TIMING_REPORT=$timing_file"
puts "DRC_REPORT=$drc_file"
puts "XSA_FILE=$xsa_file"
close_project
