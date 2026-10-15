// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XROPE_V2_HLS_H
#define XROPE_V2_HLS_H

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
#include "xrope_v2_hls_hw.h"

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
} XRope_v2_hls_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XRope_v2_hls;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XRope_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XRope_v2_hls_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XRope_v2_hls_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XRope_v2_hls_ReadReg(BaseAddress, RegOffset) \
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
int XRope_v2_hls_Initialize(XRope_v2_hls *InstancePtr, u16 DeviceId);
XRope_v2_hls_Config* XRope_v2_hls_LookupConfig(u16 DeviceId);
int XRope_v2_hls_CfgInitialize(XRope_v2_hls *InstancePtr, XRope_v2_hls_Config *ConfigPtr);
#else
int XRope_v2_hls_Initialize(XRope_v2_hls *InstancePtr, const char* InstanceName);
int XRope_v2_hls_Release(XRope_v2_hls *InstancePtr);
#endif

void XRope_v2_hls_Start(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_IsDone(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_IsIdle(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_IsReady(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_EnableAutoRestart(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_DisableAutoRestart(XRope_v2_hls *InstancePtr);

void XRope_v2_hls_Set_q_raw(XRope_v2_hls *InstancePtr, u64 Data);
u64 XRope_v2_hls_Get_q_raw(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_k_raw(XRope_v2_hls *InstancePtr, u64 Data);
u64 XRope_v2_hls_Get_k_raw(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_cos_table(XRope_v2_hls *InstancePtr, u64 Data);
u64 XRope_v2_hls_Get_cos_table(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_sin_table(XRope_v2_hls *InstancePtr, u64 Data);
u64 XRope_v2_hls_Get_sin_table(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_q_rot(XRope_v2_hls *InstancePtr, u64 Data);
u64 XRope_v2_hls_Get_q_rot(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_k_rot(XRope_v2_hls *InstancePtr, u64 Data);
u64 XRope_v2_hls_Get_k_rot(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_run_mode(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_run_mode(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_valid_rows(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_valid_rows(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_position_base(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_position_base(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_max_position(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_max_position(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_abi_version(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_abi_version(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_job_id(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_job_id(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_Set_flags(XRope_v2_hls *InstancePtr, u32 Data);
u32 XRope_v2_hls_Get_flags(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_status_code(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_status_code_vld(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_processed_rows(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_processed_rows_vld(XRope_v2_hls *InstancePtr);
u64 XRope_v2_hls_Get_result_perf_cycles_total(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_perf_cycles_total_vld(XRope_v2_hls *InstancePtr);
u64 XRope_v2_hls_Get_result_perf_cycles_compute(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_perf_cycles_compute_vld(XRope_v2_hls *InstancePtr);
u64 XRope_v2_hls_Get_result_perf_read_beats(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_perf_read_beats_vld(XRope_v2_hls *InstancePtr);
u64 XRope_v2_hls_Get_result_perf_write_beats(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_perf_write_beats_vld(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_output_crc(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_output_crc_vld(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_build_config(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_Get_result_build_config_vld(XRope_v2_hls *InstancePtr);

void XRope_v2_hls_InterruptGlobalEnable(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_InterruptGlobalDisable(XRope_v2_hls *InstancePtr);
void XRope_v2_hls_InterruptEnable(XRope_v2_hls *InstancePtr, u32 Mask);
void XRope_v2_hls_InterruptDisable(XRope_v2_hls *InstancePtr, u32 Mask);
void XRope_v2_hls_InterruptClear(XRope_v2_hls *InstancePtr, u32 Mask);
u32 XRope_v2_hls_InterruptGetEnabled(XRope_v2_hls *InstancePtr);
u32 XRope_v2_hls_InterruptGetStatus(XRope_v2_hls *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
