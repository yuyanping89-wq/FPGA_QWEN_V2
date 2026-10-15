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
// 0x010 : Data signal of q_raw
//         bit 31~0 - q_raw[31:0] (Read/Write)
// 0x014 : Data signal of q_raw
//         bit 31~0 - q_raw[63:32] (Read/Write)
// 0x018 : reserved
// 0x01c : Data signal of k_raw
//         bit 31~0 - k_raw[31:0] (Read/Write)
// 0x020 : Data signal of k_raw
//         bit 31~0 - k_raw[63:32] (Read/Write)
// 0x024 : reserved
// 0x028 : Data signal of cos_table
//         bit 31~0 - cos_table[31:0] (Read/Write)
// 0x02c : Data signal of cos_table
//         bit 31~0 - cos_table[63:32] (Read/Write)
// 0x030 : reserved
// 0x034 : Data signal of sin_table
//         bit 31~0 - sin_table[31:0] (Read/Write)
// 0x038 : Data signal of sin_table
//         bit 31~0 - sin_table[63:32] (Read/Write)
// 0x03c : reserved
// 0x040 : Data signal of q_rot
//         bit 31~0 - q_rot[31:0] (Read/Write)
// 0x044 : Data signal of q_rot
//         bit 31~0 - q_rot[63:32] (Read/Write)
// 0x048 : reserved
// 0x04c : Data signal of k_rot
//         bit 31~0 - k_rot[31:0] (Read/Write)
// 0x050 : Data signal of k_rot
//         bit 31~0 - k_rot[63:32] (Read/Write)
// 0x054 : reserved
// 0x058 : Data signal of run_mode
//         bit 31~0 - run_mode[31:0] (Read/Write)
// 0x05c : reserved
// 0x060 : Data signal of valid_rows
//         bit 31~0 - valid_rows[31:0] (Read/Write)
// 0x064 : reserved
// 0x068 : Data signal of position_base
//         bit 31~0 - position_base[31:0] (Read/Write)
// 0x06c : reserved
// 0x070 : Data signal of max_position
//         bit 31~0 - max_position[31:0] (Read/Write)
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
// 0x090 : Data signal of result_status_code
//         bit 31~0 - result_status_code[31:0] (Read)
// 0x094 : Control signal of result_status_code
//         bit 0  - result_status_code_ap_vld (Read/COR)
//         others - reserved
// 0x0a0 : Data signal of result_processed_rows
//         bit 31~0 - result_processed_rows[31:0] (Read)
// 0x0a4 : Control signal of result_processed_rows
//         bit 0  - result_processed_rows_ap_vld (Read/COR)
//         others - reserved
// 0x0b0 : Data signal of result_perf_cycles_total
//         bit 31~0 - result_perf_cycles_total[31:0] (Read)
// 0x0b4 : Data signal of result_perf_cycles_total
//         bit 31~0 - result_perf_cycles_total[63:32] (Read)
// 0x0b8 : Control signal of result_perf_cycles_total
//         bit 0  - result_perf_cycles_total_ap_vld (Read/COR)
//         others - reserved
// 0x0c8 : Data signal of result_perf_cycles_compute
//         bit 31~0 - result_perf_cycles_compute[31:0] (Read)
// 0x0cc : Data signal of result_perf_cycles_compute
//         bit 31~0 - result_perf_cycles_compute[63:32] (Read)
// 0x0d0 : Control signal of result_perf_cycles_compute
//         bit 0  - result_perf_cycles_compute_ap_vld (Read/COR)
//         others - reserved
// 0x0e0 : Data signal of result_perf_read_beats
//         bit 31~0 - result_perf_read_beats[31:0] (Read)
// 0x0e4 : Data signal of result_perf_read_beats
//         bit 31~0 - result_perf_read_beats[63:32] (Read)
// 0x0e8 : Control signal of result_perf_read_beats
//         bit 0  - result_perf_read_beats_ap_vld (Read/COR)
//         others - reserved
// 0x0f8 : Data signal of result_perf_write_beats
//         bit 31~0 - result_perf_write_beats[31:0] (Read)
// 0x0fc : Data signal of result_perf_write_beats
//         bit 31~0 - result_perf_write_beats[63:32] (Read)
// 0x100 : Control signal of result_perf_write_beats
//         bit 0  - result_perf_write_beats_ap_vld (Read/COR)
//         others - reserved
// 0x110 : Data signal of result_output_crc
//         bit 31~0 - result_output_crc[31:0] (Read)
// 0x114 : Control signal of result_output_crc
//         bit 0  - result_output_crc_ap_vld (Read/COR)
//         others - reserved
// 0x120 : Data signal of result_build_config
//         bit 31~0 - result_build_config[31:0] (Read)
// 0x124 : Control signal of result_build_config
//         bit 0  - result_build_config_ap_vld (Read/COR)
//         others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL                         0x000
#define XROPE_V2_HLS_CONTROL_ADDR_GIE                             0x004
#define XROPE_V2_HLS_CONTROL_ADDR_IER                             0x008
#define XROPE_V2_HLS_CONTROL_ADDR_ISR                             0x00c
#define XROPE_V2_HLS_CONTROL_ADDR_Q_RAW_DATA                      0x010
#define XROPE_V2_HLS_CONTROL_BITS_Q_RAW_DATA                      64
#define XROPE_V2_HLS_CONTROL_ADDR_K_RAW_DATA                      0x01c
#define XROPE_V2_HLS_CONTROL_BITS_K_RAW_DATA                      64
#define XROPE_V2_HLS_CONTROL_ADDR_COS_TABLE_DATA                  0x028
#define XROPE_V2_HLS_CONTROL_BITS_COS_TABLE_DATA                  64
#define XROPE_V2_HLS_CONTROL_ADDR_SIN_TABLE_DATA                  0x034
#define XROPE_V2_HLS_CONTROL_BITS_SIN_TABLE_DATA                  64
#define XROPE_V2_HLS_CONTROL_ADDR_Q_ROT_DATA                      0x040
#define XROPE_V2_HLS_CONTROL_BITS_Q_ROT_DATA                      64
#define XROPE_V2_HLS_CONTROL_ADDR_K_ROT_DATA                      0x04c
#define XROPE_V2_HLS_CONTROL_BITS_K_ROT_DATA                      64
#define XROPE_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA                   0x058
#define XROPE_V2_HLS_CONTROL_BITS_RUN_MODE_DATA                   32
#define XROPE_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA                 0x060
#define XROPE_V2_HLS_CONTROL_BITS_VALID_ROWS_DATA                 32
#define XROPE_V2_HLS_CONTROL_ADDR_POSITION_BASE_DATA              0x068
#define XROPE_V2_HLS_CONTROL_BITS_POSITION_BASE_DATA              32
#define XROPE_V2_HLS_CONTROL_ADDR_MAX_POSITION_DATA               0x070
#define XROPE_V2_HLS_CONTROL_BITS_MAX_POSITION_DATA               32
#define XROPE_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA                0x078
#define XROPE_V2_HLS_CONTROL_BITS_ABI_VERSION_DATA                32
#define XROPE_V2_HLS_CONTROL_ADDR_JOB_ID_DATA                     0x080
#define XROPE_V2_HLS_CONTROL_BITS_JOB_ID_DATA                     32
#define XROPE_V2_HLS_CONTROL_ADDR_FLAGS_DATA                      0x088
#define XROPE_V2_HLS_CONTROL_BITS_FLAGS_DATA                      32
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_DATA         0x090
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_STATUS_CODE_DATA         32
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL         0x094
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA      0x0a0
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_PROCESSED_ROWS_DATA      32
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL      0x0a4
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA   0x0b0
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_PERF_CYCLES_TOTAL_DATA   64
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL   0x0b8
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA 0x0c8
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_PERF_CYCLES_COMPUTE_DATA 64
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL 0x0d0
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA     0x0e0
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_PERF_READ_BEATS_DATA     64
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL     0x0e8
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA    0x0f8
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_PERF_WRITE_BEATS_DATA    64
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL    0x100
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA          0x110
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_OUTPUT_CRC_DATA          32
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL          0x114
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA        0x120
#define XROPE_V2_HLS_CONTROL_BITS_RESULT_BUILD_CONFIG_DATA        32
#define XROPE_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL        0x124

