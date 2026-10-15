# M05 Token Embedding V2 优化版交付包

日期2026-10-10。主交付：三级DATAFLOW、双FIFO、RTL实际性能计数器；ABI=2，build=0x20010011，IP VLNV=llm-fpga.local:user:m05_embedding_v2:2.1。
可提交集成联调，单模块已有真实AXU9EGB上板成功证据；尚未满足全部项目统一验收项，见KNOWN_ISSUES.md。

## 集成同学先看

1. `ip_repo/m05_embedding_v2_2_1/`整个目录是可添加到Vivado的IP；component.xml描述IP，hdl/xgui/其他相对文件必须一起保留，不能只交XML或Verilog单文件。
2. Vivado 2021.1在Settings → IP → Repository添加本包ip_repo，更新Catalog，实例化`M05 Token Embedding V2 with Performance Counters`。
3. 控制口经PS AXI主口连接；3个数据主口经SmartConnect接PS HP0 DDR；时钟100MHz，ap_rst_n低有效。板测地址0xA0000000，系统地址由集成分配。
4. 详细参数、容量、模式、缓存、错误和寄存器见interface_contract.json与register_semantics.md；驱动头见drivers。不要把HLS核心直接当作带计数器的最终IP。
5. PS准备完整逻辑词表、IDs和输出缓冲区，写参数/start，完成并status=0后将输出ACT_TH_V2描述符交M06。M05不分配DDR。

## 文件目录

- src/include/tb：主优化源码、头、测试；计数器及包装顶层源亦在src。
- V2：保持相对路径的优化版源码和分步操作脚本镜像。早期专题文档保留历史；当前状态以本README/顶层manifest/证据为准。
- reference/golden：独立bit-copy参考脚本和确定性小样本（不含真实Qwen权重）。
- ip_repo/drivers：完整优化版IP与寄存器驱动。
- reports/hls、implementation、packaging、numeric：报告、证据、数值范围。
- reports/board_test_v22_20261010：11项板测完整证据。
- reports/performance_comparison：1/2/4/8/16/32/64各100次结果。只保留顺序版结果，未交付顺序版源码、IP或工程。
- board_test：优化版XSA与bit，仅用于AXU9EGB单模块测试，不是整个课题组的最终系统bitstream。
- specifications：作为核对依据的两份项目文档。
- SHA256SUMS：发布文件哈希清单；ZIP的哈希另附于压缩包旁。

## 已验证结果

核心38项C和12项RTL通过；计数器8项单元和13项集成通过；包装2.1在AXU9EGB的11项板测通过。
优化版700次正式benchmark通过，顺序版700次结果仅用于对比：T=64时1.590×加速，周期减少37.12%；T=1时略慢。
优化板级实现WNS+3.726ns、WHS+0.010ns、DRC0；报告为相应板级环境，不保证完整系统同样时序/吞吐。
软件测试使用合成词表选定行。完整真实模型、系统争用DDR、M02身份探测、统一覆盖/数值统计等仍待集成或补验。
本次只整理、生成参考辅助文件和压缩包，没有重新执行FPGA编译、仿真、综合、实现或板测。
