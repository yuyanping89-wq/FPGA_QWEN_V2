// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1ns/1ps
module swiglu_v2_control_s_axi
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
    output wire [63:0]                   x_offset,
    output wire [63:0]                   w_gate_offset,
    output wire [63:0]                   w_up_offset,
    output wire [63:0]                   w_down_offset,
    output wire [63:0]                   y_offset,
    output wire [31:0]                   run_mode,
    output wire [31:0]                   valid_rows,
    output wire [31:0]                   hidden_size,
    output wire [31:0]                   intermediate_size,
    output wire [31:0]                   layout_id,
    output wire [31:0]                   abi_version,
    output wire [31:0]                   job_id,
    output wire [31:0]                   flags,
    input  wire [31:0]                   result_status_code,
    input  wire                          result_status_code_ap_vld,
    input  wire [31:0]                   result_processed_rows,
    input  wire                          result_processed_rows_ap_vld,
    input  wire [63:0]                   result_perf_cycles_total,
    input  wire                          result_perf_cycles_total_ap_vld,
    input  wire [63:0]                   result_perf_cycles_compute,
    input  wire                          result_perf_cycles_compute_ap_vld,
    input  wire [63:0]                   result_perf_read_beats,
    input  wire                          result_perf_read_beats_ap_vld,
    input  wire [63:0]                   result_perf_write_beats,
    input  wire                          result_perf_write_beats_ap_vld,
    input  wire [31:0]                   result_output_crc,
    input  wire                          result_output_crc_ap_vld,
    input  wire [31:0]                   result_build_config,
    input  wire                          result_build_config_ap_vld,
    input  wire [31:0]                   result_mid_tiles,
    input  wire                          result_mid_tiles_ap_vld,
    input  wire [63:0]                   result_gate_up_active_cycles,
    input  wire                          result_gate_up_active_cycles_ap_vld,
    input  wire [63:0]                   result_down_active_cycles,
    input  wire                          result_down_active_cycles_ap_vld,
    input  wire [63:0]                   result_weight_wait_cycles,
    input  wire                          result_weight_wait_cycles_ap_vld,
    input  wire [63:0]                   result_fifo_stall_cycles,
    input  wire                          result_fifo_stall_cycles_ap_vld,
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
// 0x010 : Data signal of x_offset
//         bit 31~0 - x_offset[31:0] (Read/Write)
// 0x014 : Data signal of x_offset
//         bit 31~0 - x_offset[63:32] (Read/Write)
// 0x018 : reserved
// 0x01c : Data signal of w_gate_offset
//         bit 31~0 - w_gate_offset[31:0] (Read/Write)
// 0x020 : Data signal of w_gate_offset
//         bit 31~0 - w_gate_offset[63:32] (Read/Write)
// 0x024 : reserved
// 0x028 : Data signal of w_up_offset
//         bit 31~0 - w_up_offset[31:0] (Read/Write)
// 0x02c : Data signal of w_up_offset
//         bit 31~0 - w_up_offset[63:32] (Read/Write)
// 0x030 : reserved
// 0x034 : Data signal of w_down_offset
//         bit 31~0 - w_down_offset[31:0] (Read/Write)
// 0x038 : Data signal of w_down_offset
//         bit 31~0 - w_down_offset[63:32] (Read/Write)
// 0x03c : reserved
// 0x040 : Data signal of y_offset
//         bit 31~0 - y_offset[31:0] (Read/Write)
// 0x044 : Data signal of y_offset
//         bit 31~0 - y_offset[63:32] (Read/Write)
// 0x048 : reserved
// 0x04c : Data signal of run_mode
//         bit 31~0 - run_mode[31:0] (Read/Write)
// 0x050 : reserved
// 0x054 : Data signal of valid_rows
//         bit 31~0 - valid_rows[31:0] (Read/Write)
// 0x058 : reserved
// 0x05c : Data signal of hidden_size
//         bit 31~0 - hidden_size[31:0] (Read/Write)
// 0x060 : reserved
// 0x064 : Data signal of intermediate_size
//         bit 31~0 - intermediate_size[31:0] (Read/Write)
// 0x068 : reserved
// 0x06c : Data signal of layout_id
//         bit 31~0 - layout_id[31:0] (Read/Write)
// 0x070 : reserved
// 0x074 : Data signal of abi_version
//         bit 31~0 - abi_version[31:0] (Read/Write)
// 0x078 : reserved
// 0x07c : Data signal of job_id
//         bit 31~0 - job_id[31:0] (Read/Write)
// 0x080 : reserved
// 0x084 : Data signal of flags
//         bit 31~0 - flags[31:0] (Read/Write)
// 0x088 : reserved
// 0x08c : Data signal of result_status_code
//         bit 31~0 - result_status_code[31:0] (Read)
// 0x090 : Control signal of result_status_code
//         bit 0  - result_status_code_ap_vld (Read/COR)
//         others - reserved
// 0x09c : Data signal of result_processed_rows
//         bit 31~0 - result_processed_rows[31:0] (Read)
// 0x0a0 : Control signal of result_processed_rows
//         bit 0  - result_processed_rows_ap_vld (Read/COR)
//         others - reserved
// 0x0ac : Data signal of result_perf_cycles_total
//         bit 31~0 - result_perf_cycles_total[31:0] (Read)
// 0x0b0 : Data signal of result_perf_cycles_total
//         bit 31~0 - result_perf_cycles_total[63:32] (Read)
// 0x0b4 : Control signal of result_perf_cycles_total
//         bit 0  - result_perf_cycles_total_ap_vld (Read/COR)
//         others - reserved
// 0x0c4 : Data signal of result_perf_cycles_compute
//         bit 31~0 - result_perf_cycles_compute[31:0] (Read)
// 0x0c8 : Data signal of result_perf_cycles_compute
//         bit 31~0 - result_perf_cycles_compute[63:32] (Read)
// 0x0cc : Control signal of result_perf_cycles_compute
//         bit 0  - result_perf_cycles_compute_ap_vld (Read/COR)
//         others - reserved
// 0x0dc : Data signal of result_perf_read_beats
//         bit 31~0 - result_perf_read_beats[31:0] (Read)
// 0x0e0 : Data signal of result_perf_read_beats
//         bit 31~0 - result_perf_read_beats[63:32] (Read)
// 0x0e4 : Control signal of result_perf_read_beats
//         bit 0  - result_perf_read_beats_ap_vld (Read/COR)
//         others - reserved
// 0x0f4 : Data signal of result_perf_write_beats
//         bit 31~0 - result_perf_write_beats[31:0] (Read)
// 0x0f8 : Data signal of result_perf_write_beats
//         bit 31~0 - result_perf_write_beats[63:32] (Read)
// 0x0fc : Control signal of result_perf_write_beats
//         bit 0  - result_perf_write_beats_ap_vld (Read/COR)
//         others - reserved
// 0x10c : Data signal of result_output_crc
//         bit 31~0 - result_output_crc[31:0] (Read)
// 0x110 : Control signal of result_output_crc
//         bit 0  - result_output_crc_ap_vld (Read/COR)
//         others - reserved
// 0x11c : Data signal of result_build_config
//         bit 31~0 - result_build_config[31:0] (Read)
// 0x120 : Control signal of result_build_config
//         bit 0  - result_build_config_ap_vld (Read/COR)
//         others - reserved
// 0x12c : Data signal of result_mid_tiles
//         bit 31~0 - result_mid_tiles[31:0] (Read)
// 0x130 : Control signal of result_mid_tiles
//         bit 0  - result_mid_tiles_ap_vld (Read/COR)
//         others - reserved
// 0x13c : Data signal of result_gate_up_active_cycles
//         bit 31~0 - result_gate_up_active_cycles[31:0] (Read)
// 0x140 : Data signal of result_gate_up_active_cycles
//         bit 31~0 - result_gate_up_active_cycles[63:32] (Read)
// 0x144 : Control signal of result_gate_up_active_cycles
//         bit 0  - result_gate_up_active_cycles_ap_vld (Read/COR)
//         others - reserved
// 0x154 : Data signal of result_down_active_cycles
//         bit 31~0 - result_down_active_cycles[31:0] (Read)
// 0x158 : Data signal of result_down_active_cycles
//         bit 31~0 - result_down_active_cycles[63:32] (Read)
// 0x15c : Control signal of result_down_active_cycles
//         bit 0  - result_down_active_cycles_ap_vld (Read/COR)
//         others - reserved
// 0x16c : Data signal of result_weight_wait_cycles
//         bit 31~0 - result_weight_wait_cycles[31:0] (Read)
// 0x170 : Data signal of result_weight_wait_cycles
//         bit 31~0 - result_weight_wait_cycles[63:32] (Read)
// 0x174 : Control signal of result_weight_wait_cycles
//         bit 0  - result_weight_wait_cycles_ap_vld (Read/COR)
//         others - reserved
// 0x184 : Data signal of result_fifo_stall_cycles
//         bit 31~0 - result_fifo_stall_cycles[31:0] (Read)
// 0x188 : Data signal of result_fifo_stall_cycles
//         bit 31~0 - result_fifo_stall_cycles[63:32] (Read)
// 0x18c : Control signal of result_fifo_stall_cycles
//         bit 0  - result_fifo_stall_cycles_ap_vld (Read/COR)
//         others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

