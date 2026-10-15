// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// ctrl
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
// 0x010 : Data signal of token_ids
//         bit 31~0 - token_ids[31:0] (Read/Write)
// 0x014 : Data signal of token_ids
//         bit 31~0 - token_ids[63:32] (Read/Write)
// 0x018 : reserved
// 0x01c : Data signal of table_r
//         bit 31~0 - table_r[31:0] (Read/Write)
// 0x020 : Data signal of table_r
//         bit 31~0 - table_r[63:32] (Read/Write)
// 0x024 : reserved
// 0x028 : Data signal of hidden_out
//         bit 31~0 - hidden_out[31:0] (Read/Write)
// 0x02c : Data signal of hidden_out
//         bit 31~0 - hidden_out[63:32] (Read/Write)
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
// 0x04c : Data signal of vocab_size
//         bit 31~0 - vocab_size[31:0] (Read/Write)
// 0x050 : reserved
// 0x054 : Data signal of abi_version
//         bit 31~0 - abi_version[31:0] (Read/Write)
// 0x058 : reserved
// 0x05c : Data signal of job_id
//         bit 31~0 - job_id[31:0] (Read/Write)
// 0x060 : reserved
// 0x064 : Data signal of flags
//         bit 31~0 - flags[31:0] (Read/Write)
// 0x068 : reserved
// 0x06c : Data signal of result_status_code
//         bit 31~0 - result_status_code[31:0] (Read)
// 0x070 : Control signal of result_status_code
//         bit 0  - result_status_code_ap_vld (Read/COR)
//         others - reserved
// 0x07c : Data signal of result_processed_rows
//         bit 31~0 - result_processed_rows[31:0] (Read)
// 0x080 : Control signal of result_processed_rows
//         bit 0  - result_processed_rows_ap_vld (Read/COR)
//         others - reserved
// 0x08c : Data signal of result_perf_cycles_total
//         bit 31~0 - result_perf_cycles_total[31:0] (Read)
// 0x090 : Data signal of result_perf_cycles_total
//         bit 31~0 - result_perf_cycles_total[63:32] (Read)
// 0x094 : Control signal of result_perf_cycles_total
//         bit 0  - result_perf_cycles_total_ap_vld (Read/COR)
//         others - reserved
// 0x0a4 : Data signal of result_perf_cycles_compute
//         bit 31~0 - result_perf_cycles_compute[31:0] (Read)
// 0x0a8 : Data signal of result_perf_cycles_compute
//         bit 31~0 - result_perf_cycles_compute[63:32] (Read)
// 0x0ac : Control signal of result_perf_cycles_compute
//         bit 0  - result_perf_cycles_compute_ap_vld (Read/COR)
//         others - reserved
// 0x0bc : Data signal of result_perf_read_beats
//         bit 31~0 - result_perf_read_beats[31:0] (Read)
// 0x0c0 : Data signal of result_perf_read_beats
//         bit 31~0 - result_perf_read_beats[63:32] (Read)
// 0x0c4 : Control signal of result_perf_read_beats
//         bit 0  - result_perf_read_beats_ap_vld (Read/COR)
//         others - reserved
// 0x0d4 : Data signal of result_perf_write_beats
//         bit 31~0 - result_perf_write_beats[31:0] (Read)
// 0x0d8 : Data signal of result_perf_write_beats
//         bit 31~0 - result_perf_write_beats[63:32] (Read)
// 0x0dc : Control signal of result_perf_write_beats
//         bit 0  - result_perf_write_beats_ap_vld (Read/COR)
//         others - reserved
// 0x0ec : Data signal of result_output_crc
//         bit 31~0 - result_output_crc[31:0] (Read)
// 0x0f0 : Control signal of result_output_crc
//         bit 0  - result_output_crc_ap_vld (Read/COR)
//         others - reserved
// 0x0fc : Data signal of result_build_config
//         bit 31~0 - result_build_config[31:0] (Read)
// 0x100 : Control signal of result_build_config
//         bit 0  - result_build_config_ap_vld (Read/COR)
//         others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XEMBEDDING_V2_CTRL_ADDR_AP_CTRL                         0x000
#define XEMBEDDING_V2_CTRL_ADDR_GIE                             0x004
#define XEMBEDDING_V2_CTRL_ADDR_IER                             0x008
#define XEMBEDDING_V2_CTRL_ADDR_ISR                             0x00c
#define XEMBEDDING_V2_CTRL_ADDR_TOKEN_IDS_DATA                  0x010
#define XEMBEDDING_V2_CTRL_BITS_TOKEN_IDS_DATA                  64
#define XEMBEDDING_V2_CTRL_ADDR_TABLE_R_DATA                    0x01c
#define XEMBEDDING_V2_CTRL_BITS_TABLE_R_DATA                    64
#define XEMBEDDING_V2_CTRL_ADDR_HIDDEN_OUT_DATA                 0x028
#define XEMBEDDING_V2_CTRL_BITS_HIDDEN_OUT_DATA                 64
#define XEMBEDDING_V2_CTRL_ADDR_RUN_MODE_DATA                   0x034
#define XEMBEDDING_V2_CTRL_BITS_RUN_MODE_DATA                   32
#define XEMBEDDING_V2_CTRL_ADDR_VALID_ROWS_DATA                 0x03c
#define XEMBEDDING_V2_CTRL_BITS_VALID_ROWS_DATA                 32
#define XEMBEDDING_V2_CTRL_ADDR_HIDDEN_SIZE_DATA                0x044
#define XEMBEDDING_V2_CTRL_BITS_HIDDEN_SIZE_DATA                32
#define XEMBEDDING_V2_CTRL_ADDR_VOCAB_SIZE_DATA                 0x04c
#define XEMBEDDING_V2_CTRL_BITS_VOCAB_SIZE_DATA                 32
#define XEMBEDDING_V2_CTRL_ADDR_ABI_VERSION_DATA                0x054
#define XEMBEDDING_V2_CTRL_BITS_ABI_VERSION_DATA                32
#define XEMBEDDING_V2_CTRL_ADDR_JOB_ID_DATA                     0x05c
#define XEMBEDDING_V2_CTRL_BITS_JOB_ID_DATA                     32
#define XEMBEDDING_V2_CTRL_ADDR_FLAGS_DATA                      0x064
#define XEMBEDDING_V2_CTRL_BITS_FLAGS_DATA                      32
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_STATUS_CODE_DATA         0x06c
#define XEMBEDDING_V2_CTRL_BITS_RESULT_STATUS_CODE_DATA         32
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_STATUS_CODE_CTRL         0x070
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PROCESSED_ROWS_DATA      0x07c
#define XEMBEDDING_V2_CTRL_BITS_RESULT_PROCESSED_ROWS_DATA      32
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PROCESSED_ROWS_CTRL      0x080
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA   0x08c
#define XEMBEDDING_V2_CTRL_BITS_RESULT_PERF_CYCLES_TOTAL_DATA   64
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL   0x094
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA 0x0a4
#define XEMBEDDING_V2_CTRL_BITS_RESULT_PERF_CYCLES_COMPUTE_DATA 64
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL 0x0ac
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_READ_BEATS_DATA     0x0bc
#define XEMBEDDING_V2_CTRL_BITS_RESULT_PERF_READ_BEATS_DATA     64
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_READ_BEATS_CTRL     0x0c4
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_WRITE_BEATS_DATA    0x0d4
#define XEMBEDDING_V2_CTRL_BITS_RESULT_PERF_WRITE_BEATS_DATA    64
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL    0x0dc
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_OUTPUT_CRC_DATA          0x0ec
#define XEMBEDDING_V2_CTRL_BITS_RESULT_OUTPUT_CRC_DATA          32
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_OUTPUT_CRC_CTRL          0x0f0
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_BUILD_CONFIG_DATA        0x0fc
#define XEMBEDDING_V2_CTRL_BITS_RESULT_BUILD_CONFIG_DATA        32
#define XEMBEDDING_V2_CTRL_ADDR_RESULT_BUILD_CONFIG_CTRL        0x100

