`timescale 1ns/1ps
// Behavioral models, simulation only. One outstanding burst per port matches
// the core configuration. These are not a general-purpose AXI VIP or DDR model.
module m05_axi_read_model #(
    parameter integer DATA_BITS = 32,
    parameter integer IS_TABLE = 0,
    parameter logic [63:0] BASE = 0,
    parameter logic [63:0] SPAN = 256
) (
    input wire clk, rst_n,
    input wire [31:0] gap_cycles,
    input wire [2047:0] token_ids,
    input wire arvalid,
    output wire arready,
    input wire [63:0] araddr,
    input wire [7:0] arlen,
    input wire [2:0] arsize,
    input wire [1:0] arburst,
    input wire arid,
    output reg rvalid,
    input wire rready,
    output reg [DATA_BITS-1:0] rdata,
    output reg rlast,
    output reg rid
);
    import m05_perf_tb_pkg::*;
    localparam integer BYTES = DATA_BITS / 8;
    reg busy;
    reg [63:0] address;
    reg [8:0] remaining;
    reg [31:0] delay_count;
    reg burst_id;
    integer unsigned index;
    assign arready = rst_n && !busy && !rvalid;

    always @(posedge clk) begin
        if (!rst_n) begin
            busy <= 0; rvalid <= 0; rdata <= 0; rlast <= 0; rid <= 0;
            address <= 0; remaining <= 0; delay_count <= 0; burst_id <= 0;
        end else begin
            if (arvalid && arready) begin
                if (araddr < BASE || araddr + ({56'd0, arlen}+64'd1)*BYTES > BASE+SPAN)
                    $fatal(1, "[FAIL] Read address out of range: table=%0d address=%h len=%0d", IS_TABLE, araddr, arlen);
                if (arburst !== 2'b01 || arsize !== $clog2(BYTES) || araddr % BYTES != 0)
                    $fatal(1, "[FAIL] Unexpected read burst shape");
                if ({1'b0, araddr[11:0]} + (int'(arlen)+1)*BYTES > 4096)
                    $fatal(1, "[FAIL] Read burst crosses 4 KiB boundary");
                busy <= 1; address <= araddr; remaining <= {1'b0, arlen}+9'd1;
                burst_id <= arid; delay_count <= gap_cycles;
            end
            // RVALID and all response fields stay stable until accepted.
            if (rvalid) begin
                if (rready) begin
                    rvalid <= 0;
                    if (remaining == 1) begin
                        busy <= 0; remaining <= 0;
                    end else begin
                        address <= address + BYTES;
                        remaining <= remaining - 1'b1;
                        delay_count <= gap_cycles;
                    end
                end
            end else if (busy) begin
                if (delay_count != 0) delay_count <= delay_count - 1'b1;
                else begin
                    index = (address-BASE) / BYTES;
                    if (IS_TABLE != 0)
                        rdata <= row_word(index / ROW_WORDS, index % ROW_WORDS);
                    else
                        rdata <= token_ids[index*32 +: 32];
                    rid <= burst_id;
                    rlast <= (remaining == 1);
                    rvalid <= 1;
                end
            end
        end
    end
endmodule

module m05_axi_write_model (
    input wire clk, rst_n,
    input wire [31:0] gap_cycles, response_delay,
    input wire [31:0] allowed_rows,
    input wire awvalid,
    output wire awready,
    input wire [63:0] awaddr,
    input wire [7:0] awlen,
    input wire [2:0] awsize,
    input wire [1:0] awburst,
    input wire awid,
    input wire wvalid,
    output wire wready,
    input wire [127:0] wdata,
    input wire [15:0] wstrb,
    input wire wlast,
    output reg bvalid,
    input wire bready,
    output reg bid
);
    import m05_perf_tb_pkg::*;
    // Only output storage is allocated; table words are generated on demand.
    reg [127:0] memory [0:OUT_WORDS-1];
    reg seen [0:OUT_WORDS-1];
    reg busy, response_pending, burst_id;
    reg [63:0] address;
    reg [8:0] remaining;
    reg [31:0] delay_count, response_count;
    reg was_stalled;
    reg [144:0] held_write;
    integer unsigned index;
    assign awready = rst_n && !busy && !response_pending && !bvalid;
    assign wready = rst_n && busy && delay_count == 0;

    always @(posedge clk) begin
        if (!rst_n) begin
            busy <= 0; response_pending <= 0; bvalid <= 0; bid <= 0;
            address <= 0; remaining <= 0; delay_count <= 0; response_count <= 0;
            burst_id <= 0; was_stalled <= 0; held_write <= 0;
        end else begin
            if (was_stalled && (wvalid !== 1'b1 || {wdata,wstrb,wlast} !== held_write))
                $fatal(1, "[FAIL] DUT changed W data/valid while backpressured");
            was_stalled <= wvalid && !wready;
            held_write <= {wdata,wstrb,wlast};
            if (awvalid && awready) begin
                if (awaddr < OUT_BASE || awaddr + ({56'd0,awlen}+64'd1)*16 > OUT_BASE+OUT_WORDS*16)
                    $fatal(1, "[FAIL] Write address outside output buffer");
                if (awaddr + ({56'd0,awlen}+64'd1)*16 > OUT_BASE+allowed_rows*1792)
                    $fatal(1, "[FAIL] Write request extends beyond completed prefix");
                if (awburst !== 2'b01 || awsize !== 3'd4 || awaddr[3:0] !== 4'd0)
                    $fatal(1, "[FAIL] Unexpected write burst shape");
                if ({1'b0,awaddr[11:0]} + (int'(awlen)+1)*16 > 4096)
                    $fatal(1, "[FAIL] Write burst crosses 4 KiB boundary");
                busy <= 1; address <= awaddr; remaining <= {1'b0,awlen}+9'd1;
                burst_id <= awid; delay_count <= gap_cycles;
            end
            if (busy) begin
                if (delay_count != 0) delay_count <= delay_count - 1'b1;
                if (wvalid && wready) begin
                    if (wstrb !== 16'hffff || wlast !== (remaining == 1))
                        $fatal(1, "[FAIL] Incorrect WSTRB or WLAST");
                    index = (address-OUT_BASE)/16;
                    if (seen[index]) $fatal(1, "[FAIL] Duplicate output write at word %0d", index);
                    memory[index] <= wdata;
                    seen[index] <= 1;
                    address <= address + 16;
                    remaining <= remaining - 1'b1;
                    delay_count <= gap_cycles;
                    if (remaining == 1) begin
                        busy <= 0; response_pending <= 1;
                        response_count <= response_delay;
                    end
                end
            end
            if (response_pending) begin
                if (response_count != 0) response_count <= response_count - 1'b1;
                else begin
                    response_pending <= 0; bvalid <= 1; bid <= burst_id;
                end
            end
            if (bvalid && bready) bvalid <= 0;
        end
    end
endmodule