//------------------------Parameter----------------------
localparam
    ADDR_AP_CTRL                             = 9'h000,
    ADDR_GIE                                 = 9'h004,
    ADDR_IER                                 = 9'h008,
    ADDR_ISR                                 = 9'h00c,
    ADDR_X_OFFSET_DATA_0                     = 9'h010,
    ADDR_X_OFFSET_DATA_1                     = 9'h014,
    ADDR_X_OFFSET_CTRL                       = 9'h018,
    ADDR_W_GATE_OFFSET_DATA_0                = 9'h01c,
    ADDR_W_GATE_OFFSET_DATA_1                = 9'h020,
    ADDR_W_GATE_OFFSET_CTRL                  = 9'h024,
    ADDR_W_UP_OFFSET_DATA_0                  = 9'h028,
    ADDR_W_UP_OFFSET_DATA_1                  = 9'h02c,
    ADDR_W_UP_OFFSET_CTRL                    = 9'h030,
    ADDR_W_DOWN_OFFSET_DATA_0                = 9'h034,
    ADDR_W_DOWN_OFFSET_DATA_1                = 9'h038,
    ADDR_W_DOWN_OFFSET_CTRL                  = 9'h03c,
    ADDR_Y_OFFSET_DATA_0                     = 9'h040,
    ADDR_Y_OFFSET_DATA_1                     = 9'h044,
    ADDR_Y_OFFSET_CTRL                       = 9'h048,
    ADDR_RUN_MODE_DATA_0                     = 9'h04c,
    ADDR_RUN_MODE_CTRL                       = 9'h050,
    ADDR_VALID_ROWS_DATA_0                   = 9'h054,
    ADDR_VALID_ROWS_CTRL                     = 9'h058,
    ADDR_HIDDEN_SIZE_DATA_0                  = 9'h05c,
    ADDR_HIDDEN_SIZE_CTRL                    = 9'h060,
    ADDR_INTERMEDIATE_SIZE_DATA_0            = 9'h064,
    ADDR_INTERMEDIATE_SIZE_CTRL              = 9'h068,
    ADDR_LAYOUT_ID_DATA_0                    = 9'h06c,
    ADDR_LAYOUT_ID_CTRL                      = 9'h070,
    ADDR_ABI_VERSION_DATA_0                  = 9'h074,
    ADDR_ABI_VERSION_CTRL                    = 9'h078,
    ADDR_JOB_ID_DATA_0                       = 9'h07c,
    ADDR_JOB_ID_CTRL                         = 9'h080,
    ADDR_FLAGS_DATA_0                        = 9'h084,
    ADDR_FLAGS_CTRL                          = 9'h088,
    ADDR_RESULT_STATUS_CODE_DATA_0           = 9'h08c,
    ADDR_RESULT_STATUS_CODE_CTRL             = 9'h090,
    ADDR_RESULT_PROCESSED_ROWS_DATA_0        = 9'h09c,
    ADDR_RESULT_PROCESSED_ROWS_CTRL          = 9'h0a0,
    ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_0     = 9'h0ac,
    ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_1     = 9'h0b0,
    ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL       = 9'h0b4,
    ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_0   = 9'h0c4,
    ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_1   = 9'h0c8,
    ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL     = 9'h0cc,
    ADDR_RESULT_PERF_READ_BEATS_DATA_0       = 9'h0dc,
    ADDR_RESULT_PERF_READ_BEATS_DATA_1       = 9'h0e0,
    ADDR_RESULT_PERF_READ_BEATS_CTRL         = 9'h0e4,
    ADDR_RESULT_PERF_WRITE_BEATS_DATA_0      = 9'h0f4,
    ADDR_RESULT_PERF_WRITE_BEATS_DATA_1      = 9'h0f8,
    ADDR_RESULT_PERF_WRITE_BEATS_CTRL        = 9'h0fc,
    ADDR_RESULT_OUTPUT_CRC_DATA_0            = 9'h10c,
    ADDR_RESULT_OUTPUT_CRC_CTRL              = 9'h110,
    ADDR_RESULT_BUILD_CONFIG_DATA_0          = 9'h11c,
    ADDR_RESULT_BUILD_CONFIG_CTRL            = 9'h120,
    ADDR_RESULT_MID_TILES_DATA_0             = 9'h12c,
    ADDR_RESULT_MID_TILES_CTRL               = 9'h130,
    ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA_0 = 9'h13c,
    ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA_1 = 9'h140,
    ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_CTRL   = 9'h144,
    ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA_0    = 9'h154,
    ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA_1    = 9'h158,
    ADDR_RESULT_DOWN_ACTIVE_CYCLES_CTRL      = 9'h15c,
    ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA_0    = 9'h16c,
    ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA_1    = 9'h170,
    ADDR_RESULT_WEIGHT_WAIT_CYCLES_CTRL      = 9'h174,
    ADDR_RESULT_FIFO_STALL_CYCLES_DATA_0     = 9'h184,
    ADDR_RESULT_FIFO_STALL_CYCLES_DATA_1     = 9'h188,
    ADDR_RESULT_FIFO_STALL_CYCLES_CTRL       = 9'h18c,
    WRIDLE                                   = 2'd0,
    WRDATA                                   = 2'd1,
    WRRESP                                   = 2'd2,
    WRRESET                                  = 2'd3,
    RDIDLE                                   = 2'd0,
    RDDATA                                   = 2'd1,
    RDRESET                                  = 2'd2,
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
    reg  [63:0]                   int_x_offset = 'b0;
    reg  [63:0]                   int_w_gate_offset = 'b0;
    reg  [63:0]                   int_w_up_offset = 'b0;
    reg  [63:0]                   int_w_down_offset = 'b0;
    reg  [63:0]                   int_y_offset = 'b0;
    reg  [31:0]                   int_run_mode = 'b0;
    reg  [31:0]                   int_valid_rows = 'b0;
    reg  [31:0]                   int_hidden_size = 'b0;
    reg  [31:0]                   int_intermediate_size = 'b0;
    reg  [31:0]                   int_layout_id = 'b0;
    reg  [31:0]                   int_abi_version = 'b0;
    reg  [31:0]                   int_job_id = 'b0;
    reg  [31:0]                   int_flags = 'b0;
    reg                           int_result_status_code_ap_vld;
    reg  [31:0]                   int_result_status_code = 'b0;
    reg                           int_result_processed_rows_ap_vld;
    reg  [31:0]                   int_result_processed_rows = 'b0;
    reg                           int_result_perf_cycles_total_ap_vld;
    reg  [63:0]                   int_result_perf_cycles_total = 'b0;
    reg                           int_result_perf_cycles_compute_ap_vld;
    reg  [63:0]                   int_result_perf_cycles_compute = 'b0;
    reg                           int_result_perf_read_beats_ap_vld;
    reg  [63:0]                   int_result_perf_read_beats = 'b0;
    reg                           int_result_perf_write_beats_ap_vld;
    reg  [63:0]                   int_result_perf_write_beats = 'b0;
    reg                           int_result_output_crc_ap_vld;
    reg  [31:0]                   int_result_output_crc = 'b0;
    reg                           int_result_build_config_ap_vld;
    reg  [31:0]                   int_result_build_config = 'b0;
    reg                           int_result_mid_tiles_ap_vld;
    reg  [31:0]                   int_result_mid_tiles = 'b0;
    reg                           int_result_gate_up_active_cycles_ap_vld;
    reg  [63:0]                   int_result_gate_up_active_cycles = 'b0;
    reg                           int_result_down_active_cycles_ap_vld;
    reg  [63:0]                   int_result_down_active_cycles = 'b0;
    reg                           int_result_weight_wait_cycles_ap_vld;
    reg  [63:0]                   int_result_weight_wait_cycles = 'b0;
    reg                           int_result_fifo_stall_cycles_ap_vld;
    reg  [63:0]                   int_result_fifo_stall_cycles = 'b0;

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
                ADDR_X_OFFSET_DATA_0: begin
                    rdata <= int_x_offset[31:0];
                end
                ADDR_X_OFFSET_DATA_1: begin
                    rdata <= int_x_offset[63:32];
                end
                ADDR_W_GATE_OFFSET_DATA_0: begin
                    rdata <= int_w_gate_offset[31:0];
                end
                ADDR_W_GATE_OFFSET_DATA_1: begin
                    rdata <= int_w_gate_offset[63:32];
                end
                ADDR_W_UP_OFFSET_DATA_0: begin
                    rdata <= int_w_up_offset[31:0];
                end
                ADDR_W_UP_OFFSET_DATA_1: begin
                    rdata <= int_w_up_offset[63:32];
                end
                ADDR_W_DOWN_OFFSET_DATA_0: begin
                    rdata <= int_w_down_offset[31:0];
                end
                ADDR_W_DOWN_OFFSET_DATA_1: begin
                    rdata <= int_w_down_offset[63:32];
                end
                ADDR_Y_OFFSET_DATA_0: begin
                    rdata <= int_y_offset[31:0];
                end
                ADDR_Y_OFFSET_DATA_1: begin
                    rdata <= int_y_offset[63:32];
                end
                ADDR_RUN_MODE_DATA_0: begin
                    rdata <= int_run_mode[31:0];
                end
                ADDR_VALID_ROWS_DATA_0: begin
                    rdata <= int_valid_rows[31:0];
                end
                ADDR_HIDDEN_SIZE_DATA_0: begin
                    rdata <= int_hidden_size[31:0];
                end
                ADDR_INTERMEDIATE_SIZE_DATA_0: begin
                    rdata <= int_intermediate_size[31:0];
                end
                ADDR_LAYOUT_ID_DATA_0: begin
                    rdata <= int_layout_id[31:0];
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
                ADDR_RESULT_STATUS_CODE_DATA_0: begin
                    rdata <= int_result_status_code[31:0];
                end
                ADDR_RESULT_STATUS_CODE_CTRL: begin
                    rdata[0] <= int_result_status_code_ap_vld;
                end
                ADDR_RESULT_PROCESSED_ROWS_DATA_0: begin
                    rdata <= int_result_processed_rows[31:0];
                end
                ADDR_RESULT_PROCESSED_ROWS_CTRL: begin
                    rdata[0] <= int_result_processed_rows_ap_vld;
                end
                ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_0: begin
                    rdata <= int_result_perf_cycles_total[31:0];
                end
                ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_1: begin
                    rdata <= int_result_perf_cycles_total[63:32];
                end
                ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL: begin
                    rdata[0] <= int_result_perf_cycles_total_ap_vld;
                end
                ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_0: begin
                    rdata <= int_result_perf_cycles_compute[31:0];
                end
                ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_1: begin
                    rdata <= int_result_perf_cycles_compute[63:32];
                end
                ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL: begin
                    rdata[0] <= int_result_perf_cycles_compute_ap_vld;
                end
                ADDR_RESULT_PERF_READ_BEATS_DATA_0: begin
                    rdata <= int_result_perf_read_beats[31:0];
                end
                ADDR_RESULT_PERF_READ_BEATS_DATA_1: begin
                    rdata <= int_result_perf_read_beats[63:32];
                end
                ADDR_RESULT_PERF_READ_BEATS_CTRL: begin
                    rdata[0] <= int_result_perf_read_beats_ap_vld;
                end
                ADDR_RESULT_PERF_WRITE_BEATS_DATA_0: begin
                    rdata <= int_result_perf_write_beats[31:0];
                end
                ADDR_RESULT_PERF_WRITE_BEATS_DATA_1: begin
                    rdata <= int_result_perf_write_beats[63:32];
                end
                ADDR_RESULT_PERF_WRITE_BEATS_CTRL: begin
                    rdata[0] <= int_result_perf_write_beats_ap_vld;
                end
                ADDR_RESULT_OUTPUT_CRC_DATA_0: begin
                    rdata <= int_result_output_crc[31:0];
                end
                ADDR_RESULT_OUTPUT_CRC_CTRL: begin
                    rdata[0] <= int_result_output_crc_ap_vld;
                end
                ADDR_RESULT_BUILD_CONFIG_DATA_0: begin
                    rdata <= int_result_build_config[31:0];
                end
                ADDR_RESULT_BUILD_CONFIG_CTRL: begin
                    rdata[0] <= int_result_build_config_ap_vld;
                end
                ADDR_RESULT_MID_TILES_DATA_0: begin
                    rdata <= int_result_mid_tiles[31:0];
                end
                ADDR_RESULT_MID_TILES_CTRL: begin
                    rdata[0] <= int_result_mid_tiles_ap_vld;
                end
                ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA_0: begin
                    rdata <= int_result_gate_up_active_cycles[31:0];
                end
                ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA_1: begin
                    rdata <= int_result_gate_up_active_cycles[63:32];
                end
                ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_CTRL: begin
                    rdata[0] <= int_result_gate_up_active_cycles_ap_vld;
                end
                ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA_0: begin
                    rdata <= int_result_down_active_cycles[31:0];
                end
                ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA_1: begin
                    rdata <= int_result_down_active_cycles[63:32];
                end
                ADDR_RESULT_DOWN_ACTIVE_CYCLES_CTRL: begin
                    rdata[0] <= int_result_down_active_cycles_ap_vld;
                end
                ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA_0: begin
                    rdata <= int_result_weight_wait_cycles[31:0];
                end
                ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA_1: begin
                    rdata <= int_result_weight_wait_cycles[63:32];
                end
                ADDR_RESULT_WEIGHT_WAIT_CYCLES_CTRL: begin
                    rdata[0] <= int_result_weight_wait_cycles_ap_vld;
                end
                ADDR_RESULT_FIFO_STALL_CYCLES_DATA_0: begin
                    rdata <= int_result_fifo_stall_cycles[31:0];
                end
                ADDR_RESULT_FIFO_STALL_CYCLES_DATA_1: begin
                    rdata <= int_result_fifo_stall_cycles[63:32];
                end
                ADDR_RESULT_FIFO_STALL_CYCLES_CTRL: begin
                    rdata[0] <= int_result_fifo_stall_cycles_ap_vld;
                end
            endcase
        end
    end
