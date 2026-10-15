// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xswiglu_v2.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XSwiglu_v2_CfgInitialize(XSwiglu_v2 *InstancePtr, XSwiglu_v2_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XSwiglu_v2_Start(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL) & 0x80;
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XSwiglu_v2_IsDone(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XSwiglu_v2_IsIdle(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XSwiglu_v2_IsReady(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XSwiglu_v2_EnableAutoRestart(XSwiglu_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XSwiglu_v2_DisableAutoRestart(XSwiglu_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_AP_CTRL, 0);
}

void XSwiglu_v2_Set_x_offset(XSwiglu_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_X_OFFSET_DATA, (u32)(Data));
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_X_OFFSET_DATA + 4, (u32)(Data >> 32));
}

u64 XSwiglu_v2_Get_x_offset(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_X_OFFSET_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_X_OFFSET_DATA + 4) << 32;
    return Data;
}

void XSwiglu_v2_Set_w_gate_offset(XSwiglu_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_GATE_OFFSET_DATA, (u32)(Data));
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_GATE_OFFSET_DATA + 4, (u32)(Data >> 32));
}

u64 XSwiglu_v2_Get_w_gate_offset(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_GATE_OFFSET_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_GATE_OFFSET_DATA + 4) << 32;
    return Data;
}

void XSwiglu_v2_Set_w_up_offset(XSwiglu_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_UP_OFFSET_DATA, (u32)(Data));
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_UP_OFFSET_DATA + 4, (u32)(Data >> 32));
}

u64 XSwiglu_v2_Get_w_up_offset(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_UP_OFFSET_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_UP_OFFSET_DATA + 4) << 32;
    return Data;
}

void XSwiglu_v2_Set_w_down_offset(XSwiglu_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_DOWN_OFFSET_DATA, (u32)(Data));
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_DOWN_OFFSET_DATA + 4, (u32)(Data >> 32));
}

u64 XSwiglu_v2_Get_w_down_offset(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_DOWN_OFFSET_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_W_DOWN_OFFSET_DATA + 4) << 32;
    return Data;
}

void XSwiglu_v2_Set_y_offset(XSwiglu_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_Y_OFFSET_DATA, (u32)(Data));
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_Y_OFFSET_DATA + 4, (u32)(Data >> 32));
}

u64 XSwiglu_v2_Get_y_offset(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_Y_OFFSET_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_Y_OFFSET_DATA + 4) << 32;
    return Data;
}

void XSwiglu_v2_Set_run_mode(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XSwiglu_v2_Get_run_mode(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XSwiglu_v2_Set_valid_rows(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XSwiglu_v2_Get_valid_rows(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XSwiglu_v2_Set_hidden_size(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_HIDDEN_SIZE_DATA, Data);
}

u32 XSwiglu_v2_Get_hidden_size(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_HIDDEN_SIZE_DATA);
    return Data;
}

void XSwiglu_v2_Set_intermediate_size(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_INTERMEDIATE_SIZE_DATA, Data);
}

u32 XSwiglu_v2_Get_intermediate_size(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_INTERMEDIATE_SIZE_DATA);
    return Data;
}

void XSwiglu_v2_Set_layout_id(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_LAYOUT_ID_DATA, Data);
}

u32 XSwiglu_v2_Get_layout_id(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_LAYOUT_ID_DATA);
    return Data;
}

void XSwiglu_v2_Set_abi_version(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XSwiglu_v2_Get_abi_version(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XSwiglu_v2_Set_job_id(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XSwiglu_v2_Get_job_id(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XSwiglu_v2_Set_flags(XSwiglu_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XSwiglu_v2_Get_flags(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XSwiglu_v2_Get_result_status_code(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_STATUS_CODE_DATA);
    return Data;
}

u32 XSwiglu_v2_Get_result_status_code_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XSwiglu_v2_Get_result_processed_rows(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XSwiglu_v2_Get_result_processed_rows_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_perf_cycles_total(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_perf_cycles_total_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_perf_cycles_compute(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_perf_cycles_compute_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_perf_read_beats(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_perf_read_beats_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_perf_write_beats(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_perf_write_beats_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XSwiglu_v2_Get_result_output_crc(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA);
    return Data;
}

u32 XSwiglu_v2_Get_result_output_crc_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XSwiglu_v2_Get_result_build_config(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA);
    return Data;
}

u32 XSwiglu_v2_Get_result_build_config_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

u32 XSwiglu_v2_Get_result_mid_tiles(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_MID_TILES_DATA);
    return Data;
}

u32 XSwiglu_v2_Get_result_mid_tiles_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_MID_TILES_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_gate_up_active_cycles(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_gate_up_active_cycles_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_GATE_UP_ACTIVE_CYCLES_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_down_active_cycles(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_DOWN_ACTIVE_CYCLES_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_down_active_cycles_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_DOWN_ACTIVE_CYCLES_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_weight_wait_cycles(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_WEIGHT_WAIT_CYCLES_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_weight_wait_cycles_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_WEIGHT_WAIT_CYCLES_CTRL);
    return Data & 0x1;
}

u64 XSwiglu_v2_Get_result_fifo_stall_cycles(XSwiglu_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_FIFO_STALL_CYCLES_DATA);
    Data += (u64)XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_FIFO_STALL_CYCLES_DATA + 4) << 32;
    return Data;
}

u32 XSwiglu_v2_Get_result_fifo_stall_cycles_vld(XSwiglu_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_RESULT_FIFO_STALL_CYCLES_CTRL);
    return Data & 0x1;
}

void XSwiglu_v2_InterruptGlobalEnable(XSwiglu_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_GIE, 1);
}

void XSwiglu_v2_InterruptGlobalDisable(XSwiglu_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_GIE, 0);
}

void XSwiglu_v2_InterruptEnable(XSwiglu_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_IER);
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_IER, Register | Mask);
}

void XSwiglu_v2_InterruptDisable(XSwiglu_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_IER);
    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_IER, Register & (~Mask));
}

void XSwiglu_v2_InterruptClear(XSwiglu_v2 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSwiglu_v2_WriteReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_ISR, Mask);
}

u32 XSwiglu_v2_InterruptGetEnabled(XSwiglu_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_IER);
}

u32 XSwiglu_v2_InterruptGetStatus(XSwiglu_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSwiglu_v2_ReadReg(InstancePtr->Control_BaseAddress, XSWIGLU_V2_CONTROL_ADDR_ISR);
}

