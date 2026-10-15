// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xswiglu_v2.h"

extern XSwiglu_v2_Config XSwiglu_v2_ConfigTable[];

XSwiglu_v2_Config *XSwiglu_v2_LookupConfig(u16 DeviceId) {
	XSwiglu_v2_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XSWIGLU_V2_NUM_INSTANCES; Index++) {
		if (XSwiglu_v2_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XSwiglu_v2_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XSwiglu_v2_Initialize(XSwiglu_v2 *InstancePtr, u16 DeviceId) {
	XSwiglu_v2_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XSwiglu_v2_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XSwiglu_v2_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

