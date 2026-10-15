// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlm_head_v2.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLm_head_v2_CfgInitialize(XLm_head_v2 *InstancePtr, XLm_head_v2_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLm_head_v2_Start(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLm_head_v2_IsDone(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLm_head_v2_IsIdle(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLm_head_v2_IsReady(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLm_head_v2_EnableAutoRestart(XLm_head_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XLm_head_v2_DisableAutoRestart(XLm_head_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_AP_CTRL, 0);
}

void XLm_head_v2_Set_hidden_offset(XLm_head_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_OFFSET_DATA, (u32)(Data));
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_OFFSET_DATA + 4, (u32)(Data >> 32));
}

u64 XLm_head_v2_Get_hidden_offset(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_OFFSET_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_OFFSET_DATA + 4) << 32;
    return Data;
}

void XLm_head_v2_Set_shared_vocab_weight(XLm_head_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SHARED_VOCAB_WEIGHT_DATA, (u32)(Data));
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SHARED_VOCAB_WEIGHT_DATA + 4, (u32)(Data >> 32));
}

u64 XLm_head_v2_Get_shared_vocab_weight(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SHARED_VOCAB_WEIGHT_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SHARED_VOCAB_WEIGHT_DATA + 4) << 32;
    return Data;
}

void XLm_head_v2_Set_history_bitmap(XLm_head_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HISTORY_BITMAP_DATA, (u32)(Data));
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HISTORY_BITMAP_DATA + 4, (u32)(Data >> 32));
}

u64 XLm_head_v2_Get_history_bitmap(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HISTORY_BITMAP_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HISTORY_BITMAP_DATA + 4) << 32;
    return Data;
}

void XLm_head_v2_Set_optional_logits(XLm_head_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_OPTIONAL_LOGITS_DATA, (u32)(Data));
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_OPTIONAL_LOGITS_DATA + 4, (u32)(Data >> 32));
}

u64 XLm_head_v2_Get_optional_logits(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_OPTIONAL_LOGITS_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_OPTIONAL_LOGITS_DATA + 4) << 32;
    return Data;
}

void XLm_head_v2_Set_hidden_size_r(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_SIZE_R_DATA, Data);
}

u32 XLm_head_v2_Get_hidden_size_r(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HIDDEN_SIZE_R_DATA);
    return Data;
}

void XLm_head_v2_Set_vocab_size(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_VOCAB_SIZE_DATA, Data);
}

u32 XLm_head_v2_Get_vocab_size(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_VOCAB_SIZE_DATA);
    return Data;
}

void XLm_head_v2_Set_run_mode(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_RUN_MODE_DATA, Data);
}

u32 XLm_head_v2_Get_run_mode(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XLm_head_v2_Set_valid_rows(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XLm_head_v2_Get_valid_rows(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XLm_head_v2_Set_write_logits(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WRITE_LOGITS_DATA, Data);
}

u32 XLm_head_v2_Get_write_logits(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WRITE_LOGITS_DATA);
    return Data;
}

void XLm_head_v2_Set_enable_penalty(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_ENABLE_PENALTY_DATA, Data);
}

u32 XLm_head_v2_Get_enable_penalty(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_ENABLE_PENALTY_DATA);
    return Data;
}

void XLm_head_v2_Set_repetition_penalty(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_REPETITION_PENALTY_DATA, Data);
}

u32 XLm_head_v2_Get_repetition_penalty(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_REPETITION_PENALTY_DATA);
    return Data;
}

void XLm_head_v2_Set_abi_version(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XLm_head_v2_Get_abi_version(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XLm_head_v2_Set_job_id(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_JOB_ID_DATA, Data);
}

u32 XLm_head_v2_Get_job_id(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_JOB_ID_DATA);
    return Data;
}

void XLm_head_v2_Set_flags(XLm_head_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_FLAGS_DATA, Data);
}

