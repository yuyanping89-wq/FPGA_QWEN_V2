# PS application sources

This directory contains the C application sources and headers for the board-tested Qwen V2 inference flow. The application reads the named weight files from FATFS, stages weights into PL DDR with CDMA, runs prefill/decode operators through AXI-Lite, and serves the host client over lwIP TCP port 5000.

Build it in a Vitis A53 standalone application generated from vivado/proj/qwen/qwen.xsa. The Xilinx BSP is intentionally not checked in. See docs/VITIS_TESTING.md for BSP settings, the Marvell patch, JTAG loading, SD contents, and the host test commands.
