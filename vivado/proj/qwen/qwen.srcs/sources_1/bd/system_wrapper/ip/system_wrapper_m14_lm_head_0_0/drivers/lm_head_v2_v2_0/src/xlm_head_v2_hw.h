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

#define XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL                     0x000
#define XLM_HEAD_V2_CONTROL_ADDR_GIE                         0x004
#define XLM_HEAD_V2_CONTROL_ADDR_IER                         0x008
#define XLM_HEAD_V2_CONTROL_ADDR_ISR                         0x00c
#define XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_OFFSET_DATA          0x010
#define XLM_HEAD_V2_CONTROL_BITS_HIDDEN_OFFSET_DATA          64
#define XLM_HEAD_V2_CONTROL_ADDR_SHARED_VOCAB_WEIGHT_DATA    0x01c
#define XLM_HEAD_V2_CONTROL_BITS_SHARED_VOCAB_WEIGHT_DATA    64
#define XLM_HEAD_V2_CONTROL_ADDR_HISTORY_BITMAP_DATA         0x028
#define XLM_HEAD_V2_CONTROL_BITS_HISTORY_BITMAP_DATA         64
#define XLM_HEAD_V2_CONTROL_ADDR_OPTIONAL_LOGITS_DATA        0x034
#define XLM_HEAD_V2_CONTROL_BITS_OPTIONAL_LOGITS_DATA        64
#define XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_SIZE_R_DATA          0x040
#define XLM_HEAD_V2_CONTROL_BITS_HIDDEN_SIZE_R_DATA          32
#define XLM_HEAD_V2_CONTROL_ADDR_VOCAB_SIZE_DATA             0x048
#define XLM_HEAD_V2_CONTROL_BITS_VOCAB_SIZE_DATA             32
#define XLM_HEAD_V2_CONTROL_ADDR_RUN_MODE_DATA               0x050
#define XLM_HEAD_V2_CONTROL_BITS_RUN_MODE_DATA               32
#define XLM_HEAD_V2_CONTROL_ADDR_VALID_ROWS_DATA             0x058
#define XLM_HEAD_V2_CONTROL_BITS_VALID_ROWS_DATA             32
#define XLM_HEAD_V2_CONTROL_ADDR_WRITE_LOGITS_DATA           0x060
#define XLM_HEAD_V2_CONTROL_BITS_WRITE_LOGITS_DATA           32
#define XLM_HEAD_V2_CONTROL_ADDR_ENABLE_PENALTY_DATA         0x068
#define XLM_HEAD_V2_CONTROL_BITS_ENABLE_PENALTY_DATA         32
#define XLM_HEAD_V2_CONTROL_ADDR_REPETITION_PENALTY_DATA     0x070
#define XLM_HEAD_V2_CONTROL_BITS_REPETITION_PENALTY_DATA     32
#define XLM_HEAD_V2_CONTROL_ADDR_ABI_VERSION_DATA            0x078
#define XLM_HEAD_V2_CONTROL_BITS_ABI_VERSION_DATA            32
#define XLM_HEAD_V2_CONTROL_ADDR_JOB_ID_DATA                 0x080
#define XLM_HEAD_V2_CONTROL_BITS_JOB_ID_DATA                 32
#define XLM_HEAD_V2_CONTROL_ADDR_FLAGS_DATA                  0x088
#define XLM_HEAD_V2_CONTROL_BITS_FLAGS_DATA                  32
#define XLM_HEAD_V2_CONTROL_ADDR_SELECTED_TOKEN_DATA         0x090
#define XLM_HEAD_V2_CONTROL_BITS_SELECTED_TOKEN_DATA         32
#define XLM_HEAD_V2_CONTROL_ADDR_SELECTED_TOKEN_CTRL         0x094
#define XLM_HEAD_V2_CONTROL_ADDR_SELECTED_SCORE_DATA         0x0a0
#define XLM_HEAD_V2_CONTROL_BITS_SELECTED_SCORE_DATA         32
#define XLM_HEAD_V2_CONTROL_ADDR_SELECTED_SCORE_CTRL         0x0a4
#define XLM_HEAD_V2_CONTROL_ADDR_RESULT_JOB_ID_DATA          0x0b0
#define XLM_HEAD_V2_CONTROL_BITS_RESULT_JOB_ID_DATA          32
#define XLM_HEAD_V2_CONTROL_ADDR_RESULT_JOB_ID_CTRL          0x0b4
#define XLM_HEAD_V2_CONTROL_ADDR_STATUS_CODE_DATA            0x0c0
#define XLM_HEAD_V2_CONTROL_BITS_STATUS_CODE_DATA            32
#define XLM_HEAD_V2_CONTROL_ADDR_STATUS_CODE_CTRL            0x0c4
#define XLM_HEAD_V2_CONTROL_ADDR_PROCESSED_ROWS_DATA         0x0d0
#define XLM_HEAD_V2_CONTROL_BITS_PROCESSED_ROWS_DATA         32
#define XLM_HEAD_V2_CONTROL_ADDR_PROCESSED_ROWS_CTRL         0x0d4
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_DATA      0x0e0
#define XLM_HEAD_V2_CONTROL_BITS_PERF_CYCLES_TOTAL_DATA      64
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_CTRL      0x0e8
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_DATA    0x0f8
#define XLM_HEAD_V2_CONTROL_BITS_PERF_CYCLES_COMPUTE_DATA    64
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_CTRL    0x100
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_READ_BEATS_DATA        0x110
#define XLM_HEAD_V2_CONTROL_BITS_PERF_READ_BEATS_DATA        64
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_READ_BEATS_CTRL        0x118
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_WRITE_BEATS_DATA       0x128
#define XLM_HEAD_V2_CONTROL_BITS_PERF_WRITE_BEATS_DATA       64
#define XLM_HEAD_V2_CONTROL_ADDR_PERF_WRITE_BEATS_CTRL       0x130
#define XLM_HEAD_V2_CONTROL_ADDR_OUTPUT_CRC_DATA             0x140
#define XLM_HEAD_V2_CONTROL_BITS_OUTPUT_CRC_DATA             32
#define XLM_HEAD_V2_CONTROL_ADDR_OUTPUT_CRC_CTRL             0x144
#define XLM_HEAD_V2_CONTROL_ADDR_BUILD_CONFIG_DATA           0x150
#define XLM_HEAD_V2_CONTROL_BITS_BUILD_CONFIG_DATA           32
#define XLM_HEAD_V2_CONTROL_ADDR_BUILD_CONFIG_CTRL           0x154
#define XLM_HEAD_V2_CONTROL_ADDR_CAPABILITIES_DATA           0x160
#define XLM_HEAD_V2_CONTROL_BITS_CAPABILITIES_DATA           32
#define XLM_HEAD_V2_CONTROL_ADDR_CAPABILITIES_CTRL           0x164
#define XLM_HEAD_V2_CONTROL_ADDR_VOCAB_ROWS_DONE_DATA        0x170
#define XLM_HEAD_V2_CONTROL_BITS_VOCAB_ROWS_DONE_DATA        32
#define XLM_HEAD_V2_CONTROL_ADDR_VOCAB_ROWS_DONE_CTRL        0x174
#define XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_READ_BEATS_128_DATA  0x180
#define XLM_HEAD_V2_CONTROL_BITS_WEIGHT_READ_BEATS_128_DATA  64
#define XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_READ_BEATS_128_CTRL  0x188
#define XLM_HEAD_V2_CONTROL_ADDR_HISTORY_READ_WORDS_DATA     0x198
#define XLM_HEAD_V2_CONTROL_BITS_HISTORY_READ_WORDS_DATA     32
#define XLM_HEAD_V2_CONTROL_ADDR_HISTORY_READ_WORDS_CTRL     0x19c
#define XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITE_BEATS_128_DATA 0x1a8
#define XLM_HEAD_V2_CONTROL_BITS_LOGITS_WRITE_BEATS_128_DATA 64
#define XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITE_BEATS_128_CTRL 0x1b0
#define XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_WAIT_CYCLES_DATA     0x1c0
#define XLM_HEAD_V2_CONTROL_BITS_WEIGHT_WAIT_CYCLES_DATA     64
#define XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_WAIT_CYCLES_CTRL     0x1c8
#define XLM_HEAD_V2_CONTROL_ADDR_DOT_ACTIVE_CYCLES_DATA      0x1d8
#define XLM_HEAD_V2_CONTROL_BITS_DOT_ACTIVE_CYCLES_DATA      64
#define XLM_HEAD_V2_CONTROL_ADDR_DOT_ACTIVE_CYCLES_CTRL      0x1e0
#define XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITTEN_DATA         0x1f0
#define XLM_HEAD_V2_CONTROL_BITS_LOGITS_WRITTEN_DATA         32
#define XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITTEN_CTRL         0x1f4

