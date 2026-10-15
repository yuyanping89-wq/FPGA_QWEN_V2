// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1ns/1ps
module lm_head_v2_control_s_axi
#(parameter
    C_S_AXI_ADDR_WIDTH = 9,
    C_S_AXI_DATA_WIDTH = 32
)(
    input  wire                          ACLK,
    input  wire                          ARESET,
    input  wire                          ACLK_EN,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] AWADDR,
    input  wire                          AWVALID,
    output wire                          AWREADY,
    input  wire [C_S_AXI_DATA_WIDTH-1:0] WDATA,
    input  wire [C_S_AXI_DATA_WIDTH/8-1:0] WSTRB,
    input  wire                          WVALID,
    output wire                          WREADY,
    output wire [1:0]                    BRESP,
    output wire                          BVALID,
    input  wire                          BREADY,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] ARADDR,
    input  wire                          ARVALID,
    output wire                          ARREADY,
    output wire [C_S_AXI_DATA_WIDTH-1:0] RDATA,
    output wire [1:0]                    RRESP,
    output wire                          RVALID,
    input  wire                          RREADY,
    output wire                          interrupt,
    output wire [63:0]                   hidden_offset,
    output wire [63:0]                   shared_vocab_weight,
    output wire [63:0]                   history_bitmap,
    output wire [63:0]                   optional_logits,
    output wire [31:0]                   hidden_size_r,
    output wire [31:0]                   vocab_size,
    output wire [31:0]                   run_mode,
    output wire [31:0]                   valid_rows,
    output wire [31:0]                   write_logits,
    output wire [31:0]                   enable_penalty,
    output wire [31:0]                   repetition_penalty,
    output wire [31:0]                   abi_version,
    output wire [31:0]                   job_id,
    output wire [31:0]                   flags,
    input  wire [31:0]                   selected_token,
    input  wire                          selected_token_ap_vld,
    input  wire [31:0]                   selected_score,
    input  wire                          selected_score_ap_vld,
    input  wire [31:0]                   result_job_id,
    input  wire                          result_job_id_ap_vld,
    input  wire [31:0]                   status_code,
    input  wire                          status_code_ap_vld,
    input  wire [31:0]                   processed_rows,
    input  wire                          processed_rows_ap_vld,
    input  wire [63:0]                   perf_cycles_total,
    input  wire                          perf_cycles_total_ap_vld,
    input  wire [63:0]                   perf_cycles_compute,
    input  wire                          perf_cycles_compute_ap_vld,
    input  wire [63:0]                   perf_read_beats,
    input  wire                          perf_read_beats_ap_vld,
    input  wire [63:0]                   perf_write_beats,
    input  wire                          perf_write_beats_ap_vld,
    input  wire [31:0]                   output_crc,
    input  wire                          output_crc_ap_vld,
    input  wire [31:0]                   build_config,
    input  wire                          build_config_ap_vld,
    input  wire [31:0]                   capabilities,
    input  wire                          capabilities_ap_vld,
    input  wire [31:0]                   vocab_rows_done,
    input  wire                          vocab_rows_done_ap_vld,
    input  wire [63:0]                   weight_read_beats_128,
    input  wire                          weight_read_beats_128_ap_vld,
    input  wire [31:0]                   history_read_words,
    input  wire                          history_read_words_ap_vld,
    input  wire [63:0]                   logits_write_beats_128,
    input  wire                          logits_write_beats_128_ap_vld,
    input  wire [63:0]                   weight_wait_cycles,
    input  wire                          weight_wait_cycles_ap_vld,
    input  wire [63:0]                   dot_active_cycles,
    input  wire                          dot_active_cycles_ap_vld,
    input  wire [31:0]                   logits_written,
    input  wire                          logits_written_ap_vld,
    output wire                          ap_start,
    input  wire                          ap_done,
    input  wire                          ap_ready,
    input  wire                          ap_idle
);
//------------------------Address Info-------------------
// 0x000 : Control signals
//         bit 0  - ap_start (Read/Write/COH)
//         bit 1  - ap_done (Read/COR)
//         bit 2  - ap_idle (Read)
//         bit 3  - ap_ready (Read/COR)
//         bit 7  - auto_restart (Read/Write)
//         others - reserved
// 0x004 : Global Interrupt Enable Register
//         bit 0  - Global Interrupt Enable (Read/Write)
//         others - reserved
// 0x008 : IP Interrupt Enable Register (Read/Write)
//         bit 0  - enable ap_done interrupt (Read/Write)
//         bit 1  - enable ap_ready interrupt (Read/Write)
//         others - reserved
// 0x00c : IP Interrupt Status Register (Read/TOW)
//         bit 0  - ap_done (COR/TOW)
//         bit 1  - ap_ready (COR/TOW)
//         others - reserved
// 0x010 : Data signal of hidden_offset
//         bit 31~0 - hidden_offset[31:0] (Read/Write)
// 0x014 : Data signal of hidden_offset
//         bit 31~0 - hidden_offset[63:32] (Read/Write)
// 0x018 : reserved
// 0x01c : Data signal of shared_vocab_weight
//         bit 31~0 - shared_vocab_weight[31:0] (Read/Write)
// 0x020 : Data signal of shared_vocab_weight
//         bit 31~0 - shared_vocab_weight[63:32] (Read/Write)
// 0x024 : reserved
// 0x028 : Data signal of history_bitmap
//         bit 31~0 - history_bitmap[31:0] (Read/Write)
// 0x02c : Data signal of history_bitmap
//         bit 31~0 - history_bitmap[63:32] (Read/Write)
// 0x030 : reserved
// 0x034 : Data signal of optional_logits
//         bit 31~0 - optional_logits[31:0] (Read/Write)
// 0x038 : Data signal of optional_logits
//         bit 31~0 - optional_logits[63:32] (Read/Write)
// 0x03c : reserved
// 0x040 : Data signal of hidden_size_r
//         bit 31~0 - hidden_size_r[31:0] (Read/Write)
// 0x044 : reserved
// 0x048 : Data signal of vocab_size
//         bit 31~0 - vocab_size[31:0] (Read/Write)
// 0x04c : reserved
// 0x050 : Data signal of run_mode
//         bit 31~0 - run_mode[31:0] (Read/Write)
// 0x054 : reserved
// 0x058 : Data signal of valid_rows
//         bit 31~0 - valid_rows[31:0] (Read/Write)
// 0x05c : reserved
// 0x060 : Data signal of write_logits
//         bit 31~0 - write_logits[31:0] (Read/Write)
// 0x064 : reserved
// 0x068 : Data signal of enable_penalty
//         bit 31~0 - enable_penalty[31:0] (Read/Write)
// 0x06c : reserved
// 0x070 : Data signal of repetition_penalty
//         bit 31~0 - repetition_penalty[31:0] (Read/Write)
// 0x074 : reserved
// 0x078 : Data signal of abi_version
//         bit 31~0 - abi_version[31:0] (Read/Write)
// 0x07c : reserved
// 0x080 : Data signal of job_id
//         bit 31~0 - job_id[31:0] (Read/Write)
// 0x084 : reserved
// 0x088 : Data signal of flags
//         bit 31~0 - flags[31:0] (Read/Write)
// 0x08c : reserved
// 0x090 : Data signal of selected_token
//         bit 31~0 - selected_token[31:0] (Read)
// 0x094 : Control signal of selected_token
//         bit 0  - selected_token_ap_vld (Read/COR)
//         others - reserved
// 0x0a0 : Data signal of selected_score
//         bit 31~0 - selected_score[31:0] (Read)
// 0x0a4 : Control signal of selected_score
//         bit 0  - selected_score_ap_vld (Read/COR)
//         others - reserved
// 0x0b0 : Data signal of result_job_id
//         bit 31~0 - result_job_id[31:0] (Read)
// 0x0b4 : Control signal of result_job_id
//         bit 0  - result_job_id_ap_vld (Read/COR)
//         others - reserved
// 0x0c0 : Data signal of status_code
//         bit 31~0 - status_code[31:0] (Read)
// 0x0c4 : Control signal of status_code
//         bit 0  - status_code_ap_vld (Read/COR)
//         others - reserved
// 0x0d0 : Data signal of processed_rows
//         bit 31~0 - processed_rows[31:0] (Read)
// 0x0d4 : Control signal of processed_rows
//         bit 0  - processed_rows_ap_vld (Read/COR)
//         others - reserved
// 0x0e0 : Data signal of perf_cycles_total
//         bit 31~0 - perf_cycles_total[31:0] (Read)
// 0x0e4 : Data signal of perf_cycles_total
//         bit 31~0 - perf_cycles_total[63:32] (Read)
// 0x0e8 : Control signal of perf_cycles_total
//         bit 0  - perf_cycles_total_ap_vld (Read/COR)
//         others - reserved
// 0x0f8 : Data signal of perf_cycles_compute
//         bit 31~0 - perf_cycles_compute[31:0] (Read)
// 0x0fc : Data signal of perf_cycles_compute
//         bit 31~0 - perf_cycles_compute[63:32] (Read)
// 0x100 : Control signal of perf_cycles_compute
//         bit 0  - perf_cycles_compute_ap_vld (Read/COR)
//         others - reserved
// 0x110 : Data signal of perf_read_beats
//         bit 31~0 - perf_read_beats[31:0] (Read)
// 0x114 : Data signal of perf_read_beats
//         bit 31~0 - perf_read_beats[63:32] (Read)
// 0x118 : Control signal of perf_read_beats
//         bit 0  - perf_read_beats_ap_vld (Read/COR)
//         others - reserved
// 0x128 : Data signal of perf_write_beats
//         bit 31~0 - perf_write_beats[31:0] (Read)
// 0x12c : Data signal of perf_write_beats
//         bit 31~0 - perf_write_beats[63:32] (Read)
// 0x130 : Control signal of perf_write_beats
//         bit 0  - perf_write_beats_ap_vld (Read/COR)
//         others - reserved
// 0x140 : Data signal of output_crc
//         bit 31~0 - output_crc[31:0] (Read)
// 0x144 : Control signal of output_crc
//         bit 0  - output_crc_ap_vld (Read/COR)
//         others - reserved
// 0x150 : Data signal of build_config
//         bit 31~0 - build_config[31:0] (Read)
// 0x154 : Control signal of build_config
//         bit 0  - build_config_ap_vld (Read/COR)
//         others - reserved
// 0x160 : Data signal of capabilities
//         bit 31~0 - capabilities[31:0] (Read)
// 0x164 : Control signal of capabilities
//         bit 0  - capabilities_ap_vld (Read/COR)
//         others - reserved
// 0x170 : Data signal of vocab_rows_done
//         bit 31~0 - vocab_rows_done[31:0] (Read)
// 0x174 : Control signal of vocab_rows_done
//         bit 0  - vocab_rows_done_ap_vld (Read/COR)
//         others - reserved
// 0x180 : Data signal of weight_read_beats_128
//         bit 31~0 - weight_read_beats_128[31:0] (Read)
// 0x184 : Data signal of weight_read_beats_128
//         bit 31~0 - weight_read_beats_128[63:32] (Read)
// 0x188 : Control signal of weight_read_beats_128
//         bit 0  - weight_read_beats_128_ap_vld (Read/COR)
//         others - reserved
// 0x198 : Data signal of history_read_words
//         bit 31~0 - history_read_words[31:0] (Read)
// 0x19c : Control signal of history_read_words
//         bit 0  - history_read_words_ap_vld (Read/COR)
//         others - reserved
// 0x1a8 : Data signal of logits_write_beats_128
//         bit 31~0 - logits_write_beats_128[31:0] (Read)
// 0x1ac : Data signal of logits_write_beats_128
//         bit 31~0 - logits_write_beats_128[63:32] (Read)
// 0x1b0 : Control signal of logits_write_beats_128
//         bit 0  - logits_write_beats_128_ap_vld (Read/COR)
//         others - reserved
// 0x1c0 : Data signal of weight_wait_cycles
//         bit 31~0 - weight_wait_cycles[31:0] (Read)
// 0x1c4 : Data signal of weight_wait_cycles
//         bit 31~0 - weight_wait_cycles[63:32] (Read)
// 0x1c8 : Control signal of weight_wait_cycles
//         bit 0  - weight_wait_cycles_ap_vld (Read/COR)
//         others - reserved
// 0x1d8 : Data signal of dot_active_cycles
//         bit 31~0 - dot_active_cycles[31:0] (Read)
// 0x1dc : Data signal of dot_active_cycles
//         bit 31~0 - dot_active_cycles[63:32] (Read)
// 0x1e0 : Control signal of dot_active_cycles
//         bit 0  - dot_active_cycles_ap_vld (Read/COR)
//         others - reserved
// 0x1f0 : Data signal of logits_written
//         bit 31~0 - logits_written[31:0] (Read)
// 0x1f4 : Control signal of logits_written
//         bit 0  - logits_written_ap_vld (Read/COR)
//         others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

