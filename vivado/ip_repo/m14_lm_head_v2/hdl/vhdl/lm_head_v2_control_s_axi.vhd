-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
-- Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity lm_head_v2_control_s_axi is
generic (
    C_S_AXI_ADDR_WIDTH    : INTEGER := 9;
    C_S_AXI_DATA_WIDTH    : INTEGER := 32);
port (
    ACLK                  :in   STD_LOGIC;
    ARESET                :in   STD_LOGIC;
    ACLK_EN               :in   STD_LOGIC;
    AWADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    AWVALID               :in   STD_LOGIC;
    AWREADY               :out  STD_LOGIC;
    WDATA                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    WSTRB                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH/8-1 downto 0);
    WVALID                :in   STD_LOGIC;
    WREADY                :out  STD_LOGIC;
    BRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    BVALID                :out  STD_LOGIC;
    BREADY                :in   STD_LOGIC;
    ARADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    ARVALID               :in   STD_LOGIC;
    ARREADY               :out  STD_LOGIC;
    RDATA                 :out  STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    RRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    RVALID                :out  STD_LOGIC;
    RREADY                :in   STD_LOGIC;
    interrupt             :out  STD_LOGIC;
    hidden_offset         :out  STD_LOGIC_VECTOR(63 downto 0);
    shared_vocab_weight   :out  STD_LOGIC_VECTOR(63 downto 0);
    history_bitmap        :out  STD_LOGIC_VECTOR(63 downto 0);
    optional_logits       :out  STD_LOGIC_VECTOR(63 downto 0);
    hidden_size_r         :out  STD_LOGIC_VECTOR(31 downto 0);
    vocab_size            :out  STD_LOGIC_VECTOR(31 downto 0);
    run_mode              :out  STD_LOGIC_VECTOR(31 downto 0);
    valid_rows            :out  STD_LOGIC_VECTOR(31 downto 0);
    write_logits          :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_penalty        :out  STD_LOGIC_VECTOR(31 downto 0);
    repetition_penalty    :out  STD_LOGIC_VECTOR(31 downto 0);
    abi_version           :out  STD_LOGIC_VECTOR(31 downto 0);
    job_id                :out  STD_LOGIC_VECTOR(31 downto 0);
    flags                 :out  STD_LOGIC_VECTOR(31 downto 0);
    selected_token        :in   STD_LOGIC_VECTOR(31 downto 0);
    selected_token_ap_vld :in   STD_LOGIC;
    selected_score        :in   STD_LOGIC_VECTOR(31 downto 0);
    selected_score_ap_vld :in   STD_LOGIC;
    result_job_id         :in   STD_LOGIC_VECTOR(31 downto 0);
    result_job_id_ap_vld  :in   STD_LOGIC;
    status_code           :in   STD_LOGIC_VECTOR(31 downto 0);
    status_code_ap_vld    :in   STD_LOGIC;
    processed_rows        :in   STD_LOGIC_VECTOR(31 downto 0);
    processed_rows_ap_vld :in   STD_LOGIC;
    perf_cycles_total     :in   STD_LOGIC_VECTOR(63 downto 0);
    perf_cycles_total_ap_vld :in   STD_LOGIC;
    perf_cycles_compute   :in   STD_LOGIC_VECTOR(63 downto 0);
    perf_cycles_compute_ap_vld :in   STD_LOGIC;
    perf_read_beats       :in   STD_LOGIC_VECTOR(63 downto 0);
    perf_read_beats_ap_vld :in   STD_LOGIC;
    perf_write_beats      :in   STD_LOGIC_VECTOR(63 downto 0);
    perf_write_beats_ap_vld :in   STD_LOGIC;
    output_crc            :in   STD_LOGIC_VECTOR(31 downto 0);
    output_crc_ap_vld     :in   STD_LOGIC;
    build_config          :in   STD_LOGIC_VECTOR(31 downto 0);
    build_config_ap_vld   :in   STD_LOGIC;
    capabilities          :in   STD_LOGIC_VECTOR(31 downto 0);
    capabilities_ap_vld   :in   STD_LOGIC;
    vocab_rows_done       :in   STD_LOGIC_VECTOR(31 downto 0);
    vocab_rows_done_ap_vld :in   STD_LOGIC;
    weight_read_beats_128 :in   STD_LOGIC_VECTOR(63 downto 0);
    weight_read_beats_128_ap_vld :in   STD_LOGIC;
    history_read_words    :in   STD_LOGIC_VECTOR(31 downto 0);
    history_read_words_ap_vld :in   STD_LOGIC;
    logits_write_beats_128 :in   STD_LOGIC_VECTOR(63 downto 0);
    logits_write_beats_128_ap_vld :in   STD_LOGIC;
    weight_wait_cycles    :in   STD_LOGIC_VECTOR(63 downto 0);
    weight_wait_cycles_ap_vld :in   STD_LOGIC;
    dot_active_cycles     :in   STD_LOGIC_VECTOR(63 downto 0);
    dot_active_cycles_ap_vld :in   STD_LOGIC;
    logits_written        :in   STD_LOGIC_VECTOR(31 downto 0);
    logits_written_ap_vld :in   STD_LOGIC;
    ap_start              :out  STD_LOGIC;
    ap_done               :in   STD_LOGIC;
    ap_ready              :in   STD_LOGIC;
    ap_idle               :in   STD_LOGIC
);
end entity lm_head_v2_control_s_axi;

