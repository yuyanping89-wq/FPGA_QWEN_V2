// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xqkv_projection_v2_hls.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XQkv_projection_v2_hls_CfgInitialize(XQkv_projection_v2_hls *InstancePtr, XQkv_projection_v2_hls_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XQkv_projection_v2_hls_Start(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XQkv_projection_v2_hls_IsDone(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XQkv_projection_v2_hls_IsIdle(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XQkv_projection_v2_hls_IsReady(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XQkv_projection_v2_hls_EnableAutoRestart(XQkv_projection_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XQkv_projection_v2_hls_DisableAutoRestart(XQkv_projection_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_AP_CTRL, 0);
}

void XQkv_projection_v2_hls_Set_x(XQkv_projection_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_X_DATA, (u32)(Data));
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_X_DATA + 4, (u32)(Data >> 32));
}

u64 XQkv_projection_v2_hls_Get_x(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_X_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_X_DATA + 4) << 32;
    return Data;
}

void XQkv_projection_v2_hls_Set_w_qkv(XQkv_projection_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_W_QKV_DATA, (u32)(Data));
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_W_QKV_DATA + 4, (u32)(Data >> 32));
}

u64 XQkv_projection_v2_hls_Get_w_qkv(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_W_QKV_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_W_QKV_DATA + 4) << 32;
    return Data;
}

void XQkv_projection_v2_hls_Set_bias_qkv(XQkv_projection_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_BIAS_QKV_DATA, (u32)(Data));
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_BIAS_QKV_DATA + 4, (u32)(Data >> 32));
}

u64 XQkv_projection_v2_hls_Get_bias_qkv(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_BIAS_QKV_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_BIAS_QKV_DATA + 4) << 32;
    return Data;
}

void XQkv_projection_v2_hls_Set_q(XQkv_projection_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_Q_DATA, (u32)(Data));
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_Q_DATA + 4, (u32)(Data >> 32));
}

u64 XQkv_projection_v2_hls_Get_q(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_Q_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_Q_DATA + 4) << 32;
    return Data;
}

void XQkv_projection_v2_hls_Set_k(XQkv_projection_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_K_DATA, (u32)(Data));
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_K_DATA + 4, (u32)(Data >> 32));
}

u64 XQkv_projection_v2_hls_Get_k(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_K_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_K_DATA + 4) << 32;
    return Data;
}

void XQkv_projection_v2_hls_Set_v(XQkv_projection_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_V_DATA, (u32)(Data));
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_V_DATA + 4, (u32)(Data >> 32));
}

u64 XQkv_projection_v2_hls_Get_v(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_V_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_V_DATA + 4) << 32;
    return Data;
}

void XQkv_projection_v2_hls_Set_run_mode(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_run_mode(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XQkv_projection_v2_hls_Set_valid_rows(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_valid_rows(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XQkv_projection_v2_hls_Set_hidden_size(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_HIDDEN_SIZE_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_hidden_size(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_HIDDEN_SIZE_DATA);
    return Data;
}

void XQkv_projection_v2_hls_Set_layout_id(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_LAYOUT_ID_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_layout_id(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_LAYOUT_ID_DATA);
    return Data;
}

void XQkv_projection_v2_hls_Set_abi_version(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_abi_version(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XQkv_projection_v2_hls_Set_job_id(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_job_id(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XQkv_projection_v2_hls_Set_flags(XQkv_projection_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XQkv_projection_v2_hls_Get_flags(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_status_code(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_DATA);
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_status_code_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XQkv_projection_v2_hls_Get_result_processed_rows(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_processed_rows_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XQkv_projection_v2_hls_Get_result_perf_cycles_total(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_perf_cycles_total_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XQkv_projection_v2_hls_Get_result_perf_cycles_compute(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_perf_cycles_compute_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XQkv_projection_v2_hls_Get_result_perf_read_beats(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_perf_read_beats_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XQkv_projection_v2_hls_Get_result_perf_write_beats(XQkv_projection_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA);
    Data += (u64)XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_perf_write_beats_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XQkv_projection_v2_hls_Get_result_output_crc(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA);
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_output_crc_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XQkv_projection_v2_hls_Get_result_build_config(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA);
    return Data;
}

u32 XQkv_projection_v2_hls_Get_result_build_config_vld(XQkv_projection_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

void XQkv_projection_v2_hls_InterruptGlobalEnable(XQkv_projection_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_GIE, 1);
}

void XQkv_projection_v2_hls_InterruptGlobalDisable(XQkv_projection_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_GIE, 0);
}

void XQkv_projection_v2_hls_InterruptEnable(XQkv_projection_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_IER);
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_IER, Register | Mask);
}

void XQkv_projection_v2_hls_InterruptDisable(XQkv_projection_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_IER);
    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_IER, Register & (~Mask));
}

void XQkv_projection_v2_hls_InterruptClear(XQkv_projection_v2_hls *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XQkv_projection_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_ISR, Mask);
}

u32 XQkv_projection_v2_hls_InterruptGetEnabled(XQkv_projection_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_IER);
}

u32 XQkv_projection_v2_hls_InterruptGetStatus(XQkv_projection_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XQkv_projection_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XQKV_PROJECTION_V2_HLS_CONTROL_ADDR_ISR);
}