//------------------------Parameter----------------------
localparam
    ADDR_AP_CTRL                       = 9'h000,
    ADDR_GIE                           = 9'h004,
    ADDR_IER                           = 9'h008,
    ADDR_ISR                           = 9'h00c,
    ADDR_HIDDEN_OFFSET_DATA_0          = 9'h010,
    ADDR_HIDDEN_OFFSET_DATA_1          = 9'h014,
    ADDR_HIDDEN_OFFSET_CTRL            = 9'h018,
    ADDR_SHARED_VOCAB_WEIGHT_DATA_0    = 9'h01c,
    ADDR_SHARED_VOCAB_WEIGHT_DATA_1    = 9'h020,
    ADDR_SHARED_VOCAB_WEIGHT_CTRL      = 9'h024,
    ADDR_HISTORY_BITMAP_DATA_0         = 9'h028,
    ADDR_HISTORY_BITMAP_DATA_1         = 9'h02c,
    ADDR_HISTORY_BITMAP_CTRL           = 9'h030,
    ADDR_OPTIONAL_LOGITS_DATA_0        = 9'h034,
    ADDR_OPTIONAL_LOGITS_DATA_1        = 9'h038,
    ADDR_OPTIONAL_LOGITS_CTRL          = 9'h03c,
    ADDR_HIDDEN_SIZE_R_DATA_0          = 9'h040,
    ADDR_HIDDEN_SIZE_R_CTRL            = 9'h044,
    ADDR_VOCAB_SIZE_DATA_0             = 9'h048,
    ADDR_VOCAB_SIZE_CTRL               = 9'h04c,
    ADDR_RUN_MODE_DATA_0               = 9'h050,
    ADDR_RUN_MODE_CTRL                 = 9'h054,
    ADDR_VALID_ROWS_DATA_0             = 9'h058,
    ADDR_VALID_ROWS_CTRL               = 9'h05c,
    ADDR_WRITE_LOGITS_DATA_0           = 9'h060,
    ADDR_WRITE_LOGITS_CTRL             = 9'h064,
    ADDR_ENABLE_PENALTY_DATA_0         = 9'h068,
    ADDR_ENABLE_PENALTY_CTRL           = 9'h06c,
    ADDR_REPETITION_PENALTY_DATA_0     = 9'h070,
    ADDR_REPETITION_PENALTY_CTRL       = 9'h074,
    ADDR_ABI_VERSION_DATA_0            = 9'h078,
    ADDR_ABI_VERSION_CTRL              = 9'h07c,
    ADDR_JOB_ID_DATA_0                 = 9'h080,
    ADDR_JOB_ID_CTRL                   = 9'h084,
    ADDR_FLAGS_DATA_0                  = 9'h088,
    ADDR_FLAGS_CTRL                    = 9'h08c,
    ADDR_SELECTED_TOKEN_DATA_0         = 9'h090,
    ADDR_SELECTED_TOKEN_CTRL           = 9'h094,
    ADDR_SELECTED_SCORE_DATA_0         = 9'h0a0,
    ADDR_SELECTED_SCORE_CTRL           = 9'h0a4,
    ADDR_RESULT_JOB_ID_DATA_0          = 9'h0b0,
    ADDR_RESULT_JOB_ID_CTRL            = 9'h0b4,
    ADDR_STATUS_CODE_DATA_0            = 9'h0c0,
    ADDR_STATUS_CODE_CTRL              = 9'h0c4,
    ADDR_PROCESSED_ROWS_DATA_0         = 9'h0d0,
    ADDR_PROCESSED_ROWS_CTRL           = 9'h0d4,
    ADDR_PERF_CYCLES_TOTAL_DATA_0      = 9'h0e0,
    ADDR_PERF_CYCLES_TOTAL_DATA_1      = 9'h0e4,
    ADDR_PERF_CYCLES_TOTAL_CTRL        = 9'h0e8,
    ADDR_PERF_CYCLES_COMPUTE_DATA_0    = 9'h0f8,
    ADDR_PERF_CYCLES_COMPUTE_DATA_1    = 9'h0fc,
    ADDR_PERF_CYCLES_COMPUTE_CTRL      = 9'h100,
    ADDR_PERF_READ_BEATS_DATA_0        = 9'h110,
    ADDR_PERF_READ_BEATS_DATA_1        = 9'h114,
    ADDR_PERF_READ_BEATS_CTRL          = 9'h118,
    ADDR_PERF_WRITE_BEATS_DATA_0       = 9'h128,
    ADDR_PERF_WRITE_BEATS_DATA_1       = 9'h12c,
    ADDR_PERF_WRITE_BEATS_CTRL         = 9'h130,
    ADDR_OUTPUT_CRC_DATA_0             = 9'h140,
    ADDR_OUTPUT_CRC_CTRL               = 9'h144,
    ADDR_BUILD_CONFIG_DATA_0           = 9'h150,
    ADDR_BUILD_CONFIG_CTRL             = 9'h154,
    ADDR_CAPABILITIES_DATA_0           = 9'h160,
    ADDR_CAPABILITIES_CTRL             = 9'h164,
    ADDR_VOCAB_ROWS_DONE_DATA_0        = 9'h170,
    ADDR_VOCAB_ROWS_DONE_CTRL          = 9'h174,
    ADDR_WEIGHT_READ_BEATS_128_DATA_0  = 9'h180,
    ADDR_WEIGHT_READ_BEATS_128_DATA_1  = 9'h184,
    ADDR_WEIGHT_READ_BEATS_128_CTRL    = 9'h188,
    ADDR_HISTORY_READ_WORDS_DATA_0     = 9'h198,
    ADDR_HISTORY_READ_WORDS_CTRL       = 9'h19c,
    ADDR_LOGITS_WRITE_BEATS_128_DATA_0 = 9'h1a8,
    ADDR_LOGITS_WRITE_BEATS_128_DATA_1 = 9'h1ac,
    ADDR_LOGITS_WRITE_BEATS_128_CTRL   = 9'h1b0,
    ADDR_WEIGHT_WAIT_CYCLES_DATA_0     = 9'h1c0,
    ADDR_WEIGHT_WAIT_CYCLES_DATA_1     = 9'h1c4,
    ADDR_WEIGHT_WAIT_CYCLES_CTRL       = 9'h1c8,
    ADDR_DOT_ACTIVE_CYCLES_DATA_0      = 9'h1d8,
    ADDR_DOT_ACTIVE_CYCLES_DATA_1      = 9'h1dc,
    ADDR_DOT_ACTIVE_CYCLES_CTRL        = 9'h1e0,
    ADDR_LOGITS_WRITTEN_DATA_0         = 9'h1f0,
    ADDR_LOGITS_WRITTEN_CTRL           = 9'h1f4,
    WRIDLE                             = 2'd0,
    WRDATA                             = 2'd1,
    WRRESP                             = 2'd2,
    WRRESET                            = 2'd3,
    RDIDLE                             = 2'd0,
    RDDATA                             = 2'd1,
    RDRESET                            = 2'd2,
    ADDR_BITS                = 9;

