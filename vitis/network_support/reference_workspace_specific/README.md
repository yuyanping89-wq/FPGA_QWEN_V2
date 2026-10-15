These scripts are copied from the complete original Vitis workspace. They expect that workspace's generated platform, BSP libraries, ELF, and (for the full rebuild helper) BOOT.BIN packaging inputs. They are retained as references only and are not the package's SD/JTAG workflow.

For a cloned release, use the portable apply_phy_patch.py with the generated BSP root, rebuild/export the BSP, and relink the application. The tested board workflow loads the bitstream and ELF through Vitis/JTAG and keeps only model weight files on SD.
