// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2021.1 (win64) Build 3247384 Thu Jun 10 19:36:33 MDT 2021
// Date        : Mon Oct 12 15:24:12 2026
// Host        : LAPTOP-VTIKQNGE running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               d:/FPGA_PRO/FPGA_QWEN/vivado/proj/qwen/qwen.srcs/sources_1/bd/system_wrapper/ip/system_wrapper_m14_lm_head_0_0/system_wrapper_m14_lm_head_0_0_stub.v
// Design      : system_wrapper_m14_lm_head_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu9eg-ffvb1156-2-i
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "lm_head_v2,Vivado 2021.1" *)
module system_wrapper_m14_lm_head_0_0(s_axi_control_AWADDR, 
  s_axi_control_AWVALID, s_axi_control_AWREADY, s_axi_control_WDATA, s_axi_control_WSTRB, 
  s_axi_control_WVALID, s_axi_control_WREADY, s_axi_control_BRESP, s_axi_control_BVALID, 
  s_axi_control_BREADY, s_axi_control_ARADDR, s_axi_control_ARVALID, 
  s_axi_control_ARREADY, s_axi_control_RDATA, s_axi_control_RRESP, s_axi_control_RVALID, 
  s_axi_control_RREADY, ap_clk, ap_rst_n, interrupt, m_axi_hidden_AWADDR, m_axi_hidden_AWLEN, 
  m_axi_hidden_AWSIZE, m_axi_hidden_AWBURST, m_axi_hidden_AWLOCK, m_axi_hidden_AWREGION, 
  m_axi_hidden_AWCACHE, m_axi_hidden_AWPROT, m_axi_hidden_AWQOS, m_axi_hidden_AWVALID, 
  m_axi_hidden_AWREADY, m_axi_hidden_WDATA, m_axi_hidden_WSTRB, m_axi_hidden_WLAST, 
  m_axi_hidden_WVALID, m_axi_hidden_WREADY, m_axi_hidden_BRESP, m_axi_hidden_BVALID, 
  m_axi_hidden_BREADY, m_axi_hidden_ARADDR, m_axi_hidden_ARLEN, m_axi_hidden_ARSIZE, 
  m_axi_hidden_ARBURST, m_axi_hidden_ARLOCK, m_axi_hidden_ARREGION, m_axi_hidden_ARCACHE, 
  m_axi_hidden_ARPROT, m_axi_hidden_ARQOS, m_axi_hidden_ARVALID, m_axi_hidden_ARREADY, 
  m_axi_hidden_RDATA, m_axi_hidden_RRESP, m_axi_hidden_RLAST, m_axi_hidden_RVALID, 
  m_axi_hidden_RREADY, m_axi_weight_AWADDR, m_axi_weight_AWLEN, m_axi_weight_AWSIZE, 
  m_axi_weight_AWBURST, m_axi_weight_AWLOCK, m_axi_weight_AWREGION, m_axi_weight_AWCACHE, 
  m_axi_weight_AWPROT, m_axi_weight_AWQOS, m_axi_weight_AWVALID, m_axi_weight_AWREADY, 
  m_axi_weight_WDATA, m_axi_weight_WSTRB, m_axi_weight_WLAST, m_axi_weight_WVALID, 
  m_axi_weight_WREADY, m_axi_weight_BRESP, m_axi_weight_BVALID, m_axi_weight_BREADY, 
  m_axi_weight_ARADDR, m_axi_weight_ARLEN, m_axi_weight_ARSIZE, m_axi_weight_ARBURST, 
  m_axi_weight_ARLOCK, m_axi_weight_ARREGION, m_axi_weight_ARCACHE, m_axi_weight_ARPROT, 
  m_axi_weight_ARQOS, m_axi_weight_ARVALID, m_axi_weight_ARREADY, m_axi_weight_RDATA, 
  m_axi_weight_RRESP, m_axi_weight_RLAST, m_axi_weight_RVALID, m_axi_weight_RREADY, 
  m_axi_history_AWADDR, m_axi_history_AWLEN, m_axi_history_AWSIZE, m_axi_history_AWBURST, 
  m_axi_history_AWLOCK, m_axi_history_AWREGION, m_axi_history_AWCACHE, 
  m_axi_history_AWPROT, m_axi_history_AWQOS, m_axi_history_AWVALID, 
  m_axi_history_AWREADY, m_axi_history_WDATA, m_axi_history_WSTRB, m_axi_history_WLAST, 
  m_axi_history_WVALID, m_axi_history_WREADY, m_axi_history_BRESP, m_axi_history_BVALID, 
  m_axi_history_BREADY, m_axi_history_ARADDR, m_axi_history_ARLEN, m_axi_history_ARSIZE, 
  m_axi_history_ARBURST, m_axi_history_ARLOCK, m_axi_history_ARREGION, 
  m_axi_history_ARCACHE, m_axi_history_ARPROT, m_axi_history_ARQOS, 
  m_axi_history_ARVALID, m_axi_history_ARREADY, m_axi_history_RDATA, m_axi_history_RRESP, 
  m_axi_history_RLAST, m_axi_history_RVALID, m_axi_history_RREADY, m_axi_logits_AWADDR, 
  m_axi_logits_AWLEN, m_axi_logits_AWSIZE, m_axi_logits_AWBURST, m_axi_logits_AWLOCK, 
  m_axi_logits_AWREGION, m_axi_logits_AWCACHE, m_axi_logits_AWPROT, m_axi_logits_AWQOS, 
  m_axi_logits_AWVALID, m_axi_logits_AWREADY, m_axi_logits_WDATA, m_axi_logits_WSTRB, 
  m_axi_logits_WLAST, m_axi_logits_WVALID, m_axi_logits_WREADY, m_axi_logits_BRESP, 
  m_axi_logits_BVALID, m_axi_logits_BREADY, m_axi_logits_ARADDR, m_axi_logits_ARLEN, 
  m_axi_logits_ARSIZE, m_axi_logits_ARBURST, m_axi_logits_ARLOCK, m_axi_logits_ARREGION, 
  m_axi_logits_ARCACHE, m_axi_logits_ARPROT, m_axi_logits_ARQOS, m_axi_logits_ARVALID, 
  m_axi_logits_ARREADY, m_axi_logits_RDATA, m_axi_logits_RRESP, m_axi_logits_RLAST, 
  m_axi_logits_RVALID, m_axi_logits_RREADY)
