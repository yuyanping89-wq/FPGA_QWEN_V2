# Vitis application and board test

## Platform setup

Use the hardware handoff vivado/proj/qwen/qwen.xsa with Vitis 2021.1. Create an A53 64-bit standalone Empty Application and enable lwIP 2.1.1 and xilffs in the BSP. Import the C sources under vitis/qwen_app/src and headers under vitis/qwen_app/include into the application. Keep the generated BSP and libraries local to the Vitis workspace; this package does not include Xilinx-generated BSP archives.

Use FILE_SYSTEM_USE_LFN=2 for the long weight filenames. The tested lwIP settings were MEM_SIZE=524288, MEMP_NUM_TCP_SEG=512, PBUF_POOL_SIZE=512, TCP_SND_BUF=32768, and TCP_WND=32768. The tested static address is 192.168.8.190 with gateway 192.168.8.1, on PS GEM3. TCP inference uses port 5000.

The PHY patch is in vitis/network_support/lwip211_v1_5. After generating the BSP, apply it to that BSP's lwIP port directory with:

    python vitis/network_support/apply_phy_patch.py --bsp-root PATH_TO_GENERATED_BSP

The BSP root is the generated bsp directory containing psu_cortexa53_0. Rebuild the BSP and relink the application after installing the patch. The patch adds the Marvell PHY handling and the qwen_phy_debug status structure; it does not change the operator scheduler or TCP protocol.

The prebuilt file vitis/prebuilt/qwen_app_board_tested.elf is included for the packaged XSA/bitstream. It can be loaded with Vitis/JTAG for a quick reproduction. Use the C source and rebuild when modifying the app or BSP. The JTAG test does not require BOOT.BIN on SD.

## SD card and host setup

Copy the 196 model weight .bin files to the SD model directory expected by the application. Do not copy BOOT.BIN or the host manifest for this workflow. Keep manifest.json on the PC and use vitis/host_tools/verify_model_assets.py to check the SD directory against it before testing. This release intentionally contains no model weights.

Prepare the PC tools with Python 3.11 and the packages listed in vitis/host_tools/requirements-client.txt. Activate the Conda environment used for board testing, then run the host client from the release root. Replace MODEL_DIR with the tokenizer/model directory on the PC:

    python vitis/host_tools/qwen_client.py --host 192.168.8.190 --tokenizer MODEL_DIR --prompt "你好" --max-new-tokens 1 --out RESULTS_DIR

For a full answer and profile record:

    python vitis/host_tools/qwen_client.py --host 192.168.8.190 --tokenizer MODEL_DIR --prompt "你好，请用一句话介绍自己。" --max-new-tokens 32 --out RESULTS_DIR

The client prints the answer, status, prefill/decode time, per-module time, AXI read/write amounts, cache operations, and CDMA measurements. A successful short run should report Status: OK. Results from the current board are documented in docs/BOARD_TEST_RESULTS_20261015.md.

## Test and rebuild tools

Run python tools/verify_release.py to verify the release manifest and the critical bitstream/XSA hashes. The release integrity check is tools/verify_release.py; the portable PHY installer can be smoke-tested with tools/test_phy_patch_installer.py; host model assets are checked with vitis/host_tools/verify_model_assets.py. The original workspace-only compile/test utilities are preserved under vitis/host_tools/reference_workspace_specific. Full Vivado rebuilding is available from vivado/proj/qwen/build_qwen.tcl; it resets and launches synthesis and implementation, so inspect the copied project and reports before choosing to run the long build.

A fresh Vivado/Vitis build is a new artifact and must be rechecked on hardware. The packaged board-test results apply to the exact file hashes recorded in the manifest, not to later rebuilds.


