// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XPSPL_V2_M06_RMSNORM_V2_H
#define XPSPL_V2_M06_RMSNORM_V2_H

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
#include "xpspl_v2_m06_rmsnorm_v2_hw.h"

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
} XPspl_v2_m06_rmsnorm_v2_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XPspl_v2_m06_rmsnorm_v2;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XPspl_v2_m06_rmsnorm_v2_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XPspl_v2_m06_rmsnorm_v2_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XPspl_v2_m06_rmsnorm_v2_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XPspl_v2_m06_rmsnorm_v2_ReadReg(BaseAddress, RegOffset) \
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
int XPspl_v2_m06_rmsnorm_v2_Initialize(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u16 DeviceId);
XPspl_v2_m06_rmsnorm_v2_Config* XPspl_v2_m06_rmsnorm_v2_LookupConfig(u16 DeviceId);
int XPspl_v2_m06_rmsnorm_v2_CfgInitialize(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, XPspl_v2_m06_rmsnorm_v2_Config *ConfigPtr);
#else
int XPspl_v2_m06_rmsnorm_v2_Initialize(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, const char* InstanceName);
int XPspl_v2_m06_rmsnorm_v2_Release(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
#endif

void XPspl_v2_m06_rmsnorm_v2_Start(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_IsDone(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_IsIdle(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_IsReady(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_EnableAutoRestart(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_DisableAutoRestart(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);

void XPspl_v2_m06_rmsnorm_v2_Set_x(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u64 Data);
u64 XPspl_v2_m06_rmsnorm_v2_Get_x(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_weight(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u64 Data);
u64 XPspl_v2_m06_rmsnorm_v2_Get_weight(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_y(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u64 Data);
u64 XPspl_v2_m06_rmsnorm_v2_Get_y(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_run_mode(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_run_mode(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_valid_rows(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_valid_rows(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_hidden_size(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_hidden_size(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_eps(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_eps(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_norm_role(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_norm_role(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_abi_version(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_abi_version(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_job_id(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_job_id(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_Set_flags(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Data);
u32 XPspl_v2_m06_rmsnorm_v2_Get_flags(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_status_code(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_status_code_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_processed_rows(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_processed_rows_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_total(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_total_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_compute(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_cycles_compute_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_read_beats(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_read_beats_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u64 XPspl_v2_m06_rmsnorm_v2_Get_perf_write_beats(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_perf_write_beats_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_output_crc(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_output_crc_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_build_config(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_Get_build_config_vld(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);

void XPspl_v2_m06_rmsnorm_v2_InterruptGlobalEnable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_InterruptGlobalDisable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
void XPspl_v2_m06_rmsnorm_v2_InterruptEnable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Mask);
void XPspl_v2_m06_rmsnorm_v2_InterruptDisable(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Mask);
void XPspl_v2_m06_rmsnorm_v2_InterruptClear(XPspl_v2_m06_rmsnorm_v2 *InstancePtr, u32 Mask);
u32 XPspl_v2_m06_rmsnorm_v2_InterruptGetEnabled(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);
u32 XPspl_v2_m06_rmsnorm_v2_InterruptGetStatus(XPspl_v2_m06_rmsnorm_v2 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
