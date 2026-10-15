// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XRESIDUAL_ADD_V2_HLS_H
#define XRESIDUAL_ADD_V2_HLS_H

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
#include "xresidual_add_v2_hls_hw.h"

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
} XResidual_add_v2_hls_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XResidual_add_v2_hls;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XResidual_add_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XResidual_add_v2_hls_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XResidual_add_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XResidual_add_v2_hls_ReadReg(BaseAddress, RegOffset) \
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
int XResidual_add_v2_hls_Initialize(XResidual_add_v2_hls *InstancePtr, u16 DeviceId);
XResidual_add_v2_hls_Config* XResidual_add_v2_hls_LookupConfig(u16 DeviceId);
int XResidual_add_v2_hls_CfgInitialize(XResidual_add_v2_hls *InstancePtr, XResidual_add_v2_hls_Config *ConfigPtr);
#else
int XResidual_add_v2_hls_Initialize(XResidual_add_v2_hls *InstancePtr, const char* InstanceName);
int XResidual_add_v2_hls_Release(XResidual_add_v2_hls *InstancePtr);
#endif

void XResidual_add_v2_hls_Start(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_IsDone(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_IsIdle(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_IsReady(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_EnableAutoRestart(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_DisableAutoRestart(XResidual_add_v2_hls *InstancePtr);

void XResidual_add_v2_hls_Set_a(XResidual_add_v2_hls *InstancePtr, u64 Data);
u64 XResidual_add_v2_hls_Get_a(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_b(XResidual_add_v2_hls *InstancePtr, u64 Data);
u64 XResidual_add_v2_hls_Get_b(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_y(XResidual_add_v2_hls *InstancePtr, u64 Data);
u64 XResidual_add_v2_hls_Get_y(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_run_mode(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_run_mode(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_valid_rows(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_valid_rows(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_hidden_size(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_hidden_size(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_residual_role(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_residual_role(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_abi_version(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_abi_version(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_job_id(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_job_id(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_Set_flags(XResidual_add_v2_hls *InstancePtr, u32 Data);
u32 XResidual_add_v2_hls_Get_flags(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_status_code(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_status_code_vld(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_processed_rows(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_processed_rows_vld(XResidual_add_v2_hls *InstancePtr);
u64 XResidual_add_v2_hls_Get_result_perf_cycles_total(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_perf_cycles_total_vld(XResidual_add_v2_hls *InstancePtr);
u64 XResidual_add_v2_hls_Get_result_perf_cycles_compute(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_perf_cycles_compute_vld(XResidual_add_v2_hls *InstancePtr);
u64 XResidual_add_v2_hls_Get_result_perf_read_beats(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_perf_read_beats_vld(XResidual_add_v2_hls *InstancePtr);
u64 XResidual_add_v2_hls_Get_result_perf_write_beats(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_perf_write_beats_vld(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_output_crc(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_output_crc_vld(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_build_config(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_Get_result_build_config_vld(XResidual_add_v2_hls *InstancePtr);

void XResidual_add_v2_hls_InterruptGlobalEnable(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_InterruptGlobalDisable(XResidual_add_v2_hls *InstancePtr);
void XResidual_add_v2_hls_InterruptEnable(XResidual_add_v2_hls *InstancePtr, u32 Mask);
void XResidual_add_v2_hls_InterruptDisable(XResidual_add_v2_hls *InstancePtr, u32 Mask);
void XResidual_add_v2_hls_InterruptClear(XResidual_add_v2_hls *InstancePtr, u32 Mask);
u32 XResidual_add_v2_hls_InterruptGetEnabled(XResidual_add_v2_hls *InstancePtr);
u32 XResidual_add_v2_hls_InterruptGetStatus(XResidual_add_v2_hls *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
