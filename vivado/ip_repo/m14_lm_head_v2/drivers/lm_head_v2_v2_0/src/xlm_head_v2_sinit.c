// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xlm_head_v2.h"

extern XLm_head_v2_Config XLm_head_v2_ConfigTable[];

XLm_head_v2_Config *XLm_head_v2_LookupConfig(u16 DeviceId) {
	XLm_head_v2_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XLM_HEAD_V2_NUM_INSTANCES; Index++) {
		if (XLm_head_v2_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XLm_head_v2_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XLm_head_v2_Initialize(XLm_head_v2 *InstancePtr, u16 DeviceId) {
	XLm_head_v2_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XLm_head_v2_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XLm_head_v2_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