end


//------------------------Register logic-----------------
assign interrupt         = int_gie & (|int_isr);
assign ap_start          = int_ap_start;
assign task_ap_done      = (ap_done && !auto_restart_status) || auto_restart_done;
assign task_ap_ready     = ap_ready && !int_auto_restart;
assign auto_restart_done = auto_restart_status && (ap_idle && !int_ap_idle);
assign x_offset          = int_x_offset;
assign w_gate_offset     = int_w_gate_offset;
assign w_up_offset       = int_w_up_offset;
assign w_down_offset     = int_w_down_offset;
assign y_offset          = int_y_offset;
assign run_mode          = int_run_mode;
assign valid_rows        = int_valid_rows;
assign hidden_size       = int_hidden_size;
assign intermediate_size = int_intermediate_size;
assign layout_id         = int_layout_id;
assign abi_version       = int_abi_version;
assign job_id            = int_job_id;
assign flags             = int_flags;
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

// int_x_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_x_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_X_OFFSET_DATA_0)
            int_x_offset[31:0] <= (WDATA[31:0] & wmask) | (int_x_offset[31:0] & ~wmask);
    end
end

// int_x_offset[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_x_offset[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_X_OFFSET_DATA_1)
            int_x_offset[63:32] <= (WDATA[31:0] & wmask) | (int_x_offset[63:32] & ~wmask);
    end
