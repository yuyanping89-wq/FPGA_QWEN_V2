# M05 V2：C/RTL 联合仿真与边界回归

## 当前状态

**用户已确认新增 build_config=0x10010011 的核心通过，最新本地 RTL 报告为
2026-10-08 17:42:04、Verilog Pass。后续新增实际性能计数位于独立 RTL 产物，
不包含在本文件的 HLS C/RTL 仿真流程中，见 [PERFORMANCE.md](PERFORMANCE.md)。**

**2026-10-08 边界回归结果：用户已运行扩展后的 12 项测试，C Simulation 为 0 errors，
重新综合成功，12 个 RTL 事务完成，C post checking 全部通过，最终 Verilog Pass。
助手仅读取用户日志及生成报告，没有执行编译、C 仿真、综合或 RTL 仿真。**

本轮联合仿真报告时间为 16:48:42；延迟 min/avg/max 为 280/2765/8510 周期，
总执行周期为 32028。综合估计仍为 LUT=5935、FF=4530、BRAM_18K=46、
目标周期 10 ns、估计周期 7.300 ns，读写行循环 II=1，ID 检查循环 II=10。
以下首次通过记录作为历史保留。12 项回归通过不等于 AXI 无越界/无额外写入或板级验收通过。

**版本提醒：下面首次通过记录属于原循环版本；三级 DATAFLOW 版本随后也已重新综合并完成
三个事务的联合仿真，最终 PASS。新旧对比见 [DATAFLOW.md](DATAFLOW.md)。**

用户已完成原 C 测试的 38 项检查，并于 2026-09-29 完成 C Synthesis。
用户已确认 `test_embedding_v2_cosim.cpp` 的 C Simulation 通过，并重新完成 C Synthesis。
首次 C/RTL 运行在测试程序编译阶段失败；用户在 GUI 中统一编译标准后，
于 2026-10-08 完成重跑：Verilog Pass，C post checking 三个用例通过，
最终日志为 `C/RTL co-simulation finished: PASS`。
报告延迟 min/avg/max 为 441/3588/9878 周期；三次调用的类型和行数不同，
平均值不应作为单个 token 的延迟。该结果仅对应仿真内存模型。
首次测试当时未修改设计和 depth；随后 DATAFLOW 改造已修改设计，接口 depth 保持原值。

本轮是首次 RTL 功能检查，不是完整交付验收。无须开发板，也无须真实 Qwen 权重文件。

## 当前 12 项测试内容

| 顺序 | 调用 | 预期 |
| --- | --- | --- |
| 1 | Prefill，64 个 token，包含词表首尾、重复和非顺序 ID | status=0，processed_rows=64，逐个 BF16 位模式一致 |
| 2 | Prefill，IDs 为 `[0, 151936, 42]` | status=7，processed_rows=1，第一行数据正确 |
| 3 | 错误后的 Decode，ID 为 151935，并更换 job_id | status=0，processed_rows=1，词表最后一行数据正确 |
| 4 | 第一个 ID 为 UINT32_MAX，后面仍有合法 ID | status=7，processed_rows=0，任务正常结束 |
| 5 | 紧接上一错误任务，第 64 个 ID 为 151936 | status=7，processed_rows=63，前 63 行正确 |
| 6 | 连续两次 token 错误后，执行正常 Prefill T=64 | status=0，processed_rows=64，全部数据正确 |
| 7 | Prefill T=1，ID=0 | status=0，processed_rows=1 |
| 8 | Prefill T=36，使用最大批次的前 36 个 ID | status=0，processed_rows=36，全部数据正确 |
| 9 | valid_rows=0，指针仍有效 | status=2，processed_rows=0 |
| 10 | valid_rows=65，IDs 仍按接口 depth 分配 64 项 | status=2，processed_rows=0，设计应在读取 IDs 前拒绝 |
| 11 | Decode T=2 | status=2，processed_rows=0 |
| 12 | 连续三次参数错误后，执行正常 Decode，ID=42 | status=0，processed_rows=1，数据正确 |

这些调用按顺序执行，不在用例之间主动复位 IP；第 4～6 项检查 DATAFLOW 错误收尾和恢复，
第 9～12 项检查参数拒绝及恢复。第 4 项只核对状态和完成行数，未用 AXI 总线监测证明零写入。
日志中最后一个 `[RUN]` 可以帮助定位未返回的调用；测试程序本身没有 RTL 超时监测器。

同时检查当前版本的 perf、CRC 为 0，build_config 固定为 `0x10010011`。
测试包含零、Inf、NaN 等位模式，M05 必须原样搬运。

所有指针都有效。IDs 分配 64 个元素，表分配 17016832 个 128-bit 元素，
输出分配 7168 个 128-bit 元素；每次调用都满足声明的接口 depth。
不把容量只有一行的主机数组传给深度为 64 行的仿真包装器。

这份测试只显式核对已完成行，不把主机数组外的哨兵当作 RTL 越界访问检查。
它没有完整证明错误后未写入的区域保持不变；后续需补充 AXI 写地址/范围监测。
原来的 `test_embedding_v2.cpp` 保留用于完整 C 回归；其空指针测试不用于 RTL。

## 电脑内存和磁盘

这是完整词表测试，没有缩小词表、接口 depth 或硬件地址计算范围。
词表原始数据占 272269312 字节（约 260 MiB），包装器和模拟器还会占用额外内存。
HLS 可能按完整 depth 为每次调用生成文本形式的输入向量。
按每个 128-bit 元素约 33～35 字节文本估算，12 次调用仅词表向量就约 6.7～7.2 GB；
中间文件、编译产物和模型会进一步增加占用。建议工程所在盘预留至少 20 GB 可用空间，
这只是操作余量，并非精确峰值需求。实际用量取决于工具生成方式。

