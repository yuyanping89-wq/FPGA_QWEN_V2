// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xrope_v2_hls.h"

extern XRope_v2_hls_Config XRope_v2_hls_ConfigTable[];

XRope_v2_hls_Config *XRope_v2_hls_LookupConfig(u16 DeviceId) {
	XRope_v2_hls_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XROPE_V2_HLS_NUM_INSTANCES; Index++) {
		if (XRope_v2_hls_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XRope_v2_hls_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XRope_v2_hls_Initialize(XRope_v2_hls *InstancePtr, u16 DeviceId) {
	XRope_v2_hls_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XRope_v2_hls_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XRope_v2_hls_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