end

// int_w_gate_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w_gate_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_GATE_OFFSET_DATA_0)
            int_w_gate_offset[31:0] <= (WDATA[31:0] & wmask) | (int_w_gate_offset[31:0] & ~wmask);
    end
end

// int_w_gate_offset[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w_gate_offset[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_GATE_OFFSET_DATA_1)
            int_w_gate_offset[63:32] <= (WDATA[31:0] & wmask) | (int_w_gate_offset[63:32] & ~wmask);
    end
end

// int_w_up_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w_up_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_UP_OFFSET_DATA_0)
            int_w_up_offset[31:0] <= (WDATA[31:0] & wmask) | (int_w_up_offset[31:0] & ~wmask);
    end
end

// int_w_up_offset[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w_up_offset[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_UP_OFFSET_DATA_1)
            int_w_up_offset[63:32] <= (WDATA[31:0] & wmask) | (int_w_up_offset[63:32] & ~wmask);
    end
end

// int_w_down_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w_down_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_DOWN_OFFSET_DATA_0)
            int_w_down_offset[31:0] <= (WDATA[31:0] & wmask) | (int_w_down_offset[31:0] & ~wmask);
    end
end

// int_w_down_offset[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w_down_offset[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_DOWN_OFFSET_DATA_1)
            int_w_down_offset[63:32] <= (WDATA[31:0] & wmask) | (int_w_down_offset[63:32] & ~wmask);
    end
