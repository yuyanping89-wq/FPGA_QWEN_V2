// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XSWIGLU_V2_H
#define XSWIGLU_V2_H

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
#include "xswiglu_v2_hw.h"

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
} XSwiglu_v2_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XSwiglu_v2;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XSwiglu_v2_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XSwiglu_v2_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XSwiglu_v2_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XSwiglu_v2_ReadReg(BaseAddress, RegOffset) \
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
int XSwiglu_v2_Initialize(XSwiglu_v2 *InstancePtr, u16 DeviceId);
XSwiglu_v2_Config* XSwiglu_v2_LookupConfig(u16 DeviceId);
int XSwiglu_v2_CfgInitialize(XSwiglu_v2 *InstancePtr, XSwiglu_v2_Config *ConfigPtr);
#else
int XSwiglu_v2_Initialize(XSwiglu_v2 *InstancePtr, const char* InstanceName);
int XSwiglu_v2_Release(XSwiglu_v2 *InstancePtr);
#endif

void XSwiglu_v2_Start(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_IsDone(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_IsIdle(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_IsReady(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_EnableAutoRestart(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_DisableAutoRestart(XSwiglu_v2 *InstancePtr);

void XSwiglu_v2_Set_x_offset(XSwiglu_v2 *InstancePtr, u64 Data);
u64 XSwiglu_v2_Get_x_offset(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_w_gate_offset(XSwiglu_v2 *InstancePtr, u64 Data);
u64 XSwiglu_v2_Get_w_gate_offset(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_w_up_offset(XSwiglu_v2 *InstancePtr, u64 Data);
u64 XSwiglu_v2_Get_w_up_offset(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_w_down_offset(XSwiglu_v2 *InstancePtr, u64 Data);
u64 XSwiglu_v2_Get_w_down_offset(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_y_offset(XSwiglu_v2 *InstancePtr, u64 Data);
u64 XSwiglu_v2_Get_y_offset(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_run_mode(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_run_mode(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_valid_rows(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_valid_rows(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_hidden_size(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_hidden_size(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_intermediate_size(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_intermediate_size(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_layout_id(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_layout_id(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_abi_version(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_abi_version(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_job_id(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_job_id(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_Set_flags(XSwiglu_v2 *InstancePtr, u32 Data);
u32 XSwiglu_v2_Get_flags(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_status_code(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_status_code_vld(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_processed_rows(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_processed_rows_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_perf_cycles_total(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_perf_cycles_total_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_perf_cycles_compute(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_perf_cycles_compute_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_perf_read_beats(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_perf_read_beats_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_perf_write_beats(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_perf_write_beats_vld(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_output_crc(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_output_crc_vld(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_build_config(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_build_config_vld(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_mid_tiles(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_mid_tiles_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_gate_up_active_cycles(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_gate_up_active_cycles_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_down_active_cycles(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_down_active_cycles_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_weight_wait_cycles(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_weight_wait_cycles_vld(XSwiglu_v2 *InstancePtr);
u64 XSwiglu_v2_Get_result_fifo_stall_cycles(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_Get_result_fifo_stall_cycles_vld(XSwiglu_v2 *InstancePtr);

void XSwiglu_v2_InterruptGlobalEnable(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_InterruptGlobalDisable(XSwiglu_v2 *InstancePtr);
void XSwiglu_v2_InterruptEnable(XSwiglu_v2 *InstancePtr, u32 Mask);
void XSwiglu_v2_InterruptDisable(XSwiglu_v2 *InstancePtr, u32 Mask);
void XSwiglu_v2_InterruptClear(XSwiglu_v2 *InstancePtr, u32 Mask);
u32 XSwiglu_v2_InterruptGetEnabled(XSwiglu_v2 *InstancePtr);
u32 XSwiglu_v2_InterruptGetStatus(XSwiglu_v2 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
