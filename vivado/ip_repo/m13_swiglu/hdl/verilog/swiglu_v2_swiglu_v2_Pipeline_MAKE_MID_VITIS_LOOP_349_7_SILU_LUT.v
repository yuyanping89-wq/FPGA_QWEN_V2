// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1 ns / 1 ps
module swiglu_v2_swiglu_v2_Pipeline_MAKE_MID_VITIS_LOOP_349_7_SILU_LUT (
address0, ce0, q0, reset,clk);

parameter DataWidth = 32;
parameter AddressWidth = 12;
parameter AddressRange = 2049;

input[AddressWidth-1:0] address0;
input ce0;
output reg[DataWidth-1:0] q0;
input reset;
input clk;

reg [DataWidth-1:0] ram[0:AddressRange-1];

initial begin
    $readmemh("./swiglu_v2_swiglu_v2_Pipeline_MAKE_MID_VITIS_LOOP_349_7_SILU_LUT.dat", ram);
end



always @(posedge clk)  
begin 
    if (ce0) 
    begin
        q0 <= ram[address0];
    end
end



endmodule

