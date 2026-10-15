// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xrope_v2_hls.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XRope_v2_hls_CfgInitialize(XRope_v2_hls *InstancePtr, XRope_v2_hls_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XRope_v2_hls_Start(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XRope_v2_hls_IsDone(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XRope_v2_hls_IsIdle(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XRope_v2_hls_IsReady(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XRope_v2_hls_EnableAutoRestart(XRope_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XRope_v2_hls_DisableAutoRestart(XRope_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_AP_CTRL, 0);
}

void XRope_v2_hls_Set_q_raw(XRope_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_RAW_DATA, (u32)(Data));
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_RAW_DATA + 4, (u32)(Data >> 32));
}

u64 XRope_v2_hls_Get_q_raw(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_RAW_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_RAW_DATA + 4) << 32;
    return Data;
}

void XRope_v2_hls_Set_k_raw(XRope_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_RAW_DATA, (u32)(Data));
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_RAW_DATA + 4, (u32)(Data >> 32));
}

u64 XRope_v2_hls_Get_k_raw(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_RAW_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_RAW_DATA + 4) << 32;
    return Data;
}

void XRope_v2_hls_Set_cos_table(XRope_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_COS_TABLE_DATA, (u32)(Data));
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_COS_TABLE_DATA + 4, (u32)(Data >> 32));
}

u64 XRope_v2_hls_Get_cos_table(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_COS_TABLE_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_COS_TABLE_DATA + 4) << 32;
    return Data;
}

void XRope_v2_hls_Set_sin_table(XRope_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_SIN_TABLE_DATA, (u32)(Data));
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_SIN_TABLE_DATA + 4, (u32)(Data >> 32));
}

u64 XRope_v2_hls_Get_sin_table(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_SIN_TABLE_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_SIN_TABLE_DATA + 4) << 32;
    return Data;
}

void XRope_v2_hls_Set_q_rot(XRope_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_ROT_DATA, (u32)(Data));
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_ROT_DATA + 4, (u32)(Data >> 32));
}

u64 XRope_v2_hls_Get_q_rot(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_ROT_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_Q_ROT_DATA + 4) << 32;
    return Data;
}

void XRope_v2_hls_Set_k_rot(XRope_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_ROT_DATA, (u32)(Data));
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_ROT_DATA + 4, (u32)(Data >> 32));
}

u64 XRope_v2_hls_Get_k_rot(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_ROT_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_K_ROT_DATA + 4) << 32;
    return Data;
}

void XRope_v2_hls_Set_run_mode(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XRope_v2_hls_Get_run_mode(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XRope_v2_hls_Set_valid_rows(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XRope_v2_hls_Get_valid_rows(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XRope_v2_hls_Set_position_base(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_POSITION_BASE_DATA, Data);
}

u32 XRope_v2_hls_Get_position_base(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_POSITION_BASE_DATA);
    return Data;
}

void XRope_v2_hls_Set_max_position(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_MAX_POSITION_DATA, Data);
}

u32 XRope_v2_hls_Get_max_position(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_MAX_POSITION_DATA);
    return Data;
}

void XRope_v2_hls_Set_abi_version(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XRope_v2_hls_Get_abi_version(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XRope_v2_hls_Set_job_id(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XRope_v2_hls_Get_job_id(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XRope_v2_hls_Set_flags(XRope_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XRope_v2_hls_Get_flags(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XRope_v2_hls_Get_result_status_code(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_DATA);
    return Data;
}

u32 XRope_v2_hls_Get_result_status_code_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XRope_v2_hls_Get_result_processed_rows(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XRope_v2_hls_Get_result_processed_rows_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XRope_v2_hls_Get_result_perf_cycles_total(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XRope_v2_hls_Get_result_perf_cycles_total_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XRope_v2_hls_Get_result_perf_cycles_compute(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XRope_v2_hls_Get_result_perf_cycles_compute_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XRope_v2_hls_Get_result_perf_read_beats(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XRope_v2_hls_Get_result_perf_read_beats_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XRope_v2_hls_Get_result_perf_write_beats(XRope_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA);
    Data += (u64)XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XRope_v2_hls_Get_result_perf_write_beats_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XRope_v2_hls_Get_result_output_crc(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA);
    return Data;
}

u32 XRope_v2_hls_Get_result_output_crc_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XRope_v2_hls_Get_result_build_config(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA);
    return Data;
}

u32 XRope_v2_hls_Get_result_build_config_vld(XRope_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

void XRope_v2_hls_InterruptGlobalEnable(XRope_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_GIE, 1);
}

void XRope_v2_hls_InterruptGlobalDisable(XRope_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_GIE, 0);
}

void XRope_v2_hls_InterruptEnable(XRope_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_IER);
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_IER, Register | Mask);
}

void XRope_v2_hls_InterruptDisable(XRope_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_IER);
    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_IER, Register & (~Mask));
}

void XRope_v2_hls_InterruptClear(XRope_v2_hls *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XRope_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_ISR, Mask);
}

u32 XRope_v2_hls_InterruptGetEnabled(XRope_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_IER);
}

u32 XRope_v2_hls_InterruptGetStatus(XRope_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XRope_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XROPE_V2_HLS_CONTROL_ADDR_ISR);
}

