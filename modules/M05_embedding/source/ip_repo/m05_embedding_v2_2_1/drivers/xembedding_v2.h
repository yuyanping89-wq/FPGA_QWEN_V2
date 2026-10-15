// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XEMBEDDING_V2_H
#define XEMBEDDING_V2_H

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
#include "xembedding_v2_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Ctrl_BaseAddress;
} XEmbedding_v2_Config;
#endif

typedef struct {
    u64 Ctrl_BaseAddress;
    u32 IsReady;
} XEmbedding_v2;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XEmbedding_v2_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XEmbedding_v2_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XEmbedding_v2_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XEmbedding_v2_ReadReg(BaseAddress, RegOffset) \
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
int XEmbedding_v2_Initialize(XEmbedding_v2 *InstancePtr, u16 DeviceId);
XEmbedding_v2_Config* XEmbedding_v2_LookupConfig(u16 DeviceId);
int XEmbedding_v2_CfgInitialize(XEmbedding_v2 *InstancePtr, XEmbedding_v2_Config *ConfigPtr);
#else
int XEmbedding_v2_Initialize(XEmbedding_v2 *InstancePtr, const char* InstanceName);
int XEmbedding_v2_Release(XEmbedding_v2 *InstancePtr);
#endif

void XEmbedding_v2_Start(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_IsDone(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_IsIdle(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_IsReady(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_EnableAutoRestart(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_DisableAutoRestart(XEmbedding_v2 *InstancePtr);

void XEmbedding_v2_Set_token_ids(XEmbedding_v2 *InstancePtr, u64 Data);
u64 XEmbedding_v2_Get_token_ids(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_table_r(XEmbedding_v2 *InstancePtr, u64 Data);
u64 XEmbedding_v2_Get_table_r(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_hidden_out(XEmbedding_v2 *InstancePtr, u64 Data);
u64 XEmbedding_v2_Get_hidden_out(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_run_mode(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_run_mode(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_valid_rows(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_valid_rows(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_hidden_size(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_hidden_size(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_vocab_size(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_vocab_size(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_abi_version(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_abi_version(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_job_id(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_job_id(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_Set_flags(XEmbedding_v2 *InstancePtr, u32 Data);
u32 XEmbedding_v2_Get_flags(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_status_code(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_status_code_vld(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_processed_rows(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_processed_rows_vld(XEmbedding_v2 *InstancePtr);
u64 XEmbedding_v2_Get_result_perf_cycles_total(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_perf_cycles_total_vld(XEmbedding_v2 *InstancePtr);
u64 XEmbedding_v2_Get_result_perf_cycles_compute(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_perf_cycles_compute_vld(XEmbedding_v2 *InstancePtr);
u64 XEmbedding_v2_Get_result_perf_read_beats(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_perf_read_beats_vld(XEmbedding_v2 *InstancePtr);
u64 XEmbedding_v2_Get_result_perf_write_beats(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_perf_write_beats_vld(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_output_crc(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_output_crc_vld(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_build_config(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_Get_result_build_config_vld(XEmbedding_v2 *InstancePtr);

void XEmbedding_v2_InterruptGlobalEnable(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_InterruptGlobalDisable(XEmbedding_v2 *InstancePtr);
void XEmbedding_v2_InterruptEnable(XEmbedding_v2 *InstancePtr, u32 Mask);
void XEmbedding_v2_InterruptDisable(XEmbedding_v2 *InstancePtr, u32 Mask);
void XEmbedding_v2_InterruptClear(XEmbedding_v2 *InstancePtr, u32 Mask);
u32 XEmbedding_v2_InterruptGetEnabled(XEmbedding_v2 *InstancePtr);
u32 XEmbedding_v2_InterruptGetStatus(XEmbedding_v2 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
