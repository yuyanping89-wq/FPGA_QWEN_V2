# M05 V2 接口配置与首次综合

用户已于 2026-09-29 在 Vitis HLS 2021.1 / Vivado IP 流程完成综合。
下表名称、数据位宽及三个主接口的 64-bit 地址已用生成 RTL 核对。
以上为旧循环版本记录。当前已改为三级 DATAFLOW，见 [DATAFLOW.md](DATAFLOW.md)，
接口指令保持相同，2026-10-08 新版综合后的位宽、地址和控制寄存器数据偏移已重新核对。
三个事务的 RTL 仿真通过；本文保留首次综合操作作为参考。

后续 DATAFLOW 基线已通过 12 项 RTL 回归及用户确认的 38 项 C 回归。
build_config=0x10010011 的核心已由用户确认通过。后续计数器通过独立 RTL 集成，
保持端口及原结果地址，build_config=0x20010011，待专项验证，见 [PERFORMANCE.md](PERFORMANCE.md)。

## 数据接口

| C++ 参数 | bundle | 已生成 RTL 接口 | 数据位宽 | 用途 | depth（元素数） |
| --- | --- | --- | --- | --- | --- |
| `token_ids` | `ids` | `m_axi_ids` | 32 bit | 读取 token 编号 | 64 |
| `table` | `weight` | `m_axi_weight` | 128 bit | 读取 BF16 词表 | 17016832 |
| `hidden_out` | `out` | `m_axi_out_r` | 128 bit | 写入 BF16 hidden | 7168 |

`depth` 描述接口的元素深度，供接口/协同仿真使用，不等于在 PL 内分配这么多存储。
`table` 和 `hidden_out` 每个元素 16 字节；`token_ids` 每个元素 4 字节。
它也不提供自动硬件越界保护，有效行和 token 范围仍由函数检查。

三个 `m_axi` 口均使用 `offset=slave`，因此首地址通过 AXI-Lite 配置。
表读取和输出写入的最大 burst 长度设为 64 拍，ID 读取为 16 拍。
最大长度只是一项限制，不能据此认定已经实现了某种 burst 或每拍一周期。
当前源码已拆分为三级 DATAFLOW 和深度 64 的双 FIFO，实际结构已通过用户的新综合报告确认。

HLS 的 bundle 只决定 IP 内接口的分组，不能直接选择 PS 的 HP0。
集成时，由 Vivado Block Design 中的互连将 `m_axi_weight`、`m_axi_out_r` 接到 HP0，
并将 `m_axi_ids` 接入 PS DDR 通路。控制口由 PS 侧 AXI 主口通过互连接入。
时钟、复位、地址段与数据宽度转换也由系统集成阶段配置。

## AXI-Lite 控制和结果

所有控制指令使用 `bundle=ctrl`，预期生成一个 `s_axi_ctrl` 接口。
这是 HLS 的命名方式，对应项目文档中的 `s_axilite_ctrl` 控制接口。
保持 AXI-Lite 数据宽度为默认 32 bit。

| 字段 | 语义位宽 | 方向/语义 |
| --- | --- | --- |
| `token_ids`、`table`、`hidden_out` | 每个地址 64 bit | PS 写入 DDR 首地址，通常分为低/高 32 bit |
| `run_mode`、`valid_rows`、`hidden_size`、`vocab_size` | 每项 32 bit | PS 写入模式、有效长度和维度 |
| `abi_version`、`job_id`、`flags` | 每项 32 bit | PS 写入公共任务字段 |
| `result.status_code`、`result.processed_rows` | 每项 32 bit | IP 输出状态与已完成行数 |
| `result.perf_cycles_total`、`result.perf_cycles_compute` | 每项 64 bit | HLS 核心为 0；带计数器 RTL 返回总窗口周期及数据搬运活跃周期 |
| `result.perf_read_beats`、`result.perf_write_beats` | 每项 64 bit | HLS 核心为 0；带计数器 RTL 返回实际 AXI 数据握手次数 |
| `result.output_crc`、`result.build_config` | 每项 32 bit | CRC 未启用为 0；核心标识 0x10010011，带计数器 RTL 为 0x20010011，完成后读取 |
| `return` 对应的块控制 | 控制寄存器中的位 | `ap_ctrl_hs` 的启动、完成、空闲、就绪 |

