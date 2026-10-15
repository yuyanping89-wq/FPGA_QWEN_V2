// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xresidual_add_v2_hls.h"

extern XResidual_add_v2_hls_Config XResidual_add_v2_hls_ConfigTable[];

XResidual_add_v2_hls_Config *XResidual_add_v2_hls_LookupConfig(u16 DeviceId) {
	XResidual_add_v2_hls_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XRESIDUAL_ADD_V2_HLS_NUM_INSTANCES; Index++) {
		if (XResidual_add_v2_hls_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XResidual_add_v2_hls_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XResidual_add_v2_hls_Initialize(XResidual_add_v2_hls *InstancePtr, u16 DeviceId) {
	XResidual_add_v2_hls_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XResidual_add_v2_hls_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XResidual_add_v2_hls_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