//------------------------Local signal-------------------
    reg  [1:0]                    wstate = WRRESET;
    reg  [1:0]                    wnext;
    reg  [ADDR_BITS-1:0]          waddr;
    wire [C_S_AXI_DATA_WIDTH-1:0] wmask;
    wire                          aw_hs;
    wire                          w_hs;
    reg  [1:0]                    rstate = RDRESET;
    reg  [1:0]                    rnext;
    reg  [C_S_AXI_DATA_WIDTH-1:0] rdata;
    wire                          ar_hs;
    wire [ADDR_BITS-1:0]          raddr;
    // internal registers
    reg                           int_ap_idle;
    reg                           int_ap_ready = 1'b0;
    wire                          task_ap_ready;
    reg                           int_ap_done = 1'b0;
    wire                          task_ap_done;
    reg                           int_task_ap_done = 1'b0;
    reg                           int_ap_start = 1'b0;
    reg                           int_auto_restart = 1'b0;
    reg                           auto_restart_status = 1'b0;
    wire                          auto_restart_done;
    reg                           int_gie = 1'b0;
    reg  [1:0]                    int_ier = 2'b0;
    reg  [1:0]                    int_isr = 2'b0;
    reg  [63:0]                   int_hidden_offset = 'b0;
    reg  [63:0]                   int_shared_vocab_weight = 'b0;
    reg  [63:0]                   int_history_bitmap = 'b0;
    reg  [63:0]                   int_optional_logits = 'b0;
    reg  [31:0]                   int_hidden_size_r = 'b0;
    reg  [31:0]                   int_vocab_size = 'b0;
    reg  [31:0]                   int_run_mode = 'b0;
    reg  [31:0]                   int_valid_rows = 'b0;
    reg  [31:0]                   int_write_logits = 'b0;
    reg  [31:0]                   int_enable_penalty = 'b0;
    reg  [31:0]                   int_repetition_penalty = 'b0;
    reg  [31:0]                   int_abi_version = 'b0;
    reg  [31:0]                   int_job_id = 'b0;
    reg  [31:0]                   int_flags = 'b0;
    reg                           int_selected_token_ap_vld;
    reg  [31:0]                   int_selected_token = 'b0;
    reg                           int_selected_score_ap_vld;
    reg  [31:0]                   int_selected_score = 'b0;
    reg                           int_result_job_id_ap_vld;
    reg  [31:0]                   int_result_job_id = 'b0;
    reg                           int_status_code_ap_vld;
    reg  [31:0]                   int_status_code = 'b0;
    reg                           int_processed_rows_ap_vld;
    reg  [31:0]                   int_processed_rows = 'b0;
    reg                           int_perf_cycles_total_ap_vld;
    reg  [63:0]                   int_perf_cycles_total = 'b0;
    reg                           int_perf_cycles_compute_ap_vld;
    reg  [63:0]                   int_perf_cycles_compute = 'b0;
    reg                           int_perf_read_beats_ap_vld;
    reg  [63:0]                   int_perf_read_beats = 'b0;
    reg                           int_perf_write_beats_ap_vld;
    reg  [63:0]                   int_perf_write_beats = 'b0;
    reg                           int_output_crc_ap_vld;
    reg  [31:0]                   int_output_crc = 'b0;
    reg                           int_build_config_ap_vld;
    reg  [31:0]                   int_build_config = 'b0;
    reg                           int_capabilities_ap_vld;
    reg  [31:0]                   int_capabilities = 'b0;
    reg                           int_vocab_rows_done_ap_vld;
    reg  [31:0]                   int_vocab_rows_done = 'b0;
    reg                           int_weight_read_beats_128_ap_vld;
    reg  [63:0]                   int_weight_read_beats_128 = 'b0;
    reg                           int_history_read_words_ap_vld;
    reg  [31:0]                   int_history_read_words = 'b0;
    reg                           int_logits_write_beats_128_ap_vld;
    reg  [63:0]                   int_logits_write_beats_128 = 'b0;
    reg                           int_weight_wait_cycles_ap_vld;
    reg  [63:0]                   int_weight_wait_cycles = 'b0;
    reg                           int_dot_active_cycles_ap_vld;
    reg  [63:0]                   int_dot_active_cycles = 'b0;
    reg                           int_logits_written_ap_vld;
    reg  [31:0]                   int_logits_written = 'b0;

