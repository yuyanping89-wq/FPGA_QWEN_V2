# M05 两种架构的上板性能比较

## 测量方法

比较顺序循环与 DATAFLOW 两种架构，T=1、2、4、8、16、32、64，全部使用 prefill。
每组预热 5 次，随后测量 100 次；每次验证输出、未写区域、保护区、输入未修改、状态及计数拍数。
每版共 735 次任务，其中 700 次进入性能统计。组内数据存入数组，完成该组后再打印串口。
使用相同的合成词表行、非排序和重复 token 模式；这是查表数据通路比较，不是模型推理精度测试。

## 文件与地址

- `m05_benchmark.c`：替换新 Hello World 应用中的 `helloworld.c` 内容。
- `xembedding_v2_hw.h`：复制到应用 src。
- `lscript.ld`：替换新应用 src 的链接脚本。必须同时使用，代码会检查地址。

链接脚本将应用、栈、堆限制在低地址 16MiB 内；三个独立 NOLOAD 区域固定缓冲区，排除相互覆盖：

|对象|地址|保留字节数|
|---|---|---|
|完整逻辑词表|0x10000000|272269312|
|token_ids|0x20400000|256|
|输出存储（含保护区）|0x20400100|114816|
|传给 M05 的 hidden_out|0x20400140|最多114688|

仅初始化所选 8 行，未选词表行不参与访问。NOLOAD 区不存储完整词表的 ELF 文件载荷，也不属于普通 `.bss` 清零区域。

## 用户执行步骤：先测已有 DATAFLOW 板级版本

1. 保留原 `m05_board_test`。Vitis 创建新 Application Project `m05_benchmark`，使用已更新到 v22 的 `m05_platform`，A53_0 / standalone / Hello World。
2. 将以上三个文件放入应用 src：C 文件的内容替换 `helloworld.c`，保留 platform.c/platform.h；不要同时编译两个含 main 的 C 文件。
3. Build Project；将完整输出提供给协作者。协作者只准备文件，没有执行编译或板测。
4. 编译完成后，创建这个新应用对应的运行配置。仍使用已验证 v22 bitstream、FSBL 初始化和 JTAG，串口 COM9/115200。
5. 保存整个串口输出为 `dataflow.log`。成功结尾必须是 `ALL 7 BENCHMARK GROUPS PASSED; 700 measured jobs; table unchanged.`
6. 顺序版还需要 HLS 验证、适配相同计数器、独立封装及生成独立板级 bitstream。完成后用相同 benchmark 源文件及链接脚本运行，保存 `sequential.log`。当前不能把 DATAFLOW bitstream 当成顺序版运行。

当前硬件 build=0x20010011；顺序版带计数器的计划 build=0x40010011。
程序允许这两个 build，且一次运行中不能变化。后者尚未构建/验证，预留标识不代表顺序硬件已完成。

## 输出与结论

`RAW,build,T,iteration,total_cycles,active_cycles,read_beats,write_beats` 保存全部原始样本。
`SUMMARY,build,T,repeats,min_cycles,sum_cycles,max_cycles` 保存汇总，平均周期为 sum_cycles / repeats。
读拍数应为 T×113（ID 加词表），写拍数应为 T×112。

最终表格按每个 T 列出顺序版平均周期、DATAFLOW 平均周期、加速比、周期减少百分比：

- 加速比 = 顺序平均周期 / DATAFLOW 平均周期。
- 周期减少百分比 = (1 - DATAFLOW平均周期 / 顺序平均周期) × 100%。
- 同为名义100MHz时，平均耗时 µs = 平均周期 / 100。

total_cycles 为同口径硬件任务计数，排除 PS 准备数据、缓存维护、寄存器配置和串口打印时间。
active_cycles 是存在数据传输握手的周期数，不是计算利用率；主要速度指标使用 total_cycles。
两版保持相同 PS/DDR/HP0/SmartConnect/100MHz 目标/AXI 配置/复位连接，以及相同计数器实现。
DDR 调度可能使结果波动；保留 min/max 和全部样本，建议最终再重复整轮对照。
资源另比较 M05 核（含相同计数器）的 LUT、FF、BRAM、DSP，不把共享 PS/互连当成核的资源。
