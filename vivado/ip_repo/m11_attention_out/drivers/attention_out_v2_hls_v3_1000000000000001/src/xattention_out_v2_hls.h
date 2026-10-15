// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XATTENTION_OUT_V2_HLS_H
#define XATTENTION_OUT_V2_HLS_H

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
#include "xattention_out_v2_hls_hw.h"

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
} XAttention_out_v2_hls_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XAttention_out_v2_hls;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XAttention_out_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XAttention_out_v2_hls_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XAttention_out_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XAttention_out_v2_hls_ReadReg(BaseAddress, RegOffset) \
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
int XAttention_out_v2_hls_Initialize(XAttention_out_v2_hls *InstancePtr, u16 DeviceId);
XAttention_out_v2_hls_Config* XAttention_out_v2_hls_LookupConfig(u16 DeviceId);
int XAttention_out_v2_hls_CfgInitialize(XAttention_out_v2_hls *InstancePtr, XAttention_out_v2_hls_Config *ConfigPtr);
#else
int XAttention_out_v2_hls_Initialize(XAttention_out_v2_hls *InstancePtr, const char* InstanceName);
int XAttention_out_v2_hls_Release(XAttention_out_v2_hls *InstancePtr);
#endif

void XAttention_out_v2_hls_Start(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_IsDone(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_IsIdle(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_IsReady(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_EnableAutoRestart(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_DisableAutoRestart(XAttention_out_v2_hls *InstancePtr);

void XAttention_out_v2_hls_Set_x(XAttention_out_v2_hls *InstancePtr, u64 Data);
u64 XAttention_out_v2_hls_Get_x(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_weight(XAttention_out_v2_hls *InstancePtr, u64 Data);
u64 XAttention_out_v2_hls_Get_weight(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_y(XAttention_out_v2_hls *InstancePtr, u64 Data);
u64 XAttention_out_v2_hls_Get_y(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_run_mode(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_run_mode(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_valid_rows(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_valid_rows(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_hidden_size(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_hidden_size(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_layout_id(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_layout_id(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_abi_version(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_abi_version(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_job_id(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_job_id(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_Set_flags(XAttention_out_v2_hls *InstancePtr, u32 Data);
u32 XAttention_out_v2_hls_Get_flags(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_status_code(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_status_code_vld(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_processed_rows(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_processed_rows_vld(XAttention_out_v2_hls *InstancePtr);
u64 XAttention_out_v2_hls_Get_result_perf_cycles_total(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_perf_cycles_total_vld(XAttention_out_v2_hls *InstancePtr);
u64 XAttention_out_v2_hls_Get_result_perf_cycles_compute(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_perf_cycles_compute_vld(XAttention_out_v2_hls *InstancePtr);
u64 XAttention_out_v2_hls_Get_result_perf_read_beats(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_perf_read_beats_vld(XAttention_out_v2_hls *InstancePtr);
u64 XAttention_out_v2_hls_Get_result_perf_write_beats(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_perf_write_beats_vld(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_output_crc(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_output_crc_vld(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_build_config(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_Get_result_build_config_vld(XAttention_out_v2_hls *InstancePtr);

void XAttention_out_v2_hls_InterruptGlobalEnable(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_InterruptGlobalDisable(XAttention_out_v2_hls *InstancePtr);
void XAttention_out_v2_hls_InterruptEnable(XAttention_out_v2_hls *InstancePtr, u32 Mask);
void XAttention_out_v2_hls_InterruptDisable(XAttention_out_v2_hls *InstancePtr, u32 Mask);
void XAttention_out_v2_hls_InterruptClear(XAttention_out_v2_hls *InstancePtr, u32 Mask);
u32 XAttention_out_v2_hls_InterruptGetEnabled(XAttention_out_v2_hls *InstancePtr);
u32 XAttention_out_v2_hls_InterruptGetStatus(XAttention_out_v2_hls *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