//------------------------Instantiation------------------


//------------------------AXI write fsm------------------
assign AWREADY = (wstate == WRIDLE);
assign WREADY  = (wstate == WRDATA);
assign BRESP   = 2'b00;  // OKAY
assign BVALID  = (wstate == WRRESP);
assign wmask   = { {8{WSTRB[3]}}, {8{WSTRB[2]}}, {8{WSTRB[1]}}, {8{WSTRB[0]}} };
assign aw_hs   = AWVALID & AWREADY;
assign w_hs    = WVALID & WREADY;

// wstate
always @(posedge ACLK) begin
    if (ARESET)
        wstate <= WRRESET;
    else if (ACLK_EN)
        wstate <= wnext;
end

// wnext
always @(*) begin
    case (wstate)
        WRIDLE:
            if (AWVALID)
                wnext = WRDATA;
            else
                wnext = WRIDLE;
        WRDATA:
            if (WVALID)
                wnext = WRRESP;
            else
                wnext = WRDATA;
        WRRESP:
            if (BREADY)
                wnext = WRIDLE;
            else
                wnext = WRRESP;
        default:
            wnext = WRIDLE;
    endcase
end

// waddr
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (aw_hs)
            waddr <= AWADDR[ADDR_BITS-1:0];
    end
end

//------------------------AXI read fsm-------------------
assign ARREADY = (rstate == RDIDLE);
assign RDATA   = rdata;
assign RRESP   = 2'b00;  // OKAY
assign RVALID  = (rstate == RDDATA);
assign ar_hs   = ARVALID & ARREADY;
assign raddr   = ARADDR[ADDR_BITS-1:0];

// rstate
always @(posedge ACLK) begin
    if (ARESET)
        rstate <= RDRESET;
    else if (ACLK_EN)
        rstate <= rnext;
end

// rnext
always @(*) begin
    case (rstate)
        RDIDLE:
            if (ARVALID)
                rnext = RDDATA;
            else
                rnext = RDIDLE;
        RDDATA:
            if (RREADY & RVALID)
                rnext = RDIDLE;
            else
                rnext = RDDATA;
        default:
            rnext = RDIDLE;
    endcase
end

