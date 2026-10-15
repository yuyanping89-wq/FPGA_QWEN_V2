`timescale 1ns/1ps
// Instantiated by tb_m05_perf; does not finish the overall simulation.
module m05_perf_unit_test(input wire clk, output reg finished);
    reg rst_n = 0, start_event = 0, done_event = 0;
    reg [31:0] job_id = 0;
    reg [5:0] bus = 0;
    wire valid;
    wire [31:0] saved_job;
    wire [63:0] total, active, reads, writes;
    integer checks = 0;
    m05_perf_counters dut (
        .ap_clk(clk), .ap_rst_n(rst_n), .start_event(start_event),
        .done_event(done_event), .job_id(job_id),
        .ids_rvalid(bus[5]), .ids_rready(bus[4]),
        .weight_rvalid(bus[3]), .weight_rready(bus[2]),
        .out_wvalid(bus[1]), .out_wready(bus[0]),
        .snapshot_valid(valid), .snapshot_job_id(saved_job),
        .perf_cycles_total(total), .perf_cycles_compute(active),
        .perf_read_beats(reads), .perf_write_beats(writes)
    );

    task automatic tick(input bit start_bit, done_bit,
                        input logic [5:0] handshakes, input logic [31:0] job);
        @(negedge clk);
        start_event = start_bit; done_event = done_bit; bus = handshakes; job_id = job;
        @(posedge clk); #1;
    endtask
    task automatic expect_counts(input string name, input logic [63:0] t,a,r,w,
                                  input logic [31:0] j);
        if ({total,active,reads,writes,saved_job} !== {t,a,r,w,j})
            $fatal(1, "[FAIL][UNIT] %s actual=%0d/%0d/%0d/%0d job=%0d expected=%0d/%0d/%0d/%0d job=%0d",
                   name,total,active,reads,writes,saved_job,t,a,r,w,j);
        checks++;
        $display("[PASS][UNIT] %s", name);
    endtask

    initial begin
        finished = 0;
        repeat (3) @(posedge clk);
        #1; expect_counts("reset clears counters",0,0,0,0,0);
        @(negedge clk); rst_n = 1;
        tick(1,0,6'b000000,10);
        tick(0,0,6'b000000,10);
        tick(0,1,6'b000000,10);
        expect_counts("three cycles of pure waiting",3,0,0,0,10);
        tick(0,0,6'b111111,99);
        tick(0,1,6'b111111,99);
        expect_counts("idle traffic and stray done do not change snapshot",3,0,0,0,10);

        tick(1,0,6'b111111,20); // Two reads and one write, ONE active cycle.
        tick(0,0,6'b101010,999); // VALID without READY: no transfers.
        tick(0,0,6'b010101,999); // READY without VALID: no transfers.
        tick(0,0,6'b110000,999);
        // Check final counts BEFORE the completion edge, as the control bank sees them.
        @(negedge clk); bus = 6'b001111; done_event = 1; start_event = 0;
        #1;
        if (valid !== 1'b1 || total !== 64'd5 || active !== 64'd3 ||
            reads !== 64'd4 || writes !== 64'd2 || saved_job !== 32'd20)
            $fatal(1,"[FAIL][UNIT] Final-edge snapshot is not ready for same-edge capture");
        @(posedge clk); #1;
        expect_counts("stall exclusion, dual read, final edge, latched job_id",5,3,4,2,20);

        tick(1,0,6'b000000,30); // Immediately next accepted job.
        tick(0,1,6'b000011,30);
        expect_counts("next job restarts all counters",2,1,0,1,30);
        tick(1,0,6'b111111,40);
        tick(0,0,6'b111111,40);
        @(negedge clk); rst_n = 0; start_event = 0; done_event = 0; bus = 0;
        @(posedge clk); #1;
        expect_counts("reset aborts active job and clears snapshot",0,0,0,0,0);
        @(negedge clk); rst_n = 1;
        tick(0,1,6'b111111,40);
        expect_counts("aborted completion is ignored",0,0,0,0,0);
        tick(1,0,6'b000000,50);
        tick(0,1,6'b001100,50);
        expect_counts("recovery after reset",2,1,1,0,50);
        tick(0,0,6'b000000,50);
        if (checks != 8) $fatal(1,"[FAIL][UNIT] Wrong unit case count");
        $display("ALL 8 COUNTER UNIT CASES PASSED");
        finished = 1;
    end
endmodule