`DISAGGREGATE variable=result` 将结构体拆成字段，每个字段单独指定 `s_axilite`。
PS 无须写入 `result` 地址。默认输出协议可能附带 valid 状态寄存器，由 HLS 生成。
PS 在启动前写好全部参数，运行期间保持参数不变，等待完成后再读取结果。
当前使用普通一次启动/一次完成方式，先保持自动重启关闭。

不手写寄存器偏移：C++ 结构体内存偏移不是寄存器偏移。
综合后检查控制接口 RTL 的地址注释；导出 IP 后使用生成的驱动头文件记录最终偏移、位宽和读写属性。
尤其需要确认未参与数学计算的 `job_id` 仍保留在寄存器表中，以及 8 个结果字段均存在。

## 必须加载的 solution 配置

在已经打开的 `solution1` 中加载 `scripts/config_interfaces.tcl`，其内容为：

```tcl
config_interface -m_axi_addr64
config_interface -m_axi_max_bitwidth 128
config_interface -m_axi_max_widen_bitwidth 0
```

第一条启用 64-bit AXI 地址。项目中的 PS DDR 高地址段起于 `0x800000000`，
32-bit 地址无法覆盖它。数据位宽 128 bit 与地址位宽 64 bit 是独立配置。
HLS IP 使用 64-bit 地址，集成到当前 40-bit PS HP 地址口时，由互连进行适配；
运行时必须确认地址落入已登记区域且高位合法，不能直接截成 32 bit。

后两条将最大数据宽度限制为 128 bit，并关闭自动扩宽，使 ID 保持 32 bit、表和输出保持 128 bit。
这些是 solution 设置，不是单独复制 C++ 文件就能一并带走的源码 pragma。
以后新建 solution 或交付工程时，必须同步加载此配置。

## 空指针检查的边界

`result` 在 C 仿真中是结果对象的地址，在硬件中是固定的输出寄存器集合。
数据指针在 C 仿真中是主机内存地址，在硬件中对应 AXI-Lite 配置的物理基地址。
代码用 `#ifndef __SYNTHESIS__` 将 C++ 空指针保护限定在主机仿真中。

因此，C 仿真中的空指针用例继续保留，但它们不证明硬件会拒绝非法物理地址。
硬件启动之前，驱动/M03 必须检查容量、对齐、DDR 地址范围和不允许的缓冲区重叠。
ABI、mode、valid_rows、H/V、flags 和 token ID 的检查继续存在于硬件逻辑中。

## 首次综合操作（已完成，留作复现参考）

1. 保持已有工程打开，顶层函数仍为 `embedding_v2`，源码仍为 `V2/embedding_v2.cpp`。
   如果编辑器提示磁盘文件改变，重新加载磁盘版本；不要用旧编辑器内容覆盖新文件。
2. 选中当前 `solution1`，在 HLS 的 Tcl Console 中输入：

   ```tcl
   source C:/Users/87767/Desktop/work/program/llm-fpga/LLM_FPGA-main/V2/scripts/config_interfaces.tcl
   ```

   这条命令只设置接口参数，不启动仿真或综合。也可以在 Solution Settings 的
   `config_interface` 中手动设置上述三个选项。
3. 检查流程为 Vivado IP，器件为 `xczu9eg-ffvb1156-2-i`，目标时钟为 10 ns。
   保持 `s_axilite_data64` 关闭，使用 32-bit AXI-Lite 数据口。
4. 重新运行 C Simulation，预期仍为 `ALL 38 CHECKS PASSED` 和 `CSim done with 0 errors`。
5. 点击 Run C Synthesis。测试文件仅属于 Test Bench，不作为顶层设计综合。
6. 完成后查看 `embedding_v2_csynth.rpt` 以及 Interface/接口相关信息：
   - 三个数据口是否分别为 32/128/128 bit；
   - AXI 地址是否为 64 bit；
   - 是否只有一组 AXI-Lite 控制接口，包含三个基地址、全部控制参数和全部结果字段；
   - 是否没有 `m_axi_result` 或结果用的外部 BRAM 接口；
   - 是否有接口指令被忽略、字段丢失或其他错误/警告。

本轮优先核对接口。Latency、II、资源和时钟是报告中的估计值；后续 DATAFLOW 优化和
Vivado 实现可能改变它们。若遇到综合错误，请提供首个 ERROR 及其前后日志。

## 依据

- 项目两份 V2 文档的公共结果和 M05 接口章节。
- [AMD 2021.1：结构体拆分与成员接口](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/set_directive_disaggregate)
- [AMD 2021.1：HLS INTERFACE 指令](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/pragma-HLS-interface)
- [AMD 2021.1：config_interface](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/config_interface)