// rdata
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (ar_hs) begin
            rdata <= 'b0;
            case (raddr)
                ADDR_AP_CTRL: begin
                    rdata[0] <= int_ap_start;
                    rdata[1] <= int_task_ap_done;
                    rdata[2] <= int_ap_idle;
                    rdata[3] <= int_ap_ready;
                    rdata[7] <= int_auto_restart;
                end
                ADDR_GIE: begin
                    rdata <= int_gie;
                end
                ADDR_IER: begin
                    rdata <= int_ier;
                end
                ADDR_ISR: begin
                    rdata <= int_isr;
                end
                ADDR_HIDDEN_OFFSET_DATA_0: begin
                    rdata <= int_hidden_offset[31:0];
                end
                ADDR_HIDDEN_OFFSET_DATA_1: begin
                    rdata <= int_hidden_offset[63:32];
                end
                ADDR_SHARED_VOCAB_WEIGHT_DATA_0: begin
                    rdata <= int_shared_vocab_weight[31:0];
                end
                ADDR_SHARED_VOCAB_WEIGHT_DATA_1: begin
                    rdata <= int_shared_vocab_weight[63:32];
                end
                ADDR_HISTORY_BITMAP_DATA_0: begin
                    rdata <= int_history_bitmap[31:0];
                end
                ADDR_HISTORY_BITMAP_DATA_1: begin
                    rdata <= int_history_bitmap[63:32];
                end
                ADDR_OPTIONAL_LOGITS_DATA_0: begin
                    rdata <= int_optional_logits[31:0];
                end
                ADDR_OPTIONAL_LOGITS_DATA_1: begin
                    rdata <= int_optional_logits[63:32];
                end
                ADDR_HIDDEN_SIZE_R_DATA_0: begin
                    rdata <= int_hidden_size_r[31:0];
                end
                ADDR_VOCAB_SIZE_DATA_0: begin
                    rdata <= int_vocab_size[31:0];
                end
                ADDR_RUN_MODE_DATA_0: begin
                    rdata <= int_run_mode[31:0];
                end
                ADDR_VALID_ROWS_DATA_0: begin
                    rdata <= int_valid_rows[31:0];
                end
                ADDR_WRITE_LOGITS_DATA_0: begin
                    rdata <= int_write_logits[31:0];
                end
                ADDR_ENABLE_PENALTY_DATA_0: begin
                    rdata <= int_enable_penalty[31:0];
                end
                ADDR_REPETITION_PENALTY_DATA_0: begin
                    rdata <= int_repetition_penalty[31:0];
                end
                ADDR_ABI_VERSION_DATA_0: begin
                    rdata <= int_abi_version[31:0];
                end
                ADDR_JOB_ID_DATA_0: begin
                    rdata <= int_job_id[31:0];
                end
                ADDR_FLAGS_DATA_0: begin
                    rdata <= int_flags[31:0];
                end
                ADDR_SELECTED_TOKEN_DATA_0: begin
                    rdata <= int_selected_token[31:0];
                end
                ADDR_SELECTED_TOKEN_CTRL: begin
                    rdata[0] <= int_selected_token_ap_vld;
                end
                ADDR_SELECTED_SCORE_DATA_0: begin
                    rdata <= int_selected_score[31:0];
                end
                ADDR_SELECTED_SCORE_CTRL: begin
                    rdata[0] <= int_selected_score_ap_vld;
                end
                ADDR_RESULT_JOB_ID_DATA_0: begin
                    rdata <= int_result_job_id[31:0];
                end
                ADDR_RESULT_JOB_ID_CTRL: begin
                    rdata[0] <= int_result_job_id_ap_vld;
                end
                ADDR_STATUS_CODE_DATA_0: begin
                    rdata <= int_status_code[31:0];
                end
                ADDR_STATUS_CODE_CTRL: begin
                    rdata[0] <= int_status_code_ap_vld;
                end
                ADDR_PROCESSED_ROWS_DATA_0: begin
                    rdata <= int_processed_rows[31:0];
                end
                ADDR_PROCESSED_ROWS_CTRL: begin
                    rdata[0] <= int_processed_rows_ap_vld;
                end
                ADDR_PERF_CYCLES_TOTAL_DATA_0: begin
                    rdata <= int_perf_cycles_total[31:0];
                end
                ADDR_PERF_CYCLES_TOTAL_DATA_1: begin
                    rdata <= int_perf_cycles_total[63:32];
                end
                ADDR_PERF_CYCLES_TOTAL_CTRL: begin
                    rdata[0] <= int_perf_cycles_total_ap_vld;
                end
                ADDR_PERF_CYCLES_COMPUTE_DATA_0: begin
                    rdata <= int_perf_cycles_compute[31:0];
                end
                ADDR_PERF_CYCLES_COMPUTE_DATA_1: begin
                    rdata <= int_perf_cycles_compute[63:32];
                end
                ADDR_PERF_CYCLES_COMPUTE_CTRL: begin
                    rdata[0] <= int_perf_cycles_compute_ap_vld;
                end
                ADDR_PERF_READ_BEATS_DATA_0: begin
                    rdata <= int_perf_read_beats[31:0];
                end
                ADDR_PERF_READ_BEATS_DATA_1: begin
                    rdata <= int_perf_read_beats[63:32];
                end
                ADDR_PERF_READ_BEATS_CTRL: begin
                    rdata[0] <= int_perf_read_beats_ap_vld;
                end
                ADDR_PERF_WRITE_BEATS_DATA_0: begin
                    rdata <= int_perf_write_beats[31:0];
                end
                ADDR_PERF_WRITE_BEATS_DATA_1: begin
                    rdata <= int_perf_write_beats[63:32];
                end
                ADDR_PERF_WRITE_BEATS_CTRL: begin
                    rdata[0] <= int_perf_write_beats_ap_vld;
                end
                ADDR_OUTPUT_CRC_DATA_0: begin
                    rdata <= int_output_crc[31:0];
                end
                ADDR_OUTPUT_CRC_CTRL: begin
                    rdata[0] <= int_output_crc_ap_vld;
                end
                ADDR_BUILD_CONFIG_DATA_0: begin
                    rdata <= int_build_config[31:0];
                end
                ADDR_BUILD_CONFIG_CTRL: begin
                    rdata[0] <= int_build_config_ap_vld;
                end
                ADDR_CAPABILITIES_DATA_0: begin
                    rdata <= int_capabilities[31:0];
                end
                ADDR_CAPABILITIES_CTRL: begin
                    rdata[0] <= int_capabilities_ap_vld;
                end
                ADDR_VOCAB_ROWS_DONE_DATA_0: begin
                    rdata <= int_vocab_rows_done[31:0];
                end
                ADDR_VOCAB_ROWS_DONE_CTRL: begin
                    rdata[0] <= int_vocab_rows_done_ap_vld;
                end
                ADDR_WEIGHT_READ_BEATS_128_DATA_0: begin
                    rdata <= int_weight_read_beats_128[31:0];
                end
                ADDR_WEIGHT_READ_BEATS_128_DATA_1: begin
                    rdata <= int_weight_read_beats_128[63:32];
                end
                ADDR_WEIGHT_READ_BEATS_128_CTRL: begin
                    rdata[0] <= int_weight_read_beats_128_ap_vld;
                end
                ADDR_HISTORY_READ_WORDS_DATA_0: begin
                    rdata <= int_history_read_words[31:0];
                end
                ADDR_HISTORY_READ_WORDS_CTRL: begin
                    rdata[0] <= int_history_read_words_ap_vld;
                end
                ADDR_LOGITS_WRITE_BEATS_128_DATA_0: begin
                    rdata <= int_logits_write_beats_128[31:0];
                end
                ADDR_LOGITS_WRITE_BEATS_128_DATA_1: begin
                    rdata <= int_logits_write_beats_128[63:32];
                end
                ADDR_LOGITS_WRITE_BEATS_128_CTRL: begin
                    rdata[0] <= int_logits_write_beats_128_ap_vld;
                end
                ADDR_WEIGHT_WAIT_CYCLES_DATA_0: begin
                    rdata <= int_weight_wait_cycles[31:0];
                end
                ADDR_WEIGHT_WAIT_CYCLES_DATA_1: begin
                    rdata <= int_weight_wait_cycles[63:32];
                end
                ADDR_WEIGHT_WAIT_CYCLES_CTRL: begin
                    rdata[0] <= int_weight_wait_cycles_ap_vld;
                end
                ADDR_DOT_ACTIVE_CYCLES_DATA_0: begin
                    rdata <= int_dot_active_cycles[31:0];
                end
                ADDR_DOT_ACTIVE_CYCLES_DATA_1: begin
                    rdata <= int_dot_active_cycles[63:32];
                end
                ADDR_DOT_ACTIVE_CYCLES_CTRL: begin
                    rdata[0] <= int_dot_active_cycles_ap_vld;
                end
                ADDR_LOGITS_WRITTEN_DATA_0: begin
                    rdata <= int_logits_written[31:0];
                end
                ADDR_LOGITS_WRITTEN_CTRL: begin
                    rdata[0] <= int_logits_written_ap_vld;
                end
            endcase
        end
    end