end

// int_y_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_y_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_Y_OFFSET_DATA_0)
            int_y_offset[31:0] <= (WDATA[31:0] & wmask) | (int_y_offset[31:0] & ~wmask);
    end
end

// int_y_offset[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_y_offset[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_Y_OFFSET_DATA_1)
            int_y_offset[63:32] <= (WDATA[31:0] & wmask) | (int_y_offset[63:32] & ~wmask);
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

// int_hidden_size[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_hidden_size[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_HIDDEN_SIZE_DATA_0)
            int_hidden_size[31:0] <= (WDATA[31:0] & wmask) | (int_hidden_size[31:0] & ~wmask);
    end
end

// int_intermediate_size[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_intermediate_size[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_INTERMEDIATE_SIZE_DATA_0)
            int_intermediate_size[31:0] <= (WDATA[31:0] & wmask) | (int_intermediate_size[31:0] & ~wmask);
    end
end

// int_layout_id[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_layout_id[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_LAYOUT_ID_DATA_0)
            int_layout_id[31:0] <= (WDATA[31:0] & wmask) | (int_layout_id[31:0] & ~wmask);
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

// int_result_status_code
always @(posedge ACLK) begin
    if (ARESET)
        int_result_status_code <= 0;
    else if (ACLK_EN) begin
        if (result_status_code_ap_vld)
            int_result_status_code <= result_status_code;
    end