初始化词表和生成输入向量时可能暂时没有新的逐用例输出；不要仅因此判断仿真卡死。
本轮关闭波形记录；若发生错误，再有针对性地打开波形。

## Vitis HLS 图形界面操作

1. 打开已有工程 `V2/hls/embedding_v2_csim`，选中 `solution1`。
   顶层函数仍为 `embedding_v2`，Design Sources 中仍使用 `V2/embedding_v2.cpp`。
2. 在工程的 Test Bench 文件列表中，移除 `test_embedding_v2.cpp` 的工程引用。
   **保留磁盘文件，不要删除文件本身。** 添加 `V2/test_embedding_v2_cosim.cpp` 为 Test Bench。
   可以通过 Project Settings 中的 Test Bench 文件列表操作。
   两个文件都有 `main()`，同一工程的测试列表中只能选其中一个。
3. 给设计文件和新测试文件都显式设置 CFLAGS 和 CSIMFLAGS：
   `-std=c++11 -O2 -Wno-unknown-pragmas`。不能只设置 Design Sources。
   本机 GUI 路径为 Project > Project Settings：在 Synthesis 和 Simulation 文件列表中
   分别选中设计文件和测试文件，使用 Edit CFLAGS / Edit CSIMFLAGS。
   这是本次用户采用的配置方式。`scripts/fix_cosim_cflags.tcl` 保留作 HLS Tcl 脚本用途；
   不需要在左侧工程树寻找 Tcl Console。
4. 先执行 **Run C Simulation**，选择清理旧构建（Clean Build），确认新测试程序能运行。
   预期最后出现：

   ```text
   [PASS] prefill T=64, boundaries and repeated tokens
   [PASS] bad middle token, processed_rows=1
   [PASS] decode T=1 after error, last token
   [PASS] bad first token UINT32_MAX, processed_rows=0
   [PASS] bad last token at row 63, processed_rows=63
   [PASS] prefill T=64 after two token errors
   [PASS] prefill T=1, first token
   [PASS] prefill T=36
   [PASS] reject valid_rows=0
   [PASS] reject valid_rows=65
   [PASS] reject decode T=2
   [PASS] decode T=1 after three parameter errors
   ALL 12 RTL-SUITABLE CASES PASSED
   ```

   并且 CSim 报告 0 errors。此时还没有验证 RTL。
5. 只更换 Test Bench 不改变设计硬件，可使用现有成功综合的 solution。
   若工具要求重新综合，或者设计源码/solution 设置已经改变，则先运行 C Synthesis；
   新建 solution 时必须先加载 `scripts/config_interfaces.tcl`。
6. 执行 **Run C/RTL Co-Simulation**，设置：

   | 设置 | 选择 |
   | --- | --- |
   | RTL | Verilog |
   | Simulator / Tool | XSim（Vivado Simulator） |
   | Dump Trace / Trace Level | None |
   | 其他选项 | 保持默认，不启用额外 profiling 或波形调试 |

7. 等待全部阶段完成，再查看最终报告。联合仿真会经历 C 侧向量准备、RTL 仿真、结果核对，
   测试程序的 PASS 信息可能在不同阶段重复出现。
   **仅出现 `ALL 12 RTL-SUITABLE CASES PASSED` 还不够，必须看最终 RTL 仿真状态。**

12 项测试扩展时仅修改 Test Bench 和说明文件；后续 build_config 更新已修改设计源码，须重新综合。
新报告包含 12 次不同任务，平均延迟和总周期不能直接与旧 3 次调用报告比较优化收益。

## 应该发回哪些结果

报告通常位于：

```text
V2/hls/embedding_v2_csim/solution1/sim/report/embedding_v2_cosim.rpt
```

成功时应看到报告中的 `Verilog` 行状态为 `Pass`，并且工具报告 C/RTL co-simulation 完成。
把该报告和控制台末尾约 30 行发回来即可。

如果失败，发送第一个 ERROR、其前后日志，以及 `[FAIL]` 的具体内容；
如果停留很久，发送当前最后一段日志，用于区分向量生成、RTL 编译和 RTL 执行阶段。
不要只根据进程退出码或 C 测试的 PASS 判断成功。

联合仿真使用模拟内存，不包含实际 PS、HP0 互连、DDR 控制器或缓存一致性行为。
即使通过，也不代表高物理地址、板上实际带宽、所有错误分支和板级时序已完成验证。

## 恢复完整 C 测试

需要运行原 38 项检查时，从 Test Bench 列表移除新文件的引用，再加回
`test_embedding_v2.cpp`。不要同时保留两个 main。
`scripts/run_csim.tcl` 原本也是面向旧 C 测试的脚本；切换到联合仿真期间不要运行该脚本，
避免把旧测试重新加进同一个工程。

## 已遇到的编译标准冲突

报错包括 `quoted function uses auto type specifier without trailing return type`，
以及 `body of constexpr function ... not a return-statement`。
当时工程中设计文件带 `-std=c++11`，测试文件没有显式标准；测试包装代码包含 C++14
标准库内容，但 `cosim.tv.mk` 又使用 C++11。失败发生在 Instrumenting C test bench 阶段，
尚未运行 RTL。修正方法是统一两个文件的 CFLAGS / CSIMFLAGS，见上面的配置脚本。
不要直接修改自动生成的 makefile 或工具安装目录中的标准库。
脚本尚未经 HLS 执行验证；本次由助手准备修改、用户运行验证。

## 工具依据

- [AMD Vitis HLS 2021.1：cosim_design](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/cosim_design)
- [AMD Vitis HLS 2021.1：接口与 depth](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/pragma-HLS-interface)
- [AMD Vitis HLS 2021.1：CFLAGS 与 CSIMFLAGS](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/add_files)