end


//------------------------Register logic-----------------
assign interrupt           = int_gie & (|int_isr);
assign ap_start            = int_ap_start;
assign task_ap_done        = (ap_done && !auto_restart_status) || auto_restart_done;
assign task_ap_ready       = ap_ready && !int_auto_restart;
assign auto_restart_done   = auto_restart_status && (ap_idle && !int_ap_idle);
assign hidden_offset       = int_hidden_offset;
assign shared_vocab_weight = int_shared_vocab_weight;
assign history_bitmap      = int_history_bitmap;
assign optional_logits     = int_optional_logits;
assign hidden_size_r       = int_hidden_size_r;
assign vocab_size          = int_vocab_size;
assign run_mode            = int_run_mode;
assign valid_rows          = int_valid_rows;
assign write_logits        = int_write_logits;
assign enable_penalty      = int_enable_penalty;
assign repetition_penalty  = int_repetition_penalty;
assign abi_version         = int_abi_version;
assign job_id              = int_job_id;
assign flags               = int_flags;
// int_ap_start
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_start <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0] && WDATA[0])
            int_ap_start <= 1'b1;
        else if (ap_ready)
            int_ap_start <= int_auto_restart; // clear on handshake/auto restart
    end
end

// int_ap_done
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_done <= 1'b0;
    else if (ACLK_EN) begin
            int_ap_done <= ap_done;
    end
end

// int_task_ap_done
always @(posedge ACLK) begin
    if (ARESET)
        int_task_ap_done <= 1'b0;
    else if (ACLK_EN) begin
        if (task_ap_done)
            int_task_ap_done <= 1'b1;
        else if (ar_hs && raddr == ADDR_AP_CTRL)
            int_task_ap_done <= 1'b0; // clear on read
    end
end

// int_ap_idle
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_idle <= 1'b0;
    else if (ACLK_EN) begin
            int_ap_idle <= ap_idle;
    end
end

// int_ap_ready
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_ready <= 1'b0;
    else if (ACLK_EN) begin
        if (task_ap_ready)
            int_ap_ready <= 1'b1;
        else if (ar_hs && raddr == ADDR_AP_CTRL)
            int_ap_ready <= 1'b0;
    end
end

// int_auto_restart
always @(posedge ACLK) begin
    if (ARESET)
        int_auto_restart <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0])
            int_auto_restart <=  WDATA[7];
    end
end

// auto_restart_status
always @(posedge ACLK) begin
    if (ARESET)
        auto_restart_status <= 1'b0;
    else if (ACLK_EN) begin
        if (int_auto_restart)
            auto_restart_status <= 1'b1;
        else if (ap_idle)
            auto_restart_status <= 1'b0;
    end
end

// int_gie
always @(posedge ACLK) begin
    if (ARESET)
        int_gie <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_GIE && WSTRB[0])
            int_gie <= WDATA[0];
    end
end

// int_ier
always @(posedge ACLK) begin
    if (ARESET)
        int_ier <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_IER && WSTRB[0])
            int_ier <= WDATA[1:0];
    end
end

// int_isr[0]
always @(posedge ACLK) begin
    if (ARESET)
        int_isr[0] <= 1'b0;
    else if (ACLK_EN) begin
        if (int_ier[0] & ap_done)
            int_isr[0] <= 1'b1;
        else if (w_hs && waddr == ADDR_ISR && WSTRB[0])
            int_isr[0] <= int_isr[0] ^ WDATA[0]; // toggle on write
    end
end

// int_isr[1]
always @(posedge ACLK) begin
    if (ARESET)
        int_isr[1] <= 1'b0;
    else if (ACLK_EN) begin
        if (int_ier[1] & ap_ready)
            int_isr[1] <= 1'b1;
        else if (w_hs && waddr == ADDR_ISR && WSTRB[0])
            int_isr[1] <= int_isr[1] ^ WDATA[1]; // toggle on write
    end
end

// int_hidden_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_hidden_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_HIDDEN_OFFSET_DATA_0)
            int_hidden_offset[31:0] <= (WDATA[31:0] & wmask) | (int_hidden_offset[31:0] & ~wmask);
    end
end

