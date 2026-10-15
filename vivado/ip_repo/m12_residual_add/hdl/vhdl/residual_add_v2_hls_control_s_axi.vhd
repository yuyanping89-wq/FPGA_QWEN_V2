-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
-- Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity residual_add_v2_hls_control_s_axi is
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
    a                     :out  STD_LOGIC_VECTOR(63 downto 0);
    b                     :out  STD_LOGIC_VECTOR(63 downto 0);
    y                     :out  STD_LOGIC_VECTOR(63 downto 0);
    run_mode              :out  STD_LOGIC_VECTOR(31 downto 0);
    valid_rows            :out  STD_LOGIC_VECTOR(31 downto 0);
    hidden_size           :out  STD_LOGIC_VECTOR(31 downto 0);
    residual_role         :out  STD_LOGIC_VECTOR(31 downto 0);
    abi_version           :out  STD_LOGIC_VECTOR(31 downto 0);
    job_id                :out  STD_LOGIC_VECTOR(31 downto 0);
    flags                 :out  STD_LOGIC_VECTOR(31 downto 0);
    result_status_code    :in   STD_LOGIC_VECTOR(31 downto 0);
    result_status_code_ap_vld :in   STD_LOGIC;
    result_processed_rows :in   STD_LOGIC_VECTOR(31 downto 0);
    result_processed_rows_ap_vld :in   STD_LOGIC;
    result_perf_cycles_total :in   STD_LOGIC_VECTOR(63 downto 0);
    result_perf_cycles_total_ap_vld :in   STD_LOGIC;
    result_perf_cycles_compute :in   STD_LOGIC_VECTOR(63 downto 0);
    result_perf_cycles_compute_ap_vld :in   STD_LOGIC;
    result_perf_read_beats :in   STD_LOGIC_VECTOR(63 downto 0);
    result_perf_read_beats_ap_vld :in   STD_LOGIC;
    result_perf_write_beats :in   STD_LOGIC_VECTOR(63 downto 0);
    result_perf_write_beats_ap_vld :in   STD_LOGIC;
    result_output_crc     :in   STD_LOGIC_VECTOR(31 downto 0);
    result_output_crc_ap_vld :in   STD_LOGIC;
    result_build_config   :in   STD_LOGIC_VECTOR(31 downto 0);
    result_build_config_ap_vld :in   STD_LOGIC;
    ap_start              :out  STD_LOGIC;
    ap_done               :in   STD_LOGIC;
    ap_ready              :in   STD_LOGIC;
    ap_idle               :in   STD_LOGIC
);
end entity residual_add_v2_hls_control_s_axi;

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
-- 0x010 : Data signal of a
--         bit 31~0 - a[31:0] (Read/Write)
-- 0x014 : Data signal of a
--         bit 31~0 - a[63:32] (Read/Write)
-- 0x018 : reserved
-- 0x01c : Data signal of b
--         bit 31~0 - b[31:0] (Read/Write)
-- 0x020 : Data signal of b
--         bit 31~0 - b[63:32] (Read/Write)
-- 0x024 : reserved
-- 0x028 : Data signal of y
--         bit 31~0 - y[31:0] (Read/Write)
-- 0x02c : Data signal of y
--         bit 31~0 - y[63:32] (Read/Write)
-- 0x030 : reserved
-- 0x034 : Data signal of run_mode
--         bit 31~0 - run_mode[31:0] (Read/Write)
-- 0x038 : reserved
-- 0x03c : Data signal of valid_rows
--         bit 31~0 - valid_rows[31:0] (Read/Write)
-- 0x040 : reserved
-- 0x044 : Data signal of hidden_size
--         bit 31~0 - hidden_size[31:0] (Read/Write)
-- 0x048 : reserved
-- 0x04c : Data signal of residual_role
--         bit 31~0 - residual_role[31:0] (Read/Write)
-- 0x050 : reserved
-- 0x054 : Data signal of abi_version
--         bit 31~0 - abi_version[31:0] (Read/Write)
-- 0x058 : reserved
-- 0x05c : Data signal of job_id
--         bit 31~0 - job_id[31:0] (Read/Write)
-- 0x060 : reserved
-- 0x064 : Data signal of flags
--         bit 31~0 - flags[31:0] (Read/Write)
-- 0x068 : reserved
-- 0x06c : Data signal of result_status_code
--         bit 31~0 - result_status_code[31:0] (Read)
-- 0x070 : Control signal of result_status_code
--         bit 0  - result_status_code_ap_vld (Read/COR)
--         others - reserved
-- 0x07c : Data signal of result_processed_rows
--         bit 31~0 - result_processed_rows[31:0] (Read)
-- 0x080 : Control signal of result_processed_rows
--         bit 0  - result_processed_rows_ap_vld (Read/COR)
--         others - reserved
-- 0x08c : Data signal of result_perf_cycles_total
--         bit 31~0 - result_perf_cycles_total[31:0] (Read)
-- 0x090 : Data signal of result_perf_cycles_total
--         bit 31~0 - result_perf_cycles_total[63:32] (Read)
-- 0x094 : Control signal of result_perf_cycles_total
--         bit 0  - result_perf_cycles_total_ap_vld (Read/COR)
--         others - reserved
-- 0x0a4 : Data signal of result_perf_cycles_compute
--         bit 31~0 - result_perf_cycles_compute[31:0] (Read)
-- 0x0a8 : Data signal of result_perf_cycles_compute
--         bit 31~0 - result_perf_cycles_compute[63:32] (Read)
-- 0x0ac : Control signal of result_perf_cycles_compute
--         bit 0  - result_perf_cycles_compute_ap_vld (Read/COR)
--         others - reserved
-- 0x0bc : Data signal of result_perf_read_beats
--         bit 31~0 - result_perf_read_beats[31:0] (Read)
-- 0x0c0 : Data signal of result_perf_read_beats
--         bit 31~0 - result_perf_read_beats[63:32] (Read)
-- 0x0c4 : Control signal of result_perf_read_beats
--         bit 0  - result_perf_read_beats_ap_vld (Read/COR)
--         others - reserved
-- 0x0d4 : Data signal of result_perf_write_beats
--         bit 31~0 - result_perf_write_beats[31:0] (Read)
-- 0x0d8 : Data signal of result_perf_write_beats
--         bit 31~0 - result_perf_write_beats[63:32] (Read)
-- 0x0dc : Control signal of result_perf_write_beats
--         bit 0  - result_perf_write_beats_ap_vld (Read/COR)
--         others - reserved
-- 0x0ec : Data signal of result_output_crc
--         bit 31~0 - result_output_crc[31:0] (Read)
-- 0x0f0 : Control signal of result_output_crc
--         bit 0  - result_output_crc_ap_vld (Read/COR)
--         others - reserved
-- 0x0fc : Data signal of result_build_config
--         bit 31~0 - result_build_config[31:0] (Read)
-- 0x100 : Control signal of result_build_config
--         bit 0  - result_build_config_ap_vld (Read/COR)
--         others - reserved
-- (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

architecture behave of residual_add_v2_hls_control_s_axi is
    type states is (wridle, wrdata, wrresp, wrreset, rdidle, rddata, rdreset);  -- read and write fsm states
    signal wstate  : states := wrreset;
    signal rstate  : states := rdreset;
    signal wnext, rnext: states;
    constant ADDR_AP_CTRL                           : INTEGER := 16#000#;
    constant ADDR_GIE                               : INTEGER := 16#004#;
    constant ADDR_IER                               : INTEGER := 16#008#;
    constant ADDR_ISR                               : INTEGER := 16#00c#;
    constant ADDR_A_DATA_0                          : INTEGER := 16#010#;
    constant ADDR_A_DATA_1                          : INTEGER := 16#014#;
    constant ADDR_A_CTRL                            : INTEGER := 16#018#;
    constant ADDR_B_DATA_0                          : INTEGER := 16#01c#;
    constant ADDR_B_DATA_1                          : INTEGER := 16#020#;
    constant ADDR_B_CTRL                            : INTEGER := 16#024#;
    constant ADDR_Y_DATA_0                          : INTEGER := 16#028#;
    constant ADDR_Y_DATA_1                          : INTEGER := 16#02c#;
    constant ADDR_Y_CTRL                            : INTEGER := 16#030#;
    constant ADDR_RUN_MODE_DATA_0                   : INTEGER := 16#034#;
    constant ADDR_RUN_MODE_CTRL                     : INTEGER := 16#038#;
    constant ADDR_VALID_ROWS_DATA_0                 : INTEGER := 16#03c#;
    constant ADDR_VALID_ROWS_CTRL                   : INTEGER := 16#040#;
    constant ADDR_HIDDEN_SIZE_DATA_0                : INTEGER := 16#044#;
    constant ADDR_HIDDEN_SIZE_CTRL                  : INTEGER := 16#048#;
    constant ADDR_RESIDUAL_ROLE_DATA_0              : INTEGER := 16#04c#;
    constant ADDR_RESIDUAL_ROLE_CTRL                : INTEGER := 16#050#;
    constant ADDR_ABI_VERSION_DATA_0                : INTEGER := 16#054#;
    constant ADDR_ABI_VERSION_CTRL                  : INTEGER := 16#058#;
    constant ADDR_JOB_ID_DATA_0                     : INTEGER := 16#05c#;
    constant ADDR_JOB_ID_CTRL                       : INTEGER := 16#060#;
    constant ADDR_FLAGS_DATA_0                      : INTEGER := 16#064#;
    constant ADDR_FLAGS_CTRL                        : INTEGER := 16#068#;
    constant ADDR_RESULT_STATUS_CODE_DATA_0         : INTEGER := 16#06c#;
    constant ADDR_RESULT_STATUS_CODE_CTRL           : INTEGER := 16#070#;
    constant ADDR_RESULT_PROCESSED_ROWS_DATA_0      : INTEGER := 16#07c#;
    constant ADDR_RESULT_PROCESSED_ROWS_CTRL        : INTEGER := 16#080#;
    constant ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_0   : INTEGER := 16#08c#;
    constant ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_1   : INTEGER := 16#090#;
    constant ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL     : INTEGER := 16#094#;
    constant ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_0 : INTEGER := 16#0a4#;
    constant ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_1 : INTEGER := 16#0a8#;
    constant ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL   : INTEGER := 16#0ac#;
    constant ADDR_RESULT_PERF_READ_BEATS_DATA_0     : INTEGER := 16#0bc#;
    constant ADDR_RESULT_PERF_READ_BEATS_DATA_1     : INTEGER := 16#0c0#;
    constant ADDR_RESULT_PERF_READ_BEATS_CTRL       : INTEGER := 16#0c4#;
    constant ADDR_RESULT_PERF_WRITE_BEATS_DATA_0    : INTEGER := 16#0d4#;
    constant ADDR_RESULT_PERF_WRITE_BEATS_DATA_1    : INTEGER := 16#0d8#;
    constant ADDR_RESULT_PERF_WRITE_BEATS_CTRL      : INTEGER := 16#0dc#;
    constant ADDR_RESULT_OUTPUT_CRC_DATA_0          : INTEGER := 16#0ec#;
    constant ADDR_RESULT_OUTPUT_CRC_CTRL            : INTEGER := 16#0f0#;
    constant ADDR_RESULT_BUILD_CONFIG_DATA_0        : INTEGER := 16#0fc#;
    constant ADDR_RESULT_BUILD_CONFIG_CTRL          : INTEGER := 16#100#;
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
    signal int_a               : UNSIGNED(63 downto 0) := (others => '0');
    signal int_b               : UNSIGNED(63 downto 0) := (others => '0');
    signal int_y               : UNSIGNED(63 downto 0) := (others => '0');
    signal int_run_mode        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_valid_rows      : UNSIGNED(31 downto 0) := (others => '0');
    signal int_hidden_size     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_residual_role   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_abi_version     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_job_id          : UNSIGNED(31 downto 0) := (others => '0');
    signal int_flags           : UNSIGNED(31 downto 0) := (others => '0');
    signal int_result_status_code_ap_vld : STD_LOGIC;
    signal int_result_status_code : UNSIGNED(31 downto 0) := (others => '0');
    signal int_result_processed_rows_ap_vld : STD_LOGIC;
    signal int_result_processed_rows : UNSIGNED(31 downto 0) := (others => '0');
    signal int_result_perf_cycles_total_ap_vld : STD_LOGIC;
    signal int_result_perf_cycles_total : UNSIGNED(63 downto 0) := (others => '0');
    signal int_result_perf_cycles_compute_ap_vld : STD_LOGIC;
    signal int_result_perf_cycles_compute : UNSIGNED(63 downto 0) := (others => '0');
    signal int_result_perf_read_beats_ap_vld : STD_LOGIC;
    signal int_result_perf_read_beats : UNSIGNED(63 downto 0) := (others => '0');
    signal int_result_perf_write_beats_ap_vld : STD_LOGIC;
    signal int_result_perf_write_beats : UNSIGNED(63 downto 0) := (others => '0');
    signal int_result_output_crc_ap_vld : STD_LOGIC;
    signal int_result_output_crc : UNSIGNED(31 downto 0) := (others => '0');
    signal int_result_build_config_ap_vld : STD_LOGIC;
    signal int_result_build_config : UNSIGNED(31 downto 0) := (others => '0');


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
                    when ADDR_A_DATA_0 =>
                        rdata_data <= RESIZE(int_a(31 downto 0), 32);
                    when ADDR_A_DATA_1 =>
                        rdata_data <= RESIZE(int_a(63 downto 32), 32);
                    when ADDR_B_DATA_0 =>
                        rdata_data <= RESIZE(int_b(31 downto 0), 32);
                    when ADDR_B_DATA_1 =>
                        rdata_data <= RESIZE(int_b(63 downto 32), 32);
                    when ADDR_Y_DATA_0 =>
                        rdata_data <= RESIZE(int_y(31 downto 0), 32);
                    when ADDR_Y_DATA_1 =>
                        rdata_data <= RESIZE(int_y(63 downto 32), 32);
                    when ADDR_RUN_MODE_DATA_0 =>
                        rdata_data <= RESIZE(int_run_mode(31 downto 0), 32);
                    when ADDR_VALID_ROWS_DATA_0 =>
                        rdata_data <= RESIZE(int_valid_rows(31 downto 0), 32);
                    when ADDR_HIDDEN_SIZE_DATA_0 =>
                        rdata_data <= RESIZE(int_hidden_size(31 downto 0), 32);
                    when ADDR_RESIDUAL_ROLE_DATA_0 =>
                        rdata_data <= RESIZE(int_residual_role(31 downto 0), 32);
                    when ADDR_ABI_VERSION_DATA_0 =>
                        rdata_data <= RESIZE(int_abi_version(31 downto 0), 32);
                    when ADDR_JOB_ID_DATA_0 =>
                        rdata_data <= RESIZE(int_job_id(31 downto 0), 32);
                    when ADDR_FLAGS_DATA_0 =>
                        rdata_data <= RESIZE(int_flags(31 downto 0), 32);
                    when ADDR_RESULT_STATUS_CODE_DATA_0 =>
                        rdata_data <= RESIZE(int_result_status_code(31 downto 0), 32);
                    when ADDR_RESULT_STATUS_CODE_CTRL =>
                        rdata_data(0) <= int_result_status_code_ap_vld;
                    when ADDR_RESULT_PROCESSED_ROWS_DATA_0 =>
                        rdata_data <= RESIZE(int_result_processed_rows(31 downto 0), 32);
                    when ADDR_RESULT_PROCESSED_ROWS_CTRL =>
                        rdata_data(0) <= int_result_processed_rows_ap_vld;
                    when ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_0 =>
                        rdata_data <= RESIZE(int_result_perf_cycles_total(31 downto 0), 32);
                    when ADDR_RESULT_PERF_CYCLES_TOTAL_DATA_1 =>
                        rdata_data <= RESIZE(int_result_perf_cycles_total(63 downto 32), 32);
                    when ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL =>
                        rdata_data(0) <= int_result_perf_cycles_total_ap_vld;
                    when ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_0 =>
                        rdata_data <= RESIZE(int_result_perf_cycles_compute(31 downto 0), 32);
                    when ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA_1 =>
                        rdata_data <= RESIZE(int_result_perf_cycles_compute(63 downto 32), 32);
                    when ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL =>
                        rdata_data(0) <= int_result_perf_cycles_compute_ap_vld;
                    when ADDR_RESULT_PERF_READ_BEATS_DATA_0 =>
                        rdata_data <= RESIZE(int_result_perf_read_beats(31 downto 0), 32);
                    when ADDR_RESULT_PERF_READ_BEATS_DATA_1 =>
                        rdata_data <= RESIZE(int_result_perf_read_beats(63 downto 32), 32);
                    when ADDR_RESULT_PERF_READ_BEATS_CTRL =>
                        rdata_data(0) <= int_result_perf_read_beats_ap_vld;
                    when ADDR_RESULT_PERF_WRITE_BEATS_DATA_0 =>
                        rdata_data <= RESIZE(int_result_perf_write_beats(31 downto 0), 32);
                    when ADDR_RESULT_PERF_WRITE_BEATS_DATA_1 =>
                        rdata_data <= RESIZE(int_result_perf_write_beats(63 downto 32), 32);
                    when ADDR_RESULT_PERF_WRITE_BEATS_CTRL =>
                        rdata_data(0) <= int_result_perf_write_beats_ap_vld;
                    when ADDR_RESULT_OUTPUT_CRC_DATA_0 =>
                        rdata_data <= RESIZE(int_result_output_crc(31 downto 0), 32);
                    when ADDR_RESULT_OUTPUT_CRC_CTRL =>
                        rdata_data(0) <= int_result_output_crc_ap_vld;
                    when ADDR_RESULT_BUILD_CONFIG_DATA_0 =>
                        rdata_data <= RESIZE(int_result_build_config(31 downto 0), 32);
                    when ADDR_RESULT_BUILD_CONFIG_CTRL =>
                        rdata_data(0) <= int_result_build_config_ap_vld;
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
    a                    <= STD_LOGIC_VECTOR(int_a);
    b                    <= STD_LOGIC_VECTOR(int_b);
    y                    <= STD_LOGIC_VECTOR(int_y);
    run_mode             <= STD_LOGIC_VECTOR(int_run_mode);
    valid_rows           <= STD_LOGIC_VECTOR(int_valid_rows);
    hidden_size          <= STD_LOGIC_VECTOR(int_hidden_size);
    residual_role        <= STD_LOGIC_VECTOR(int_residual_role);
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
                if (w_hs = '1' and waddr = ADDR_A_DATA_0) then
                    int_a(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_a(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_A_DATA_1) then
                    int_a(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_a(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_B_DATA_0) then
                    int_b(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_b(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_B_DATA_1) then
                    int_b(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_b(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_Y_DATA_0) then
                    int_y(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_y(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_Y_DATA_1) then
                    int_y(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_y(63 downto 32));
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
                if (w_hs = '1' and waddr = ADDR_HIDDEN_SIZE_DATA_0) then
                    int_hidden_size(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_hidden_size(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_RESIDUAL_ROLE_DATA_0) then
                    int_residual_role(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_residual_role(31 downto 0));
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
                int_result_status_code <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_status_code_ap_vld = '1') then
                    int_result_status_code <= UNSIGNED(result_status_code);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_status_code_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_status_code_ap_vld = '1') then
                    int_result_status_code_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_STATUS_CODE_CTRL) then
                    int_result_status_code_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_processed_rows <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_processed_rows_ap_vld = '1') then
                    int_result_processed_rows <= UNSIGNED(result_processed_rows);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_processed_rows_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_processed_rows_ap_vld = '1') then
                    int_result_processed_rows_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_PROCESSED_ROWS_CTRL) then
                    int_result_processed_rows_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_cycles_total <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_perf_cycles_total_ap_vld = '1') then
                    int_result_perf_cycles_total <= UNSIGNED(result_perf_cycles_total);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_cycles_total_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_perf_cycles_total_ap_vld = '1') then
                    int_result_perf_cycles_total_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL) then
                    int_result_perf_cycles_total_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_cycles_compute <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_perf_cycles_compute_ap_vld = '1') then
                    int_result_perf_cycles_compute <= UNSIGNED(result_perf_cycles_compute);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_cycles_compute_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_perf_cycles_compute_ap_vld = '1') then
                    int_result_perf_cycles_compute_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL) then
                    int_result_perf_cycles_compute_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_read_beats <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_perf_read_beats_ap_vld = '1') then
                    int_result_perf_read_beats <= UNSIGNED(result_perf_read_beats);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_read_beats_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_perf_read_beats_ap_vld = '1') then
                    int_result_perf_read_beats_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_PERF_READ_BEATS_CTRL) then
                    int_result_perf_read_beats_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_write_beats <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_perf_write_beats_ap_vld = '1') then
                    int_result_perf_write_beats <= UNSIGNED(result_perf_write_beats);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_perf_write_beats_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_perf_write_beats_ap_vld = '1') then
                    int_result_perf_write_beats_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_PERF_WRITE_BEATS_CTRL) then
                    int_result_perf_write_beats_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_output_crc <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_output_crc_ap_vld = '1') then
                    int_result_output_crc <= UNSIGNED(result_output_crc);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_output_crc_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_output_crc_ap_vld = '1') then
                    int_result_output_crc_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_OUTPUT_CRC_CTRL) then
                    int_result_output_crc_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_build_config <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (result_build_config_ap_vld = '1') then
                    int_result_build_config <= UNSIGNED(result_build_config);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_result_build_config_ap_vld <= '0';
            elsif (ACLK_EN = '1') then
                if (result_build_config_ap_vld = '1') then
                    int_result_build_config_ap_vld <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_RESULT_BUILD_CONFIG_CTRL) then
                    int_result_build_config_ap_vld <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;


-- ----------------------- Memory logic ------------------

end architecture behave;
