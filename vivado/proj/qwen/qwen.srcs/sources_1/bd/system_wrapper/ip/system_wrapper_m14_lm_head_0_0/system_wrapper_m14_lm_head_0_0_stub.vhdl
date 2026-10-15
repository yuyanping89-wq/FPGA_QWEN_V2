-- Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2021.1 (win64) Build 3247384 Thu Jun 10 19:36:33 MDT 2021
-- Date        : Mon Oct 12 15:24:12 2026
-- Host        : LAPTOP-VTIKQNGE running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               d:/FPGA_PRO/FPGA_QWEN/vivado/proj/qwen/qwen.srcs/sources_1/bd/system_wrapper/ip/system_wrapper_m14_lm_head_0_0/system_wrapper_m14_lm_head_0_0_stub.vhdl
-- Design      : system_wrapper_m14_lm_head_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xczu9eg-ffvb1156-2-i
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity system_wrapper_m14_lm_head_0_0 is
  Port ( 
    s_axi_control_AWADDR : in STD_LOGIC_VECTOR ( 8 downto 0 );
    s_axi_control_AWVALID : in STD_LOGIC;
    s_axi_control_AWREADY : out STD_LOGIC;
    s_axi_control_WDATA : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_control_WSTRB : in STD_LOGIC_VECTOR ( 3 downto 0 );
    s_axi_control_WVALID : in STD_LOGIC;
    s_axi_control_WREADY : out STD_LOGIC;
    s_axi_control_BRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_control_BVALID : out STD_LOGIC;
    s_axi_control_BREADY : in STD_LOGIC;
    s_axi_control_ARADDR : in STD_LOGIC_VECTOR ( 8 downto 0 );
    s_axi_control_ARVALID : in STD_LOGIC;
    s_axi_control_ARREADY : out STD_LOGIC;
    s_axi_control_RDATA : out STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_control_RRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_control_RVALID : out STD_LOGIC;
    s_axi_control_RREADY : in STD_LOGIC;
    ap_clk : in STD_LOGIC;
    ap_rst_n : in STD_LOGIC;
    interrupt : out STD_LOGIC;
    m_axi_hidden_AWADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_hidden_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_hidden_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_hidden_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_hidden_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_hidden_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_hidden_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_hidden_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_hidden_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_hidden_AWVALID : out STD_LOGIC;
    m_axi_hidden_AWREADY : in STD_LOGIC;
    m_axi_hidden_WDATA : out STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_hidden_WSTRB : out STD_LOGIC_VECTOR ( 15 downto 0 );
    m_axi_hidden_WLAST : out STD_LOGIC;
    m_axi_hidden_WVALID : out STD_LOGIC;
    m_axi_hidden_WREADY : in STD_LOGIC;
    m_axi_hidden_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_hidden_BVALID : in STD_LOGIC;
    m_axi_hidden_BREADY : out STD_LOGIC;
    m_axi_hidden_ARADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_hidden_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_hidden_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_hidden_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_hidden_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_hidden_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_hidden_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_hidden_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_hidden_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_hidden_ARVALID : out STD_LOGIC;
    m_axi_hidden_ARREADY : in STD_LOGIC;
    m_axi_hidden_RDATA : in STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_hidden_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_hidden_RLAST : in STD_LOGIC;
    m_axi_hidden_RVALID : in STD_LOGIC;
    m_axi_hidden_RREADY : out STD_LOGIC;
    m_axi_weight_AWADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_weight_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_weight_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_weight_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_weight_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_weight_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_weight_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_weight_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_weight_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_weight_AWVALID : out STD_LOGIC;
    m_axi_weight_AWREADY : in STD_LOGIC;
    m_axi_weight_WDATA : out STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_weight_WSTRB : out STD_LOGIC_VECTOR ( 15 downto 0 );
    m_axi_weight_WLAST : out STD_LOGIC;
    m_axi_weight_WVALID : out STD_LOGIC;
    m_axi_weight_WREADY : in STD_LOGIC;
    m_axi_weight_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_weight_BVALID : in STD_LOGIC;
    m_axi_weight_BREADY : out STD_LOGIC;
    m_axi_weight_ARADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_weight_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_weight_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_weight_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_weight_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_weight_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_weight_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_weight_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_weight_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_weight_ARVALID : out STD_LOGIC;
    m_axi_weight_ARREADY : in STD_LOGIC;
    m_axi_weight_RDATA : in STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_weight_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_weight_RLAST : in STD_LOGIC;
    m_axi_weight_RVALID : in STD_LOGIC;
    m_axi_weight_RREADY : out STD_LOGIC;
    m_axi_history_AWADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_history_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_history_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_history_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_history_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_history_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_history_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_history_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_history_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_history_AWVALID : out STD_LOGIC;
    m_axi_history_AWREADY : in STD_LOGIC;
    m_axi_history_WDATA : out STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_history_WSTRB : out STD_LOGIC_VECTOR ( 15 downto 0 );
    m_axi_history_WLAST : out STD_LOGIC;
    m_axi_history_WVALID : out STD_LOGIC;
    m_axi_history_WREADY : in STD_LOGIC;
    m_axi_history_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_history_BVALID : in STD_LOGIC;
    m_axi_history_BREADY : out STD_LOGIC;
    m_axi_history_ARADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_history_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_history_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_history_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_history_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_history_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_history_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_history_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_history_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_history_ARVALID : out STD_LOGIC;
    m_axi_history_ARREADY : in STD_LOGIC;
    m_axi_history_RDATA : in STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_history_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_history_RLAST : in STD_LOGIC;
    m_axi_history_RVALID : in STD_LOGIC;
    m_axi_history_RREADY : out STD_LOGIC;
    m_axi_logits_AWADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_logits_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_logits_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_logits_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_logits_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_logits_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_logits_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_logits_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_logits_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_logits_AWVALID : out STD_LOGIC;
    m_axi_logits_AWREADY : in STD_LOGIC;
    m_axi_logits_WDATA : out STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_logits_WSTRB : out STD_LOGIC_VECTOR ( 15 downto 0 );
    m_axi_logits_WLAST : out STD_LOGIC;
    m_axi_logits_WVALID : out STD_LOGIC;
    m_axi_logits_WREADY : in STD_LOGIC;
    m_axi_logits_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_logits_BVALID : in STD_LOGIC;
    m_axi_logits_BREADY : out STD_LOGIC;
    m_axi_logits_ARADDR : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_logits_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_logits_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_logits_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_logits_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_logits_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_logits_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_logits_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_logits_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_logits_ARVALID : out STD_LOGIC;
    m_axi_logits_ARREADY : in STD_LOGIC;
    m_axi_logits_RDATA : in STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axi_logits_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_logits_RLAST : in STD_LOGIC;
    m_axi_logits_RVALID : in STD_LOGIC;
    m_axi_logits_RREADY : out STD_LOGIC
  );

