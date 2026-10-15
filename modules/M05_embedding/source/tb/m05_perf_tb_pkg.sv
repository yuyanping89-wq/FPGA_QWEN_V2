`timescale 1ns/1ps
package m05_perf_tb_pkg;
    // Test-only address map above 4 GiB. No PS or physical DDR is required.
    localparam logic [63:0] IDS_BASE   = 64'h00000001_00010000;
    localparam logic [63:0] TABLE_BASE = 64'h00000002_00000000;
    localparam logic [63:0] OUT_BASE   = 64'h00000003_00020000;
    localparam int VOCAB = 151936;
    localparam int HIDDEN = 896;
    localparam int ROW_WORDS = 112;
    localparam int OUT_WORDS = 64 * ROW_WORDS;
    localparam logic [127:0] SENTINEL = 128'h5a5a1234_9876abcd_2468ace0_13579bdf;

    function automatic logic [15:0] bf16_pattern(input logic [31:0] token,
                                                 input int unsigned hidden);
        logic [31:0] bits;
        case (hidden)
            0: bf16_pattern = 16'h0000;
            1: bf16_pattern = 16'h8000;
            2: bf16_pattern = 16'h7f80;
            3: bf16_pattern = 16'hff80;
            4: bf16_pattern = 16'h7fc1;
            5: bf16_pattern = 16'h7f81;
            6: bf16_pattern = 16'h0001;
            7: bf16_pattern = 16'h3f80;
            default: begin
                bits = (token * 32'h9e3779b9) ^ (hidden * 32'h85ebca6b);
                bits = bits ^ (bits >> 16);
                bf16_pattern = bits[15:0];
            end
        endcase
    endfunction

    function automatic logic [127:0] row_word(input logic [31:0] token,
                                               input int unsigned word_index);
        for (int lane = 0; lane < 8; lane++)
            row_word[lane*16 +: 16] = bf16_pattern(token, word_index*8 + lane);
    endfunction
endpackage
