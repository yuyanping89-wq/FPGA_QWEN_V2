// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xembedding_v2.h"

extern XEmbedding_v2_Config XEmbedding_v2_ConfigTable[];

XEmbedding_v2_Config *XEmbedding_v2_LookupConfig(u16 DeviceId) {
	XEmbedding_v2_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XEMBEDDING_V2_NUM_INSTANCES; Index++) {
		if (XEmbedding_v2_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XEmbedding_v2_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XEmbedding_v2_Initialize(XEmbedding_v2 *InstancePtr, u16 DeviceId) {
	XEmbedding_v2_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XEmbedding_v2_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XEmbedding_v2_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

