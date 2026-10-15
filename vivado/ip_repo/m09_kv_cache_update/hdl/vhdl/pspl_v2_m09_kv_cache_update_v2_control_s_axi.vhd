-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
-- Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity pspl_v2_m09_kv_cache_update_v2_control_s_axi is
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
    k_new                 :out  STD_LOGIC_VECTOR(63 downto 0);
    v_new                 :out  STD_LOGIC_VECTOR(63 downto 0);
    k_cache               :out  STD_LOGIC_VECTOR(63 downto 0);
    v_cache               :out  STD_LOGIC_VECTOR(63 downto 0);
    run_mode              :out  STD_LOGIC_VECTOR(31 downto 0);
    layer_id              :out  STD_LOGIC_VECTOR(31 downto 0);
    start_pos             :out  STD_LOGIC_VECTOR(31 downto 0);
    valid_rows            :out  STD_LOGIC_VECTOR(31 downto 0);
    max_sequence          :out  STD_LOGIC_VECTOR(31 downto 0);
    abi_version           :out  STD_LOGIC_VECTOR(31 downto 0);
    job_id                :out  STD_LOGIC_VECTOR(31 downto 0);
    flags                 :out  STD_LOGIC_VECTOR(31 downto 0);
    cache_len_after       :in   STD_LOGIC_VECTOR(31 downto 0);
    cache_len_after_ap_vld :in   STD_LOGIC;
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
    ap_start              :out  STD_LOGIC;
    ap_done               :in   STD_LOGIC;
    ap_ready              :in   STD_LOGIC;
    ap_idle               :in   STD_LOGIC
);
end entity pspl_v2_m09_kv_cache_update_v2_control_s_axi;

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
-- 0x010 : Data signal of k_new
--         bit 31~0 - k_new[31:0] (Read/Write)
-- 0x014 : Data signal of k_new
--         bit 31~0 - k_new[63:32] (Read/Write)
-- 0x018 : reserved
-- 0x01c : Data signal of v_new
--         bit 31~0 - v_new[31:0] (Read/Write)
-- 0x020 : Data signal of v_new
--         bit 31~0 - v_new[63:32] (Read/Write)
-- 0x024 : reserved
-- 0x028 : Data signal of k_cache
--         bit 31~0 - k_cache[31:0] (Read/Write)
-- 0x02c : Data signal of k_cache
--         bit 31~0 - k_cache[63:32] (Read/Write)
-- 0x030 : reserved
-- 0x034 : Data signal of v_cache
--         bit 31~0 - v_cache[31:0] (Read/Write)
-- 0x038 : Data signal of v_cache
--         bit 31~0 - v_cache[63:32] (Read/Write)
-- 0x03c : reserved
-- 0x040 : Data signal of run_mode
--         bit 31~0 - run_mode[31:0] (Read/Write)
-- 0x044 : reserved
-- 0x048 : Data signal of layer_id
--         bit 31~0 - layer_id[31:0] (Read/Write)
-- 0x04c : reserved
-- 0x050 : Data signal of start_pos
--         bit 31~0 - start_pos[31:0] (Read/Write)
-- 0x054 : reserved
-- 0x058 : Data signal of valid_rows
--         bit 31~0 - valid_rows[31:0] (Read/Write)
-- 0x05c : reserved
-- 0x060 : Data signal of max_sequence
--         bit 31~0 - max_sequence[31:0] (Read/Write)
-- 0x064 : reserved
-- 0x068 : Data signal of abi_version
--         bit 31~0 - abi_version[31:0] (Read/Write)
-- 0x06c : reserved
-- 0x070 : Data signal of job_id
--         bit 31~0 - job_id[31:0] (Read/Write)
-- 0x074 : reserved
-- 0x078 : Data signal of flags
--         bit 31~0 - flags[31:0] (Read/Write)
-- 0x07c : reserved
-- 0x080 : Data signal of cache_len_after
--         bit 31~0 - cache_len_after[31:0] (Read)
-- 0x084 : Control signal of cache_len_after
--         bit 0  - cache_len_after_ap_vld (Read/COR)
--         others - reserved
-- 0x090 : Data signal of status_code
--         bit 31~0 - status_code[31:0] (Read)
-- 0x094 : Control signal of status_code
--         bit 0  - status_code_ap_vld (Read/COR)
--         others - reserved
-- 0x0a0 : Data signal of processed_rows
--         bit 31~0 - processed_rows[31:0] (Read)
-- 0x0a4 : Control signal of processed_rows
--         bit 0  - processed_rows_ap_vld (Read/COR)
--         others - reserved
-- 0x0b0 : Data signal of perf_cycles_total
--         bit 31~0 - perf_cycles_total[31:0] (Read)
-- 0x0b4 : Data signal of perf_cycles_total
--         bit 31~0 - perf_cycles_total[63:32] (Read)
-- 0x0b8 : Control signal of perf_cycles_total
--         bit 0  - perf_cycles_total_ap_vld (Read/COR)
--         others - reserved
-- 0x0c8 : Data signal of perf_cycles_compute
--         bit 31~0 - perf_cycles_compute[31:0] (Read)
-- 0x0cc : Data signal of perf_cycles_compute
--         bit 31~0 - perf_cycles_compute[63:32] (Read)
-- 0x0d0 : Control signal of perf_cycles_compute
--         bit 0  - perf_cycles_compute_ap_vld (Read/COR)
--         others - reserved
-- 0x0e0 : Data signal of perf_read_beats
--         bit 31~0 - perf_read_beats[31:0] (Read)
-- 0x0e4 : Data signal of perf_read_beats
--         bit 31~0 - perf_read_beats[63:32] (Read)
-- 0x0e8 : Control signal of perf_read_beats
--         bit 0  - perf_read_beats_ap_vld (Read/COR)
--         others - reserved
-- 0x0f8 : Data signal of perf_write_beats
--         bit 31~0 - perf_write_beats[31:0] (Read)
-- 0x0fc : Data signal of perf_write_beats
--         bit 31~0 - perf_write_beats[63:32] (Read)
-- 0x100 : Control signal of perf_write_beats
--         bit 0  - perf_write_beats_ap_vld (Read/COR)
--         others - reserved
-- 0x110 : Data signal of output_crc
--         bit 31~0 - output_crc[31:0] (Read)
-- 0x114 : Control signal of output_crc
--         bit 0  - output_crc_ap_vld (Read/COR)
--         others - reserved
-- 0x120 : Data signal of build_config
--         bit 31~0 - build_config[31:0] (Read)
-- 0x124 : Control signal of build_config
--         bit 0  - build_config_ap_vld (Read/COR)
--         others - reserved
-- (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

architecture behave of pspl_v2_m09_kv_cache_update_v2_control_s_axi is
    type states is (wridle, wrdata, wrresp, wrreset, rdidle, rddata, rdreset);  -- read and write fsm states
    signal wstate  : states := wrreset;
    signal rstate  : states := rdreset;
    signal wnext, rnext: states;
    constant ADDR_AP_CTRL                    : INTEGER := 16#000#;
    constant ADDR_GIE                        : INTEGER := 16#004#;
    constant ADDR_IER                        : INTEGER := 16#008#;
    constant ADDR_ISR                        : INTEGER := 16#00c#;
    constant ADDR_K_NEW_DATA_0               : INTEGER := 16#010#;
    constant ADDR_K_NEW_DATA_1               : INTEGER := 16#014#;
    constant ADDR_K_NEW_CTRL                 : INTEGER := 16#018#;
    constant ADDR_V_NEW_DATA_0               : INTEGER := 16#01c#;
    constant ADDR_V_NEW_DATA_1               : INTEGER := 16#020#;
    constant ADDR_V_NEW_CTRL                 : INTEGER := 16#024#;
    constant ADDR_K_CACHE_DATA_0             : INTEGER := 16#028#;
    constant ADDR_K_CACHE_DATA_1             : INTEGER := 16#02c#;
    constant ADDR_K_CACHE_CTRL               : INTEGER := 16#030#;
    constant ADDR_V_CACHE_DATA_0             : INTEGER := 16#034#;
    constant ADDR_V_CACHE_DATA_1             : INTEGER := 16#038#;
    constant ADDR_V_CACHE_CTRL               : INTEGER := 16#03c#;
    constant ADDR_RUN_MODE_DATA_0            : INTEGER := 16#040#;
    constant ADDR_RUN_MODE_CTRL              : INTEGER := 16#044#;
    constant ADDR_LAYER_ID_DATA_0            : INTEGER := 16#048#;
    constant ADDR_LAYER_ID_CTRL              : INTEGER := 16#04c#;
    constant ADDR_START_POS_DATA_0           : INTEGER := 16#050#;
    constant ADDR_START_POS_CTRL             : INTEGER := 16#054#;
    constant ADDR_VALID_ROWS_DATA_0          : INTEGER := 16#058#;
    constant ADDR_VALID_ROWS_CTRL            : INTEGER := 16#05c#;
    constant ADDR_MAX_SEQUENCE_DATA_0        : INTEGER := 16#060#;
    constant ADDR_MAX_SEQUENCE_CTRL          : INTEGER := 16#064#;
    constant ADDR_ABI_VERSION_DATA_0         : INTEGER := 16#068#;
    constant ADDR_ABI_VERSION_CTRL           : INTEGER := 16#06c#;
    constant ADDR_JOB_ID_DATA_0              : INTEGER := 16#070#;
    constant ADDR_JOB_ID_CTRL                : INTEGER := 16#074#;
    constant ADDR_FLAGS_DATA_0               : INTEGER := 16#078#;
    constant ADDR_FLAGS_CTRL                 : INTEGER := 16#07c#;
    constant ADDR_CACHE_LEN_AFTER_DATA_0     : INTEGER := 16#080#;
    constant ADDR_CACHE_LEN_AFTER_CTRL       : INTEGER := 16#084#;
    constant ADDR_STATUS_CODE_DATA_0         : INTEGER := 16#090#;
    constant ADDR_STATUS_CODE_CTRL           : INTEGER := 16#094#;
    constant ADDR_PROCESSED_ROWS_DATA_0      : INTEGER := 16#0a0#;
    constant ADDR_PROCESSED_ROWS_CTRL        : INTEGER := 16#0a4#;
    constant ADDR_PERF_CYCLES_TOTAL_DATA_0   : INTEGER := 16#0b0#;
    constant ADDR_PERF_CYCLES_TOTAL_DATA_1   : INTEGER := 16#0b4#;
    constant ADDR_PERF_CYCLES_TOTAL_CTRL     : INTEGER := 16#0b8#;
    constant ADDR_PERF_CYCLES_COMPUTE_DATA_0 : INTEGER := 16#0c8#;
    constant ADDR_PERF_CYCLES_COMPUTE_DATA_1 : INTEGER := 16#0cc#;
    constant ADDR_PERF_CYCLES_COMPUTE_CTRL   : INTEGER := 16#0d0#;
    constant ADDR_PERF_READ_BEATS_DATA_0     : INTEGER := 16#0e0#;
    constant ADDR_PERF_READ_BEATS_DATA_1     : INTEGER := 16#0e4#;
    constant ADDR_PERF_READ_BEATS_CTRL       : INTEGER := 16#0e8#;
    constant ADDR_PERF_WRITE_BEATS_DATA_0    : INTEGER := 16#0f8#;
    constant ADDR_PERF_WRITE_BEATS_DATA_1    : INTEGER := 16#0fc#;
    constant ADDR_PERF_WRITE_BEATS_CTRL      : INTEGER := 16#100#;
    constant ADDR_OUTPUT_CRC_DATA_0          : INTEGER := 16#110#;
    constant ADDR_OUTPUT_CRC_CTRL            : INTEGER := 16#114#;
    constant ADDR_BUILD_CONFIG_DATA_0        : INTEGER := 16#120#;
    constant ADDR_BUILD_CONFIG_CTRL          : INTEGER := 16#124#;
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
    signal int_k_new           : UNSIGNED(63 downto 0) := (others => '0');
    signal int_v_new           : UNSIGNED(63 downto 0) := (others => '0');
    signal int_k_cache         : UNSIGNED(63 downto 0) := (others => '0');
    signal int_v_cache         : UNSIGNED(63 downto 0) := (others => '0');
    signal int_run_mode        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_layer_id        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_start_pos       : UNSIGNED(31 downto 0) := (others => '0');
    signal int_valid_rows      : UNSIGNED(31 downto 0) := (others => '0');
    signal int_max_sequence    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_abi_version     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_job_id          : UNSIGNED(31 downto 0) := (others => '0');
    signal int_flags           : UNSIGNED(31 downto 0) := (others => '0');
    signal int_cache_len_after_ap_vld : STD_LOGIC;
    signal int_cache_len_after : UNSIGNED(31 downto 0) := (others => '0');
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
                    when ADDR_K_NEW_DATA_0 =>
                        rdata_data <= RESIZE(int_k_new(31 downto 0), 32);
                    when ADDR_K_NEW_DATA_1 =>
                        rdata_data <= RESIZE(int_k_new(63 downto 32), 32);
                    when ADDR_V_NEW_DATA_0 =>
                        rdata_data <= RESIZE(int_v_new(31 downto 0), 32);
                    when ADDR_V_NEW_DATA_1 =>
                        rdata_data <= RESIZE(int_v_new(63 downto 32), 32);
                    when ADDR_K_CACHE_DATA_0 =>
                        rdata_data <= RESIZE(int_k_cache(31 downto 0), 32);
                    when ADDR_K_CACHE_DATA_1 =>
                        rdata_data <= RESIZE(int_k_cache(63 downto 32), 32);
                    when ADDR_V_CACHE_DATA_0 =>
                        rdata_data <= RESIZE(int_v_cache(31 downto 0), 32);
                    when ADDR_V_CACHE_DATA_1 =>
                        rdata_data <= RESIZE(int_v_cache(63 downto 32), 32);
                    when ADDR_RUN_MODE_DATA_0 =>
                        rdata_data <= RESIZE(int_run_mode(31 downto 0), 32);
                    when ADDR_LAYER_ID_DATA_0 =>
                        rdata_data <= RESIZE(int_layer_id(31 downto 0), 32);
                    when ADDR_START_POS_DATA_0 =>
                        rdata_data <= RESIZE(int_start_pos(31 downto 0), 32);
                    when ADDR_VALID_ROWS_DATA_0 =>
                        rdata_data <= RESIZE(int_valid_rows(31 downto 0), 32);
                    when ADDR_MAX_SEQUENCE_DATA_0 =>
                        rdata_data <= RESIZE(int_max_sequence(31 downto 0), 32);
                    when ADDR_ABI_VERSION_DATA_0 =>
                        rdata_data <= RESIZE(int_abi_version(31 downto 0), 32);
                    when ADDR_JOB_ID_DATA_0 =>
                        rdata_data <= RESIZE(int_job_id(31 downto 0), 32);
                    when ADDR_FLAGS_DATA_0 =>
                        rdata_data <= RESIZE(int_flags(31 downto 0), 32);
                    when ADDR_CACHE_LEN_AFTER_DATA_0 =>
                        rdata_data <= RESIZE(int_cache_len_after(31 downto 0), 32);
                    when ADDR_CACHE_LEN_AFTER_CTRL =>
                        rdata_data(0) <= int_cache_len_after_ap_vld;
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
    k_new                <= STD_LOGIC_VECTOR(int_k_new);
    v_new                <= STD_LOGIC_VECTOR(int_v_new);
    k_cache              <= STD_LOGIC_VECTOR(int_k_cache);
    v_cache              <= STD_LOGIC_VECTOR(int_v_cache);
    run_mode             <= STD_LOGIC_VECTOR(int_run_mode);
    layer_id             <= STD_LOGIC_VECTOR(int_layer_id);
    start_pos            <= STD_LOGIC_VECTOR(int_start_pos);
    valid_rows           <= STD_LOGIC_VECTOR(int_valid_rows);
    max_sequence         <= STD_LOGIC_VECTOR(int_max_sequence);
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
                if (w_hs = '1' and waddr = ADDR_K_NEW_DATA_0) then
                    int_k_new(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_k_new(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_K_NEW_DATA_1) then
                    int_k_new(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_k_new(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_V_NEW_DATA_0) then
                    int_v_new(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_v_new(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_V_NEW_DATA_1) then
                    int_v_new(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_v_new(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_K_CACHE_DATA_0) then
                    int_k_cache(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_k_cache(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_K_CACHE_DATA_1) then
                    int_k_cache(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_k_cache(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_V_CACHE_DATA_0) then
                    int_v_cache(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_v_cache(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_V_CACHE_DATA_1) then
                    int_v_cache(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_v_cache(63 downto 32));
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
                if (w_hs = '1' and waddr = ADDR_LAYER_ID_DATA_0) then
                    int_layer_id(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_layer_id(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_START_POS_DATA_0) then
                    int_start_pos(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_start_pos(31 downto 0));
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
                if (w_hs = '1' and waddr = ADDR_MAX_SEQUENCE_DATA_0) then
                    int_max_sequence(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_max_sequence(31 downto 0));
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
                int_cache_len_after <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (cache_len_after_ap_vld = '1') then
                    int_cache_len_after <= UNSIGNED(cache_len_after);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_cache_len_after_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (cache_len_after_ap_vld = '1') then
                    int_cache_len_after_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_CACHE_LEN_AFTER_CTRL) then
                    int_cache_len_after_ap_vld <= '0'; -- clear on read
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


-- ----------------------- Memory logic ------------------

end architecture behave;