// int_hidden_offset[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_hidden_offset[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_HIDDEN_OFFSET_DATA_1)
            int_hidden_offset[63:32] <= (WDATA[31:0] & wmask) | (int_hidden_offset[63:32] & ~wmask);
    end
end

// int_shared_vocab_weight[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_shared_vocab_weight[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_SHARED_VOCAB_WEIGHT_DATA_0)
            int_shared_vocab_weight[31:0] <= (WDATA[31:0] & wmask) | (int_shared_vocab_weight[31:0] & ~wmask);
    end
end

// int_shared_vocab_weight[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_shared_vocab_weight[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_SHARED_VOCAB_WEIGHT_DATA_1)
            int_shared_vocab_weight[63:32] <= (WDATA[31:0] & wmask) | (int_shared_vocab_weight[63:32] & ~wmask);
    end
end

// int_history_bitmap[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_history_bitmap[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_HISTORY_BITMAP_DATA_0)
            int_history_bitmap[31:0] <= (WDATA[31:0] & wmask) | (int_history_bitmap[31:0] & ~wmask);
    end
end

// int_history_bitmap[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_history_bitmap[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_HISTORY_BITMAP_DATA_1)
            int_history_bitmap[63:32] <= (WDATA[31:0] & wmask) | (int_history_bitmap[63:32] & ~wmask);
    end
end

// int_optional_logits[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_optional_logits[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_OPTIONAL_LOGITS_DATA_0)
            int_optional_logits[31:0] <= (WDATA[31:0] & wmask) | (int_optional_logits[31:0] & ~wmask);
    end
end

// int_optional_logits[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_optional_logits[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_OPTIONAL_LOGITS_DATA_1)
            int_optional_logits[63:32] <= (WDATA[31:0] & wmask) | (int_optional_logits[63:32] & ~wmask);
    end
end

// int_hidden_size_r[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_hidden_size_r[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_HIDDEN_SIZE_R_DATA_0)
            int_hidden_size_r[31:0] <= (WDATA[31:0] & wmask) | (int_hidden_size_r[31:0] & ~wmask);
    end
end

// int_vocab_size[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_vocab_size[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_VOCAB_SIZE_DATA_0)
            int_vocab_size[31:0] <= (WDATA[31:0] & wmask) | (int_vocab_size[31:0] & ~wmask);
    end
end

// int_run_mode[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_run_mode[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_RUN_MODE_DATA_0)
            int_run_mode[31:0] <= (WDATA[31:0] & wmask) | (int_run_mode[31:0] & ~wmask);
    end
end

// int_valid_rows[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_valid_rows[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_VALID_ROWS_DATA_0)
            int_valid_rows[31:0] <= (WDATA[31:0] & wmask) | (int_valid_rows[31:0] & ~wmask);
    end
end

// int_write_logits[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_write_logits[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_WRITE_LOGITS_DATA_0)
            int_write_logits[31:0] <= (WDATA[31:0] & wmask) | (int_write_logits[31:0] & ~wmask);
    end
end

// int_enable_penalty[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_penalty[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_PENALTY_DATA_0)
            int_enable_penalty[31:0] <= (WDATA[31:0] & wmask) | (int_enable_penalty[31:0] & ~wmask);
    end
end

// int_repetition_penalty[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_repetition_penalty[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_REPETITION_PENALTY_DATA_0)
            int_repetition_penalty[31:0] <= (WDATA[31:0] & wmask) | (int_repetition_penalty[31:0] & ~wmask);
    end
end

// int_abi_version[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_abi_version[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ABI_VERSION_DATA_0)
            int_abi_version[31:0] <= (WDATA[31:0] & wmask) | (int_abi_version[31:0] & ~wmask);
    end
end

// int_job_id[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_job_id[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_JOB_ID_DATA_0)
            int_job_id[31:0] <= (WDATA[31:0] & wmask) | (int_job_id[31:0] & ~wmask);
    end
end

// int_flags[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_flags[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_FLAGS_DATA_0)
            int_flags[31:0] <= (WDATA[31:0] & wmask) | (int_flags[31:0] & ~wmask);
    end
end

// int_selected_token
always @(posedge ACLK) begin
    if (ARESET)
        int_selected_token <= 0;
    else if (ACLK_EN) begin
        if (selected_token_ap_vld)
            int_selected_token <= selected_token;
    end
end

// int_selected_token_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_selected_token_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (selected_token_ap_vld)
            int_selected_token_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_SELECTED_TOKEN_CTRL)
            int_selected_token_ap_vld <= 1'b0; // clear on read
    end
end

// int_selected_score
always @(posedge ACLK) begin
    if (ARESET)
        int_selected_score <= 0;
    else if (ACLK_EN) begin
        if (selected_score_ap_vld)
            int_selected_score <= selected_score;
    end
end

// int_selected_score_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_selected_score_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (selected_score_ap_vld)
            int_selected_score_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_SELECTED_SCORE_CTRL)
            int_selected_score_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_job_id
always @(posedge ACLK) begin
    if (ARESET)
        int_result_job_id <= 0;
    else if (ACLK_EN) begin
        if (result_job_id_ap_vld)
            int_result_job_id <= result_job_id;
    end
end

// int_result_job_id_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_job_id_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_job_id_ap_vld)
            int_result_job_id_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_JOB_ID_CTRL)
            int_result_job_id_ap_vld <= 1'b0; // clear on read
    end
end

// int_status_code
always @(posedge ACLK) begin
    if (ARESET)
        int_status_code <= 0;
    else if (ACLK_EN) begin
        if (status_code_ap_vld)
            int_status_code <= status_code;
    end
end

// int_status_code_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_status_code_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (status_code_ap_vld)
            int_status_code_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_STATUS_CODE_CTRL)
            int_status_code_ap_vld <= 1'b0; // clear on read
    end
end

// int_processed_rows
always @(posedge ACLK) begin
    if (ARESET)
        int_processed_rows <= 0;
    else if (ACLK_EN) begin
        if (processed_rows_ap_vld)
            int_processed_rows <= processed_rows;
    end
end

// int_processed_rows_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_processed_rows_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (processed_rows_ap_vld)
            int_processed_rows_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PROCESSED_ROWS_CTRL)
            int_processed_rows_ap_vld <= 1'b0; // clear on read
    end
end

// int_perf_cycles_total
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_cycles_total <= 0;
    else if (ACLK_EN) begin
        if (perf_cycles_total_ap_vld)
            int_perf_cycles_total <= perf_cycles_total;
    end
end

// int_perf_cycles_total_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_cycles_total_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (perf_cycles_total_ap_vld)
            int_perf_cycles_total_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PERF_CYCLES_TOTAL_CTRL)
            int_perf_cycles_total_ap_vld <= 1'b0; // clear on read
    end
end

