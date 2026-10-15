// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xresidual_add_v2_hls.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XResidual_add_v2_hls_CfgInitialize(XResidual_add_v2_hls *InstancePtr, XResidual_add_v2_hls_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XResidual_add_v2_hls_Start(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XResidual_add_v2_hls_IsDone(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XResidual_add_v2_hls_IsIdle(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XResidual_add_v2_hls_IsReady(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XResidual_add_v2_hls_EnableAutoRestart(XResidual_add_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XResidual_add_v2_hls_DisableAutoRestart(XResidual_add_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_AP_CTRL, 0);
}

void XResidual_add_v2_hls_Set_a(XResidual_add_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_A_DATA, (u32)(Data));
    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_A_DATA + 4, (u32)(Data >> 32));
}

u64 XResidual_add_v2_hls_Get_a(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_A_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_A_DATA + 4) << 32;
    return Data;
}

void XResidual_add_v2_hls_Set_b(XResidual_add_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_B_DATA, (u32)(Data));
    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_B_DATA + 4, (u32)(Data >> 32));
}

u64 XResidual_add_v2_hls_Get_b(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_B_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_B_DATA + 4) << 32;
    return Data;
}

void XResidual_add_v2_hls_Set_y(XResidual_add_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_Y_DATA, (u32)(Data));
    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_Y_DATA + 4, (u32)(Data >> 32));
}

u64 XResidual_add_v2_hls_Get_y(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_Y_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_Y_DATA + 4) << 32;
    return Data;
}

void XResidual_add_v2_hls_Set_run_mode(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_run_mode(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XResidual_add_v2_hls_Set_valid_rows(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_valid_rows(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XResidual_add_v2_hls_Set_hidden_size(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_HIDDEN_SIZE_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_hidden_size(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_HIDDEN_SIZE_DATA);
    return Data;
}

void XResidual_add_v2_hls_Set_residual_role(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESIDUAL_ROLE_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_residual_role(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESIDUAL_ROLE_DATA);
    return Data;
}

void XResidual_add_v2_hls_Set_abi_version(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_abi_version(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XResidual_add_v2_hls_Set_job_id(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_job_id(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XResidual_add_v2_hls_Set_flags(XResidual_add_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XResidual_add_v2_hls_Get_flags(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_status_code(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_DATA);
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_status_code_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XResidual_add_v2_hls_Get_result_processed_rows(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_processed_rows_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XResidual_add_v2_hls_Get_result_perf_cycles_total(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_perf_cycles_total_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XResidual_add_v2_hls_Get_result_perf_cycles_compute(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_perf_cycles_compute_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XResidual_add_v2_hls_Get_result_perf_read_beats(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_perf_read_beats_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XResidual_add_v2_hls_Get_result_perf_write_beats(XResidual_add_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA);
    Data += (u64)XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_perf_write_beats_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XResidual_add_v2_hls_Get_result_output_crc(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA);
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_output_crc_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XResidual_add_v2_hls_Get_result_build_config(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA);
    return Data;
}

u32 XResidual_add_v2_hls_Get_result_build_config_vld(XResidual_add_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

void XResidual_add_v2_hls_InterruptGlobalEnable(XResidual_add_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_GIE, 1);
}

void XResidual_add_v2_hls_InterruptGlobalDisable(XResidual_add_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_GIE, 0);
}

void XResidual_add_v2_hls_InterruptEnable(XResidual_add_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_IER);
    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_IER, Register | Mask);
}

void XResidual_add_v2_hls_InterruptDisable(XResidual_add_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_IER);
    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_IER, Register & (~Mask));
}

void XResidual_add_v2_hls_InterruptClear(XResidual_add_v2_hls *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XResidual_add_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_ISR, Mask);
}

u32 XResidual_add_v2_hls_InterruptGetEnabled(XResidual_add_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_IER);
}

u32 XResidual_add_v2_hls_InterruptGetStatus(XResidual_add_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XResidual_add_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XRESIDUAL_ADD_V2_HLS_CONTROL_ADDR_ISR);
}