/* synthesis syn_black_box black_box_pad_pin="s_axi_control_AWADDR[8:0],s_axi_control_AWVALID,s_axi_control_AWREADY,s_axi_control_WDATA[31:0],s_axi_control_WSTRB[3:0],s_axi_control_WVALID,s_axi_control_WREADY,s_axi_control_BRESP[1:0],s_axi_control_BVALID,s_axi_control_BREADY,s_axi_control_ARADDR[8:0],s_axi_control_ARVALID,s_axi_control_ARREADY,s_axi_control_RDATA[31:0],s_axi_control_RRESP[1:0],s_axi_control_RVALID,s_axi_control_RREADY,ap_clk,ap_rst_n,interrupt,m_axi_hidden_AWADDR[63:0],m_axi_hidden_AWLEN[7:0],m_axi_hidden_AWSIZE[2:0],m_axi_hidden_AWBURST[1:0],m_axi_hidden_AWLOCK[1:0],m_axi_hidden_AWREGION[3:0],m_axi_hidden_AWCACHE[3:0],m_axi_hidden_AWPROT[2:0],m_axi_hidden_AWQOS[3:0],m_axi_hidden_AWVALID,m_axi_hidden_AWREADY,m_axi_hidden_WDATA[127:0],m_axi_hidden_WSTRB[15:0],m_axi_hidden_WLAST,m_axi_hidden_WVALID,m_axi_hidden_WREADY,m_axi_hidden_BRESP[1:0],m_axi_hidden_BVALID,m_axi_hidden_BREADY,m_axi_hidden_ARADDR[63:0],m_axi_hidden_ARLEN[7:0],m_axi_hidden_ARSIZE[2:0],m_axi_hidden_ARBURST[1:0],m_axi_hidden_ARLOCK[1:0],m_axi_hidden_ARREGION[3:0],m_axi_hidden_ARCACHE[3:0],m_axi_hidden_ARPROT[2:0],m_axi_hidden_ARQOS[3:0],m_axi_hidden_ARVALID,m_axi_hidden_ARREADY,m_axi_hidden_RDATA[127:0],m_axi_hidden_RRESP[1:0],m_axi_hidden_RLAST,m_axi_hidden_RVALID,m_axi_hidden_RREADY,m_axi_weight_AWADDR[63:0],m_axi_weight_AWLEN[7:0],m_axi_weight_AWSIZE[2:0],m_axi_weight_AWBURST[1:0],m_axi_weight_AWLOCK[1:0],m_axi_weight_AWREGION[3:0],m_axi_weight_AWCACHE[3:0],m_axi_weight_AWPROT[2:0],m_axi_weight_AWQOS[3:0],m_axi_weight_AWVALID,m_axi_weight_AWREADY,m_axi_weight_WDATA[127:0],m_axi_weight_WSTRB[15:0],m_axi_weight_WLAST,m_axi_weight_WVALID,m_axi_weight_WREADY,m_axi_weight_BRESP[1:0],m_axi_weight_BVALID,m_axi_weight_BREADY,m_axi_weight_ARADDR[63:0],m_axi_weight_ARLEN[7:0],m_axi_weight_ARSIZE[2:0],m_axi_weight_ARBURST[1:0],m_axi_weight_ARLOCK[1:0],m_axi_weight_ARREGION[3:0],m_axi_weight_ARCACHE[3:0],m_axi_weight_ARPROT[2:0],m_axi_weight_ARQOS[3:0],m_axi_weight_ARVALID,m_axi_weight_ARREADY,m_axi_weight_RDATA[127:0],m_axi_weight_RRESP[1:0],m_axi_weight_RLAST,m_axi_weight_RVALID,m_axi_weight_RREADY,m_axi_history_AWADDR[63:0],m_axi_history_AWLEN[7:0],m_axi_history_AWSIZE[2:0],m_axi_history_AWBURST[1:0],m_axi_history_AWLOCK[1:0],m_axi_history_AWREGION[3:0],m_axi_history_AWCACHE[3:0],m_axi_history_AWPROT[2:0],m_axi_history_AWQOS[3:0],m_axi_history_AWVALID,m_axi_history_AWREADY,m_axi_history_WDATA[127:0],m_axi_history_WSTRB[15:0],m_axi_history_WLAST,m_axi_history_WVALID,m_axi_history_WREADY,m_axi_history_BRESP[1:0],m_axi_history_BVALID,m_axi_history_BREADY,m_axi_history_ARADDR[63:0],m_axi_history_ARLEN[7:0],m_axi_history_ARSIZE[2:0],m_axi_history_ARBURST[1:0],m_axi_history_ARLOCK[1:0],m_axi_history_ARREGION[3:0],m_axi_history_ARCACHE[3:0],m_axi_history_ARPROT[2:0],m_axi_history_ARQOS[3:0],m_axi_history_ARVALID,m_axi_history_ARREADY,m_axi_history_RDATA[127:0],m_axi_history_RRESP[1:0],m_axi_history_RLAST,m_axi_history_RVALID,m_axi_history_RREADY,m_axi_logits_AWADDR[63:0],m_axi_logits_AWLEN[7:0],m_axi_logits_AWSIZE[2:0],m_axi_logits_AWBURST[1:0],m_axi_logits_AWLOCK[1:0],m_axi_logits_AWREGION[3:0],m_axi_logits_AWCACHE[3:0],m_axi_logits_AWPROT[2:0],m_axi_logits_AWQOS[3:0],m_axi_logits_AWVALID,m_axi_logits_AWREADY,m_axi_logits_WDATA[127:0],m_axi_logits_WSTRB[15:0],m_axi_logits_WLAST,m_axi_logits_WVALID,m_axi_logits_WREADY,m_axi_logits_BRESP[1:0],m_axi_logits_BVALID,m_axi_logits_BREADY,m_axi_logits_ARADDR[63:0],m_axi_logits_ARLEN[7:0],m_axi_logits_ARSIZE[2:0],m_axi_logits_ARBURST[1:0],m_axi_logits_ARLOCK[1:0],m_axi_logits_ARREGION[3:0],m_axi_logits_ARCACHE[3:0],m_axi_logits_ARPROT[2:0],m_axi_logits_ARQOS[3:0],m_axi_logits_ARVALID,m_axi_logits_ARREADY,m_axi_logits_RDATA[127:0],m_axi_logits_RRESP[1:0],m_axi_logits_RLAST,m_axi_logits_RVALID,m_axi_logits_RREADY" */;
  input [8:0]s_axi_control_AWADDR;
  input s_axi_control_AWVALID;
  output s_axi_control_AWREADY;
  input [31:0]s_axi_control_WDATA;
  input [3:0]s_axi_control_WSTRB;
  input s_axi_control_WVALID;
  output s_axi_control_WREADY;
  output [1:0]s_axi_control_BRESP;
  output s_axi_control_BVALID;
  input s_axi_control_BREADY;
  input [8:0]s_axi_control_ARADDR;
  input s_axi_control_ARVALID;
  output s_axi_control_ARREADY;
  output [31:0]s_axi_control_RDATA;
  output [1:0]s_axi_control_RRESP;
  output s_axi_control_RVALID;
  input s_axi_control_RREADY;
  input ap_clk;
  input ap_rst_n;
  output interrupt;
  output [63:0]m_axi_hidden_AWADDR;
  output [7:0]m_axi_hidden_AWLEN;
  output [2:0]m_axi_hidden_AWSIZE;
  output [1:0]m_axi_hidden_AWBURST;
  output [1:0]m_axi_hidden_AWLOCK;
  output [3:0]m_axi_hidden_AWREGION;
  output [3:0]m_axi_hidden_AWCACHE;
  output [2:0]m_axi_hidden_AWPROT;
  output [3:0]m_axi_hidden_AWQOS;
  output m_axi_hidden_AWVALID;
  input m_axi_hidden_AWREADY;
  output [127:0]m_axi_hidden_WDATA;
  output [15:0]m_axi_hidden_WSTRB;
  output m_axi_hidden_WLAST;
  output m_axi_hidden_WVALID;
  input m_axi_hidden_WREADY;
  input [1:0]m_axi_hidden_BRESP;
  input m_axi_hidden_BVALID;
  output m_axi_hidden_BREADY;
  output [63:0]m_axi_hidden_ARADDR;
  output [7:0]m_axi_hidden_ARLEN;
  output [2:0]m_axi_hidden_ARSIZE;
  output [1:0]m_axi_hidden_ARBURST;
  output [1:0]m_axi_hidden_ARLOCK;
  output [3:0]m_axi_hidden_ARREGION;
  output [3:0]m_axi_hidden_ARCACHE;
  output [2:0]m_axi_hidden_ARPROT;
  output [3:0]m_axi_hidden_ARQOS;
  output m_axi_hidden_ARVALID;
  input m_axi_hidden_ARREADY;
  input [127:0]m_axi_hidden_RDATA;
  input [1:0]m_axi_hidden_RRESP;
  input m_axi_hidden_RLAST;
  input m_axi_hidden_RVALID;
  output m_axi_hidden_RREADY;
  output [63:0]m_axi_weight_AWADDR;
  output [7:0]m_axi_weight_AWLEN;
  output [2:0]m_axi_weight_AWSIZE;
  output [1:0]m_axi_weight_AWBURST;
  output [1:0]m_axi_weight_AWLOCK;
  output [3:0]m_axi_weight_AWREGION;
  output [3:0]m_axi_weight_AWCACHE;
  output [2:0]m_axi_weight_AWPROT;
  output [3:0]m_axi_weight_AWQOS;
  output m_axi_weight_AWVALID;
  input m_axi_weight_AWREADY;
  output [127:0]m_axi_weight_WDATA;
  output [15:0]m_axi_weight_WSTRB;
  output m_axi_weight_WLAST;
  output m_axi_weight_WVALID;
  input m_axi_weight_WREADY;
  input [1:0]m_axi_weight_BRESP;
  input m_axi_weight_BVALID;
  output m_axi_weight_BREADY;
  output [63:0]m_axi_weight_ARADDR;
  output [7:0]m_axi_weight_ARLEN;
  output [2:0]m_axi_weight_ARSIZE;
  output [1:0]m_axi_weight_ARBURST;
  output [1:0]m_axi_weight_ARLOCK;
  output [3:0]m_axi_weight_ARREGION;
  output [3:0]m_axi_weight_ARCACHE;
  output [2:0]m_axi_weight_ARPROT;
  output [3:0]m_axi_weight_ARQOS;
  output m_axi_weight_ARVALID;
  input m_axi_weight_ARREADY;
  input [127:0]m_axi_weight_RDATA;
  input [1:0]m_axi_weight_RRESP;
  input m_axi_weight_RLAST;
  input m_axi_weight_RVALID;
  output m_axi_weight_RREADY;
  output [63:0]m_axi_history_AWADDR;
  output [7:0]m_axi_history_AWLEN;
  output [2:0]m_axi_history_AWSIZE;
  output [1:0]m_axi_history_AWBURST;
  output [1:0]m_axi_history_AWLOCK;
  output [3:0]m_axi_history_AWREGION;
  output [3:0]m_axi_history_AWCACHE;
  output [2:0]m_axi_history_AWPROT;
  output [3:0]m_axi_history_AWQOS;
  output m_axi_history_AWVALID;
  input m_axi_history_AWREADY;
  output [127:0]m_axi_history_WDATA;
  output [15:0]m_axi_history_WSTRB;
  output m_axi_history_WLAST;
  output m_axi_history_WVALID;
  input m_axi_history_WREADY;
  input [1:0]m_axi_history_BRESP;
  input m_axi_history_BVALID;
  output m_axi_history_BREADY;
  output [63:0]m_axi_history_ARADDR;
  output [7:0]m_axi_history_ARLEN;
  output [2:0]m_axi_history_ARSIZE;
  output [1:0]m_axi_history_ARBURST;
  output [1:0]m_axi_history_ARLOCK;
  output [3:0]m_axi_history_ARREGION;
  output [3:0]m_axi_history_ARCACHE;
  output [2:0]m_axi_history_ARPROT;
  output [3:0]m_axi_history_ARQOS;
  output m_axi_history_ARVALID;
  input m_axi_history_ARREADY;
  input [127:0]m_axi_history_RDATA;
  input [1:0]m_axi_history_RRESP;
  input m_axi_history_RLAST;
  input m_axi_history_RVALID;
  output m_axi_history_RREADY;
  output [63:0]m_axi_logits_AWADDR;
  output [7:0]m_axi_logits_AWLEN;
  output [2:0]m_axi_logits_AWSIZE;
  output [1:0]m_axi_logits_AWBURST;
  output [1:0]m_axi_logits_AWLOCK;
  output [3:0]m_axi_logits_AWREGION;
  output [3:0]m_axi_logits_AWCACHE;
  output [2:0]m_axi_logits_AWPROT;
  output [3:0]m_axi_logits_AWQOS;
  output m_axi_logits_AWVALID;
  input m_axi_logits_AWREADY;
  output [127:0]m_axi_logits_WDATA;
  output [15:0]m_axi_logits_WSTRB;
  output m_axi_logits_WLAST;
  output m_axi_logits_WVALID;
  input m_axi_logits_WREADY;
  input [1:0]m_axi_logits_BRESP;
  input m_axi_logits_BVALID;
  output m_axi_logits_BREADY;
  output [63:0]m_axi_logits_ARADDR;
  output [7:0]m_axi_logits_ARLEN;
  output [2:0]m_axi_logits_ARSIZE;
  output [1:0]m_axi_logits_ARBURST;
  output [1:0]m_axi_logits_ARLOCK;
  output [3:0]m_axi_logits_ARREGION;
  output [3:0]m_axi_logits_ARCACHE;
  output [2:0]m_axi_logits_ARPROT;
  output [3:0]m_axi_logits_ARQOS;
  output m_axi_logits_ARVALID;
  input m_axi_logits_ARREADY;
  input [127:0]m_axi_logits_RDATA;
  input [1:0]m_axi_logits_RRESP;
  input m_axi_logits_RLAST;
  input m_axi_logits_RVALID;
  output m_axi_logits_RREADY;
endmodule
