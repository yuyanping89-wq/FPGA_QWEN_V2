# Vivado 2021.1 IP packaging only. No synthesis, simulation or implementation.
# The preparation script copies this file into the self-contained IP directory.
set m05_script_dir [file dirname [file normalize [info script]]]
if {[file tail $m05_script_dir] eq "scripts"} {
    set m05_root [file normalize [file join $m05_script_dir .. ip_repo m05_embedding_v2_2_1]]
} else {
    set m05_root $m05_script_dir
}
if {![string match "2021.1*" [version -short]]} {
    error "This packaging script targets Vivado 2021.1. Current: [version -short]"
}
if {[llength [get_projects -quiet]] != 0} {
    error "Save and close the current Vivado project first (File > Close Project), then source this script again."
}
if {![file isfile [file join $m05_root package_data.tcl]]} {
    error "Prepared sources missing: run prepare_ip_package.py first."
}
if {[file exists [file join $m05_root component.xml]]} {
    error "component.xml already exists. Existing packaged IP will not be overwritten."
}
source [file join $m05_root package_data.tcl]
set m05_stamp [clock format [clock seconds] -format %Y%m%d_%H%M%S]
set m05_work [file join $m05_root _packaging_project $m05_stamp]
create_project m05_ip_packaging $m05_work -part xczu9eg-ffvb1156-2-i
set m05_old_pwd [pwd]

proc m05_bus_parameter {bus name value} {
    set p [ipx::add_bus_parameter $name $bus]
    set_property value $value $p
}
proc m05_signal_interface {core name type mode logical physical} {
    set bus [ipx::add_bus_interface $name $core]
    set_property bus_type_vlnv xilinx.com:signal:${type}:1.0 $bus
    set_property abstraction_type_vlnv xilinx.com:signal:${type}_rtl:1.0 $bus
    set_property interface_mode $mode $bus
    set_property physical_name $physical [ipx::add_port_map $logical $bus]
    return $bus
}

