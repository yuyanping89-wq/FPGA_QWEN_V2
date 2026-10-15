`timescale 1ns/1ps
module tb_m05_perf;
    import m05_perf_tb_pkg::*;
    reg clk = 0;
    always #5 clk = !clk;
    reg rst_n = 0;
    wire unit_finished;
    m05_perf_unit_test unit_suite(.clk(clk), .finished(unit_finished));

    reg [2047:0] ids_flat = 0;
    reg [31:0] gap_cycles = 0, response_delay = 0, allowed_rows = 0;
    reg [31:0] current_job = 0;
    reg [31:0] requested_rows = 0;
    reg ctrl_awvalid = 0, ctrl_wvalid = 0, ctrl_arvalid = 0;
    reg [8:0] ctrl_awaddr = 0, ctrl_araddr = 0;
    reg [31:0] ctrl_wdata = 0;
    reg ctrl_rready = 0;
    wire ctrl_awready, ctrl_wready, ctrl_bvalid, ctrl_arready, ctrl_rvalid;
    wire [31:0] ctrl_rdata;
    wire [1:0] ctrl_rresp, ctrl_bresp;
    wire interrupt;

    wire ids_arvalid, ids_arready, ids_arid, ids_rvalid, ids_rready, ids_rlast, ids_rid;
    wire [63:0] ids_araddr;
    wire [7:0] ids_arlen;
    wire [2:0] ids_arsize;
    wire [1:0] ids_arburst;
    wire [31:0] ids_rdata;
    wire weight_arvalid, weight_arready, weight_arid, weight_rvalid, weight_rready;
    wire weight_rlast, weight_rid;
    wire [63:0] weight_araddr;
    wire [7:0] weight_arlen;
    wire [2:0] weight_arsize;
    wire [1:0] weight_arburst;
    wire [127:0] weight_rdata;
    wire out_awvalid, out_awready, out_awid, out_wvalid, out_wready, out_wlast;
    wire out_bvalid, out_bready, out_bid;
    wire [63:0] out_awaddr;
    wire [7:0] out_awlen;
    wire [2:0] out_awsize;
    wire [1:0] out_awburst;
    wire [127:0] out_wdata;
    wire [15:0] out_wstrb;
    wire unused_ids_awvalid, unused_ids_wvalid, unused_weight_awvalid;
    wire unused_weight_wvalid, unused_out_arvalid;

    // Every input of the DUT is driven. Unused write/read directions are tied
    // off; their request outputs are monitored so illegal traffic cannot hide.
    embedding_v2 dut (
        .ap_clk(clk), .ap_rst_n(rst_n),
        .s_axi_ctrl_AWVALID(ctrl_awvalid), .s_axi_ctrl_AWREADY(ctrl_awready),
        .s_axi_ctrl_AWADDR(ctrl_awaddr), .s_axi_ctrl_WVALID(ctrl_wvalid),
        .s_axi_ctrl_WREADY(ctrl_wready), .s_axi_ctrl_WDATA(ctrl_wdata),
        .s_axi_ctrl_WSTRB(4'hf), .s_axi_ctrl_BVALID(ctrl_bvalid),
        .s_axi_ctrl_BREADY(1'b1), .s_axi_ctrl_BRESP(ctrl_bresp),
        .s_axi_ctrl_ARVALID(ctrl_arvalid), .s_axi_ctrl_ARREADY(ctrl_arready),
        .s_axi_ctrl_ARADDR(ctrl_araddr), .s_axi_ctrl_RVALID(ctrl_rvalid),
        .s_axi_ctrl_RREADY(ctrl_rready), .s_axi_ctrl_RDATA(ctrl_rdata),
        .s_axi_ctrl_RRESP(ctrl_rresp), .interrupt(interrupt),

        .m_axi_ids_ARVALID(ids_arvalid), .m_axi_ids_ARREADY(ids_arready),
        .m_axi_ids_ARADDR(ids_araddr), .m_axi_ids_ARLEN(ids_arlen),
        .m_axi_ids_ARSIZE(ids_arsize), .m_axi_ids_ARBURST(ids_arburst),
        .m_axi_ids_ARID(ids_arid), .m_axi_ids_RVALID(ids_rvalid),
        .m_axi_ids_RREADY(ids_rready), .m_axi_ids_RDATA(ids_rdata),
        .m_axi_ids_RLAST(ids_rlast), .m_axi_ids_RID(ids_rid),
        .m_axi_ids_RRESP(2'b00), .m_axi_ids_RUSER(1'b0),
        .m_axi_ids_AWREADY(1'b0), .m_axi_ids_WREADY(1'b0),
        .m_axi_ids_BVALID(1'b0), .m_axi_ids_BRESP(2'b00),
        .m_axi_ids_BID(1'b0), .m_axi_ids_BUSER(1'b0),
        .m_axi_ids_AWVALID(unused_ids_awvalid), .m_axi_ids_WVALID(unused_ids_wvalid),

        .m_axi_weight_ARVALID(weight_arvalid), .m_axi_weight_ARREADY(weight_arready),
        .m_axi_weight_ARADDR(weight_araddr), .m_axi_weight_ARLEN(weight_arlen),
        .m_axi_weight_ARSIZE(weight_arsize), .m_axi_weight_ARBURST(weight_arburst),
        .m_axi_weight_ARID(weight_arid), .m_axi_weight_RVALID(weight_rvalid),
        .m_axi_weight_RREADY(weight_rready), .m_axi_weight_RDATA(weight_rdata),
        .m_axi_weight_RLAST(weight_rlast), .m_axi_weight_RID(weight_rid),
        .m_axi_weight_RRESP(2'b00), .m_axi_weight_RUSER(1'b0),
        .m_axi_weight_AWREADY(1'b0), .m_axi_weight_WREADY(1'b0),
        .m_axi_weight_BVALID(1'b0), .m_axi_weight_BRESP(2'b00),
        .m_axi_weight_BID(1'b0), .m_axi_weight_BUSER(1'b0),
        .m_axi_weight_AWVALID(unused_weight_awvalid), .m_axi_weight_WVALID(unused_weight_wvalid),

        .m_axi_out_r_AWVALID(out_awvalid), .m_axi_out_r_AWREADY(out_awready),
        .m_axi_out_r_AWADDR(out_awaddr), .m_axi_out_r_AWLEN(out_awlen),
        .m_axi_out_r_AWSIZE(out_awsize), .m_axi_out_r_AWBURST(out_awburst),
        .m_axi_out_r_AWID(out_awid), .m_axi_out_r_WVALID(out_wvalid),
        .m_axi_out_r_WREADY(out_wready), .m_axi_out_r_WDATA(out_wdata),
        .m_axi_out_r_WSTRB(out_wstrb), .m_axi_out_r_WLAST(out_wlast),
        .m_axi_out_r_BVALID(out_bvalid), .m_axi_out_r_BREADY(out_bready),
        .m_axi_out_r_BID(out_bid), .m_axi_out_r_BRESP(2'b00), .m_axi_out_r_BUSER(1'b0),
        .m_axi_out_r_ARREADY(1'b0), .m_axi_out_r_RVALID(1'b0),
        .m_axi_out_r_RDATA(128'd0), .m_axi_out_r_RLAST(1'b0),
        .m_axi_out_r_RID(1'b0), .m_axi_out_r_RUSER(1'b0), .m_axi_out_r_RRESP(2'b00),
        .m_axi_out_r_ARVALID(unused_out_arvalid)
    );

    m05_axi_read_model #(.DATA_BITS(32), .IS_TABLE(0), .BASE(IDS_BASE), .SPAN(64'd256))
    ids_memory (
        .clk(clk), .rst_n(rst_n), .gap_cycles(gap_cycles), .token_ids(ids_flat),
        .arvalid(ids_arvalid), .arready(ids_arready), .araddr(ids_araddr),
        .arlen(ids_arlen), .arsize(ids_arsize), .arburst(ids_arburst), .arid(ids_arid),
        .rvalid(ids_rvalid), .rready(ids_rready), .rdata(ids_rdata), .rlast(ids_rlast), .rid(ids_rid)
    );
    m05_axi_read_model #(.DATA_BITS(128), .IS_TABLE(1), .BASE(TABLE_BASE), .SPAN(64'd272269312))
    table_memory (
        .clk(clk), .rst_n(rst_n), .gap_cycles(gap_cycles), .token_ids(ids_flat),
        .arvalid(weight_arvalid), .arready(weight_arready), .araddr(weight_araddr),
        .arlen(weight_arlen), .arsize(weight_arsize), .arburst(weight_arburst), .arid(weight_arid),
        .rvalid(weight_rvalid), .rready(weight_rready), .rdata(weight_rdata),
        .rlast(weight_rlast), .rid(weight_rid)
    );
    m05_axi_write_model output_memory (
        .clk(clk), .rst_n(rst_n), .gap_cycles(gap_cycles), .response_delay(response_delay),
        .allowed_rows(allowed_rows), .awvalid(out_awvalid), .awready(out_awready),
        .awaddr(out_awaddr), .awlen(out_awlen), .awsize(out_awsize), .awburst(out_awburst),
        .awid(out_awid), .wvalid(out_wvalid), .wready(out_wready), .wdata(out_wdata),
        .wstrb(out_wstrb), .wlast(out_wlast), .bvalid(out_bvalid), .bready(out_bready), .bid(out_bid)
    );

    // Independent reference: timestamp subtraction plus observed AXI events.
    // Does not read the DUT's work counters or use its next_* expressions.
    longint unsigned clock_number = 0, start_clock = 0;
    longint unsigned ref_active = 0, ref_ids = 0, ref_table = 0, ref_write = 0;
    longint unsigned expected_total = 0, expected_active = 0;
    longint unsigned expected_ids = 0, expected_table = 0, expected_write = 0;
    bit measuring = 0;
    integer started = 0, completed = 0;
    always @(posedge clk) begin
        clock_number = clock_number + 1;
        if (!rst_n) begin
            measuring = 0; started = 0; completed = 0;
            ref_active = 0; ref_ids = 0; ref_table = 0; ref_write = 0;
            expected_total = 0; expected_active = 0;
            expected_ids = 0; expected_table = 0; expected_write = 0;
        end else begin
            if (unused_ids_awvalid || unused_ids_wvalid || unused_weight_awvalid ||
                unused_weight_wvalid || unused_out_arvalid)
                $fatal(1,"[FAIL] Traffic in an unused AXI direction");
            if (dut.ap_start && dut.ap_CS_fsm_state1) begin
                if (measuring) $fatal(1,"[FAIL] Unexpected overlapping job start");
                measuring = 1; started++;
                start_clock = clock_number;
                ref_active = 0; ref_ids = 0; ref_table = 0; ref_write = 0;
            end
            if (measuring) begin
                if (ids_rvalid && ids_rready) ref_ids++;
                if (weight_rvalid && weight_rready) ref_table++;
                if (out_wvalid && out_wready) ref_write++;
                if ((ids_rvalid && ids_rready) || (weight_rvalid && weight_rready) ||
                    (out_wvalid && out_wready)) ref_active++;
                if (dut.ap_done) begin
                    expected_total = clock_number - start_clock + 1;
                    expected_active = ref_active; expected_ids = ref_ids;
                    expected_table = ref_table; expected_write = ref_write;
                    if (dut.m05_perf_snapshot_valid !== 1'b1 ||
                        dut.m05_perf_snapshot_job_id !== current_job ||
                        dut.m05_measured_cycles_total !== expected_total ||
                        dut.m05_measured_cycles_compute !== expected_active ||
                        dut.m05_measured_read_beats !== (expected_ids+expected_table) ||
                        dut.m05_measured_write_beats !== expected_write)
                        $fatal(1,"[FAIL] Counters incorrect on the completion edge, job=%0d",current_job);
                    measuring = 0; completed++;
                    #1;
                    if (dut.ctrl_s_axi_U.int_result_perf_cycles_total !== expected_total ||
                        dut.ctrl_s_axi_U.int_result_perf_cycles_compute !== expected_active ||
                        dut.ctrl_s_axi_U.int_result_perf_read_beats !== (expected_ids+expected_table) ||
                        dut.ctrl_s_axi_U.int_result_perf_write_beats !== expected_write)
                        $fatal(1,"[FAIL] Counter bank was not captured on the done edge");
                end
            end else if ((ids_rvalid && ids_rready) || (weight_rvalid && weight_rready) ||
                         (out_wvalid && out_wready))
                $fatal(1,"[FAIL] AXI data transfer outside job window");
        end
    end

    // AXI-Lite BFM. Drive on falling edges; sample on rising edges.
    // AW and W deliberately occur in separate phases, as permitted by AXI-Lite.
    task automatic write32(input logic [8:0] address, input logic [31:0] value);
        integer n;
        @(negedge clk); ctrl_awaddr = address; ctrl_awvalid = 1;
        for (n=0;n<1000;n++) begin @(posedge clk); if (ctrl_awready === 1'b1) break; end
        if (n==1000) $fatal(1,"[FAIL] AXI-Lite AW timeout address=%h",address);
        @(negedge clk); ctrl_awvalid = 0; ctrl_wdata = value; ctrl_wvalid = 1;
        for (n=0;n<1000;n++) begin @(posedge clk); if (ctrl_wready === 1'b1) break; end
        if (n==1000) $fatal(1,"[FAIL] AXI-Lite W timeout address=%h",address);
        @(negedge clk); ctrl_wvalid = 0;
        for (n=0;n<1000;n++) begin @(posedge clk); if (ctrl_bvalid === 1'b1) break; end
        if (n==1000 || ctrl_bresp !== 2'b00) $fatal(1,"[FAIL] AXI-Lite B response");
    endtask
    task automatic read32(input logic [8:0] address, output logic [31:0] value);
        integer n;
        @(negedge clk); ctrl_araddr = address; ctrl_arvalid = 1; ctrl_rready = 0;
        for (n=0;n<1000;n++) begin @(posedge clk); if (ctrl_arready === 1'b1) break; end
        if (n==1000) $fatal(1,"[FAIL] AXI-Lite AR timeout address=%h",address);
        @(negedge clk); ctrl_arvalid = 0; ctrl_rready = 1;
        for (n=0;n<1000;n++) begin @(posedge clk); if (ctrl_rvalid === 1'b1) break; end
        if (n==1000 || ctrl_rresp !== 2'b00) $fatal(1,"[FAIL] AXI-Lite R response");
        value = ctrl_rdata;
        @(negedge clk); ctrl_rready = 0;
    endtask
    task automatic read64(input logic [8:0] address, output logic [63:0] value);
        logic [31:0] low_word, high_word;
        read32(address,low_word); read32(address+9'd4,high_word);
        value = {high_word,low_word};
    endtask
    task automatic reset_core;
        @(negedge clk); rst_n = 0;
        ctrl_awvalid = 0; ctrl_wvalid = 0; ctrl_arvalid = 0; ctrl_rready = 0;
        repeat (6) @(posedge clk);
        @(negedge clk); rst_n = 1;
        repeat (6) @(posedge clk);
    endtask
    task automatic fill_ids;
        logic [31:0] seed;
        seed = 32'd20210929;
        for (int row=0;row<64;row++) begin
            seed = seed * 32'd1664525 + 32'd1013904223;
            ids_flat[row*32 +: 32] = seed % VOCAB;
        end
        ids_flat[0 +: 32] = VOCAB-1;
        ids_flat[32 +: 32] = 0;
        ids_flat[64 +: 32] = 42;
        ids_flat[96 +: 32] = 42;
        ids_flat[63*32 +: 32] = VOCAB-1;
    endtask

    integer passed_jobs = 0;
    logic [63:0] last_total, fast_total;
    task automatic run_job(input string name, input logic [31:0] rows, mode, job,
                           input logic [31:0] expected_status, prefix_rows,
                           input logic [31:0] gap, b_delay);
        logic [31:0] value;
        logic [63:0] total, active, reads, writes, again;
        logic [15:0] expected_element;
        integer before_done, n;
        $display("[RUN][M05] %s",name);
        @(negedge clk);
        current_job = job; requested_rows = rows; allowed_rows = prefix_rows;
        gap_cycles = gap; response_delay = b_delay;
        for (int w=0;w<OUT_WORDS;w++) begin
            output_memory.memory[w] = SENTINEL; output_memory.seen[w] = 0;
        end
        before_done = completed;
        write32(9'h000,32'd0); // Disable auto_restart; each job has an explicit start.
        write32(9'h010,IDS_BASE[31:0]); write32(9'h014,IDS_BASE[63:32]);
        write32(9'h01c,TABLE_BASE[31:0]); write32(9'h020,TABLE_BASE[63:32]);
        write32(9'h028,OUT_BASE[31:0]); write32(9'h02c,OUT_BASE[63:32]);
        write32(9'h034,mode); write32(9'h03c,rows);
        write32(9'h044,HIDDEN); write32(9'h04c,VOCAB);
        write32(9'h054,32'd2); write32(9'h05c,job); write32(9'h064,32'd0);
        write32(9'h000,32'd1);
        for (n=0;n<100000;n++) begin
            read32(9'h000,value);
            if (value[1] === 1'b1) break;
        end
        if (n==100000) $fatal(1,"[FAIL] Job timeout/deadlock: %s",name);
        if (completed != before_done+1 || started != completed)
            $fatal(1,"[FAIL] Incorrect job start/done count: %s",name);
        read32(9'h06c,value);
        if (value !== expected_status) $fatal(1,"[FAIL] %s status=%0d expected=%0d",name,value,expected_status);
        read32(9'h07c,value);
        if (value !== prefix_rows) $fatal(1,"[FAIL] %s processed_rows=%0d expected=%0d",name,value,prefix_rows);
        read32(9'h0fc,value);
        if (value !== 32'h20010011) $fatal(1,"[FAIL] Wrong RTL build_config=%h; use instrumented RTL",value);
        read64(9'h08c,total); read64(9'h0a4,active);
        read64(9'h0bc,reads); read64(9'h0d4,writes);
        if (total !== expected_total || active !== expected_active ||
            reads !== (expected_ids+expected_table) || writes !== expected_write)
            $fatal(1,"[FAIL] AXI-Lite counters disagree with independent bus observer: %s",name);
        if (total == 0 || active > total) $fatal(1,"[FAIL] Invalid cycle counts");
        if (expected_table != prefix_rows*ROW_WORDS || expected_write != prefix_rows*ROW_WORDS)
            $fatal(1,"[FAIL] Wrong table/output traffic count: %s",name);
        if (expected_status == 2) begin
            if (reads != 0 || writes != 0 || active != 0)
                $fatal(1,"[FAIL] Rejected parameters generated data traffic");
        end else begin
            if (expected_ids < prefix_rows + ((expected_status == 7) ? 1 : 0) || expected_ids > rows)
                $fatal(1,"[FAIL] ID traffic outside expected range");
        end
        for (int row=0;row<prefix_rows;row++) begin
            for (int h=0;h<HIDDEN;h++) begin
                expected_element = bf16_pattern(ids_flat[row*32 +: 32],h);
                if (output_memory.memory[row*ROW_WORDS+h/8][(h%8)*16 +: 16] !== expected_element)
                    $fatal(1,"[FAIL] %s output mismatch row=%0d hidden=%0d",name,row,h);
            end
        end
        for (int w=0;w<OUT_WORDS;w++) begin
            if (w < prefix_rows*ROW_WORDS) begin
                if (output_memory.seen[w] !== 1'b1) $fatal(1,"[FAIL] Missing output write");
            end else if (output_memory.seen[w] !== 1'b0 || output_memory.memory[w] !== SENTINEL)
                $fatal(1,"[FAIL] Output beyond valid prefix changed");
        end
        // Check all four valid bits and their clear-on-read behavior.
        for (int field=0;field<4;field++) begin
            read32(9'h094+field*9'h018,value);
            if (value[0] !== 1'b1) $fatal(1,"[FAIL] Missing counter valid bit");
            read32(9'h094+field*9'h018,value);
            if (value[0] !== 1'b0) $fatal(1,"[FAIL] Counter valid bit did not clear");
        end
        repeat (8) @(posedge clk);
        read64(9'h08c,again);
        if (again !== total) $fatal(1,"[FAIL] Completed snapshot did not remain stable");
        last_total = total;
        passed_jobs++;
        $display("[PASS][M05] %s total=%0d active=%0d reads=%0d (ids=%0d table=%0d) writes=%0d",
                 name,total,active,reads,expected_ids,expected_table,writes);
    endtask

    initial begin : integration_suite
        logic [63:0] cleared;
        $display("[INFO] M05 performance RTL test: 8 unit cases + 13 integrated jobs.");
        $display("[INFO] Full logical vocabulary, generated read data, high 64-bit addresses; no table files.");
        reset_core();
        ids_flat = 0; ids_flat[0 +: 32] = VOCAB-1;
        run_job("decode baseline",1,1,100,0,1,0,0);
        fast_total = last_total;
        run_job("same decode with AXI waits",1,1,101,0,1,5,11);
        if (last_total <= fast_total) $fatal(1,"[FAIL] Injected waits did not increase total cycles");
        fill_ids();
        run_job("prefill T=64 boundaries and repeats",64,0,102,0,64,0,0);
        run_job("prefill T=36 with waits",36,0,103,0,36,1,3);
        ids_flat[0 +: 32] = 32'hffffffff;
        run_job("bad first ID",3,0,104,7,0,1,0);
        fill_ids(); ids_flat[63*32 +: 32] = VOCAB;
        run_job("bad last ID at row 63",64,0,105,7,63,0,0);
        ids_flat = 0; ids_flat[32 +: 32] = VOCAB; ids_flat[64 +: 32] = 42;
        run_job("bad middle ID",3,0,106,7,1,2,3);
        fill_ids();
        run_job("prefill after three token errors",64,0,107,0,64,0,0);
        run_job("reject valid_rows=0",0,0,108,2,0,0,0);
        run_job("reject valid_rows=65",65,0,109,2,0,0,0);
        run_job("reject decode T=2",2,1,110,2,0,0,0);
        ids_flat[0 +: 32] = 42;
        run_job("decode after three parameter errors",1,1,111,0,1,1,4);
        reset_core();
        read64(9'h08c,cleared);
        if (cleared !== 0) $fatal(1,"[FAIL] Counter bank not cleared by reset");
        read64(9'h0a4,cleared);
        if (cleared !== 0) $fatal(1,"[FAIL] Active counter not cleared by reset");
        read64(9'h0bc,cleared);
        if (cleared !== 0) $fatal(1,"[FAIL] Read counter not cleared by reset");
        read64(9'h0d4,cleared);
        if (cleared !== 0) $fatal(1,"[FAIL] Write counter not cleared by reset");
        ids_flat[0 +: 32] = 0;
        run_job("prefill T=1 after full reset",1,0,112,0,1,1,1);
        if (passed_jobs != 13) $fatal(1,"[FAIL] Wrong integrated case count");
        wait (unit_finished === 1'b1);
        $display("ALL 13 M05 INTEGRATION CASES PASSED");
        $display("M05 PERFORMANCE RTL TEST PASSED");
        $finish;
    end

    initial begin
        #20000000; // 20 ms of simulated time; catches hangs independently of PS polling.
        $fatal(1,"[FAIL] Global simulation timeout");
    end
endmodule