end

// int_result_status_code_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_status_code_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_status_code_ap_vld)
            int_result_status_code_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_STATUS_CODE_CTRL)
            int_result_status_code_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_processed_rows
always @(posedge ACLK) begin
    if (ARESET)
        int_result_processed_rows <= 0;
    else if (ACLK_EN) begin
        if (result_processed_rows_ap_vld)
            int_result_processed_rows <= result_processed_rows;
    end
end

// int_result_processed_rows_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_processed_rows_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_processed_rows_ap_vld)
            int_result_processed_rows_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_PROCESSED_ROWS_CTRL)
            int_result_processed_rows_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_perf_cycles_total
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_cycles_total <= 0;
    else if (ACLK_EN) begin
        if (result_perf_cycles_total_ap_vld)
            int_result_perf_cycles_total <= result_perf_cycles_total;
    end
end

// int_result_perf_cycles_total_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_cycles_total_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_perf_cycles_total_ap_vld)
            int_result_perf_cycles_total_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL)
            int_result_perf_cycles_total_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_perf_cycles_compute
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_cycles_compute <= 0;
    else if (ACLK_EN) begin
        if (result_perf_cycles_compute_ap_vld)
            int_result_perf_cycles_compute <= result_perf_cycles_compute;
    end
end

// int_result_perf_cycles_compute_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_cycles_compute_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_perf_cycles_compute_ap_vld)
            int_result_perf_cycles_compute_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL)
            int_result_perf_cycles_compute_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_perf_read_beats
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_read_beats <= 0;
    else if (ACLK_EN) begin
        if (result_perf_read_beats_ap_vld)
            int_result_perf_read_beats <= result_perf_read_beats;
    end