# create_core writes component.xml relative to the current directory, matching
# Vivado 2021.1's own scripts/ip/ipxhls.tcl packaging convention.
set m05_code [catch {
    cd $m05_root
    set m05_core [ipx::create_core llm-fpga.local user m05_embedding_v2 2.1]
    set_property display_name {M05 Token Embedding V2 with CRC and AXI Performance Counters} $m05_core
    set_property description {Fixed 151936 x 896 BF16 embedding lookup. ABI 2, build 0x30010011. CRC-32 and AXI handshake counters are implemented. Compute-cycle signal is unavailable and reports zero.} $m05_core
    set_property core_revision 3 $m05_core
    set_property taxonomy {/UserIP} $m05_core
    set_property supported_families {zynquplus Production} $m05_core

    foreach group_type {xilinx_verilogsynthesis xilinx_verilogbehavioralsimulation} {
        set group [ipx::add_file_group $group_type $m05_core]
        foreach f [lsort [glob hdl/*.v]] {
            set item [ipx::add_file $f $group]
            set_property type verilogSource $item
        }
        set_property model_name m05_embedding_v2_ip $group
    }
    # Driver files are distributed as sources, not advertised as a verified
    # automatically generated BSP driver for this newly named IP.
    set group [ipx::add_file_group xilinx_miscfiles $m05_core]
    foreach f [concat [glob drivers/*] {core_manifest.json module_manifest.json package_data.tcl package_m05_ip.tcl SHA256SUMS.prepared}] {
        ipx::add_file $f $group
    }
    set group [ipx::add_file_group xilinx_documentation $m05_core]
    ipx::add_file README.md $group

    foreach spec $m05_ports {
        lassign $spec name direction width vector
        set port [ipx::add_port $name $m05_core]
        set_property direction [expr {$direction eq "input" ? "in" : "out"}] $port
        set_property is_vector [expr {$vector ? "true" : "false"}] $port
        if {$vector} {
            set_property type_name std_logic_vector $port
            set_property size_left [expr {$width - 1}] $port
            set_property size_right 0 $port
        } else {
            set_property type_name std_logic $port
        }
    }

    foreach spec {{m_axi_ids 32 64 master AXI4} {m_axi_weight 128 64 master AXI4} {m_axi_out_r 128 64 master AXI4} {s_axi_ctrl 32 9 slave AXI4LITE}} {
        lassign $spec name data_width address_width mode protocol
        set bus [ipx::add_bus_interface $name $m05_core]
        set_property bus_type_vlnv xilinx.com:interface:aximm:1.0 $bus
        set_property abstraction_type_vlnv xilinx.com:interface:aximm_rtl:1.0 $bus
        set_property interface_mode $mode $bus
        set_property endianness little $bus
        m05_bus_parameter $bus PROTOCOL $protocol
        m05_bus_parameter $bus DATA_WIDTH $data_width
        m05_bus_parameter $bus ADDR_WIDTH $address_width
        m05_bus_parameter $bus READ_WRITE_MODE READ_WRITE
        foreach port_spec $m05_ports {
            set physical [lindex $port_spec 0]
            if {[string match ${name}_* $physical]} {
                set logical [string range $physical [expr {[string length $name] + 1}] end]
                set_property physical_name $physical [ipx::add_port_map $logical $bus]
            }
        }
        if {$mode eq "master"} {
            m05_bus_parameter $bus ID_WIDTH 1
            # Preserve the generated adapter limits, including unused channels.
            if {$name eq "m_axi_ids"} {
                set burst 16
                set read_outstanding 1
                set write_outstanding 16
            } elseif {$name eq "m_axi_weight"} {
                set burst 64
                set read_outstanding 1
                set write_outstanding 16
            } else {
                set burst 64
                set read_outstanding 16
                set write_outstanding 1
            }
            m05_bus_parameter $bus MAX_BURST_LENGTH $burst
            m05_bus_parameter $bus NUM_READ_OUTSTANDING $read_outstanding
            m05_bus_parameter $bus NUM_WRITE_OUTSTANDING $write_outstanding
            foreach channel {AW AR W R B} {
                m05_bus_parameter $bus ${channel}USER_WIDTH 0
            }
            set address_space [ipx::add_address_space Data_$name $m05_core]
            set_property range 16E $address_space
            set_property width 64 $address_space
            set_property master_address_space_ref Data_$name $bus
        } else {
            set map [ipx::add_memory_map $name $m05_core]
            set block [ipx::add_address_block Reg $map]
            set_property base_address 0 $block
            set_property range 512 $block
            set_property width 32 $block
            set_property usage register $block
            set_property access read-write $block
            set_property slave_memory_map_ref $name $bus
            foreach reg_spec $m05_registers {
                lassign $reg_spec reg_name offset bits access
                set reg [ipx::add_register $reg_name $block]
                set_property address_offset $offset $reg
                set_property size $bits $reg
                set_property access $access $reg
                # Detailed bit semantics (COR/COH/TOW) are in xembedding_v2_hw.h.
                set_property description {See drivers/xembedding_v2_hw.h for bit-level read/write side effects.} $reg
            }
        }
    }

    set bus [m05_signal_interface $m05_core ap_clk clock slave CLK ap_clk]
    m05_bus_parameter $bus ASSOCIATED_BUSIF {m_axi_ids:m_axi_weight:m_axi_out_r:s_axi_ctrl}
    m05_bus_parameter $bus ASSOCIATED_RESET ap_rst_n
    # Input clock frequency is propagated from the connected source; timing target remains 10 ns.
    set bus [m05_signal_interface $m05_core ap_rst_n reset slave RST ap_rst_n]
    m05_bus_parameter $bus POLARITY ACTIVE_LOW
    set bus [m05_signal_interface $m05_core interrupt interrupt master INTERRUPT interrupt]
    m05_bus_parameter $bus SENSITIVITY LEVEL_HIGH

    ipx::create_xgui_files $m05_core
    ipx::update_checksums $m05_core
    ipx::check_integrity $m05_core
    ipx::save_core $m05_core
    if {![file isfile component.xml]} { error "Packager did not create component.xml" }
    set_property ip_repo_paths [list [file dirname $m05_root]] [current_project]
    update_ip_catalog
    set definitions [get_ipdefs -all llm-fpga.local:user:m05_embedding_v2:2.1]
    if {[llength $definitions] != 1} {
        error "component.xml exists but the IP catalog did not resolve the expected VLNV uniquely."
    }
    set record [open packaging_run.txt w]
    puts $record "Vivado: [version -short]"
    puts $record "Created: [clock format [clock seconds]]"
    puts $record "VLNV: llm-fpga.local:user:m05_embedding_v2:2.1"
    puts $record "component.xml saved and catalog entry found. Inspect console integrity diagnostics."
    puts $record "No simulation/synthesis/implementation/board test performed by this script."
    close $record
    puts "M05 IP PACKAGE CREATED"
    puts "Component: [file join $m05_root component.xml]"
    puts "IP Catalog: M05 Token Embedding V2 with CRC and AXI Performance Counters"
    puts "Review all integrity warnings/errors; this message is not functional or timing signoff."
} m05_result m05_options]
cd $m05_old_pwd
if {$m05_code} {
    return -options $m05_options $m05_result
}
