`timescale 1ns / 1ps
// Synchronous, active-low reset; all observations are in the ap_clk domain.
// start_event is a ONE-CYCLE accepted-start pulse, not the level ap_start.
// Count sampled rising edges from accepted start through done, inclusive.
// One M05 invocation may be active at a time. Reset aborts an active sample.
module m05_perf_counters (
    input  wire        ap_clk,
    input  wire        ap_rst_n,
    input  wire        start_event,
    input  wire        done_event,
    input  wire [31:0] job_id,
    input  wire        ids_rvalid,
    input  wire        ids_rready,
    input  wire        weight_rvalid,
    input  wire        weight_rready,
    input  wire        out_wvalid,
    input  wire        out_wready,
    output wire        snapshot_valid,
    output wire [31:0] snapshot_job_id,
    output wire [63:0] perf_cycles_total,
    output wire [63:0] perf_cycles_compute,
    output wire [63:0] perf_read_beats,
    output wire [63:0] perf_write_beats
);
    reg busy;
    reg [31:0] active_job_id;
    reg [31:0] saved_job_id;
    reg [63:0] total_count, active_count, read_count, write_count;
    reg [63:0] saved_total, saved_active, saved_read, saved_write;

    wire ids_beat = ids_rvalid && ids_rready;
    wire weight_beat = weight_rvalid && weight_rready;
    wire out_beat = out_wvalid && out_wready;
    wire active_beat = ids_beat || weight_beat || out_beat;
    // Explicit widening: simultaneous ID and table reads must add TWO.
    wire [63:0] read_increment = {63'd0, ids_beat} + {63'd0, weight_beat};
    wire begin_job = start_event && !busy;
    wire count_enable = busy || begin_job;

    wire [63:0] next_total = (begin_job ? 64'd0 : total_count) + 64'd1;
    wire [63:0] next_active = (begin_job ? 64'd0 : active_count)
                              + {63'd0, active_beat};
    wire [63:0] next_read = (begin_job ? 64'd0 : read_count) + read_increment;
    wire [63:0] next_write = (begin_job ? 64'd0 : write_count) + {63'd0, out_beat};

    // Present the FINAL value on the same edge on which the AXI-Lite control
    // block latches ap_done. A registered valid pulse one cycle later would
    // allow software to observe done before the new counters were published.
    assign snapshot_valid = ap_rst_n && count_enable && done_event;
    assign snapshot_job_id = snapshot_valid ?
                             (begin_job ? job_id : active_job_id) : saved_job_id;
    assign perf_cycles_total = snapshot_valid ? next_total : saved_total;
    assign perf_cycles_compute = snapshot_valid ? next_active : saved_active;
    assign perf_read_beats = snapshot_valid ? next_read : saved_read;
    assign perf_write_beats = snapshot_valid ? next_write : saved_write;

    always @(posedge ap_clk) begin
        if (!ap_rst_n) begin
            busy <= 1'b0;
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
        end else if (count_enable) begin
            if (begin_job) active_job_id <= job_id;
            total_count <= next_total;
            active_count <= next_active;
            read_count <= next_read;
            write_count <= next_write;
            busy <= !done_event;
            if (done_event) begin
                saved_job_id <= begin_job ? job_id : active_job_id;
                saved_total <= next_total;
                saved_active <= next_active;
                saved_read <= next_read;
                saved_write <= next_write;
            end
        end
    end
endmodule