// int_perf_cycles_compute
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_cycles_compute <= 0;
    else if (ACLK_EN) begin
        if (perf_cycles_compute_ap_vld)
            int_perf_cycles_compute <= perf_cycles_compute;
    end
end

// int_perf_cycles_compute_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_cycles_compute_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (perf_cycles_compute_ap_vld)
            int_perf_cycles_compute_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PERF_CYCLES_COMPUTE_CTRL)
            int_perf_cycles_compute_ap_vld <= 1'b0; // clear on read
    end
end

// int_perf_read_beats
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_read_beats <= 0;
    else if (ACLK_EN) begin
        if (perf_read_beats_ap_vld)
            int_perf_read_beats <= perf_read_beats;
    end
end

// int_perf_read_beats_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_read_beats_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (perf_read_beats_ap_vld)
            int_perf_read_beats_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PERF_READ_BEATS_CTRL)
            int_perf_read_beats_ap_vld <= 1'b0; // clear on read
    end
end

// int_perf_write_beats
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_write_beats <= 0;
    else if (ACLK_EN) begin
        if (perf_write_beats_ap_vld)
            int_perf_write_beats <= perf_write_beats;
    end
end

// int_perf_write_beats_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_perf_write_beats_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (perf_write_beats_ap_vld)
            int_perf_write_beats_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_PERF_WRITE_BEATS_CTRL)
            int_perf_write_beats_ap_vld <= 1'b0; // clear on read
    end
end

// int_output_crc
always @(posedge ACLK) begin
    if (ARESET)
        int_output_crc <= 0;
    else if (ACLK_EN) begin
        if (output_crc_ap_vld)
            int_output_crc <= output_crc;
    end
end

// int_output_crc_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_output_crc_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (output_crc_ap_vld)
            int_output_crc_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_OUTPUT_CRC_CTRL)
            int_output_crc_ap_vld <= 1'b0; // clear on read
    end
end

// int_build_config
always @(posedge ACLK) begin
    if (ARESET)
        int_build_config <= 0;
    else if (ACLK_EN) begin
        if (build_config_ap_vld)
            int_build_config <= build_config;
    end
end

// int_build_config_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_build_config_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (build_config_ap_vld)
            int_build_config_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_BUILD_CONFIG_CTRL)
            int_build_config_ap_vld <= 1'b0; // clear on read
    end
end

// int_capabilities
always @(posedge ACLK) begin
    if (ARESET)
        int_capabilities <= 0;
    else if (ACLK_EN) begin
        if (capabilities_ap_vld)
            int_capabilities <= capabilities;
    end
end

// int_capabilities_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_capabilities_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (capabilities_ap_vld)
            int_capabilities_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_CAPABILITIES_CTRL)
            int_capabilities_ap_vld <= 1'b0; // clear on read
    end
end

// int_vocab_rows_done
always @(posedge ACLK) begin
    if (ARESET)
        int_vocab_rows_done <= 0;
    else if (ACLK_EN) begin
        if (vocab_rows_done_ap_vld)
            int_vocab_rows_done <= vocab_rows_done;
    end
end

// int_vocab_rows_done_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_vocab_rows_done_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (vocab_rows_done_ap_vld)
            int_vocab_rows_done_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_VOCAB_ROWS_DONE_CTRL)
            int_vocab_rows_done_ap_vld <= 1'b0; // clear on read
    end
end

// int_weight_read_beats_128
always @(posedge ACLK) begin
    if (ARESET)
        int_weight_read_beats_128 <= 0;
    else if (ACLK_EN) begin
        if (weight_read_beats_128_ap_vld)
            int_weight_read_beats_128 <= weight_read_beats_128;
    end
end

// int_weight_read_beats_128_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_weight_read_beats_128_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (weight_read_beats_128_ap_vld)
            int_weight_read_beats_128_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_WEIGHT_READ_BEATS_128_CTRL)
            int_weight_read_beats_128_ap_vld <= 1'b0; // clear on read
    end
end

// int_history_read_words
always @(posedge ACLK) begin
    if (ARESET)
        int_history_read_words <= 0;
    else if (ACLK_EN) begin
        if (history_read_words_ap_vld)
            int_history_read_words <= history_read_words;
    end
end

// int_history_read_words_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_history_read_words_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (history_read_words_ap_vld)
            int_history_read_words_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_HISTORY_READ_WORDS_CTRL)
            int_history_read_words_ap_vld <= 1'b0; // clear on read
    end
end

// int_logits_write_beats_128
always @(posedge ACLK) begin
    if (ARESET)
        int_logits_write_beats_128 <= 0;
    else if (ACLK_EN) begin
        if (logits_write_beats_128_ap_vld)
            int_logits_write_beats_128 <= logits_write_beats_128;
    end
end

// int_logits_write_beats_128_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_logits_write_beats_128_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (logits_write_beats_128_ap_vld)
            int_logits_write_beats_128_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_LOGITS_WRITE_BEATS_128_CTRL)
            int_logits_write_beats_128_ap_vld <= 1'b0; // clear on read
    end
end

// int_weight_wait_cycles
always @(posedge ACLK) begin
    if (ARESET)
        int_weight_wait_cycles <= 0;
    else if (ACLK_EN) begin
        if (weight_wait_cycles_ap_vld)
            int_weight_wait_cycles <= weight_wait_cycles;
    end
end

// int_weight_wait_cycles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_weight_wait_cycles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (weight_wait_cycles_ap_vld)
            int_weight_wait_cycles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_WEIGHT_WAIT_CYCLES_CTRL)
            int_weight_wait_cycles_ap_vld <= 1'b0; // clear on read
    end
end

// int_dot_active_cycles
always @(posedge ACLK) begin
    if (ARESET)
        int_dot_active_cycles <= 0;
    else if (ACLK_EN) begin
        if (dot_active_cycles_ap_vld)
            int_dot_active_cycles <= dot_active_cycles;
    end
end

// int_dot_active_cycles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_dot_active_cycles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (dot_active_cycles_ap_vld)
            int_dot_active_cycles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_DOT_ACTIVE_CYCLES_CTRL)
            int_dot_active_cycles_ap_vld <= 1'b0; // clear on read
    end
end

// int_logits_written
always @(posedge ACLK) begin
    if (ARESET)
        int_logits_written <= 0;
    else if (ACLK_EN) begin
        if (logits_written_ap_vld)
            int_logits_written <= logits_written;
    end
end

// int_logits_written_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_logits_written_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (logits_written_ap_vld)
            int_logits_written_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_LOGITS_WRITTEN_CTRL)
            int_logits_written_ap_vld <= 1'b0; // clear on read
    end
end


//------------------------Memory logic-------------------

endmodule
