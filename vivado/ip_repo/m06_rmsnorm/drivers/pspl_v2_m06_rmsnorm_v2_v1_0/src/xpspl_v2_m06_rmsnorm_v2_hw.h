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
// 0x010 : Data signal of x
//         bit 31~0 - x[31:0] (Read/Write)
// 0x014 : Data signal of x
//         bit 31~0 - x[63:32] (Read/Write)
// 0x018 : reserved
// 0x01c : Data signal of weight
//         bit 31~0 - weight[31:0] (Read/Write)
// 0x020 : Data signal of weight
//         bit 31~0 - weight[63:32] (Read/Write)
// 0x024 : reserved
// 0x028 : Data signal of y
//         bit 31~0 - y[31:0] (Read/Write)
// 0x02c : Data signal of y
//         bit 31~0 - y[63:32] (Read/Write)
// 0x030 : reserved
// 0x034 : Data signal of run_mode
//         bit 31~0 - run_mode[31:0] (Read/Write)
// 0x038 : reserved
// 0x03c : Data signal of valid_rows
//         bit 31~0 - valid_rows[31:0] (Read/Write)
// 0x040 : reserved
// 0x044 : Data signal of hidden_size
//         bit 31~0 - hidden_size[31:0] (Read/Write)
// 0x048 : reserved
// 0x04c : Data signal of eps
//         bit 31~0 - eps[31:0] (Read/Write)
// 0x050 : reserved
// 0x054 : Data signal of norm_role
//         bit 31~0 - norm_role[31:0] (Read/Write)
// 0x058 : reserved
// 0x05c : Data signal of abi_version
//         bit 31~0 - abi_version[31:0] (Read/Write)
// 0x060 : reserved
// 0x064 : Data signal of job_id
//         bit 31~0 - job_id[31:0] (Read/Write)
// 0x068 : reserved
// 0x06c : Data signal of flags
//         bit 31~0 - flags[31:0] (Read/Write)
// 0x070 : reserved
// 0x074 : Data signal of status_code
//         bit 31~0 - status_code[31:0] (Read)
// 0x078 : Control signal of status_code
//         bit 0  - status_code_ap_vld (Read/COR)
//         others - reserved
// 0x084 : Data signal of processed_rows
//         bit 31~0 - processed_rows[31:0] (Read)
// 0x088 : Control signal of processed_rows
//         bit 0  - processed_rows_ap_vld (Read/COR)
//         others - reserved
// 0x094 : Data signal of perf_cycles_total
//         bit 31~0 - perf_cycles_total[31:0] (Read)
// 0x098 : Data signal of perf_cycles_total
//         bit 31~0 - perf_cycles_total[63:32] (Read)
// 0x09c : Control signal of perf_cycles_total
//         bit 0  - perf_cycles_total_ap_vld (Read/COR)
//         others - reserved
// 0x0ac : Data signal of perf_cycles_compute
//         bit 31~0 - perf_cycles_compute[31:0] (Read)
// 0x0b0 : Data signal of perf_cycles_compute
//         bit 31~0 - perf_cycles_compute[63:32] (Read)
// 0x0b4 : Control signal of perf_cycles_compute
//         bit 0  - perf_cycles_compute_ap_vld (Read/COR)
//         others - reserved
// 0x0c4 : Data signal of perf_read_beats
//         bit 31~0 - perf_read_beats[31:0] (Read)
// 0x0c8 : Data signal of perf_read_beats
//         bit 31~0 - perf_read_beats[63:32] (Read)
// 0x0cc : Control signal of perf_read_beats
//         bit 0  - perf_read_beats_ap_vld (Read/COR)
//         others - reserved
// 0x0dc : Data signal of perf_write_beats
//         bit 31~0 - perf_write_beats[31:0] (Read)
// 0x0e0 : Data signal of perf_write_beats
//         bit 31~0 - perf_write_beats[63:32] (Read)
// 0x0e4 : Control signal of perf_write_beats
//         bit 0  - perf_write_beats_ap_vld (Read/COR)
//         others - reserved
// 0x0f4 : Data signal of output_crc
//         bit 31~0 - output_crc[31:0] (Read)
// 0x0f8 : Control signal of output_crc
//         bit 0  - output_crc_ap_vld (Read/COR)
//         others - reserved
// 0x104 : Data signal of build_config
//         bit 31~0 - build_config[31:0] (Read)
// 0x108 : Control signal of build_config
//         bit 0  - build_config_ap_vld (Read/COR)
//         others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL                  0x000
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_GIE                      0x004
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_IER                      0x008
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_ISR                      0x00c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_X_DATA                   0x010
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_X_DATA                   64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_WEIGHT_DATA              0x01c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_WEIGHT_DATA              64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_Y_DATA                   0x028
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_Y_DATA                   64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_RUN_MODE_DATA            0x034
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_RUN_MODE_DATA            32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_VALID_ROWS_DATA          0x03c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_VALID_ROWS_DATA          32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_HIDDEN_SIZE_DATA         0x044
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_HIDDEN_SIZE_DATA         32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_EPS_DATA                 0x04c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_EPS_DATA                 32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_NORM_ROLE_DATA           0x054
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_NORM_ROLE_DATA           32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_ABI_VERSION_DATA         0x05c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_ABI_VERSION_DATA         32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_JOB_ID_DATA              0x064
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_JOB_ID_DATA              32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_FLAGS_DATA               0x06c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_FLAGS_DATA               32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_STATUS_CODE_DATA         0x074
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_STATUS_CODE_DATA         32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_STATUS_CODE_CTRL         0x078
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PROCESSED_ROWS_DATA      0x084
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_PROCESSED_ROWS_DATA      32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PROCESSED_ROWS_CTRL      0x088
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_DATA   0x094
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_PERF_CYCLES_TOTAL_DATA   64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_CTRL   0x09c
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_DATA 0x0ac
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_PERF_CYCLES_COMPUTE_DATA 64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_CTRL 0x0b4
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_READ_BEATS_DATA     0x0c4
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_PERF_READ_BEATS_DATA     64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_READ_BEATS_CTRL     0x0cc
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_WRITE_BEATS_DATA    0x0dc
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_PERF_WRITE_BEATS_DATA    64
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_WRITE_BEATS_CTRL    0x0e4
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_OUTPUT_CRC_DATA          0x0f4
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_OUTPUT_CRC_DATA          32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_OUTPUT_CRC_CTRL          0x0f8
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_BUILD_CONFIG_DATA        0x104
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_BITS_BUILD_CONFIG_DATA        32
#define XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_BUILD_CONFIG_CTRL        0x108