-- ------------------------Address Info-------------------
-- 0x000 : Control signals
--         bit 0  - ap_start (Read/Write/COH)
--         bit 1  - ap_done (Read/COR)
--         bit 2  - ap_idle (Read)
--         bit 3  - ap_ready (Read/COR)
--         bit 7  - auto_restart (Read/Write)
--         others - reserved
-- 0x004 : Global Interrupt Enable Register
--         bit 0  - Global Interrupt Enable (Read/Write)
--         others - reserved
-- 0x008 : IP Interrupt Enable Register (Read/Write)
--         bit 0  - enable ap_done interrupt (Read/Write)
--         bit 1  - enable ap_ready interrupt (Read/Write)
--         others - reserved
-- 0x00c : IP Interrupt Status Register (Read/TOW)
--         bit 0  - ap_done (COR/TOW)
--         bit 1  - ap_ready (COR/TOW)
--         others - reserved
-- 0x010 : Data signal of hidden_offset
--         bit 31~0 - hidden_offset[31:0] (Read/Write)
-- 0x014 : Data signal of hidden_offset
--         bit 31~0 - hidden_offset[63:32] (Read/Write)
-- 0x018 : reserved
-- 0x01c : Data signal of shared_vocab_weight
--         bit 31~0 - shared_vocab_weight[31:0] (Read/Write)
-- 0x020 : Data signal of shared_vocab_weight
--         bit 31~0 - shared_vocab_weight[63:32] (Read/Write)
-- 0x024 : reserved
-- 0x028 : Data signal of history_bitmap
--         bit 31~0 - history_bitmap[31:0] (Read/Write)
-- 0x02c : Data signal of history_bitmap
--         bit 31~0 - history_bitmap[63:32] (Read/Write)
-- 0x030 : reserved
-- 0x034 : Data signal of optional_logits
--         bit 31~0 - optional_logits[31:0] (Read/Write)
-- 0x038 : Data signal of optional_logits
--         bit 31~0 - optional_logits[63:32] (Read/Write)
-- 0x03c : reserved
-- 0x040 : Data signal of hidden_size_r
--         bit 31~0 - hidden_size_r[31:0] (Read/Write)
-- 0x044 : reserved
-- 0x048 : Data signal of vocab_size
--         bit 31~0 - vocab_size[31:0] (Read/Write)
-- 0x04c : reserved
-- 0x050 : Data signal of run_mode
--         bit 31~0 - run_mode[31:0] (Read/Write)
-- 0x054 : reserved
-- 0x058 : Data signal of valid_rows
--         bit 31~0 - valid_rows[31:0] (Read/Write)
-- 0x05c : reserved
-- 0x060 : Data signal of write_logits
--         bit 31~0 - write_logits[31:0] (Read/Write)
-- 0x064 : reserved
-- 0x068 : Data signal of enable_penalty
--         bit 31~0 - enable_penalty[31:0] (Read/Write)
-- 0x06c : reserved
-- 0x070 : Data signal of repetition_penalty
--         bit 31~0 - repetition_penalty[31:0] (Read/Write)
-- 0x074 : reserved
-- 0x078 : Data signal of abi_version
--         bit 31~0 - abi_version[31:0] (Read/Write)
-- 0x07c : reserved
-- 0x080 : Data signal of job_id
--         bit 31~0 - job_id[31:0] (Read/Write)
-- 0x084 : reserved
-- 0x088 : Data signal of flags
--         bit 31~0 - flags[31:0] (Read/Write)
-- 0x08c : reserved
-- 0x090 : Data signal of selected_token
--         bit 31~0 - selected_token[31:0] (Read)
-- 0x094 : Control signal of selected_token
--         bit 0  - selected_token_ap_vld (Read/COR)
--         others - reserved
-- 0x0a0 : Data signal of selected_score
--         bit 31~0 - selected_score[31:0] (Read)
-- 0x0a4 : Control signal of selected_score
--         bit 0  - selected_score_ap_vld (Read/COR)
--         others - reserved
-- 0x0b0 : Data signal of result_job_id
--         bit 31~0 - result_job_id[31:0] (Read)
-- 0x0b4 : Control signal of result_job_id
--         bit 0  - result_job_id_ap_vld (Read/COR)
--         others - reserved
-- 0x0c0 : Data signal of status_code
--         bit 31~0 - status_code[31:0] (Read)
-- 0x0c4 : Control signal of status_code
--         bit 0  - status_code_ap_vld (Read/COR)
--         others - reserved
-- 0x0d0 : Data signal of processed_rows
--         bit 31~0 - processed_rows[31:0] (Read)
-- 0x0d4 : Control signal of processed_rows
--         bit 0  - processed_rows_ap_vld (Read/COR)
--         others - reserved
-- 0x0e0 : Data signal of perf_cycles_total
--         bit 31~0 - perf_cycles_total[31:0] (Read)
-- 0x0e4 : Data signal of perf_cycles_total
--         bit 31~0 - perf_cycles_total[63:32] (Read)
-- 0x0e8 : Control signal of perf_cycles_total
--         bit 0  - perf_cycles_total_ap_vld (Read/COR)
--         others - reserved
-- 0x0f8 : Data signal of perf_cycles_compute
--         bit 31~0 - perf_cycles_compute[31:0] (Read)
-- 0x0fc : Data signal of perf_cycles_compute
--         bit 31~0 - perf_cycles_compute[63:32] (Read)
-- 0x100 : Control signal of perf_cycles_compute
--         bit 0  - perf_cycles_compute_ap_vld (Read/COR)
--         others - reserved
-- 0x110 : Data signal of perf_read_beats
--         bit 31~0 - perf_read_beats[31:0] (Read)
-- 0x114 : Data signal of perf_read_beats
--         bit 31~0 - perf_read_beats[63:32] (Read)
-- 0x118 : Control signal of perf_read_beats
--         bit 0  - perf_read_beats_ap_vld (Read/COR)
--         others - reserved
-- 0x128 : Data signal of perf_write_beats
--         bit 31~0 - perf_write_beats[31:0] (Read)
-- 0x12c : Data signal of perf_write_beats
--         bit 31~0 - perf_write_beats[63:32] (Read)
-- 0x130 : Control signal of perf_write_beats
--         bit 0  - perf_write_beats_ap_vld (Read/COR)
--         others - reserved
-- 0x140 : Data signal of output_crc
--         bit 31~0 - output_crc[31:0] (Read)
-- 0x144 : Control signal of output_crc
--         bit 0  - output_crc_ap_vld (Read/COR)
--         others - reserved
-- 0x150 : Data signal of build_config
--         bit 31~0 - build_config[31:0] (Read)
-- 0x154 : Control signal of build_config
--         bit 0  - build_config_ap_vld (Read/COR)
--         others - reserved
-- 0x160 : Data signal of capabilities
--         bit 31~0 - capabilities[31:0] (Read)
-- 0x164 : Control signal of capabilities
--         bit 0  - capabilities_ap_vld (Read/COR)
--         others - reserved
-- 0x170 : Data signal of vocab_rows_done
--         bit 31~0 - vocab_rows_done[31:0] (Read)
-- 0x174 : Control signal of vocab_rows_done
--         bit 0  - vocab_rows_done_ap_vld (Read/COR)
--         others - reserved
-- 0x180 : Data signal of weight_read_beats_128
--         bit 31~0 - weight_read_beats_128[31:0] (Read)
-- 0x184 : Data signal of weight_read_beats_128
--         bit 31~0 - weight_read_beats_128[63:32] (Read)
-- 0x188 : Control signal of weight_read_beats_128
--         bit 0  - weight_read_beats_128_ap_vld (Read/COR)
--         others - reserved
-- 0x198 : Data signal of history_read_words
--         bit 31~0 - history_read_words[31:0] (Read)
-- 0x19c : Control signal of history_read_words
--         bit 0  - history_read_words_ap_vld (Read/COR)
--         others - reserved
-- 0x1a8 : Data signal of logits_write_beats_128
--         bit 31~0 - logits_write_beats_128[31:0] (Read)
-- 0x1ac : Data signal of logits_write_beats_128
--         bit 31~0 - logits_write_beats_128[63:32] (Read)
-- 0x1b0 : Control signal of logits_write_beats_128
--         bit 0  - logits_write_beats_128_ap_vld (Read/COR)
--         others - reserved
-- 0x1c0 : Data signal of weight_wait_cycles
--         bit 31~0 - weight_wait_cycles[31:0] (Read)
-- 0x1c4 : Data signal of weight_wait_cycles
--         bit 31~0 - weight_wait_cycles[63:32] (Read)
-- 0x1c8 : Control signal of weight_wait_cycles
--         bit 0  - weight_wait_cycles_ap_vld (Read/COR)
--         others - reserved
-- 0x1d8 : Data signal of dot_active_cycles
--         bit 31~0 - dot_active_cycles[31:0] (Read)
-- 0x1dc : Data signal of dot_active_cycles
--         bit 31~0 - dot_active_cycles[63:32] (Read)
-- 0x1e0 : Control signal of dot_active_cycles
--         bit 0  - dot_active_cycles_ap_vld (Read/COR)
--         others - reserved
-- 0x1f0 : Data signal of logits_written
--         bit 31~0 - logits_written[31:0] (Read)
-- 0x1f4 : Control signal of logits_written
--         bit 0  - logits_written_ap_vld (Read/COR)
--         others - reserved
-- (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

architecture behave of lm_head_v2_control_s_axi is
    type states is (wridle, wrdata, wrresp, wrreset, rdidle, rddata, rdreset);  -- read and write fsm states
    signal wstate  : states := wrreset;
    signal rstate  : states := rdreset;
    signal wnext, rnext: states;
    constant ADDR_AP_CTRL                       : INTEGER := 16#000#;
    constant ADDR_GIE                           : INTEGER := 16#004#;
    constant ADDR_IER                           : INTEGER := 16#008#;
    constant ADDR_ISR                           : INTEGER := 16#00c#;
    constant ADDR_HIDDEN_OFFSET_DATA_0          : INTEGER := 16#010#;
    constant ADDR_HIDDEN_OFFSET_DATA_1          : INTEGER := 16#014#;
    constant ADDR_HIDDEN_OFFSET_CTRL            : INTEGER := 16#018#;
    constant ADDR_SHARED_VOCAB_WEIGHT_DATA_0    : INTEGER := 16#01c#;
    constant ADDR_SHARED_VOCAB_WEIGHT_DATA_1    : INTEGER := 16#020#;
    constant ADDR_SHARED_VOCAB_WEIGHT_CTRL      : INTEGER := 16#024#;
    constant ADDR_HISTORY_BITMAP_DATA_0         : INTEGER := 16#028#;
    constant ADDR_HISTORY_BITMAP_DATA_1         : INTEGER := 16#02c#;
    constant ADDR_HISTORY_BITMAP_CTRL           : INTEGER := 16#030#;
    constant ADDR_OPTIONAL_LOGITS_DATA_0        : INTEGER := 16#034#;
    constant ADDR_OPTIONAL_LOGITS_DATA_1        : INTEGER := 16#038#;
    constant ADDR_OPTIONAL_LOGITS_CTRL          : INTEGER := 16#03c#;
    constant ADDR_HIDDEN_SIZE_R_DATA_0          : INTEGER := 16#040#;
    constant ADDR_HIDDEN_SIZE_R_CTRL            : INTEGER := 16#044#;
    constant ADDR_VOCAB_SIZE_DATA_0             : INTEGER := 16#048#;
    constant ADDR_VOCAB_SIZE_CTRL               : INTEGER := 16#04c#;
    constant ADDR_RUN_MODE_DATA_0               : INTEGER := 16#050#;
    constant ADDR_RUN_MODE_CTRL                 : INTEGER := 16#054#;
    constant ADDR_VALID_ROWS_DATA_0             : INTEGER := 16#058#;
    constant ADDR_VALID_ROWS_CTRL               : INTEGER := 16#05c#;
    constant ADDR_WRITE_LOGITS_DATA_0           : INTEGER := 16#060#;
    constant ADDR_WRITE_LOGITS_CTRL             : INTEGER := 16#064#;
    constant ADDR_ENABLE_PENALTY_DATA_0         : INTEGER := 16#068#;
    constant ADDR_ENABLE_PENALTY_CTRL           : INTEGER := 16#06c#;
    constant ADDR_REPETITION_PENALTY_DATA_0     : INTEGER := 16#070#;
    constant ADDR_REPETITION_PENALTY_CTRL       : INTEGER := 16#074#;
    constant ADDR_ABI_VERSION_DATA_0            : INTEGER := 16#078#;
    constant ADDR_ABI_VERSION_CTRL              : INTEGER := 16#07c#;
    constant ADDR_JOB_ID_DATA_0                 : INTEGER := 16#080#;
    constant ADDR_JOB_ID_CTRL                   : INTEGER := 16#084#;
    constant ADDR_FLAGS_DATA_0                  : INTEGER := 16#088#;
    constant ADDR_FLAGS_CTRL                    : INTEGER := 16#08c#;
    constant ADDR_SELECTED_TOKEN_DATA_0         : INTEGER := 16#090#;
    constant ADDR_SELECTED_TOKEN_CTRL           : INTEGER := 16#094#;
    constant ADDR_SELECTED_SCORE_DATA_0         : INTEGER := 16#0a0#;
    constant ADDR_SELECTED_SCORE_CTRL           : INTEGER := 16#0a4#;
    constant ADDR_RESULT_JOB_ID_DATA_0          : INTEGER := 16#0b0#;
    constant ADDR_RESULT_JOB_ID_CTRL            : INTEGER := 16#0b4#;
    constant ADDR_STATUS_CODE_DATA_0            : INTEGER := 16#0c0#;
    constant ADDR_STATUS_CODE_CTRL              : INTEGER := 16#0c4#;
    constant ADDR_PROCESSED_ROWS_DATA_0         : INTEGER := 16#0d0#;
    constant ADDR_PROCESSED_ROWS_CTRL           : INTEGER := 16#0d4#;
    constant ADDR_PERF_CYCLES_TOTAL_DATA_0      : INTEGER := 16#0e0#;
    constant ADDR_PERF_CYCLES_TOTAL_DATA_1      : INTEGER := 16#0e4#;
    constant ADDR_PERF_CYCLES_TOTAL_CTRL        : INTEGER := 16#0e8#;
    constant ADDR_PERF_CYCLES_COMPUTE_DATA_0    : INTEGER := 16#0f8#;
    constant ADDR_PERF_CYCLES_COMPUTE_DATA_1    : INTEGER := 16#0fc#;
    constant ADDR_PERF_CYCLES_COMPUTE_CTRL      : INTEGER := 16#100#;
    constant ADDR_PERF_READ_BEATS_DATA_0        : INTEGER := 16#110#;
    constant ADDR_PERF_READ_BEATS_DATA_1        : INTEGER := 16#114#;
    constant ADDR_PERF_READ_BEATS_CTRL          : INTEGER := 16#118#;
    constant ADDR_PERF_WRITE_BEATS_DATA_0       : INTEGER := 16#128#;
    constant ADDR_PERF_WRITE_BEATS_DATA_1       : INTEGER := 16#12c#;
    constant ADDR_PERF_WRITE_BEATS_CTRL         : INTEGER := 16#130#;
    constant ADDR_OUTPUT_CRC_DATA_0             : INTEGER := 16#140#;
    constant ADDR_OUTPUT_CRC_CTRL               : INTEGER := 16#144#;
    constant ADDR_BUILD_CONFIG_DATA_0           : INTEGER := 16#150#;
    constant ADDR_BUILD_CONFIG_CTRL             : INTEGER := 16#154#;
    constant ADDR_CAPABILITIES_DATA_0           : INTEGER := 16#160#;
    constant ADDR_CAPABILITIES_CTRL             : INTEGER := 16#164#;
    constant ADDR_VOCAB_ROWS_DONE_DATA_0        : INTEGER := 16#170#;
    constant ADDR_VOCAB_ROWS_DONE_CTRL          : INTEGER := 16#174#;
    constant ADDR_WEIGHT_READ_BEATS_128_DATA_0  : INTEGER := 16#180#;
    constant ADDR_WEIGHT_READ_BEATS_128_DATA_1  : INTEGER := 16#184#;
    constant ADDR_WEIGHT_READ_BEATS_128_CTRL    : INTEGER := 16#188#;
    constant ADDR_HISTORY_READ_WORDS_DATA_0     : INTEGER := 16#198#;
    constant ADDR_HISTORY_READ_WORDS_CTRL       : INTEGER := 16#19c#;
    constant ADDR_LOGITS_WRITE_BEATS_128_DATA_0 : INTEGER := 16#1a8#;
    constant ADDR_LOGITS_WRITE_BEATS_128_DATA_1 : INTEGER := 16#1ac#;
    constant ADDR_LOGITS_WRITE_BEATS_128_CTRL   : INTEGER := 16#1b0#;
    constant ADDR_WEIGHT_WAIT_CYCLES_DATA_0     : INTEGER := 16#1c0#;
    constant ADDR_WEIGHT_WAIT_CYCLES_DATA_1     : INTEGER := 16#1c4#;
    constant ADDR_WEIGHT_WAIT_CYCLES_CTRL       : INTEGER := 16#1c8#;
    constant ADDR_DOT_ACTIVE_CYCLES_DATA_0      : INTEGER := 16#1d8#;
    constant ADDR_DOT_ACTIVE_CYCLES_DATA_1      : INTEGER := 16#1dc#;
    constant ADDR_DOT_ACTIVE_CYCLES_CTRL        : INTEGER := 16#1e0#;
    constant ADDR_LOGITS_WRITTEN_DATA_0         : INTEGER := 16#1f0#;
    constant ADDR_LOGITS_WRITTEN_CTRL           : INTEGER := 16#1f4#;
    constant ADDR_BITS         : INTEGER := 9;

    signal waddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal wmask               : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal aw_hs               : STD_LOGIC;
    signal w_hs                : STD_LOGIC;
    signal rdata_data          : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal ar_hs               : STD_LOGIC;
    signal raddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal AWREADY_t           : STD_LOGIC;
    signal WREADY_t            : STD_LOGIC;
    signal ARREADY_t           : STD_LOGIC;
    signal RVALID_t            : STD_LOGIC;
    -- internal registers
    signal int_ap_idle         : STD_LOGIC := '0';
    signal int_ap_ready        : STD_LOGIC := '0';
    signal task_ap_ready       : STD_LOGIC;
    signal int_ap_done         : STD_LOGIC := '0';
    signal task_ap_done        : STD_LOGIC;
    signal int_task_ap_done    : STD_LOGIC := '0';
    signal int_ap_start        : STD_LOGIC := '0';
    signal int_auto_restart    : STD_LOGIC := '0';
    signal auto_restart_status : STD_LOGIC := '0';
    signal auto_restart_done   : STD_LOGIC;
    signal int_gie             : STD_LOGIC := '0';
    signal int_ier             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_isr             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_hidden_offset   : UNSIGNED(63 downto 0) := (others => '0');
    signal int_shared_vocab_weight : UNSIGNED(63 downto 0) := (others => '0');
    signal int_history_bitmap  : UNSIGNED(63 downto 0) := (others => '0');
    signal int_optional_logits : UNSIGNED(63 downto 0) := (others => '0');
    signal int_hidden_size_r   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_vocab_size      : UNSIGNED(31 downto 0) := (others => '0');
    signal int_run_mode        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_valid_rows      : UNSIGNED(31 downto 0) := (others => '0');
    signal int_write_logits    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_penalty  : UNSIGNED(31 downto 0) := (others => '0');
    signal int_repetition_penalty : UNSIGNED(31 downto 0) := (others => '0');
    signal int_abi_version     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_job_id          : UNSIGNED(31 downto 0) := (others => '0');
    signal int_flags           : UNSIGNED(31 downto 0) := (others => '0');
    signal int_selected_token_ap_vld : STD_LOGIC;
    signal int_selected_token  : UNSIGNED(31 downto 0) := (others => '0');
    signal int_selected_score_ap_vld : STD_LOGIC;
    signal int_selected_score  : UNSIGNED(31 downto 0) := (others => '0');
    signal int_result_job_id_ap_vld : STD_LOGIC;
    signal int_result_job_id   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_status_code_ap_vld : STD_LOGIC;
    signal int_status_code     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_processed_rows_ap_vld : STD_LOGIC;
    signal int_processed_rows  : UNSIGNED(31 downto 0) := (others => '0');
    signal int_perf_cycles_total_ap_vld : STD_LOGIC;
    signal int_perf_cycles_total : UNSIGNED(63 downto 0) := (others => '0');
    signal int_perf_cycles_compute_ap_vld : STD_LOGIC;
    signal int_perf_cycles_compute : UNSIGNED(63 downto 0) := (others => '0');
    signal int_perf_read_beats_ap_vld : STD_LOGIC;
    signal int_perf_read_beats : UNSIGNED(63 downto 0) := (others => '0');
    signal int_perf_write_beats_ap_vld : STD_LOGIC;
    signal int_perf_write_beats : UNSIGNED(63 downto 0) := (others => '0');
    signal int_output_crc_ap_vld : STD_LOGIC;
    signal int_output_crc      : UNSIGNED(31 downto 0) := (others => '0');
    signal int_build_config_ap_vld : STD_LOGIC;
    signal int_build_config    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_capabilities_ap_vld : STD_LOGIC;
    signal int_capabilities    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_vocab_rows_done_ap_vld : STD_LOGIC;
    signal int_vocab_rows_done : UNSIGNED(31 downto 0) := (others => '0');
    signal int_weight_read_beats_128_ap_vld : STD_LOGIC;
    signal int_weight_read_beats_128 : UNSIGNED(63 downto 0) := (others => '0');
    signal int_history_read_words_ap_vld : STD_LOGIC;
    signal int_history_read_words : UNSIGNED(31 downto 0) := (others => '0');
    signal int_logits_write_beats_128_ap_vld : STD_LOGIC;
    signal int_logits_write_beats_128 : UNSIGNED(63 downto 0) := (others => '0');
    signal int_weight_wait_cycles_ap_vld : STD_LOGIC;
    signal int_weight_wait_cycles : UNSIGNED(63 downto 0) := (others => '0');
    signal int_dot_active_cycles_ap_vld : STD_LOGIC;
    signal int_dot_active_cycles : UNSIGNED(63 downto 0) := (others => '0');
    signal int_logits_written_ap_vld : STD_LOGIC;
    signal int_logits_written  : UNSIGNED(31 downto 0) := (others => '0');


begin
-- ----------------------- Instantiation------------------


-- ----------------------- AXI WRITE ---------------------
    AWREADY_t <=  '1' when wstate = wridle else '0';
    AWREADY   <=  AWREADY_t;
    WREADY_t  <=  '1' when wstate = wrdata else '0';
    WREADY    <=  WREADY_t;
    BRESP     <=  "00";  -- OKAY
    BVALID    <=  '1' when wstate = wrresp else '0';
    wmask     <=  (31 downto 24 => WSTRB(3), 23 downto 16 => WSTRB(2), 15 downto 8 => WSTRB(1), 7 downto 0 => WSTRB(0));
    aw_hs     <=  AWVALID and AWREADY_t;
    w_hs      <=  WVALID and WREADY_t;

    -- write FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                wstate <= wrreset;
            elsif (ACLK_EN = '1') then
                wstate <= wnext;
            end if;
        end if;
    end process;

    process (wstate, AWVALID, WVALID, BREADY)
    begin
        case (wstate) is
        when wridle =>
            if (AWVALID = '1') then
                wnext <= wrdata;
            else
                wnext <= wridle;
            end if;
        when wrdata =>
            if (WVALID = '1') then
                wnext <= wrresp;
            else
                wnext <= wrdata;
            end if;
        when wrresp =>
            if (BREADY = '1') then
                wnext <= wridle;
            else
                wnext <= wrresp;
            end if;
        when others =>
            wnext <= wridle;
        end case;
    end process;

    waddr_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (aw_hs = '1') then
                    waddr <= UNSIGNED(AWADDR(ADDR_BITS-1 downto 0));
                end if;
            end if;
        end if;
    end process;

-- ----------------------- AXI READ ----------------------
    ARREADY_t <= '1' when (rstate = rdidle) else '0';
    ARREADY <= ARREADY_t;
    RDATA   <= STD_LOGIC_VECTOR(rdata_data);
    RRESP   <= "00";  -- OKAY
    RVALID_t  <= '1' when (rstate = rddata) else '0';
    RVALID    <= RVALID_t;
    ar_hs   <= ARVALID and ARREADY_t;
    raddr   <= UNSIGNED(ARADDR(ADDR_BITS-1 downto 0));

    -- read FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                rstate <= rdreset;
            elsif (ACLK_EN = '1') then
                rstate <= rnext;
            end if;
        end if;
    end process;

    process (rstate, ARVALID, RREADY, RVALID_t)
    begin
        case (rstate) is
        when rdidle =>
            if (ARVALID = '1') then
                rnext <= rddata;
            else
                rnext <= rdidle;
            end if;
        when rddata =>
            if (RREADY = '1' and RVALID_t = '1') then
                rnext <= rdidle;
            else
                rnext <= rddata;
            end if;
        when others =>
            rnext <= rdidle;
        end case;
    end process;

    rdata_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (ar_hs = '1') then
                    rdata_data <= (others => '0');
                    case (TO_INTEGER(raddr)) is
                    when ADDR_AP_CTRL =>
                        rdata_data(7) <= int_auto_restart;
                        rdata_data(3) <= int_ap_ready;
                        rdata_data(2) <= int_ap_idle;
                        rdata_data(1) <= int_task_ap_done;
                        rdata_data(0) <= int_ap_start;
                    when ADDR_GIE =>
                        rdata_data(0) <= int_gie;
                    when ADDR_IER =>
                        rdata_data(1 downto 0) <= int_ier;
                    when ADDR_ISR =>
                        rdata_data(1 downto 0) <= int_isr;
                    when ADDR_HIDDEN_OFFSET_DATA_0 =>
                        rdata_data <= RESIZE(int_hidden_offset(31 downto 0), 32);
                    when ADDR_HIDDEN_OFFSET_DATA_1 =>
                        rdata_data <= RESIZE(int_hidden_offset(63 downto 32), 32);
                    when ADDR_SHARED_VOCAB_WEIGHT_DATA_0 =>
                        rdata_data <= RESIZE(int_shared_vocab_weight(31 downto 0), 32);
                    when ADDR_SHARED_VOCAB_WEIGHT_DATA_1 =>
                        rdata_data <= RESIZE(int_shared_vocab_weight(63 downto 32), 32);
                    when ADDR_HISTORY_BITMAP_DATA_0 =>
                        rdata_data <= RESIZE(int_history_bitmap(31 downto 0), 32);
                    when ADDR_HISTORY_BITMAP_DATA_1 =>
                        rdata_data <= RESIZE(int_history_bitmap(63 downto 32), 32);
                    when ADDR_OPTIONAL_LOGITS_DATA_0 =>
                        rdata_data <= RESIZE(int_optional_logits(31 downto 0), 32);
                    when ADDR_OPTIONAL_LOGITS_DATA_1 =>
                        rdata_data <= RESIZE(int_optional_logits(63 downto 32), 32);
                    when ADDR_HIDDEN_SIZE_R_DATA_0 =>
                        rdata_data <= RESIZE(int_hidden_size_r(31 downto 0), 32);
                    when ADDR_VOCAB_SIZE_DATA_0 =>
                        rdata_data <= RESIZE(int_vocab_size(31 downto 0), 32);
                    when ADDR_RUN_MODE_DATA_0 =>
                        rdata_data <= RESIZE(int_run_mode(31 downto 0), 32);
                    when ADDR_VALID_ROWS_DATA_0 =>
                        rdata_data <= RESIZE(int_valid_rows(31 downto 0), 32);
                    when ADDR_WRITE_LOGITS_DATA_0 =>
                        rdata_data <= RESIZE(int_write_logits(31 downto 0), 32);
                    when ADDR_ENABLE_PENALTY_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_penalty(31 downto 0), 32);
                    when ADDR_REPETITION_PENALTY_DATA_0 =>
                        rdata_data <= RESIZE(int_repetition_penalty(31 downto 0), 32);
                    when ADDR_ABI_VERSION_DATA_0 =>
                        rdata_data <= RESIZE(int_abi_version(31 downto 0), 32);
                    when ADDR_JOB_ID_DATA_0 =>
                        rdata_data <= RESIZE(int_job_id(31 downto 0), 32);
                    when ADDR_FLAGS_DATA_0 =>
                        rdata_data <= RESIZE(int_flags(31 downto 0), 32);
                    when ADDR_SELECTED_TOKEN_DATA_0 =>
                        rdata_data <= RESIZE(int_selected_token(31 downto 0), 32);
                    when ADDR_SELECTED_TOKEN_CTRL =>
                        rdata_data(0) <= int_selected_token_ap_vld;
                    when ADDR_SELECTED_SCORE_DATA_0 =>
                        rdata_data <= RESIZE(int_selected_score(31 downto 0), 32);
                    when ADDR_SELECTED_SCORE_CTRL =>
                        rdata_data(0) <= int_selected_score_ap_vld;
                    when ADDR_RESULT_JOB_ID_DATA_0 =>
                        rdata_data <= RESIZE(int_result_job_id(31 downto 0), 32);
                    when ADDR_RESULT_JOB_ID_CTRL =>
                        rdata_data(0) <= int_result_job_id_ap_vld;
                    when ADDR_STATUS_CODE_DATA_0 =>
                        rdata_data <= RESIZE(int_status_code(31 downto 0), 32);
                    when ADDR_STATUS_CODE_CTRL =>
                        rdata_data(0) <= int_status_code_ap_vld;
                    when ADDR_PROCESSED_ROWS_DATA_0 =>
                        rdata_data <= RESIZE(int_processed_rows(31 downto 0), 32);
                    when ADDR_PROCESSED_ROWS_CTRL =>
                        rdata_data(0) <= int_processed_rows_ap_vld;
                    when ADDR_PERF_CYCLES_TOTAL_DATA_0 =>
                        rdata_data <= RESIZE(int_perf_cycles_total(31 downto 0), 32);
                    when ADDR_PERF_CYCLES_TOTAL_DATA_1 =>
                        rdata_data <= RESIZE(int_perf_cycles_total(63 downto 32), 32);
                    when ADDR_PERF_CYCLES_TOTAL_CTRL =>
                        rdata_data(0) <= int_perf_cycles_total_ap_vld;
                    when ADDR_PERF_CYCLES_COMPUTE_DATA_0 =>
                        rdata_data <= RESIZE(int_perf_cycles_compute(31 downto 0), 32);
                    when ADDR_PERF_CYCLES_COMPUTE_DATA_1 =>
                        rdata_data <= RESIZE(int_perf_cycles_compute(63 downto 32), 32);
                    when ADDR_PERF_CYCLES_COMPUTE_CTRL =>
                        rdata_data(0) <= int_perf_cycles_compute_ap_vld;
                    when ADDR_PERF_READ_BEATS_DATA_0 =>
                        rdata_data <= RESIZE(int_perf_read_beats(31 downto 0), 32);
                    when ADDR_PERF_READ_BEATS_DATA_1 =>
                        rdata_data <= RESIZE(int_perf_read_beats(63 downto 32), 32);
                    when ADDR_PERF_READ_BEATS_CTRL =>
                        rdata_data(0) <= int_perf_read_beats_ap_vld;
                    when ADDR_PERF_WRITE_BEATS_DATA_0 =>
                        rdata_data <= RESIZE(int_perf_write_beats(31 downto 0), 32);
                    when ADDR_PERF_WRITE_BEATS_DATA_1 =>
                        rdata_data <= RESIZE(int_perf_write_beats(63 downto 32), 32);
                    when ADDR_PERF_WRITE_BEATS_CTRL =>
                        rdata_data(0) <= int_perf_write_beats_ap_vld;
                    when ADDR_OUTPUT_CRC_DATA_0 =>
                        rdata_data <= RESIZE(int_output_crc(31 downto 0), 32);
                    when ADDR_OUTPUT_CRC_CTRL =>
                        rdata_data(0) <= int_output_crc_ap_vld;
                    when ADDR_BUILD_CONFIG_DATA_0 =>
                        rdata_data <= RESIZE(int_build_config(31 downto 0), 32);
                    when ADDR_BUILD_CONFIG_CTRL =>
                        rdata_data(0) <= int_build_config_ap_vld;
                    when ADDR_CAPABILITIES_DATA_0 =>
                        rdata_data <= RESIZE(int_capabilities(31 downto 0), 32);
                    when ADDR_CAPABILITIES_CTRL =>
                        rdata_data(0) <= int_capabilities_ap_vld;
                    when ADDR_VOCAB_ROWS_DONE_DATA_0 =>
                        rdata_data <= RESIZE(int_vocab_rows_done(31 downto 0), 32);
                    when ADDR_VOCAB_ROWS_DONE_CTRL =>
                        rdata_data(0) <= int_vocab_rows_done_ap_vld;
                    when ADDR_WEIGHT_READ_BEATS_128_DATA_0 =>
                        rdata_data <= RESIZE(int_weight_read_beats_128(31 downto 0), 32);
                    when ADDR_WEIGHT_READ_BEATS_128_DATA_1 =>
                        rdata_data <= RESIZE(int_weight_read_beats_128(63 downto 32), 32);
                    when ADDR_WEIGHT_READ_BEATS_128_CTRL =>
                        rdata_data(0) <= int_weight_read_beats_128_ap_vld;
                    when ADDR_HISTORY_READ_WORDS_DATA_0 =>
                        rdata_data <= RESIZE(int_history_read_words(31 downto 0), 32);
                    when ADDR_HISTORY_READ_WORDS_CTRL =>
                        rdata_data(0) <= int_history_read_words_ap_vld;
                    when ADDR_LOGITS_WRITE_BEATS_128_DATA_0 =>
                        rdata_data <= RESIZE(int_logits_write_beats_128(31 downto 0), 32);
                    when ADDR_LOGITS_WRITE_BEATS_128_DATA_1 =>
                        rdata_data <= RESIZE(int_logits_write_beats_128(63 downto 32), 32);
                    when ADDR_LOGITS_WRITE_BEATS_128_CTRL =>
                        rdata_data(0) <= int_logits_write_beats_128_ap_vld;
                    when ADDR_WEIGHT_WAIT_CYCLES_DATA_0 =>
                        rdata_data <= RESIZE(int_weight_wait_cycles(31 downto 0), 32);
                    when ADDR_WEIGHT_WAIT_CYCLES_DATA_1 =>
                        rdata_data <= RESIZE(int_weight_wait_cycles(63 downto 32), 32);
                    when ADDR_WEIGHT_WAIT_CYCLES_CTRL =>
                        rdata_data(0) <= int_weight_wait_cycles_ap_vld;
                    when ADDR_DOT_ACTIVE_CYCLES_DATA_0 =>
                        rdata_data <= RESIZE(int_dot_active_cycles(31 downto 0), 32);
                    when ADDR_DOT_ACTIVE_CYCLES_DATA_1 =>
                        rdata_data <= RESIZE(int_dot_active_cycles(63 downto 32), 32);
                    when ADDR_DOT_ACTIVE_CYCLES_CTRL =>
                        rdata_data(0) <= int_dot_active_cycles_ap_vld;
                    when ADDR_LOGITS_WRITTEN_DATA_0 =>
                        rdata_data <= RESIZE(int_logits_written(31 downto 0), 32);
                    when ADDR_LOGITS_WRITTEN_CTRL =>
                        rdata_data(0) <= int_logits_written_ap_vld;
                    when others =>
                        NULL;
                    end case;
                end if;
            end if;
        end if;
    end process;

