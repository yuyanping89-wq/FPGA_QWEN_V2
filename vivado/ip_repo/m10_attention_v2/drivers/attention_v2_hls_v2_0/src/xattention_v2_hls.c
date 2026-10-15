// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xattention_v2_hls.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XAttention_v2_hls_CfgInitialize(XAttention_v2_hls *InstancePtr, XAttention_v2_hls_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XAttention_v2_hls_Start(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XAttention_v2_hls_IsDone(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XAttention_v2_hls_IsIdle(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XAttention_v2_hls_IsReady(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XAttention_v2_hls_EnableAutoRestart(XAttention_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XAttention_v2_hls_DisableAutoRestart(XAttention_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_AP_CTRL, 0);
}

void XAttention_v2_hls_Set_q_rot(XAttention_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_Q_ROT_DATA, (u32)(Data));
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_Q_ROT_DATA + 4, (u32)(Data >> 32));
}

u64 XAttention_v2_hls_Get_q_rot(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_Q_ROT_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_Q_ROT_DATA + 4) << 32;
    return Data;
}

void XAttention_v2_hls_Set_k_cache(XAttention_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_K_CACHE_DATA, (u32)(Data));
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_K_CACHE_DATA + 4, (u32)(Data >> 32));
}

u64 XAttention_v2_hls_Get_k_cache(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_K_CACHE_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_K_CACHE_DATA + 4) << 32;
    return Data;
}

void XAttention_v2_hls_Set_v_cache(XAttention_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_V_CACHE_DATA, (u32)(Data));
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_V_CACHE_DATA + 4, (u32)(Data >> 32));
}

u64 XAttention_v2_hls_Get_v_cache(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_V_CACHE_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_V_CACHE_DATA + 4) << 32;
    return Data;
}

void XAttention_v2_hls_Set_attn_out(XAttention_v2_hls *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ATTN_OUT_DATA, (u32)(Data));
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ATTN_OUT_DATA + 4, (u32)(Data >> 32));
}

u64 XAttention_v2_hls_Get_attn_out(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ATTN_OUT_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ATTN_OUT_DATA + 4) << 32;
    return Data;
}

void XAttention_v2_hls_Set_run_mode(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XAttention_v2_hls_Get_run_mode(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XAttention_v2_hls_Set_layer_id(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_LAYER_ID_DATA, Data);
}

u32 XAttention_v2_hls_Get_layer_id(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_LAYER_ID_DATA);
    return Data;
}

void XAttention_v2_hls_Set_query_pos_base(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_QUERY_POS_BASE_DATA, Data);
}

u32 XAttention_v2_hls_Get_query_pos_base(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_QUERY_POS_BASE_DATA);
    return Data;
}

void XAttention_v2_hls_Set_valid_rows(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XAttention_v2_hls_Get_valid_rows(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XAttention_v2_hls_Set_cache_len(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_CACHE_LEN_DATA, Data);
}

u32 XAttention_v2_hls_Get_cache_len(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_CACHE_LEN_DATA);
    return Data;
}

void XAttention_v2_hls_Set_scale(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_SCALE_DATA, Data);
}

u32 XAttention_v2_hls_Get_scale(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_SCALE_DATA);
    return Data;
}

void XAttention_v2_hls_Set_causal(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_CAUSAL_DATA, Data);
}

u32 XAttention_v2_hls_Get_causal(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_CAUSAL_DATA);
    return Data;
}

void XAttention_v2_hls_Set_abi_version(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XAttention_v2_hls_Get_abi_version(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XAttention_v2_hls_Set_job_id(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XAttention_v2_hls_Get_job_id(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XAttention_v2_hls_Set_flags(XAttention_v2_hls *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XAttention_v2_hls_Get_flags(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XAttention_v2_hls_Get_result_status_code(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_DATA);
    return Data;
}

u32 XAttention_v2_hls_Get_result_status_code_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XAttention_v2_hls_Get_result_processed_rows(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XAttention_v2_hls_Get_result_processed_rows_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XAttention_v2_hls_Get_result_perf_cycles_total(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XAttention_v2_hls_Get_result_perf_cycles_total_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XAttention_v2_hls_Get_result_perf_cycles_compute(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XAttention_v2_hls_Get_result_perf_cycles_compute_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XAttention_v2_hls_Get_result_perf_read_beats(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XAttention_v2_hls_Get_result_perf_read_beats_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XAttention_v2_hls_Get_result_perf_write_beats(XAttention_v2_hls *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA);
    Data += (u64)XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XAttention_v2_hls_Get_result_perf_write_beats_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XAttention_v2_hls_Get_result_output_crc(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_DATA);
    return Data;
}

u32 XAttention_v2_hls_Get_result_output_crc_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XAttention_v2_hls_Get_result_build_config(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_DATA);
    return Data;
}

u32 XAttention_v2_hls_Get_result_build_config_vld(XAttention_v2_hls *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_RESULT_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

void XAttention_v2_hls_InterruptGlobalEnable(XAttention_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_GIE, 1);
}

void XAttention_v2_hls_InterruptGlobalDisable(XAttention_v2_hls *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_GIE, 0);
}

void XAttention_v2_hls_InterruptEnable(XAttention_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_IER);
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_IER, Register | Mask);
}

void XAttention_v2_hls_InterruptDisable(XAttention_v2_hls *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_IER);
    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_IER, Register & (~Mask));
}

void XAttention_v2_hls_InterruptClear(XAttention_v2_hls *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XAttention_v2_hls_WriteReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ISR, Mask);
}

u32 XAttention_v2_hls_InterruptGetEnabled(XAttention_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_IER);
}

u32 XAttention_v2_hls_InterruptGetStatus(XAttention_v2_hls *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XAttention_v2_hls_ReadReg(InstancePtr->Control_BaseAddress, XATTENTION_V2_HLS_CONTROL_ADDR_ISR);
}

