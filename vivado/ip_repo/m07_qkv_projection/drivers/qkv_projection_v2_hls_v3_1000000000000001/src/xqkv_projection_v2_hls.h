// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XQKV_PROJECTION_V2_HLS_H
#define XQKV_PROJECTION_V2_HLS_H

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
#include "xqkv_projection_v2_hls_hw.h"

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
} XQkv_projection_v2_hls_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XQkv_projection_v2_hls;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XQkv_projection_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XQkv_projection_v2_hls_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XQkv_projection_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XQkv_projection_v2_hls_ReadReg(BaseAddress, RegOffset) \
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
int XQkv_projection_v2_hls_Initialize(XQkv_projection_v2_hls *InstancePtr, u16 DeviceId);
XQkv_projection_v2_hls_Config* XQkv_projection_v2_hls_LookupConfig(u16 DeviceId);
int XQkv_projection_v2_hls_CfgInitialize(XQkv_projection_v2_hls *InstancePtr, XQkv_projection_v2_hls_Config *ConfigPtr);
#else
int XQkv_projection_v2_hls_Initialize(XQkv_projection_v2_hls *InstancePtr, const char* InstanceName);
int XQkv_projection_v2_hls_Release(XQkv_projection_v2_hls *InstancePtr);
#endif

void XQkv_projection_v2_hls_Start(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_IsDone(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_IsIdle(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_IsReady(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_EnableAutoRestart(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_DisableAutoRestart(XQkv_projection_v2_hls *InstancePtr);

void XQkv_projection_v2_hls_Set_x(XQkv_projection_v2_hls *InstancePtr, u64 Data);
u64 XQkv_projection_v2_hls_Get_x(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_w_qkv(XQkv_projection_v2_hls *InstancePtr, u64 Data);
u64 XQkv_projection_v2_hls_Get_w_qkv(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_bias_qkv(XQkv_projection_v2_hls *InstancePtr, u64 Data);
u64 XQkv_projection_v2_hls_Get_bias_qkv(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_q(XQkv_projection_v2_hls *InstancePtr, u64 Data);
u64 XQkv_projection_v2_hls_Get_q(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_k(XQkv_projection_v2_hls *InstancePtr, u64 Data);
u64 XQkv_projection_v2_hls_Get_k(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_v(XQkv_projection_v2_hls *InstancePtr, u64 Data);
u64 XQkv_projection_v2_hls_Get_v(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_run_mode(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_run_mode(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_valid_rows(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_valid_rows(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_hidden_size(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_hidden_size(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_layout_id(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_layout_id(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_abi_version(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_abi_version(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_job_id(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_job_id(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_Set_flags(XQkv_projection_v2_hls *InstancePtr, u32 Data);
u32 XQkv_projection_v2_hls_Get_flags(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_status_code(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_status_code_vld(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_processed_rows(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_processed_rows_vld(XQkv_projection_v2_hls *InstancePtr);
u64 XQkv_projection_v2_hls_Get_result_perf_cycles_total(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_perf_cycles_total_vld(XQkv_projection_v2_hls *InstancePtr);
u64 XQkv_projection_v2_hls_Get_result_perf_cycles_compute(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_perf_cycles_compute_vld(XQkv_projection_v2_hls *InstancePtr);
u64 XQkv_projection_v2_hls_Get_result_perf_read_beats(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_perf_read_beats_vld(XQkv_projection_v2_hls *InstancePtr);
u64 XQkv_projection_v2_hls_Get_result_perf_write_beats(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_perf_write_beats_vld(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_output_crc(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_output_crc_vld(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_build_config(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_Get_result_build_config_vld(XQkv_projection_v2_hls *InstancePtr);

void XQkv_projection_v2_hls_InterruptGlobalEnable(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_InterruptGlobalDisable(XQkv_projection_v2_hls *InstancePtr);
void XQkv_projection_v2_hls_InterruptEnable(XQkv_projection_v2_hls *InstancePtr, u32 Mask);
void XQkv_projection_v2_hls_InterruptDisable(XQkv_projection_v2_hls *InstancePtr, u32 Mask);
void XQkv_projection_v2_hls_InterruptClear(XQkv_projection_v2_hls *InstancePtr, u32 Mask);
u32 XQkv_projection_v2_hls_InterruptGetEnabled(XQkv_projection_v2_hls *InstancePtr);
u32 XQkv_projection_v2_hls_InterruptGetStatus(XQkv_projection_v2_hls *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
