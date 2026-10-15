# PHY support files

The active PHY source overlay is in lwip211_v1_5. Use apply_phy_patch.py with a generated lwIP 2.1.1 BSP before rebuilding and relinking the Vitis application. The patch configures the Marvell PHY path used by this board and exposes qwen_phy_debug for JTAG inspection.

The original full-workspace helpers and BSP-dependent compile test are under reference_workspace_specific. They assume generated projects and headers that are not included in this compact release. The active board workflow loads the application and bitstream through Vitis/JTAG; BOOT.BIN is not part of the SD card contents.
