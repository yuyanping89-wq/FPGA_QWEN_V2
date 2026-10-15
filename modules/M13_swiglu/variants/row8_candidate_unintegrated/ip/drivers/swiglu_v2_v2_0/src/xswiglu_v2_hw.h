// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
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

#define XSWIGLU_V2_CONTROL_ADDR_AP_CTRL                           0x000
#define XSWIGLU_V2_CONTROL_ADDR_GIE                               0x004
#define XSWIGLU_V2_CONTROL_ADDR_IER                               0x008
#define XSWIGLU_V2_CONTROL_ADDR_ISR                               0x00c
#define XSWIGLU_V2_CONTROL_ADDR_X_OFFSET_DATA                     0x010
#define XSWIGLU_V2_CONTROL_BITS_X_OFFSET_DATA                     64
#define XSWIGLU_V2_CONTROL_ADDR_W_GATE_OFFSET_DATA                0x01c
#define XSWIGLU_V2_CONTROL_BITS_W_GATE_OFFSET_DATA                64
#define XSWIGLU_V2_CONTROL_ADDR_W_UP_OFFSET_DATA                  0x028
#define XSWIGLU_V2_CONTROL_BITS_W_UP_OFFSET_DATA                  64
#define XSWIGLU_V2_CONTROL_ADDR_W_DOWN_OFFSET_DATA                0x034
#define XSWIGLU_V2_CONTROL_BITS_W_DOWN_OFFSET_DATA                64
#define XSWIGLU_V2_CONTROL_ADDR_Y_OFFSET_DATA                     0x040
#define XSWIGLU_V2_CONTROL_BITS_Y_OFFSET_DATA                     64
#define XSWIGLU_V2_CONTROL_ADDR_RUN_MODE_DATA                     0x04c
#define XSWIGLU_V2_CONTROL_BITS_RUN_MODE_DATA                     32
#define XSWIGLU_V2_CONTROL_ADDR_VALID_ROWS_DATA                   0x054
#define XSWIGLU_V2_CONTROL_BITS_VALID_ROWS_DATA                   32
#define XSWIGLU_V2_CONTROL_ADDR_HIDDEN_SIZE_DATA                  0x05c
#define XSWIGLU_V2_CONTROL_BITS_HIDDEN_SIZE_DATA                  32
#define XSWIGLU_V2_CONTROL_ADDR_INTERMEDIATE_SIZE_DATA            0x064
#define XSWIGLU_V2_CONTROL_BITS_INTERMEDIATE_SIZE_DATA            32
#define XSWIGLU_V2_CONTROL_ADDR_LAYOUT_ID_DATA                    0x06c
#define XSWIGLU_V2_CONTROL_BITS_LAYOUT_ID_DATA                    32
#define XSWIGLU_V2_CONTROL_ADDR_ABI_VERSION_DATA                  0x074
#define XSWIGLU_V2_CONTROL_BITS_ABI_VERSION_DATA                  32
#define XSWIGLU_V2_CONTROL_ADDR_JOB_ID_DATA                       0x07c
#define XSWIGLU_V2_CONTROL_BITS_JOB_ID_DATA                       32
#define XSWIGLU_V2_CONTROL_ADDR_FLAGS_DATA                        0x084
#define XSWIGLU_V2_CONTROL_BITS_FLAGS_DATA                        32
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_STATUS_CODE_DATA           0x08c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_STATUS_CODE_DATA           32
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL           0x090
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA        0x09c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_PROCESSED_ROWS_DATA        32
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL        0x0a0
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA     0x0ac
#define XSWIGLU_V2_CONTROL_BITS_RESULT_PERF_CYCLES_TOTAL_DATA     64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL     0x0b4
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA   0x0c4
#define XSWIGLU_V2_CONTROL_BITS_RESULT_PERF_CYCLES_COMPUTE_DATA   64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL   0x0cc
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA       0x0dc
#define XSWIGLU_V2_CONTROL_BITS_RESULT_PERF_READ_BEATS_DATA       64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL       0x0e4
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA      0x0f4
#define XSWIGLU_V2_CONTROL_BITS_RESULT_PERF_WRITE_BEATS_DATA      64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL      0x0fc
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA            0x10c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_OUTPUT_CRC_DATA            32
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL            0x110
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA          0x11c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_BUILD_CONFIG_DATA          32
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL          0x120
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_MID_TILES_DATA             0x12c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_MID_TILES_DATA             32
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_MID_TILES_CTRL             0x130
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA 0x13c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_GATE_UP_ACTIVE_CYCLES_DATA 64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_CTRL 0x144
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA    0x154
#define XSWIGLU_V2_CONTROL_BITS_RESULT_DOWN_ACTIVE_CYCLES_DATA    64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_DOWN_ACTIVE_CYCLES_CTRL    0x15c
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA    0x16c
#define XSWIGLU_V2_CONTROL_BITS_RESULT_WEIGHT_WAIT_CYCLES_DATA    64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_WEIGHT_WAIT_CYCLES_CTRL    0x174
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_FIFO_STALL_CYCLES_DATA     0x184
#define XSWIGLU_V2_CONTROL_BITS_RESULT_FIFO_STALL_CYCLES_DATA     64
#define XSWIGLU_V2_CONTROL_ADDR_RESULT_FIFO_STALL_CYCLES_CTRL     0x18c

