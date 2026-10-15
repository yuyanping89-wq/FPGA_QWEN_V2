// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XATTENTION_V2_HLS_H
#define XATTENTION_V2_HLS_H

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
#include "xattention_v2_hls_hw.h"

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
} XAttention_v2_hls_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XAttention_v2_hls;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XAttention_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XAttention_v2_hls_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XAttention_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XAttention_v2_hls_ReadReg(BaseAddress, RegOffset) \
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
int XAttention_v2_hls_Initialize(XAttention_v2_hls *InstancePtr, u16 DeviceId);
XAttention_v2_hls_Config* XAttention_v2_hls_LookupConfig(u16 DeviceId);
int XAttention_v2_hls_CfgInitialize(XAttention_v2_hls *InstancePtr, XAttention_v2_hls_Config *ConfigPtr);
#else
int XAttention_v2_hls_Initialize(XAttention_v2_hls *InstancePtr, const char* InstanceName);
int XAttention_v2_hls_Release(XAttention_v2_hls *InstancePtr);
#endif

void XAttention_v2_hls_Start(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_IsDone(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_IsIdle(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_IsReady(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_EnableAutoRestart(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_DisableAutoRestart(XAttention_v2_hls *InstancePtr);

void XAttention_v2_hls_Set_q_rot(XAttention_v2_hls *InstancePtr, u64 Data);
u64 XAttention_v2_hls_Get_q_rot(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_k_cache(XAttention_v2_hls *InstancePtr, u64 Data);
u64 XAttention_v2_hls_Get_k_cache(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_v_cache(XAttention_v2_hls *InstancePtr, u64 Data);
u64 XAttention_v2_hls_Get_v_cache(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_attn_out(XAttention_v2_hls *InstancePtr, u64 Data);
u64 XAttention_v2_hls_Get_attn_out(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_run_mode(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_run_mode(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_layer_id(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_layer_id(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_query_pos_base(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_query_pos_base(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_valid_rows(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_valid_rows(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_cache_len(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_cache_len(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_scale(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_scale(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_causal(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_causal(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_abi_version(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_abi_version(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_job_id(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_job_id(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_Set_flags(XAttention_v2_hls *InstancePtr, u32 Data);
u32 XAttention_v2_hls_Get_flags(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_status_code(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_status_code_vld(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_processed_rows(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_processed_rows_vld(XAttention_v2_hls *InstancePtr);
u64 XAttention_v2_hls_Get_result_perf_cycles_total(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_perf_cycles_total_vld(XAttention_v2_hls *InstancePtr);
u64 XAttention_v2_hls_Get_result_perf_cycles_compute(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_perf_cycles_compute_vld(XAttention_v2_hls *InstancePtr);
u64 XAttention_v2_hls_Get_result_perf_read_beats(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_perf_read_beats_vld(XAttention_v2_hls *InstancePtr);
u64 XAttention_v2_hls_Get_result_perf_write_beats(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_perf_write_beats_vld(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_output_crc(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_output_crc_vld(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_build_config(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_Get_result_build_config_vld(XAttention_v2_hls *InstancePtr);

void XAttention_v2_hls_InterruptGlobalEnable(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_InterruptGlobalDisable(XAttention_v2_hls *InstancePtr);
void XAttention_v2_hls_InterruptEnable(XAttention_v2_hls *InstancePtr, u32 Mask);
void XAttention_v2_hls_InterruptDisable(XAttention_v2_hls *InstancePtr, u32 Mask);
void XAttention_v2_hls_InterruptClear(XAttention_v2_hls *InstancePtr, u32 Mask);
u32 XAttention_v2_hls_InterruptGetEnabled(XAttention_v2_hls *InstancePtr);
u32 XAttention_v2_hls_InterruptGetStatus(XAttention_v2_hls *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