-- ----------------------- Register logic ----------------
    interrupt            <= int_gie and (int_isr(0) or int_isr(1));
    ap_start             <= int_ap_start;
    task_ap_done         <= (ap_done and not auto_restart_status) or auto_restart_done;
    task_ap_ready        <= ap_ready and not int_auto_restart;
    auto_restart_done    <= auto_restart_status and (ap_idle and not int_ap_idle);
    hidden_offset        <= STD_LOGIC_VECTOR(int_hidden_offset);
    shared_vocab_weight  <= STD_LOGIC_VECTOR(int_shared_vocab_weight);
    history_bitmap       <= STD_LOGIC_VECTOR(int_history_bitmap);
    optional_logits      <= STD_LOGIC_VECTOR(int_optional_logits);
    hidden_size_r        <= STD_LOGIC_VECTOR(int_hidden_size_r);
    vocab_size           <= STD_LOGIC_VECTOR(int_vocab_size);
    run_mode             <= STD_LOGIC_VECTOR(int_run_mode);
    valid_rows           <= STD_LOGIC_VECTOR(int_valid_rows);
    write_logits         <= STD_LOGIC_VECTOR(int_write_logits);
    enable_penalty       <= STD_LOGIC_VECTOR(int_enable_penalty);
    repetition_penalty   <= STD_LOGIC_VECTOR(int_repetition_penalty);
    abi_version          <= STD_LOGIC_VECTOR(int_abi_version);
    job_id               <= STD_LOGIC_VECTOR(int_job_id);
    flags                <= STD_LOGIC_VECTOR(int_flags);

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_start <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1' and WDATA(0) = '1') then
                    int_ap_start <= '1';
                elsif (ap_ready = '1') then
                    int_ap_start <= int_auto_restart; -- clear on handshake/auto restart
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_done <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_done <= ap_done;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_task_ap_done <= '0';
            elsif (ACLK_EN = '1') then
                if (task_ap_done = '1') then
                    int_task_ap_done <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_AP_CTRL) then
                    int_task_ap_done <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_idle <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_idle <= ap_idle;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_ready <= '0';
            elsif (ACLK_EN = '1') then
                if (task_ap_ready = '1') then
                    int_ap_ready <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_AP_CTRL) then
                    int_ap_ready <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_auto_restart <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1') then
                    int_auto_restart <= WDATA(7);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                auto_restart_status <= '0';
            elsif (ACLK_EN = '1') then
                if (int_auto_restart = '1') then
                    auto_restart_status <= '1';
                elsif (ap_idle = '1') then
                    auto_restart_status <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_gie <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_GIE and WSTRB(0) = '1') then
                    int_gie <= WDATA(0);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ier <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_IER and WSTRB(0) = '1') then
                    int_ier <= UNSIGNED(WDATA(1 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(0) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(0) = '1' and ap_done = '1') then
                    int_isr(0) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(0) <= int_isr(0) xor WDATA(0); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(1) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(1) = '1' and ap_ready = '1') then
                    int_isr(1) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(1) <= int_isr(1) xor WDATA(1); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HIDDEN_OFFSET_DATA_0) then
                    int_hidden_offset(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_hidden_offset(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HIDDEN_OFFSET_DATA_1) then
                    int_hidden_offset(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_hidden_offset(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_SHARED_VOCAB_WEIGHT_DATA_0) then
                    int_shared_vocab_weight(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_shared_vocab_weight(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_SHARED_VOCAB_WEIGHT_DATA_1) then
                    int_shared_vocab_weight(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_shared_vocab_weight(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HISTORY_BITMAP_DATA_0) then
                    int_history_bitmap(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_history_bitmap(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HISTORY_BITMAP_DATA_1) then
                    int_history_bitmap(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_history_bitmap(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_OPTIONAL_LOGITS_DATA_0) then
                    int_optional_logits(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_optional_logits(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_OPTIONAL_LOGITS_DATA_1) then
                    int_optional_logits(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_optional_logits(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HIDDEN_SIZE_R_DATA_0) then
                    int_hidden_size_r(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_hidden_size_r(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_VOCAB_SIZE_DATA_0) then
                    int_vocab_size(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_vocab_size(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_RUN_MODE_DATA_0) then
                    int_run_mode(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_run_mode(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_VALID_ROWS_DATA_0) then
                    int_valid_rows(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_valid_rows(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_WRITE_LOGITS_DATA_0) then
                    int_write_logits(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_write_logits(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_PENALTY_DATA_0) then
                    int_enable_penalty(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_penalty(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_REPETITION_PENALTY_DATA_0) then
                    int_repetition_penalty(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_repetition_penalty(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ABI_VERSION_DATA_0) then
                    int_abi_version(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_abi_version(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_JOB_ID_DATA_0) then
                    int_job_id(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_job_id(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_FLAGS_DATA_0) then
                    int_flags(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_flags(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_selected_token <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (selected_token_ap_vld = '1') then
                    int_selected_token <= UNSIGNED(selected_token);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_selected_token_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (selected_token_ap_vld = '1') then
                    int_selected_token_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_SELECTED_TOKEN_CTRL) then
                    int_selected_token_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_selected_score <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (selected_score_ap_vld = '1') then
                    int_selected_score <= UNSIGNED(selected_score);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_selected_score_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (selected_score_ap_vld = '1') then
                    int_selected_score_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_SELECTED_SCORE_CTRL) then
                    int_selected_score_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_job_id <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_job_id_ap_vld = '1') then
                    int_result_job_id <= UNSIGNED(result_job_id);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_job_id_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_job_id_ap_vld = '1') then
                    int_result_job_id_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_JOB_ID_CTRL) then
                    int_result_job_id_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_status_code <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (status_code_ap_vld = '1') then
                    int_status_code <= UNSIGNED(status_code);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_status_code_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (status_code_ap_vld = '1') then
                    int_status_code_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_STATUS_CODE_CTRL) then
                    int_status_code_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_processed_rows <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (processed_rows_ap_vld = '1') then
                    int_processed_rows <= UNSIGNED(processed_rows);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_processed_rows_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (processed_rows_ap_vld = '1') then
                    int_processed_rows_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PROCESSED_ROWS_CTRL) then
                    int_processed_rows_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_cycles_total <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (perf_cycles_total_ap_vld = '1') then
                    int_perf_cycles_total <= UNSIGNED(perf_cycles_total);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_cycles_total_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (perf_cycles_total_ap_vld = '1') then
                    int_perf_cycles_total_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PERF_CYCLES_TOTAL_CTRL) then
                    int_perf_cycles_total_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_cycles_compute <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (perf_cycles_compute_ap_vld = '1') then
                    int_perf_cycles_compute <= UNSIGNED(perf_cycles_compute);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_cycles_compute_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (perf_cycles_compute_ap_vld = '1') then
                    int_perf_cycles_compute_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PERF_CYCLES_COMPUTE_CTRL) then
                    int_perf_cycles_compute_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_read_beats <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (perf_read_beats_ap_vld = '1') then
                    int_perf_read_beats <= UNSIGNED(perf_read_beats);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_read_beats_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (perf_read_beats_ap_vld = '1') then
                    int_perf_read_beats_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PERF_READ_BEATS_CTRL) then
                    int_perf_read_beats_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_write_beats <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (perf_write_beats_ap_vld = '1') then
                    int_perf_write_beats <= UNSIGNED(perf_write_beats);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_perf_write_beats_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (perf_write_beats_ap_vld = '1') then
                    int_perf_write_beats_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_PERF_WRITE_BEATS_CTRL) then
                    int_perf_write_beats_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_output_crc <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (output_crc_ap_vld = '1') then
                    int_output_crc <= UNSIGNED(output_crc);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_output_crc_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (output_crc_ap_vld = '1') then
                    int_output_crc_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_OUTPUT_CRC_CTRL) then
                    int_output_crc_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_build_config <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (build_config_ap_vld = '1') then
                    int_build_config <= UNSIGNED(build_config);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_build_config_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (build_config_ap_vld = '1') then
                    int_build_config_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_BUILD_CONFIG_CTRL) then
                    int_build_config_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_capabilities <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (capabilities_ap_vld = '1') then
                    int_capabilities <= UNSIGNED(capabilities);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_capabilities_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (capabilities_ap_vld = '1') then
                    int_capabilities_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_CAPABILITIES_CTRL) then
                    int_capabilities_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_vocab_rows_done <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (vocab_rows_done_ap_vld = '1') then
                    int_vocab_rows_done <= UNSIGNED(vocab_rows_done);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_vocab_rows_done_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (vocab_rows_done_ap_vld = '1') then
                    int_vocab_rows_done_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_VOCAB_ROWS_DONE_CTRL) then
                    int_vocab_rows_done_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_weight_read_beats_128 <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (weight_read_beats_128_ap_vld = '1') then
                    int_weight_read_beats_128 <= UNSIGNED(weight_read_beats_128);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_weight_read_beats_128_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (weight_read_beats_128_ap_vld = '1') then
                    int_weight_read_beats_128_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_WEIGHT_READ_BEATS_128_CTRL) then
                    int_weight_read_beats_128_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_history_read_words <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (history_read_words_ap_vld = '1') then
                    int_history_read_words <= UNSIGNED(history_read_words);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_history_read_words_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (history_read_words_ap_vld = '1') then
                    int_history_read_words_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_HISTORY_READ_WORDS_CTRL) then
                    int_history_read_words_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_logits_write_beats_128 <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (logits_write_beats_128_ap_vld = '1') then
                    int_logits_write_beats_128 <= UNSIGNED(logits_write_beats_128);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_logits_write_beats_128_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (logits_write_beats_128_ap_vld = '1') then
                    int_logits_write_beats_128_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_LOGITS_WRITE_BEATS_128_CTRL) then
                    int_logits_write_beats_128_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_weight_wait_cycles <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (weight_wait_cycles_ap_vld = '1') then
                    int_weight_wait_cycles <= UNSIGNED(weight_wait_cycles);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_weight_wait_cycles_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (weight_wait_cycles_ap_vld = '1') then
                    int_weight_wait_cycles_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_WEIGHT_WAIT_CYCLES_CTRL) then
                    int_weight_wait_cycles_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_dot_active_cycles <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (dot_active_cycles_ap_vld = '1') then
                    int_dot_active_cycles <= UNSIGNED(dot_active_cycles);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_dot_active_cycles_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (dot_active_cycles_ap_vld = '1') then
                    int_dot_active_cycles_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_DOT_ACTIVE_CYCLES_CTRL) then
                    int_dot_active_cycles_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_logits_written <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (logits_written_ap_vld = '1') then
                    int_logits_written <= UNSIGNED(logits_written);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_logits_written_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (logits_written_ap_vld = '1') then
                    int_logits_written_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_LOGITS_WRITTEN_CTRL) then
                    int_logits_written_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;


-- ----------------------- Memory logic ------------------

end architecture behave;
