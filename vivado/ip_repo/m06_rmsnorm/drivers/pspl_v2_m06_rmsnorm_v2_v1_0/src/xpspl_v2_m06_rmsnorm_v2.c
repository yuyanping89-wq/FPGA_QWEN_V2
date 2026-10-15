// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xpspl_v2_m06_rmsnorm_v2.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XPspl_v2_m06_rmsnorm_v2_CfgInitialize(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, XPspl_v2_m06_rmsnorm_v2_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XPspl_v2_m06_rmsnorm_v2_Start(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL) & 0x80;
    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XPspl_v2_m06_rmsnorm_v2_IsDone(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XPspl_v2_m06_rmsnorm_v2_IsIdle(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XPspl_v2_m06_rmsnorm_v2_IsReady(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XPspl_v2_m06_rmsnorm_v2_EnableAutoRestart(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XPspl_v2_m06_rmsnorm_v2_DisableAutoRestart(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_AP_CTRL, 0);
}

void XPspl_v2_m06_rmsnorm_v2_Set_x(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_X_DATA, (u32)(Data));
    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_X_DATA + 4, (u32)(Data >> 32));
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_x(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_X_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_X_DATA + 4) << 32;
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_weight(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_WEIGHT_DATA, (u32)(Data));
    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_WEIGHT_DATA + 4, (u32)(Data >> 32));
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_weight(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_WEIGHT_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_WEIGHT_DATA + 4) << 32;
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_y(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_Y_DATA, (u32)(Data));
    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_Y_DATA + 4, (u32)(Data >> 32));
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_y(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_Y_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_Y_DATA + 4) << 32;
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_run_mode(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_run_mode(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_valid_rows(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_valid_rows(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_hidden_size(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_HIDDEN_SIZE_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_hidden_size(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_HIDDEN_SIZE_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_eps(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_EPS_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_eps(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_EPS_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_norm_role(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_NORM_ROLE_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_norm_role(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_NORM_ROLE_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_abi_version(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_abi_version(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_job_id(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_job_id(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XPspl_v2_m06_rmsnorm_v2_Set_flags(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_flags(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_status_code(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_STATUS_CODE_DATA);
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_status_code_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_processed_rows(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_processed_rows_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_total(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_total_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_compute(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_compute_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_read_beats(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_READ_BEATS_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_read_beats_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_write_beats(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_WRITE_BEATS_DATA);
    Data += (u64)XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_write_beats_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_output_crc(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_OUTPUT_CRC_DATA);
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_output_crc_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_build_config(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_BUILD_CONFIG_DATA);
    return Data;
}

u32 XPspl_v2_m06_rmsnorm_v2_Get_build_config_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

void XPspl_v2_m06_rmsnorm_v2_InterruptGlobalEnable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_GIE, 1);
}

void XPspl_v2_m06_rmsnorm_v2_InterruptGlobalDisable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_GIE, 0);
}

void XPspl_v2_m06_rmsnorm_v2_InterruptEnable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_IER);
    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_IER, Register | Mask);
}

void XPspl_v2_m06_rmsnorm_v2_InterruptDisable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_IER);
    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_IER, Register & (~Mask));
}

void XPspl_v2_m06_rmsnorm_v2_InterruptClear(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPspl_v2_m06_rmsnorm_v2_WriteReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_ISR, Mask);
}

u32 XPspl_v2_m06_rmsnorm_v2_InterruptGetEnabled(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_IER);
}

u32 XPspl_v2_m06_rmsnorm_v2_InterruptGetStatus(XPspl_v2_m06_rmsnorm_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XPspl_v2_m06_rmsnorm_v2_ReadReg(InstancePtr->Control_BaseAddress, XPSPL_V2_M06_RMSNORM_V2_CONTROL_ADDR_ISR);
}

