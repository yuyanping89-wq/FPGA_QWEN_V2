# 用户手动验证入口

HLS C/综合/核心RTL：Vitis HLS 2021.1执行build_hls.tcl，-tclargs分别为csim/synth/cosim。后加RTL计数器不属于HLS C/RTL验证对象。
计数器重现：在V2/scripts/prepare_perf_rtl.py上指定--solution V2/hls/reproduce_synth/solution1和新的--output（必须位于V2）。
然后在Vivado使用V2/scripts/open_perf_project.tcl并指定m05_perf_dir，再source add_perf_tests.tcl，用户手动Run Behavioral Simulation。
IP消费无需再封装：直接添加ip_repo到Vivado IP Repository。包中已含用户生成的component.xml及全部相对路径文件。
板级重现：将主IP目录复制到V2/ip_repo下，V2/scripts/create_m05_board.tcl依赖V2/board/alinx_axu9eg_reference/ps_config.tcl；脚本只建工程，综合/实现/bitstream由用户手动执行。
单模块板测：board_test内XSA/bit和V2/board/m05_board_test代码；benchmark用V2/board/m05_benchmark全部文件及固定链接脚本；具体UART/JTAG/cache步骤见V2/BOARD_BRINGUP.md。
这些交付新增入口没有在本次整理中执行；需要用户重现验证。不要直接执行历史prepare_ip_package/prepare_ip_v21依赖脚本，这两者未纳入发布入口。
