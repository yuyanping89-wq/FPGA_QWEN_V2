// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xpspl_v2_m09_kv_cache_update_v2.h"

extern XPspl_v2_m09_kv_cache_update_v2_Config XPspl_v2_m09_kv_cache_update_v2_ConfigTable[];

XPspl_v2_m09_kv_cache_update_v2_Config *XPspl_v2_m09_kv_cache_update_v2_LookupConfig(u16 DeviceId) {
	XPspl_v2_m09_kv_cache_update_v2_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XPSPL_V2_M09_KV_CACHE_UPDATE_V2_NUM_INSTANCES; Index++) {
		if (XPspl_v2_m09_kv_cache_update_v2_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XPspl_v2_m09_kv_cache_update_v2_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XPspl_v2_m09_kv_cache_update_v2_Initialize(XPspl_v2_m09_kv_cache_update_v2 *InstancePtr, u16 DeviceId) {
	XPspl_v2_m09_kv_cache_update_v2_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XPspl_v2_m09_kv_cache_update_v2_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XPspl_v2_m09_kv_cache_update_v2_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