u32 XLm_head_v2_Get_flags(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XLm_head_v2_Get_selected_token(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SELECTED_TOKEN_DATA);
    return Data;
}

u32 XLm_head_v2_Get_selected_token_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SELECTED_TOKEN_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_selected_score(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SELECTED_SCORE_DATA);
    return Data;
}

u32 XLm_head_v2_Get_selected_score_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_SELECTED_SCORE_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_result_job_id(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_RESULT_JOB_ID_DATA);
    return Data;
}

u32 XLm_head_v2_Get_result_job_id_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_RESULT_JOB_ID_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_status_code(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_STATUS_CODE_DATA);
    return Data;
}

u32 XLm_head_v2_Get_status_code_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_processed_rows(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XLm_head_v2_Get_processed_rows_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_perf_cycles_total(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_perf_cycles_total_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_perf_cycles_compute(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_perf_cycles_compute_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_perf_read_beats(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_READ_BEATS_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_perf_read_beats_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_perf_write_beats(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_WRITE_BEATS_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_perf_write_beats_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_output_crc(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_OUTPUT_CRC_DATA);
    return Data;
}

u32 XLm_head_v2_Get_output_crc_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_build_config(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_BUILD_CONFIG_DATA);
    return Data;
}

u32 XLm_head_v2_Get_build_config_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_capabilities(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_CAPABILITIES_DATA);
    return Data;
}

u32 XLm_head_v2_Get_capabilities_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_CAPABILITIES_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_vocab_rows_done(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_VOCAB_ROWS_DONE_DATA);
    return Data;
}

u32 XLm_head_v2_Get_vocab_rows_done_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_VOCAB_ROWS_DONE_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_weight_read_beats_128(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_READ_BEATS_128_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_READ_BEATS_128_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_weight_read_beats_128_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_READ_BEATS_128_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_history_read_words(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HISTORY_READ_WORDS_DATA);
    return Data;
}

u32 XLm_head_v2_Get_history_read_words_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_HISTORY_READ_WORDS_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_logits_write_beats_128(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITE_BEATS_128_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITE_BEATS_128_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_logits_write_beats_128_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITE_BEATS_128_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_weight_wait_cycles(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_WAIT_CYCLES_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_WAIT_CYCLES_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_weight_wait_cycles_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_WEIGHT_WAIT_CYCLES_CTRL);
    return Data & 0x1;
}

u64 XLm_head_v2_Get_dot_active_cycles(XLm_head_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_DOT_ACTIVE_CYCLES_DATA);
    Data += (u64)XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_DOT_ACTIVE_CYCLES_DATA + 4) << 32;
    return Data;
}

u32 XLm_head_v2_Get_dot_active_cycles_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_DOT_ACTIVE_CYCLES_CTRL);
    return Data & 0x1;
}

u32 XLm_head_v2_Get_logits_written(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITTEN_DATA);
    return Data;
}

u32 XLm_head_v2_Get_logits_written_vld(XLm_head_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_LOGITS_WRITTEN_CTRL);
    return Data & 0x1;
}

void XLm_head_v2_InterruptGlobalEnable(XLm_head_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_GIE, 1);
}

void XLm_head_v2_InterruptGlobalDisable(XLm_head_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_GIE, 0);
}

void XLm_head_v2_InterruptEnable(XLm_head_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_IER);
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_IER, Register | Mask);
}

void XLm_head_v2_InterruptDisable(XLm_head_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_IER);
    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_IER, Register & (~Mask));
}

void XLm_head_v2_InterruptClear(XLm_head_v2 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLm_head_v2_WriteReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_ISR, Mask);
}

u32 XLm_head_v2_InterruptGetEnabled(XLm_head_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_IER);
}

u32 XLm_head_v2_InterruptGetStatus(XLm_head_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLm_head_v2_ReadReg(InstancePtr->Control_BaseAddress, XLM_HEAD_V2_CONTROL_ADDR_ISR);
}