end

// int_result_perf_read_beats_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_read_beats_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_perf_read_beats_ap_vld)
            int_result_perf_read_beats_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_PERF_READ_BEATS_CTRL)
            int_result_perf_read_beats_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_perf_write_beats
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_write_beats <= 0;
    else if (ACLK_EN) begin
        if (result_perf_write_beats_ap_vld)
            int_result_perf_write_beats <= result_perf_write_beats;
    end
end

// int_result_perf_write_beats_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_perf_write_beats_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_perf_write_beats_ap_vld)
            int_result_perf_write_beats_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_PERF_WRITE_BEATS_CTRL)
            int_result_perf_write_beats_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_output_crc
always @(posedge ACLK) begin
    if (ARESET)
        int_result_output_crc <= 0;
    else if (ACLK_EN) begin
        if (result_output_crc_ap_vld)
            int_result_output_crc <= result_output_crc;
    end
end

// int_result_output_crc_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_output_crc_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_output_crc_ap_vld)
            int_result_output_crc_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_OUTPUT_CRC_CTRL)
            int_result_output_crc_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_build_config
always @(posedge ACLK) begin
    if (ARESET)
        int_result_build_config <= 0;
    else if (ACLK_EN) begin
        if (result_build_config_ap_vld)
            int_result_build_config <= result_build_config;
    end
