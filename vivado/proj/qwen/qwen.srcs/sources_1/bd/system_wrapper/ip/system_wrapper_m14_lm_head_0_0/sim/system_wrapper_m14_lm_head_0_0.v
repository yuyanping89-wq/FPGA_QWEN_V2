// (c) Copyright 1995-2026 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


// IP VLNV: user.org:llm_fpga:lm_head_v2:2.0
// IP Revision: 1791783714

`timescale 1ns/1ps

(* IP_DEFINITION_SOURCE = "HLS" *)
(* DowngradeIPIdentifiedWarnings = "yes" *)
module system_wrapper_m14_lm_head_0_0 (
  s_axi_control_AWADDR,
  s_axi_control_AWVALID,
  s_axi_control_AWREADY,
  s_axi_control_WDATA,
  s_axi_control_WSTRB,
  s_axi_control_WVALID,
  s_axi_control_WREADY,
  s_axi_control_BRESP,
  s_axi_control_BVALID,
  s_axi_control_BREADY,
  s_axi_control_ARADDR,
  s_axi_control_ARVALID,
  s_axi_control_ARREADY,
  s_axi_control_RDATA,
  s_axi_control_RRESP,
  s_axi_control_RVALID,
  s_axi_control_RREADY,
  ap_clk,
  ap_rst_n,
  interrupt,
  m_axi_hidden_AWADDR,
  m_axi_hidden_AWLEN,
  m_axi_hidden_AWSIZE,
  m_axi_hidden_AWBURST,
  m_axi_hidden_AWLOCK,
  m_axi_hidden_AWREGION,
  m_axi_hidden_AWCACHE,
  m_axi_hidden_AWPROT,
  m_axi_hidden_AWQOS,
  m_axi_hidden_AWVALID,
  m_axi_hidden_AWREADY,
  m_axi_hidden_WDATA,
  m_axi_hidden_WSTRB,
  m_axi_hidden_WLAST,
  m_axi_hidden_WVALID,
  m_axi_hidden_WREADY,
  m_axi_hidden_BRESP,
  m_axi_hidden_BVALID,
  m_axi_hidden_BREADY,
  m_axi_hidden_ARADDR,
  m_axi_hidden_ARLEN,
  m_axi_hidden_ARSIZE,
  m_axi_hidden_ARBURST,
  m_axi_hidden_ARLOCK,
  m_axi_hidden_ARREGION,
  m_axi_hidden_ARCACHE,
  m_axi_hidden_ARPROT,
  m_axi_hidden_ARQOS,
  m_axi_hidden_ARVALID,
  m_axi_hidden_ARREADY,
  m_axi_hidden_RDATA,
  m_axi_hidden_RRESP,
  m_axi_hidden_RLAST,
  m_axi_hidden_RVALID,
  m_axi_hidden_RREADY,
  m_axi_weight_AWADDR,
  m_axi_weight_AWLEN,
  m_axi_weight_AWSIZE,
  m_axi_weight_AWBURST,
  m_axi_weight_AWLOCK,
  m_axi_weight_AWREGION,
  m_axi_weight_AWCACHE,
  m_axi_weight_AWPROT,
  m_axi_weight_AWQOS,
  m_axi_weight_AWVALID,
  m_axi_weight_AWREADY,
  m_axi_weight_WDATA,
  m_axi_weight_WSTRB,
  m_axi_weight_WLAST,
  m_axi_weight_WVALID,
  m_axi_weight_WREADY,
  m_axi_weight_BRESP,
  m_axi_weight_BVALID,
  m_axi_weight_BREADY,
  m_axi_weight_ARADDR,
  m_axi_weight_ARLEN,
  m_axi_weight_ARSIZE,
  m_axi_weight_ARBURST,
  m_axi_weight_ARLOCK,
  m_axi_weight_ARREGION,
  m_axi_weight_ARCACHE,
  m_axi_weight_ARPROT,
  m_axi_weight_ARQOS,
  m_axi_weight_ARVALID,
  m_axi_weight_ARREADY,
  m_axi_weight_RDATA,
  m_axi_weight_RRESP,
  m_axi_weight_RLAST,
  m_axi_weight_RVALID,
  m_axi_weight_RREADY,
  m_axi_history_AWADDR,
  m_axi_history_AWLEN,
  m_axi_history_AWSIZE,
  m_axi_history_AWBURST,
  m_axi_history_AWLOCK,
  m_axi_history_AWREGION,
  m_axi_history_AWCACHE,
  m_axi_history_AWPROT,
  m_axi_history_AWQOS,
  m_axi_history_AWVALID,
  m_axi_history_AWREADY,
  m_axi_history_WDATA,
  m_axi_history_WSTRB,
  m_axi_history_WLAST,
  m_axi_history_WVALID,
  m_axi_history_WREADY,
  m_axi_history_BRESP,
  m_axi_history_BVALID,
  m_axi_history_BREADY,
  m_axi_history_ARADDR,
  m_axi_history_ARLEN,
  m_axi_history_ARSIZE,
  m_axi_history_ARBURST,
  m_axi_history_ARLOCK,
  m_axi_history_ARREGION,
  m_axi_history_ARCACHE,
  m_axi_history_ARPROT,
  m_axi_history_ARQOS,
  m_axi_history_ARVALID,
  m_axi_history_ARREADY,
  m_axi_history_RDATA,
  m_axi_history_RRESP,
  m_axi_history_RLAST,
  m_axi_history_RVALID,
  m_axi_history_RREADY,
  m_axi_logits_AWADDR,
  m_axi_logits_AWLEN,
  m_axi_logits_AWSIZE,
  m_axi_logits_AWBURST,
  m_axi_logits_AWLOCK,
  m_axi_logits_AWREGION,
  m_axi_logits_AWCACHE,
  m_axi_logits_AWPROT,
  m_axi_logits_AWQOS,
  m_axi_logits_AWVALID,
  m_axi_logits_AWREADY,
  m_axi_logits_WDATA,
  m_axi_logits_WSTRB,
  m_axi_logits_WLAST,
  m_axi_logits_WVALID,
  m_axi_logits_WREADY,
  m_axi_logits_BRESP,
  m_axi_logits_BVALID,
  m_axi_logits_BREADY,
  m_axi_logits_ARADDR,
  m_axi_logits_ARLEN,
  m_axi_logits_ARSIZE,
  m_axi_logits_ARBURST,
  m_axi_logits_ARLOCK,
  m_axi_logits_ARREGION,
  m_axi_logits_ARCACHE,
  m_axi_logits_ARPROT,
  m_axi_logits_ARQOS,
  m_axi_logits_ARVALID,
  m_axi_logits_ARREADY,
  m_axi_logits_RDATA,
  m_axi_logits_RRESP,
  m_axi_logits_RLAST,
  m_axi_logits_RVALID,
  m_axi_logits_RREADY
);

(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control AWADDR" *)
input wire [8 : 0] s_axi_control_AWADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control AWVALID" *)
input wire s_axi_control_AWVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control AWREADY" *)
output wire s_axi_control_AWREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control WDATA" *)
input wire [31 : 0] s_axi_control_WDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control WSTRB" *)
input wire [3 : 0] s_axi_control_WSTRB;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control WVALID" *)
input wire s_axi_control_WVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control WREADY" *)
output wire s_axi_control_WREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control BRESP" *)
output wire [1 : 0] s_axi_control_BRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control BVALID" *)
output wire s_axi_control_BVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control BREADY" *)
input wire s_axi_control_BREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control ARADDR" *)
input wire [8 : 0] s_axi_control_ARADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control ARVALID" *)
input wire s_axi_control_ARVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control ARREADY" *)
output wire s_axi_control_ARREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control RDATA" *)
output wire [31 : 0] s_axi_control_RDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control RRESP" *)
output wire [1 : 0] s_axi_control_RRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control RVALID" *)
output wire s_axi_control_RVALID;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s_axi_control, ADDR_WIDTH 9, DATA_WIDTH 32, PROTOCOL AXI4LITE, READ_WRITE_MODE READ_WRITE, FREQ_HZ 99999001, ID_WIDTH 0, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 0, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN system_wrapper_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_T\
HREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control RREADY" *)
input wire s_axi_control_RREADY;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ap_clk, ASSOCIATED_BUSIF s_axi_control:m_axi_hidden:m_axi_weight:m_axi_history:m_axi_logits, ASSOCIATED_RESET ap_rst_n, FREQ_HZ 99999001, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN system_wrapper_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 ap_clk CLK" *)
input wire ap_clk;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ap_rst_n, POLARITY ACTIVE_LOW, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 ap_rst_n RST" *)
input wire ap_rst_n;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME interrupt, SENSITIVITY LEVEL_HIGH, PortWidth 1" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:interrupt:1.0 interrupt INTERRUPT" *)
output wire interrupt;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWADDR" *)
output wire [63 : 0] m_axi_hidden_AWADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWLEN" *)
output wire [7 : 0] m_axi_hidden_AWLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWSIZE" *)
output wire [2 : 0] m_axi_hidden_AWSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWBURST" *)
output wire [1 : 0] m_axi_hidden_AWBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWLOCK" *)
output wire [1 : 0] m_axi_hidden_AWLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWREGION" *)
output wire [3 : 0] m_axi_hidden_AWREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWCACHE" *)
output wire [3 : 0] m_axi_hidden_AWCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWPROT" *)
output wire [2 : 0] m_axi_hidden_AWPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWQOS" *)
output wire [3 : 0] m_axi_hidden_AWQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWVALID" *)
output wire m_axi_hidden_AWVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden AWREADY" *)
input wire m_axi_hidden_AWREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden WDATA" *)
output wire [127 : 0] m_axi_hidden_WDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden WSTRB" *)
output wire [15 : 0] m_axi_hidden_WSTRB;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden WLAST" *)
output wire m_axi_hidden_WLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden WVALID" *)
output wire m_axi_hidden_WVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden WREADY" *)
input wire m_axi_hidden_WREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden BRESP" *)
input wire [1 : 0] m_axi_hidden_BRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden BVALID" *)
input wire m_axi_hidden_BVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden BREADY" *)
output wire m_axi_hidden_BREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARADDR" *)
output wire [63 : 0] m_axi_hidden_ARADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARLEN" *)
output wire [7 : 0] m_axi_hidden_ARLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARSIZE" *)
output wire [2 : 0] m_axi_hidden_ARSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARBURST" *)
output wire [1 : 0] m_axi_hidden_ARBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARLOCK" *)
output wire [1 : 0] m_axi_hidden_ARLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARREGION" *)
output wire [3 : 0] m_axi_hidden_ARREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARCACHE" *)
output wire [3 : 0] m_axi_hidden_ARCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARPROT" *)
output wire [2 : 0] m_axi_hidden_ARPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARQOS" *)
output wire [3 : 0] m_axi_hidden_ARQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARVALID" *)
output wire m_axi_hidden_ARVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden ARREADY" *)
input wire m_axi_hidden_ARREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden RDATA" *)
input wire [127 : 0] m_axi_hidden_RDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden RRESP" *)
input wire [1 : 0] m_axi_hidden_RRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden RLAST" *)
input wire m_axi_hidden_RLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden RVALID" *)
input wire m_axi_hidden_RVALID;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_hidden, ADDR_WIDTH 64, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 4, NUM_WRITE_OUTSTANDING 16, MAX_READ_BURST_LENGTH 64, MAX_WRITE_BURST_LENGTH 16, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, SUPPORTS_NARROW_BURST 0, DATA_WIDTH 128, FREQ_HZ 99999001, ID_WIDTH 0, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, HAS_LOCK 1, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 1, HAS_REGION 1, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, PHASE 0.0, CLK_DOMAIN s\
ystem_wrapper_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_hidden RREADY" *)
output wire m_axi_hidden_RREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWADDR" *)
output wire [63 : 0] m_axi_weight_AWADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWLEN" *)
output wire [7 : 0] m_axi_weight_AWLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWSIZE" *)
output wire [2 : 0] m_axi_weight_AWSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWBURST" *)
output wire [1 : 0] m_axi_weight_AWBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWLOCK" *)
output wire [1 : 0] m_axi_weight_AWLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWREGION" *)
output wire [3 : 0] m_axi_weight_AWREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWCACHE" *)
output wire [3 : 0] m_axi_weight_AWCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWPROT" *)
output wire [2 : 0] m_axi_weight_AWPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWQOS" *)
output wire [3 : 0] m_axi_weight_AWQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWVALID" *)
output wire m_axi_weight_AWVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight AWREADY" *)
input wire m_axi_weight_AWREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight WDATA" *)
output wire [127 : 0] m_axi_weight_WDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight WSTRB" *)
output wire [15 : 0] m_axi_weight_WSTRB;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight WLAST" *)
output wire m_axi_weight_WLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight WVALID" *)
output wire m_axi_weight_WVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight WREADY" *)
input wire m_axi_weight_WREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight BRESP" *)
input wire [1 : 0] m_axi_weight_BRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight BVALID" *)
input wire m_axi_weight_BVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight BREADY" *)
output wire m_axi_weight_BREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARADDR" *)
output wire [63 : 0] m_axi_weight_ARADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARLEN" *)
output wire [7 : 0] m_axi_weight_ARLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARSIZE" *)
output wire [2 : 0] m_axi_weight_ARSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARBURST" *)
output wire [1 : 0] m_axi_weight_ARBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARLOCK" *)
output wire [1 : 0] m_axi_weight_ARLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARREGION" *)
output wire [3 : 0] m_axi_weight_ARREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARCACHE" *)
output wire [3 : 0] m_axi_weight_ARCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARPROT" *)
output wire [2 : 0] m_axi_weight_ARPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARQOS" *)
output wire [3 : 0] m_axi_weight_ARQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARVALID" *)
output wire m_axi_weight_ARVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight ARREADY" *)
input wire m_axi_weight_ARREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight RDATA" *)
input wire [127 : 0] m_axi_weight_RDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight RRESP" *)
input wire [1 : 0] m_axi_weight_RRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight RLAST" *)
input wire m_axi_weight_RLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight RVALID" *)
input wire m_axi_weight_RVALID;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_weight, ADDR_WIDTH 64, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_WRITE_OUTSTANDING 16, MAX_READ_BURST_LENGTH 16, MAX_WRITE_BURST_LENGTH 16, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, SUPPORTS_NARROW_BURST 0, DATA_WIDTH 128, FREQ_HZ 99999001, ID_WIDTH 0, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, HAS_LOCK 1, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 1, HAS_REGION 1, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, PHASE 0.0, CLK_DOMAIN \
system_wrapper_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_weight RREADY" *)
output wire m_axi_weight_RREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWADDR" *)
output wire [63 : 0] m_axi_history_AWADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWLEN" *)
output wire [7 : 0] m_axi_history_AWLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWSIZE" *)
output wire [2 : 0] m_axi_history_AWSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWBURST" *)
output wire [1 : 0] m_axi_history_AWBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWLOCK" *)
output wire [1 : 0] m_axi_history_AWLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWREGION" *)
output wire [3 : 0] m_axi_history_AWREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWCACHE" *)
output wire [3 : 0] m_axi_history_AWCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWPROT" *)
output wire [2 : 0] m_axi_history_AWPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWQOS" *)
output wire [3 : 0] m_axi_history_AWQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWVALID" *)
output wire m_axi_history_AWVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history AWREADY" *)
input wire m_axi_history_AWREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history WDATA" *)
output wire [127 : 0] m_axi_history_WDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history WSTRB" *)
output wire [15 : 0] m_axi_history_WSTRB;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history WLAST" *)
output wire m_axi_history_WLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history WVALID" *)
output wire m_axi_history_WVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history WREADY" *)
input wire m_axi_history_WREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history BRESP" *)
input wire [1 : 0] m_axi_history_BRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history BVALID" *)
input wire m_axi_history_BVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history BREADY" *)
output wire m_axi_history_BREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARADDR" *)
output wire [63 : 0] m_axi_history_ARADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARLEN" *)
output wire [7 : 0] m_axi_history_ARLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARSIZE" *)
output wire [2 : 0] m_axi_history_ARSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARBURST" *)
output wire [1 : 0] m_axi_history_ARBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARLOCK" *)
output wire [1 : 0] m_axi_history_ARLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARREGION" *)
output wire [3 : 0] m_axi_history_ARREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARCACHE" *)
output wire [3 : 0] m_axi_history_ARCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARPROT" *)
output wire [2 : 0] m_axi_history_ARPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARQOS" *)
output wire [3 : 0] m_axi_history_ARQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARVALID" *)
output wire m_axi_history_ARVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history ARREADY" *)
input wire m_axi_history_ARREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history RDATA" *)
input wire [127 : 0] m_axi_history_RDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history RRESP" *)
input wire [1 : 0] m_axi_history_RRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history RLAST" *)
input wire m_axi_history_RLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history RVALID" *)
input wire m_axi_history_RVALID;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_history, ADDR_WIDTH 64, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 4, NUM_WRITE_OUTSTANDING 16, MAX_READ_BURST_LENGTH 16, MAX_WRITE_BURST_LENGTH 16, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, SUPPORTS_NARROW_BURST 0, DATA_WIDTH 128, FREQ_HZ 99999001, ID_WIDTH 0, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, HAS_LOCK 1, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 1, HAS_REGION 1, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, PHASE 0.0, CLK_DOMAIN \
system_wrapper_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_history RREADY" *)
output wire m_axi_history_RREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWADDR" *)
output wire [63 : 0] m_axi_logits_AWADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWLEN" *)
output wire [7 : 0] m_axi_logits_AWLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWSIZE" *)
output wire [2 : 0] m_axi_logits_AWSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWBURST" *)
output wire [1 : 0] m_axi_logits_AWBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWLOCK" *)
output wire [1 : 0] m_axi_logits_AWLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWREGION" *)
output wire [3 : 0] m_axi_logits_AWREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWCACHE" *)
output wire [3 : 0] m_axi_logits_AWCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWPROT" *)
output wire [2 : 0] m_axi_logits_AWPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWQOS" *)
output wire [3 : 0] m_axi_logits_AWQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWVALID" *)
output wire m_axi_logits_AWVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits AWREADY" *)
input wire m_axi_logits_AWREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits WDATA" *)
output wire [127 : 0] m_axi_logits_WDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits WSTRB" *)
output wire [15 : 0] m_axi_logits_WSTRB;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits WLAST" *)
output wire m_axi_logits_WLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits WVALID" *)
output wire m_axi_logits_WVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits WREADY" *)
input wire m_axi_logits_WREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits BRESP" *)
input wire [1 : 0] m_axi_logits_BRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits BVALID" *)
input wire m_axi_logits_BVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits BREADY" *)
output wire m_axi_logits_BREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARADDR" *)
output wire [63 : 0] m_axi_logits_ARADDR;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARLEN" *)
output wire [7 : 0] m_axi_logits_ARLEN;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARSIZE" *)
output wire [2 : 0] m_axi_logits_ARSIZE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARBURST" *)
output wire [1 : 0] m_axi_logits_ARBURST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARLOCK" *)
output wire [1 : 0] m_axi_logits_ARLOCK;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARREGION" *)
output wire [3 : 0] m_axi_logits_ARREGION;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARCACHE" *)
output wire [3 : 0] m_axi_logits_ARCACHE;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARPROT" *)
output wire [2 : 0] m_axi_logits_ARPROT;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARQOS" *)
output wire [3 : 0] m_axi_logits_ARQOS;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARVALID" *)
output wire m_axi_logits_ARVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits ARREADY" *)
input wire m_axi_logits_ARREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits RDATA" *)
input wire [127 : 0] m_axi_logits_RDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits RRESP" *)
input wire [1 : 0] m_axi_logits_RRESP;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits RLAST" *)
input wire m_axi_logits_RLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits RVALID" *)
input wire m_axi_logits_RVALID;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_logits, ADDR_WIDTH 64, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_WRITE_OUTSTANDING 8, MAX_READ_BURST_LENGTH 16, MAX_WRITE_BURST_LENGTH 64, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, SUPPORTS_NARROW_BURST 0, DATA_WIDTH 128, FREQ_HZ 99999001, ID_WIDTH 0, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, HAS_LOCK 1, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 1, HAS_REGION 1, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, PHASE 0.0, CLK_DOMAIN s\
ystem_wrapper_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_logits RREADY" *)
output wire m_axi_logits_RREADY;

(* SDX_KERNEL = "true" *)
(* SDX_KERNEL_TYPE = "hls" *)
(* SDX_KERNEL_SIM_INST = "" *)
  lm_head_v2 #(
    .C_S_AXI_CONTROL_ADDR_WIDTH(9),
    .C_S_AXI_CONTROL_DATA_WIDTH(32),
    .C_M_AXI_HIDDEN_ID_WIDTH(1),
    .C_M_AXI_HIDDEN_ADDR_WIDTH(64),
    .C_M_AXI_HIDDEN_DATA_WIDTH(128),
    .C_M_AXI_HIDDEN_AWUSER_WIDTH(1),
    .C_M_AXI_HIDDEN_ARUSER_WIDTH(1),
    .C_M_AXI_HIDDEN_WUSER_WIDTH(1),
    .C_M_AXI_HIDDEN_RUSER_WIDTH(1),
    .C_M_AXI_HIDDEN_BUSER_WIDTH(1),
    .C_M_AXI_HIDDEN_USER_VALUE(32'H00000000),
    .C_M_AXI_HIDDEN_PROT_VALUE(3'B000),
    .C_M_AXI_HIDDEN_CACHE_VALUE(4'B0011),
    .C_M_AXI_WEIGHT_ID_WIDTH(1),
    .C_M_AXI_WEIGHT_ADDR_WIDTH(64),
    .C_M_AXI_WEIGHT_DATA_WIDTH(128),
    .C_M_AXI_WEIGHT_AWUSER_WIDTH(1),
    .C_M_AXI_WEIGHT_ARUSER_WIDTH(1),
    .C_M_AXI_WEIGHT_WUSER_WIDTH(1),
    .C_M_AXI_WEIGHT_RUSER_WIDTH(1),
    .C_M_AXI_WEIGHT_BUSER_WIDTH(1),
    .C_M_AXI_WEIGHT_USER_VALUE(32'H00000000),
    .C_M_AXI_WEIGHT_PROT_VALUE(3'B000),
    .C_M_AXI_WEIGHT_CACHE_VALUE(4'B0011),
    .C_M_AXI_HISTORY_ID_WIDTH(1),
    .C_M_AXI_HISTORY_ADDR_WIDTH(64),
    .C_M_AXI_HISTORY_DATA_WIDTH(128),
    .C_M_AXI_HISTORY_AWUSER_WIDTH(1),
    .C_M_AXI_HISTORY_ARUSER_WIDTH(1),
    .C_M_AXI_HISTORY_WUSER_WIDTH(1),
    .C_M_AXI_HISTORY_RUSER_WIDTH(1),
    .C_M_AXI_HISTORY_BUSER_WIDTH(1),
    .C_M_AXI_HISTORY_USER_VALUE(32'H00000000),
    .C_M_AXI_HISTORY_PROT_VALUE(3'B000),
    .C_M_AXI_HISTORY_CACHE_VALUE(4'B0011),
    .C_M_AXI_LOGITS_ID_WIDTH(1),
    .C_M_AXI_LOGITS_ADDR_WIDTH(64),
    .C_M_AXI_LOGITS_DATA_WIDTH(128),
    .C_M_AXI_LOGITS_AWUSER_WIDTH(1),
    .C_M_AXI_LOGITS_ARUSER_WIDTH(1),
    .C_M_AXI_LOGITS_WUSER_WIDTH(1),
    .C_M_AXI_LOGITS_RUSER_WIDTH(1),
    .C_M_AXI_LOGITS_BUSER_WIDTH(1),
    .C_M_AXI_LOGITS_USER_VALUE(32'H00000000),
    .C_M_AXI_LOGITS_PROT_VALUE(3'B000),
    .C_M_AXI_LOGITS_CACHE_VALUE(4'B0011)
  ) inst (
    .s_axi_control_AWADDR(s_axi_control_AWADDR),
    .s_axi_control_AWVALID(s_axi_control_AWVALID),
    .s_axi_control_AWREADY(s_axi_control_AWREADY),
    .s_axi_control_WDATA(s_axi_control_WDATA),
    .s_axi_control_WSTRB(s_axi_control_WSTRB),
    .s_axi_control_WVALID(s_axi_control_WVALID),
    .s_axi_control_WREADY(s_axi_control_WREADY),
    .s_axi_control_BRESP(s_axi_control_BRESP),
    .s_axi_control_BVALID(s_axi_control_BVALID),
    .s_axi_control_BREADY(s_axi_control_BREADY),
    .s_axi_control_ARADDR(s_axi_control_ARADDR),
    .s_axi_control_ARVALID(s_axi_control_ARVALID),
    .s_axi_control_ARREADY(s_axi_control_ARREADY),
    .s_axi_control_RDATA(s_axi_control_RDATA),
    .s_axi_control_RRESP(s_axi_control_RRESP),
    .s_axi_control_RVALID(s_axi_control_RVALID),
    .s_axi_control_RREADY(s_axi_control_RREADY),
    .ap_clk(ap_clk),
    .ap_rst_n(ap_rst_n),
    .interrupt(interrupt),
    .m_axi_hidden_AWID(),
    .m_axi_hidden_AWADDR(m_axi_hidden_AWADDR),
    .m_axi_hidden_AWLEN(m_axi_hidden_AWLEN),
    .m_axi_hidden_AWSIZE(m_axi_hidden_AWSIZE),
    .m_axi_hidden_AWBURST(m_axi_hidden_AWBURST),
    .m_axi_hidden_AWLOCK(m_axi_hidden_AWLOCK),
    .m_axi_hidden_AWREGION(m_axi_hidden_AWREGION),
    .m_axi_hidden_AWCACHE(m_axi_hidden_AWCACHE),
    .m_axi_hidden_AWPROT(m_axi_hidden_AWPROT),
    .m_axi_hidden_AWQOS(m_axi_hidden_AWQOS),
    .m_axi_hidden_AWUSER(),
    .m_axi_hidden_AWVALID(m_axi_hidden_AWVALID),
    .m_axi_hidden_AWREADY(m_axi_hidden_AWREADY),
    .m_axi_hidden_WID(),
    .m_axi_hidden_WDATA(m_axi_hidden_WDATA),
    .m_axi_hidden_WSTRB(m_axi_hidden_WSTRB),
    .m_axi_hidden_WLAST(m_axi_hidden_WLAST),
    .m_axi_hidden_WUSER(),
    .m_axi_hidden_WVALID(m_axi_hidden_WVALID),
    .m_axi_hidden_WREADY(m_axi_hidden_WREADY),
    .m_axi_hidden_BID(1'B0),
    .m_axi_hidden_BRESP(m_axi_hidden_BRESP),
    .m_axi_hidden_BUSER(1'B0),
    .m_axi_hidden_BVALID(m_axi_hidden_BVALID),
    .m_axi_hidden_BREADY(m_axi_hidden_BREADY),
    .m_axi_hidden_ARID(),
    .m_axi_hidden_ARADDR(m_axi_hidden_ARADDR),
    .m_axi_hidden_ARLEN(m_axi_hidden_ARLEN),
    .m_axi_hidden_ARSIZE(m_axi_hidden_ARSIZE),
    .m_axi_hidden_ARBURST(m_axi_hidden_ARBURST),
    .m_axi_hidden_ARLOCK(m_axi_hidden_ARLOCK),
    .m_axi_hidden_ARREGION(m_axi_hidden_ARREGION),
    .m_axi_hidden_ARCACHE(m_axi_hidden_ARCACHE),
    .m_axi_hidden_ARPROT(m_axi_hidden_ARPROT),
    .m_axi_hidden_ARQOS(m_axi_hidden_ARQOS),
    .m_axi_hidden_ARUSER(),
    .m_axi_hidden_ARVALID(m_axi_hidden_ARVALID),
    .m_axi_hidden_ARREADY(m_axi_hidden_ARREADY),
    .m_axi_hidden_RID(1'B0),
    .m_axi_hidden_RDATA(m_axi_hidden_RDATA),
    .m_axi_hidden_RRESP(m_axi_hidden_RRESP),
    .m_axi_hidden_RLAST(m_axi_hidden_RLAST),
    .m_axi_hidden_RUSER(1'B0),
    .m_axi_hidden_RVALID(m_axi_hidden_RVALID),
    .m_axi_hidden_RREADY(m_axi_hidden_RREADY),
    .m_axi_weight_AWID(),
    .m_axi_weight_AWADDR(m_axi_weight_AWADDR),
    .m_axi_weight_AWLEN(m_axi_weight_AWLEN),
    .m_axi_weight_AWSIZE(m_axi_weight_AWSIZE),
    .m_axi_weight_AWBURST(m_axi_weight_AWBURST),
    .m_axi_weight_AWLOCK(m_axi_weight_AWLOCK),
    .m_axi_weight_AWREGION(m_axi_weight_AWREGION),
    .m_axi_weight_AWCACHE(m_axi_weight_AWCACHE),
    .m_axi_weight_AWPROT(m_axi_weight_AWPROT),
    .m_axi_weight_AWQOS(m_axi_weight_AWQOS),
    .m_axi_weight_AWUSER(),
    .m_axi_weight_AWVALID(m_axi_weight_AWVALID),
    .m_axi_weight_AWREADY(m_axi_weight_AWREADY),
    .m_axi_weight_WID(),
    .m_axi_weight_WDATA(m_axi_weight_WDATA),
    .m_axi_weight_WSTRB(m_axi_weight_WSTRB),
    .m_axi_weight_WLAST(m_axi_weight_WLAST),
    .m_axi_weight_WUSER(),
    .m_axi_weight_WVALID(m_axi_weight_WVALID),
    .m_axi_weight_WREADY(m_axi_weight_WREADY),
    .m_axi_weight_BID(1'B0),
    .m_axi_weight_BRESP(m_axi_weight_BRESP),
    .m_axi_weight_BUSER(1'B0),
    .m_axi_weight_BVALID(m_axi_weight_BVALID),
    .m_axi_weight_BREADY(m_axi_weight_BREADY),
    .m_axi_weight_ARID(),
    .m_axi_weight_ARADDR(m_axi_weight_ARADDR),
    .m_axi_weight_ARLEN(m_axi_weight_ARLEN),
    .m_axi_weight_ARSIZE(m_axi_weight_ARSIZE),
    .m_axi_weight_ARBURST(m_axi_weight_ARBURST),
    .m_axi_weight_ARLOCK(m_axi_weight_ARLOCK),
    .m_axi_weight_ARREGION(m_axi_weight_ARREGION),
    .m_axi_weight_ARCACHE(m_axi_weight_ARCACHE),
    .m_axi_weight_ARPROT(m_axi_weight_ARPROT),
    .m_axi_weight_ARQOS(m_axi_weight_ARQOS),
    .m_axi_weight_ARUSER(),
    .m_axi_weight_ARVALID(m_axi_weight_ARVALID),
    .m_axi_weight_ARREADY(m_axi_weight_ARREADY),
    .m_axi_weight_RID(1'B0),
    .m_axi_weight_RDATA(m_axi_weight_RDATA),
    .m_axi_weight_RRESP(m_axi_weight_RRESP),
    .m_axi_weight_RLAST(m_axi_weight_RLAST),
    .m_axi_weight_RUSER(1'B0),
    .m_axi_weight_RVALID(m_axi_weight_RVALID),
    .m_axi_weight_RREADY(m_axi_weight_RREADY),
    .m_axi_history_AWID(),
    .m_axi_history_AWADDR(m_axi_history_AWADDR),
    .m_axi_history_AWLEN(m_axi_history_AWLEN),
    .m_axi_history_AWSIZE(m_axi_history_AWSIZE),
    .m_axi_history_AWBURST(m_axi_history_AWBURST),
    .m_axi_history_AWLOCK(m_axi_history_AWLOCK),
    .m_axi_history_AWREGION(m_axi_history_AWREGION),
    .m_axi_history_AWCACHE(m_axi_history_AWCACHE),
    .m_axi_history_AWPROT(m_axi_history_AWPROT),
    .m_axi_history_AWQOS(m_axi_history_AWQOS),
    .m_axi_history_AWUSER(),
    .m_axi_history_AWVALID(m_axi_history_AWVALID),
    .m_axi_history_AWREADY(m_axi_history_AWREADY),
    .m_axi_history_WID(),
    .m_axi_history_WDATA(m_axi_history_WDATA),
    .m_axi_history_WSTRB(m_axi_history_WSTRB),
    .m_axi_history_WLAST(m_axi_history_WLAST),
    .m_axi_history_WUSER(),
    .m_axi_history_WVALID(m_axi_history_WVALID),
    .m_axi_history_WREADY(m_axi_history_WREADY),
    .m_axi_history_BID(1'B0),
    .m_axi_history_BRESP(m_axi_history_BRESP),
    .m_axi_history_BUSER(1'B0),
    .m_axi_history_BVALID(m_axi_history_BVALID),
    .m_axi_history_BREADY(m_axi_history_BREADY),
    .m_axi_history_ARID(),
    .m_axi_history_ARADDR(m_axi_history_ARADDR),
    .m_axi_history_ARLEN(m_axi_history_ARLEN),
    .m_axi_history_ARSIZE(m_axi_history_ARSIZE),
    .m_axi_history_ARBURST(m_axi_history_ARBURST),
    .m_axi_history_ARLOCK(m_axi_history_ARLOCK),
    .m_axi_history_ARREGION(m_axi_history_ARREGION),
    .m_axi_history_ARCACHE(m_axi_history_ARCACHE),
    .m_axi_history_ARPROT(m_axi_history_ARPROT),
    .m_axi_history_ARQOS(m_axi_history_ARQOS),
    .m_axi_history_ARUSER(),
    .m_axi_history_ARVALID(m_axi_history_ARVALID),
    .m_axi_history_ARREADY(m_axi_history_ARREADY),
    .m_axi_history_RID(1'B0),
    .m_axi_history_RDATA(m_axi_history_RDATA),
    .m_axi_history_RRESP(m_axi_history_RRESP),
    .m_axi_history_RLAST(m_axi_history_RLAST),
    .m_axi_history_RUSER(1'B0),
    .m_axi_history_RVALID(m_axi_history_RVALID),
    .m_axi_history_RREADY(m_axi_history_RREADY),
    .m_axi_logits_AWID(),
    .m_axi_logits_AWADDR(m_axi_logits_AWADDR),
    .m_axi_logits_AWLEN(m_axi_logits_AWLEN),
    .m_axi_logits_AWSIZE(m_axi_logits_AWSIZE),
    .m_axi_logits_AWBURST(m_axi_logits_AWBURST),
    .m_axi_logits_AWLOCK(m_axi_logits_AWLOCK),
    .m_axi_logits_AWREGION(m_axi_logits_AWREGION),
    .m_axi_logits_AWCACHE(m_axi_logits_AWCACHE),
    .m_axi_logits_AWPROT(m_axi_logits_AWPROT),
    .m_axi_logits_AWQOS(m_axi_logits_AWQOS),
    .m_axi_logits_AWUSER(),
    .m_axi_logits_AWVALID(m_axi_logits_AWVALID),
    .m_axi_logits_AWREADY(m_axi_logits_AWREADY),
    .m_axi_logits_WID(),
    .m_axi_logits_WDATA(m_axi_logits_WDATA),
    .m_axi_logits_WSTRB(m_axi_logits_WSTRB),
    .m_axi_logits_WLAST(m_axi_logits_WLAST),
    .m_axi_logits_WUSER(),
    .m_axi_logits_WVALID(m_axi_logits_WVALID),
    .m_axi_logits_WREADY(m_axi_logits_WREADY),
    .m_axi_logits_BID(1'B0),
    .m_axi_logits_BRESP(m_axi_logits_BRESP),
    .m_axi_logits_BUSER(1'B0),
    .m_axi_logits_BVALID(m_axi_logits_BVALID),
    .m_axi_logits_BREADY(m_axi_logits_BREADY),
    .m_axi_logits_ARID(),
    .m_axi_logits_ARADDR(m_axi_logits_ARADDR),
    .m_axi_logits_ARLEN(m_axi_logits_ARLEN),
    .m_axi_logits_ARSIZE(m_axi_logits_ARSIZE),
    .m_axi_logits_ARBURST(m_axi_logits_ARBURST),
    .m_axi_logits_ARLOCK(m_axi_logits_ARLOCK),
    .m_axi_logits_ARREGION(m_axi_logits_ARREGION),
    .m_axi_logits_ARCACHE(m_axi_logits_ARCACHE),
    .m_axi_logits_ARPROT(m_axi_logits_ARPROT),
    .m_axi_logits_ARQOS(m_axi_logits_ARQOS),
    .m_axi_logits_ARUSER(),
    .m_axi_logits_ARVALID(m_axi_logits_ARVALID),
    .m_axi_logits_ARREADY(m_axi_logits_ARREADY),
    .m_axi_logits_RID(1'B0),
    .m_axi_logits_RDATA(m_axi_logits_RDATA),
    .m_axi_logits_RRESP(m_axi_logits_RRESP),
    .m_axi_logits_RLAST(m_axi_logits_RLAST),
    .m_axi_logits_RUSER(1'B0),
    .m_axi_logits_RVALID(m_axi_logits_RVALID),
    .m_axi_logits_RREADY(m_axi_logits_RREADY)
  );
endmodule