end system_wrapper_m14_lm_head_0_0;

architecture stub of system_wrapper_m14_lm_head_0_0 is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "s_axi_control_AWADDR[8:0],s_axi_control_AWVALID,s_axi_control_AWREADY,s_axi_control_WDATA[31:0],s_axi_control_WSTRB[3:0],s_axi_control_WVALID,s_axi_control_WREADY,s_axi_control_BRESP[1:0],s_axi_control_BVALID,s_axi_control_BREADY,s_axi_control_ARADDR[8:0],s_axi_control_ARVALID,s_axi_control_ARREADY,s_axi_control_RDATA[31:0],s_axi_control_RRESP[1:0],s_axi_control_RVALID,s_axi_control_RREADY,ap_clk,ap_rst_n,interrupt,m_axi_hidden_AWADDR[63:0],m_axi_hidden_AWLEN[7:0],m_axi_hidden_AWSIZE[2:0],m_axi_hidden_AWBURST[1:0],m_axi_hidden_AWLOCK[1:0],m_axi_hidden_AWREGION[3:0],m_axi_hidden_AWCACHE[3:0],m_axi_hidden_AWPROT[2:0],m_axi_hidden_AWQOS[3:0],m_axi_hidden_AWVALID,m_axi_hidden_AWREADY,m_axi_hidden_WDATA[127:0],m_axi_hidden_WSTRB[15:0],m_axi_hidden_WLAST,m_axi_hidden_WVALID,m_axi_hidden_WREADY,m_axi_hidden_BRESP[1:0],m_axi_hidden_BVALID,m_axi_hidden_BREADY,m_axi_hidden_ARADDR[63:0],m_axi_hidden_ARLEN[7:0],m_axi_hidden_ARSIZE[2:0],m_axi_hidden_ARBURST[1:0],m_axi_hidden_ARLOCK[1:0],m_axi_hidden_ARREGION[3:0],m_axi_hidden_ARCACHE[3:0],m_axi_hidden_ARPROT[2:0],m_axi_hidden_ARQOS[3:0],m_axi_hidden_ARVALID,m_axi_hidden_ARREADY,m_axi_hidden_RDATA[127:0],m_axi_hidden_RRESP[1:0],m_axi_hidden_RLAST,m_axi_hidden_RVALID,m_axi_hidden_RREADY,m_axi_weight_AWADDR[63:0],m_axi_weight_AWLEN[7:0],m_axi_weight_AWSIZE[2:0],m_axi_weight_AWBURST[1:0],m_axi_weight_AWLOCK[1:0],m_axi_weight_AWREGION[3:0],m_axi_weight_AWCACHE[3:0],m_axi_weight_AWPROT[2:0],m_axi_weight_AWQOS[3:0],m_axi_weight_AWVALID,m_axi_weight_AWREADY,m_axi_weight_WDATA[127:0],m_axi_weight_WSTRB[15:0],m_axi_weight_WLAST,m_axi_weight_WVALID,m_axi_weight_WREADY,m_axi_weight_BRESP[1:0],m_axi_weight_BVALID,m_axi_weight_BREADY,m_axi_weight_ARADDR[63:0],m_axi_weight_ARLEN[7:0],m_axi_weight_ARSIZE[2:0],m_axi_weight_ARBURST[1:0],m_axi_weight_ARLOCK[1:0],m_axi_weight_ARREGION[3:0],m_axi_weight_ARCACHE[3:0],m_axi_weight_ARPROT[2:0],m_axi_weight_ARQOS[3:0],m_axi_weight_ARVALID,m_axi_weight_ARREADY,m_axi_weight_RDATA[127:0],m_axi_weight_RRESP[1:0],m_axi_weight_RLAST,m_axi_weight_RVALID,m_axi_weight_RREADY,m_axi_history_AWADDR[63:0],m_axi_history_AWLEN[7:0],m_axi_history_AWSIZE[2:0],m_axi_history_AWBURST[1:0],m_axi_history_AWLOCK[1:0],m_axi_history_AWREGION[3:0],m_axi_history_AWCACHE[3:0],m_axi_history_AWPROT[2:0],m_axi_history_AWQOS[3:0],m_axi_history_AWVALID,m_axi_history_AWREADY,m_axi_history_WDATA[127:0],m_axi_history_WSTRB[15:0],m_axi_history_WLAST,m_axi_history_WVALID,m_axi_history_WREADY,m_axi_history_BRESP[1:0],m_axi_history_BVALID,m_axi_history_BREADY,m_axi_history_ARADDR[63:0],m_axi_history_ARLEN[7:0],m_axi_history_ARSIZE[2:0],m_axi_history_ARBURST[1:0],m_axi_history_ARLOCK[1:0],m_axi_history_ARREGION[3:0],m_axi_history_ARCACHE[3:0],m_axi_history_ARPROT[2:0],m_axi_history_ARQOS[3:0],m_axi_history_ARVALID,m_axi_history_ARREADY,m_axi_history_RDATA[127:0],m_axi_history_RRESP[1:0],m_axi_history_RLAST,m_axi_history_RVALID,m_axi_history_RREADY,m_axi_logits_AWADDR[63:0],m_axi_logits_AWLEN[7:0],m_axi_logits_AWSIZE[2:0],m_axi_logits_AWBURST[1:0],m_axi_logits_AWLOCK[1:0],m_axi_logits_AWREGION[3:0],m_axi_logits_AWCACHE[3:0],m_axi_logits_AWPROT[2:0],m_axi_logits_AWQOS[3:0],m_axi_logits_AWVALID,m_axi_logits_AWREADY,m_axi_logits_WDATA[127:0],m_axi_logits_WSTRB[15:0],m_axi_logits_WLAST,m_axi_logits_WVALID,m_axi_logits_WREADY,m_axi_logits_BRESP[1:0],m_axi_logits_BVALID,m_axi_logits_BREADY,m_axi_logits_ARADDR[63:0],m_axi_logits_ARLEN[7:0],m_axi_logits_ARSIZE[2:0],m_axi_logits_ARBURST[1:0],m_axi_logits_ARLOCK[1:0],m_axi_logits_ARREGION[3:0],m_axi_logits_ARCACHE[3:0],m_axi_logits_ARPROT[2:0],m_axi_logits_ARQOS[3:0],m_axi_logits_ARVALID,m_axi_logits_ARREADY,m_axi_logits_RDATA[127:0],m_axi_logits_RRESP[1:0],m_axi_logits_RLAST,m_axi_logits_RVALID,m_axi_logits_RREADY";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "lm_head_v2,Vivado 2021.1";
begin
end;
