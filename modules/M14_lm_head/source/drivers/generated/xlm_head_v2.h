// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XLM_HEAD_V2_H
#define XLM_HEAD_V2_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xlm_head_v2_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XLm_head_v2_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XLm_head_v2;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XLm_head_v2_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XLm_head_v2_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XLm_head_v2_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XLm_head_v2_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XLm_head_v2_Initialize(XLm_head_v2 *InstancePtr, u16 DeviceId);
XLm_head_v2_Config* XLm_head_v2_LookupConfig(u16 DeviceId);
int XLm_head_v2_CfgInitialize(XLm_head_v2 *InstancePtr, XLm_head_v2_Config *ConfigPtr);
#else
int XLm_head_v2_Initialize(XLm_head_v2 *InstancePtr, const char* InstanceName);
int XLm_head_v2_Release(XLm_head_v2 *InstancePtr);
#endif

void XLm_head_v2_Start(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_IsDone(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_IsIdle(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_IsReady(XLm_head_v2 *InstancePtr);
void XLm_head_v2_EnableAutoRestart(XLm_head_v2 *InstancePtr);
void XLm_head_v2_DisableAutoRestart(XLm_head_v2 *InstancePtr);

void XLm_head_v2_Set_hidden_offset(XLm_head_v2 *InstancePtr, u64 Data);
u64 XLm_head_v2_Get_hidden_offset(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_shared_vocab_weight(XLm_head_v2 *InstancePtr, u64 Data);
u64 XLm_head_v2_Get_shared_vocab_weight(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_history_bitmap(XLm_head_v2 *InstancePtr, u64 Data);
u64 XLm_head_v2_Get_history_bitmap(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_optional_logits(XLm_head_v2 *InstancePtr, u64 Data);
u64 XLm_head_v2_Get_optional_logits(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_hidden_size_r(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_hidden_size_r(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_vocab_size(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_vocab_size(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_run_mode(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_run_mode(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_valid_rows(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_valid_rows(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_write_logits(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_write_logits(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_enable_penalty(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_enable_penalty(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_repetition_penalty(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_repetition_penalty(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_abi_version(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_abi_version(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_job_id(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_job_id(XLm_head_v2 *InstancePtr);
void XLm_head_v2_Set_flags(XLm_head_v2 *InstancePtr, u32 Data);
u32 XLm_head_v2_Get_flags(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_selected_token(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_selected_token_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_selected_score(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_selected_score_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_result_job_id(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_result_job_id_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_status_code(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_status_code_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_processed_rows(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_processed_rows_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_perf_cycles_total(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_perf_cycles_total_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_perf_cycles_compute(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_perf_cycles_compute_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_perf_read_beats(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_perf_read_beats_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_perf_write_beats(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_perf_write_beats_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_output_crc(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_output_crc_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_build_config(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_build_config_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_capabilities(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_capabilities_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_vocab_rows_done(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_vocab_rows_done_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_weight_read_beats_128(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_weight_read_beats_128_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_history_read_words(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_history_read_words_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_logits_write_beats_128(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_logits_write_beats_128_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_weight_wait_cycles(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_weight_wait_cycles_vld(XLm_head_v2 *InstancePtr);
u64 XLm_head_v2_Get_dot_active_cycles(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_dot_active_cycles_vld(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_logits_written(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_Get_logits_written_vld(XLm_head_v2 *InstancePtr);

void XLm_head_v2_InterruptGlobalEnable(XLm_head_v2 *InstancePtr);
void XLm_head_v2_InterruptGlobalDisable(XLm_head_v2 *InstancePtr);
void XLm_head_v2_InterruptEnable(XLm_head_v2 *InstancePtr, u32 Mask);
void XLm_head_v2_InterruptDisable(XLm_head_v2 *InstancePtr, u32 Mask);
void XLm_head_v2_InterruptClear(XLm_head_v2 *InstancePtr, u32 Mask);
u32 XLm_head_v2_InterruptGetEnabled(XLm_head_v2 *InstancePtr);
u32 XLm_head_v2_InterruptGetStatus(XLm_head_v2 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
