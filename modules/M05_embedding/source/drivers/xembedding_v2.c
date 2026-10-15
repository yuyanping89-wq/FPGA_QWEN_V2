// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xembedding_v2.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XEmbedding_v2_CfgInitialize(XEmbedding_v2 *InstancePtr, XEmbedding_v2_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Ctrl_BaseAddress = ConfigPtr->Ctrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XEmbedding_v2_Start(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL) & 0x80;
    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XEmbedding_v2_IsDone(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XEmbedding_v2_IsIdle(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XEmbedding_v2_IsReady(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XEmbedding_v2_EnableAutoRestart(XEmbedding_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL, 0x80);
}

void XEmbedding_v2_DisableAutoRestart(XEmbedding_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_AP_CTRL, 0);
}

void XEmbedding_v2_Set_token_ids(XEmbedding_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TOKEN_IDS_DATA, (u32)(Data));
    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TOKEN_IDS_DATA + 4, (u32)(Data >> 32));
}

u64 XEmbedding_v2_Get_token_ids(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TOKEN_IDS_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TOKEN_IDS_DATA + 4) << 32;
    return Data;
}

void XEmbedding_v2_Set_table_r(XEmbedding_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TABLE_R_DATA, (u32)(Data));
    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TABLE_R_DATA + 4, (u32)(Data >> 32));
}

u64 XEmbedding_v2_Get_table_r(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TABLE_R_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_TABLE_R_DATA + 4) << 32;
    return Data;
}

void XEmbedding_v2_Set_hidden_out(XEmbedding_v2 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_HIDDEN_OUT_DATA, (u32)(Data));
    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_HIDDEN_OUT_DATA + 4, (u32)(Data >> 32));
}

u64 XEmbedding_v2_Get_hidden_out(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_HIDDEN_OUT_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_HIDDEN_OUT_DATA + 4) << 32;
    return Data;
}

void XEmbedding_v2_Set_run_mode(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RUN_MODE_DATA, Data);
}

u32 XEmbedding_v2_Get_run_mode(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RUN_MODE_DATA);
    return Data;
}

void XEmbedding_v2_Set_valid_rows(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_VALID_ROWS_DATA, Data);
}

u32 XEmbedding_v2_Get_valid_rows(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_VALID_ROWS_DATA);
    return Data;
}

void XEmbedding_v2_Set_hidden_size(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_HIDDEN_SIZE_DATA, Data);
}

u32 XEmbedding_v2_Get_hidden_size(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_HIDDEN_SIZE_DATA);
    return Data;
}

void XEmbedding_v2_Set_vocab_size(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_VOCAB_SIZE_DATA, Data);
}

u32 XEmbedding_v2_Get_vocab_size(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_VOCAB_SIZE_DATA);
    return Data;
}

void XEmbedding_v2_Set_abi_version(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_ABI_VERSION_DATA, Data);
}

u32 XEmbedding_v2_Get_abi_version(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_ABI_VERSION_DATA);
    return Data;
}

void XEmbedding_v2_Set_job_id(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_JOB_ID_DATA, Data);
}

u32 XEmbedding_v2_Get_job_id(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_JOB_ID_DATA);
    return Data;
}

void XEmbedding_v2_Set_flags(XEmbedding_v2 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_FLAGS_DATA, Data);
}

u32 XEmbedding_v2_Get_flags(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_FLAGS_DATA);
    return Data;
}

u32 XEmbedding_v2_Get_result_status_code(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_STATUS_CODE_DATA);
    return Data;
}

u32 XEmbedding_v2_Get_result_status_code_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_STATUS_CODE_CTRL);
    return Data & 0x1;
}

u32 XEmbedding_v2_Get_result_processed_rows(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PROCESSED_ROWS_DATA);
    return Data;
}

u32 XEmbedding_v2_Get_result_processed_rows_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PROCESSED_ROWS_CTRL);
    return Data & 0x1;
}

u64 XEmbedding_v2_Get_result_perf_cycles_total(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_TOTAL_DATA + 4) << 32;
    return Data;
}

u32 XEmbedding_v2_Get_result_perf_cycles_total_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_TOTAL_CTRL);
    return Data & 0x1;
}

u64 XEmbedding_v2_Get_result_perf_cycles_compute(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_COMPUTE_DATA + 4) << 32;
    return Data;
}

u32 XEmbedding_v2_Get_result_perf_cycles_compute_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_CYCLES_COMPUTE_CTRL);
    return Data & 0x1;
}

u64 XEmbedding_v2_Get_result_perf_read_beats(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_READ_BEATS_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_READ_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XEmbedding_v2_Get_result_perf_read_beats_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_READ_BEATS_CTRL);
    return Data & 0x1;
}

u64 XEmbedding_v2_Get_result_perf_write_beats(XEmbedding_v2 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_WRITE_BEATS_DATA);
    Data += (u64)XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_WRITE_BEATS_DATA + 4) << 32;
    return Data;
}

u32 XEmbedding_v2_Get_result_perf_write_beats_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_PERF_WRITE_BEATS_CTRL);
    return Data & 0x1;
}

u32 XEmbedding_v2_Get_result_output_crc(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_OUTPUT_CRC_DATA);
    return Data;
}

u32 XEmbedding_v2_Get_result_output_crc_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_OUTPUT_CRC_CTRL);
    return Data & 0x1;
}

u32 XEmbedding_v2_Get_result_build_config(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_BUILD_CONFIG_DATA);
    return Data;
}

u32 XEmbedding_v2_Get_result_build_config_vld(XEmbedding_v2 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_RESULT_BUILD_CONFIG_CTRL);
    return Data & 0x1;
}

void XEmbedding_v2_InterruptGlobalEnable(XEmbedding_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_GIE, 1);
}

void XEmbedding_v2_InterruptGlobalDisable(XEmbedding_v2 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_GIE, 0);
}

void XEmbedding_v2_InterruptEnable(XEmbedding_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_IER);
    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_IER, Register | Mask);
}

void XEmbedding_v2_InterruptDisable(XEmbedding_v2 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_IER);
    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_IER, Register & (~Mask));
}

void XEmbedding_v2_InterruptClear(XEmbedding_v2 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XEmbedding_v2_WriteReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_ISR, Mask);
}

u32 XEmbedding_v2_InterruptGetEnabled(XEmbedding_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_IER);
}

u32 XEmbedding_v2_InterruptGetStatus(XEmbedding_v2 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XEmbedding_v2_ReadReg(InstancePtr->Ctrl_BaseAddress, XEMBEDDING_V2_CTRL_ADDR_ISR);
}