end

// int_result_build_config_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_build_config_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_build_config_ap_vld)
            int_result_build_config_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_BUILD_CONFIG_CTRL)
            int_result_build_config_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_mid_tiles
always @(posedge ACLK) begin
    if (ARESET)
        int_result_mid_tiles <= 0;
    else if (ACLK_EN) begin
        if (result_mid_tiles_ap_vld)
            int_result_mid_tiles <= result_mid_tiles;
    end
end

// int_result_mid_tiles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_mid_tiles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_mid_tiles_ap_vld)
            int_result_mid_tiles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_MID_TILES_CTRL)
            int_result_mid_tiles_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_gate_up_active_cycles
always @(posedge ACLK) begin
    if (ARESET)
        int_result_gate_up_active_cycles <= 0;
    else if (ACLK_EN) begin
        if (result_gate_up_active_cycles_ap_vld)
            int_result_gate_up_active_cycles <= result_gate_up_active_cycles;
    end
end

// int_result_gate_up_active_cycles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_gate_up_active_cycles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_gate_up_active_cycles_ap_vld)
            int_result_gate_up_active_cycles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_CTRL)
            int_result_gate_up_active_cycles_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_down_active_cycles
always @(posedge ACLK) begin
    if (ARESET)
        int_result_down_active_cycles <= 0;
    else if (ACLK_EN) begin
        if (result_down_active_cycles_ap_vld)
            int_result_down_active_cycles <= result_down_active_cycles;
    end
end

// int_result_down_active_cycles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_down_active_cycles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_down_active_cycles_ap_vld)
            int_result_down_active_cycles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_DOWN_ACTIVE_CYCLES_CTRL)
            int_result_down_active_cycles_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_weight_wait_cycles
always @(posedge ACLK) begin
    if (ARESET)
        int_result_weight_wait_cycles <= 0;
    else if (ACLK_EN) begin
        if (result_weight_wait_cycles_ap_vld)
            int_result_weight_wait_cycles <= result_weight_wait_cycles;
    end
end

// int_result_weight_wait_cycles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_weight_wait_cycles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_weight_wait_cycles_ap_vld)
            int_result_weight_wait_cycles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_WEIGHT_WAIT_CYCLES_CTRL)
            int_result_weight_wait_cycles_ap_vld <= 1'b0; // clear on read
    end
end

// int_result_fifo_stall_cycles
always @(posedge ACLK) begin
    if (ARESET)
        int_result_fifo_stall_cycles <= 0;
    else if (ACLK_EN) begin
        if (result_fifo_stall_cycles_ap_vld)
            int_result_fifo_stall_cycles <= result_fifo_stall_cycles;
    end
end

// int_result_fifo_stall_cycles_ap_vld
always @(posedge ACLK) begin
    if (ARESET)
        int_result_fifo_stall_cycles_ap_vld <= 1'b0;
    else if (ACLK_EN) begin
        if (result_fifo_stall_cycles_ap_vld)
            int_result_fifo_stall_cycles_ap_vld <= 1'b1;
        else if (ar_hs && raddr == ADDR_RESULT_FIFO_STALL_CYCLES_CTRL)
            int_result_fifo_stall_cycles_ap_vld <= 1'b0; // clear on read
    end
end


//------------------------Memory logic-------------------

endmodule
