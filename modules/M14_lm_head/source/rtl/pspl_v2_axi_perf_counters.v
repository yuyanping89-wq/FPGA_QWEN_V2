`timescale 1ns / 1ps

// Observations must be in the ap_clk domain. Reset is synchronous, active low.
// start_event is one accepted-start pulse, not a level-sensitive ap_start.
// One job may be active. Automatic restart and overlapping jobs are excluded.
// Count every sampled edge from accepted start through done, INCLUSIVE.
// PERF is sampled at accepted start. Flags outside 0xB (including snapshot)
// disable all four results. Other parameter errors may still report the actual
// start-to-error-completion window when PERF was requested with valid flags.
// R/W counters sum native AXI data-channel handshakes across all ports; they
// are not bytes or normalized 128-bit beats. A 32-bit ID beat and a 128-bit
// weight beat each count as one. RLAST, AW/AR and B responses are not counted.
// compute_active is an independent measured compute-activity signal. Tie it
// to zero when that signal is unavailable; do not infer compute from AXI traffic.
module pspl_v2_axi_perf_counters #(
    parameter integer NUM_PORTS = 1
) (
    input  wire                 ap_clk,
    input  wire                 ap_rst_n,
    input  wire                 start_event,
    input  wire                 done_event,
    input  wire [31:0]          job_id,
    input  wire [31:0]          flags,
    input  wire                 compute_active,
    input  wire [NUM_PORTS-1:0] rvalid,
    input  wire [NUM_PORTS-1:0] rready,
    input  wire [NUM_PORTS-1:0] wvalid,
    input  wire [NUM_PORTS-1:0] wready,
    output wire                 snapshot_valid,
    output wire [31:0]          snapshot_job_id,
    output wire [63:0]          perf_cycles_total,
    output wire [63:0]          perf_cycles_compute,
    output wire [63:0]          perf_read_beats,
    output wire [63:0]          perf_write_beats
);
    reg busy;
    reg enabled;
    reg [31:0] active_job_id, saved_job_id;
    reg [63:0] total_count, active_count, read_count, write_count;
    reg [63:0] saved_total, saved_active, saved_read, saved_write;

    reg [63:0] read_increment, write_increment;
    integer port_index;
    always @* begin
        read_increment = 64'd0;
        write_increment = 64'd0;
        for (port_index = 0; port_index < NUM_PORTS; port_index = port_index + 1) begin
            // Widen EACH Boolean before summing simultaneous handshakes.
            read_increment = read_increment + {63'd0, (rvalid[port_index] && rready[port_index])};
            write_increment = write_increment + {63'd0, (wvalid[port_index] && wready[port_index])};
        end
    end

    wire begin_job = start_event && !busy;
    wire job_active = busy || begin_job;
    wire accepted_enable = flags[1] && ((flags & ~32'h0000000B) == 32'd0);
    wire window_enabled = begin_job ? accepted_enable : enabled;
    wire [63:0] next_total = window_enabled ?
        ((begin_job ? 64'd0 : total_count) + 64'd1) : 64'd0;
    wire [63:0] next_active = window_enabled ?
        ((begin_job ? 64'd0 : active_count) + {63'd0, compute_active}) : 64'd0;
    wire [63:0] next_read = window_enabled ?
        ((begin_job ? 64'd0 : read_count) + read_increment) : 64'd0;
    wire [63:0] next_write = window_enabled ?
        ((begin_job ? 64'd0 : write_count) + write_increment) : 64'd0;

    // Present the final next value in the SAME cycle as done_event. Publishing
    // a registered valid pulse one cycle later could expose done before results.
    // Disabled jobs also publish a valid snapshot of zeros to replace old data.
    assign snapshot_valid = ap_rst_n && job_active && done_event;
    assign snapshot_job_id = !ap_rst_n ? 32'd0 :
        (begin_job ? job_id : (snapshot_valid ? active_job_id : saved_job_id));
    assign perf_cycles_total = !ap_rst_n ? 64'd0 :
        (snapshot_valid ? next_total : (begin_job ? 64'd0 : saved_total));
    assign perf_cycles_compute = !ap_rst_n ? 64'd0 :
        (snapshot_valid ? next_active : (begin_job ? 64'd0 : saved_active));
    assign perf_read_beats = !ap_rst_n ? 64'd0 :
        (snapshot_valid ? next_read : (begin_job ? 64'd0 : saved_read));
    assign perf_write_beats = !ap_rst_n ? 64'd0 :
        (snapshot_valid ? next_write : (begin_job ? 64'd0 : saved_write));

    always @(posedge ap_clk) begin
        if (!ap_rst_n) begin
            busy <= 1'b0;
            enabled <= 1'b0;
            active_job_id <= 32'd0;
            saved_job_id <= 32'd0;
            total_count <= 64'd0;
            active_count <= 64'd0;
            read_count <= 64'd0;
            write_count <= 64'd0;
            saved_total <= 64'd0;
            saved_active <= 64'd0;
            saved_read <= 64'd0;
            saved_write <= 64'd0;
        end else if (job_active) begin
            if (begin_job) begin
                active_job_id <= job_id;
                enabled <= accepted_enable;
                saved_job_id <= job_id;
                saved_total <= 64'd0;
                saved_active <= 64'd0;
                saved_read <= 64'd0;
                saved_write <= 64'd0;
            end
            total_count <= next_total;
            active_count <= next_active;
            read_count <= next_read;
            write_count <= next_write;
            busy <= !done_event;
            if (done_event) begin
                enabled <= 1'b0;
                saved_job_id <= begin_job ? job_id : active_job_id;
                saved_total <= next_total;
                saved_active <= next_active;
                saved_read <= next_read;
                saved_write <= next_write;
            end
        end
    end
endmodule
