# M05 三级 DATAFLOW 改造

## 状态与基线

2026-10-08：用户已完成新版本综合及三个事务的 C/RTL 联合仿真，最终 PASS。
当前 C 仿真日志为三个用例通过；本轮尚未取得原 38 项测试重新运行的完整记录。
此次核对仅读取已有日志和报告，没有由助手执行编译或测试。

已确认 A/B/C 三个功能阶段及辅助 entry_proc 生成在 DATAFLOW 区域内。
id_fifo 为 33 bit × 64，data_fifo 为 128 bit × 64；READ_ROW / WRITE_ROW 的实际 II 均为 1。
CHECK_IDS 被自动流水线化，实际 II=10，工具此前尝试更小 II 时给出了调度警告。
112 字的权重读和输出写 burst 均被推断；外部端口位宽、64-bit 地址和结果寄存器偏移符合此前配置。

### 同一组三事务的报告比较

| 项目 | 原循环版本 | DATAFLOW 版本 |
| --- | ---: | ---: |
| RTL 延迟 min / avg / max（周期） | 441 / 3588 / 9878 | 441 / 3112 / 8452 |
| Total Execution Time（周期） | 10554 | 9128 |
| HLS LUT | 5056 | 5935 |
| HLS FF | 3562 | 4530 |
| HLS BRAM_18K | 36 | 46 |
| HLS DSP | 0 | 0 |
| HLS 估计周期（ns） | 7.300 | 7.300 |

最大延迟降低约 14.4%，三事务总执行周期减少约 13.5%。这些是当前仿真环境的结果，不是板上测量。
FIFO 在 HLS 资源估计中分别使用 2 和 8 个 BRAM_18K，可作为后续存储映射优化的检查点；
不能仅按净数据容量估计其物理 BRAM 用量。

剩余关注项：read_embedding_rows 同时依赖上游及调用方参数，工具提示可能限制吞吐量；
hidden_out_c_channel 自动从深度 2 增至 3，这是传递基地址的辅助通道，不是 data_fifo 被改小。
当前三个事务未发生死锁，但不代表全部输入和 AXI 背压模式已验证。

旧版源码、两份测试、头文件、接口配置和综合/联合仿真报告保存在：
`baselines/pre_dataflow_20261008/`。
不要把该目录中的旧版 cpp 加入当前 HLS 工程，否则会出现重复顶层或 main。

## 代码结构

```text
embedding_v2：检查公共参数、初始化结果
    |
    +-- run_embedding_dataflow（DATAFLOW 区域）
           |
           A read_and_check_ids
           |  id_fifo，33 bit × 64
           B read_embedding_rows
           |  data_fifo，128 bit × 64
           C write_embedding_rows
    |
    +-- 返回最终 status_code / processed_rows
```

- A 只把合法的 token ID 发给 B，遇到第一个非法 ID 停止产生任务。
- B 按 token ID 计算词表偏移，每行从表读取 112 个 128-bit 数据字。
- C 按收到的行顺序写到 hidden_out，完成一行后累计 processed_rows。
- B 的 READ_ROW 和 C 的 WRITE_ROW 循环显式设置 `PIPELINE II=1`。
- A 的 ID 检查循环没有强制 II=1；本轮重点是三级重叠和权重/输出数据流。
- 三个阶段均使用 `INLINE off` 保留函数边界；DATAFLOW 区域中无条件调用这三个阶段。
- 两个显式 FIFO 都是单生产者、单消费者。没有额外的结果 FIFO，也没有整行或全表片上数组。
- HLS 还可能自动生成启动/标量同步通道，它们不属于上述两个显式数据 FIFO。

DATAFLOW 和循环 pipeline 的最终实现、达到的 II、FIFO 类型及资源均需查看新的综合报告。

## FIFO 消息格式

### FIFO 1：任务或结束消息

元素为 `ap_uint<33>`：

| bit | 非结束元素 | 结束元素 |
| --- | --- | --- |
| 32 | 0 | 1 |
| 31:0 | 合法 token ID | 最终状态码，OK=0 或 BAD_TOKEN=7 |

每次有效调用发送 N 个合法 ID，随后恰好发送一个结束元素。
N 是全部有效 token 数，或非法 token 之前的合法前缀长度。
行号隐含在 FIFO 顺序中，不必额外携带。

### FIFO 2：行头、数据、结束消息

元素为 `ap_uint<128>`，依靠固定格式区分控制与数据：

```text
行头0 + 112个数据字 + 行头0 + 112个数据字 + ... + 结束行头
```

普通行头为全 0；结束行头 bit32=1、bits31:0 为状态码，其余位为 0。
阶段 C 只在“行头位置”检查 bit32。随后 112 个元素全部视为原始 embedding 数据，
不解释任何控制位，因此 NaN/Inf、全 0 和任何 BF16 位模式都不会被误识别成结束消息。
行头和结束消息只存在于片上 FIFO，不会写入 DDR，也不改变 ACT_TH_V2 的输出布局。

这种格式每行增加一个内部控制元素，但保留了固定 112 次的连续 DDR 读/写循环，
便于 HLS 推断 burst。是否实际保持有效 burst，需要重新综合核对。

FIFO depth=64 是元素数量。数据 FIFO 的净存储容量为 1024 字节，包含控制元素时，
不能把它说成始终可以缓存 64 个 embedding 数据字。实际资源还包括控制逻辑。
FIFO 不需要装满 112 个字才开始输出，B 和 C 一边生产一边消费。

## 终止与错误行为

对 N 行合法前缀：

- A 总共写入 N+1 个任务元素；B 读取同样数量。
- B 总共写入 N×113+1 个数据 FIFO 元素；C 消费同样数量。
- C 仅向 DDR 写 N×112 个数据字。
- 正常结束返回 OK/N；非法 token 返回 BAD_TOKEN/N。

即使第一个 token 就非法，也会经过 A→B→C 传递结束消息，C 不执行数据写入。
即使 N=64，FIFO 1 容量只有 64，最后的结束消息也可以等待 B 消费后再写入，
不是要求 FIFO 同时容纳所有任务与结束消息。

合法前缀被完整排空后三个阶段退出，下次调用重新开始。没有利用 stream.empty() 猜测任务完成，
也没有只退出上游、让下游永远等待的设计分支。
这说明协议的预期行为，不替代有限 FIFO 的 RTL 无死锁验证。

ABI、模式、行数、维度、flags 的检查位于 DATAFLOW 区域之外；非法任务不启动流水线。
主机空指针检查继续只用于 C 仿真。任何非零 status 下，PS 仍必须丢弃整个输出。

## 保持的外部合同

- 顶层函数签名、三个 DDR 指针、m_axi/AXI-Lite 指令和 depth 与基线相同。
- DDR 数据类型、布局、行跨度、Prefill/Decode 规则相同。
- 表/输出最大 burst=64，ID 最大 burst=16；AXI 地址配置仍需保持 64 bit。
- 结果在完成后读取，processed_rows 表示合法前缀写入行数。
- flags 仍只接受 0；perf、CRC、build_config 仍为占位值 0，尚未实现。
- 内部函数变化可能影响工具生成结果，必须重新核对最终端口和寄存器偏移，不能仅凭源码指令相同就跳过。

## 用户在 Vitis HLS 2021.1 中的下一步

1. 重新加载磁盘上的 `embedding_v2.cpp` 和头文件，避免旧编辑器内容覆盖新文件。
   Design Sources 只保留当前 `embedding_v2.cpp`，新增内部函数不需要添加其他 Design 文件。
   `hls_stream.h` 由 Vitis HLS 自带，不需要安装。
2. 保持两个源文件（设计和选中的测试）的 CFLAGS/CSIMFLAGS 都包含
   `-std=c++11 -O2 -Wno-unknown-pragmas`，通过 Project > Project Settings 设置。
3. 建议先选择原 `test_embedding_v2.cpp` 跑 Clean C Simulation，预期 38 项检查通过。
   它覆盖非法首/中/末 token、参数错误以及输出未使用区域等情况；此文件只用于 C 仿真。
4. 切换 Test Bench 为 `test_embedding_v2_cosim.cpp`，再次 Clean C Simulation，预期 3 项通过。
   工程中始终只保留一个 Test Bench 的 main；重新添加测试文件时检查编译参数。
   两份测试文件本轮未修改，原有预期结果应继续成立。
5. 运行 C Synthesis（本次设计源码已变，必须重跑）。检查：
   - run_embedding_dataflow 内识别出 A/B/C 三个数据流阶段；
   - id_fifo / data_fifo 为 FIFO，深度分别 64，宽度分别 33/128；
   - READ_ROW / WRITE_ROW 的实际 II；没有达到 1 时记录原因；
   - 表/输出读写 burst 是否仍被推断；
   - 外部接口是否仍为 32/128/128 bit，地址 64 bit；
   - 目标时钟、资源和结果寄存器映射。
6. 运行 C/RTL Co-Simulation：Verilog、XSim、Trace=None，先保持其他选项默认。
   检查三个事务结束、C post checking 通过、最终 Verilog Pass。
   仅 C 测试通过不能证明硬件 FIFO 深度足够或不会死锁。
7. 将新综合报告和联合仿真报告发回，再比较性能。若失败，发第一个 ERROR 及其上下文。

本轮现有 RTL 测试覆盖最大批量、非法中间 token、错误后恢复；首 token 非法等路径尚无新的 RTL 验证结果。
后续应补充 RTL 边界测试，但本次没有添加或执行新测试。

## 比较方法

旧版保存的 HLS 结果：LUT=5056、FF=3562、BRAM_18K=36、DSP=0，目标周期10ns、估计7.300ns。
旧版三个事务的 cosim 延迟 min/avg/max=441/3588/9878 周期。
新版实测仿真和 HLS 结果见本文件开头；不把仿真改善当作板级收益。

比较时保持同一器件、10ns时钟、AXI配置、同一测试用例及仿真设置。
除了延迟，也看新增 FIFO/控制逻辑的资源成本及 burst 效率；总体平均值不能当成单 token 延迟。
DATAFLOW 仿真速度收益不等于共享 HP0/真实 DDR 条件下的板级收益。

## 工具参考

- [AMD 2021.1：Dataflow Viewer](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/Dataflow-Viewer)
- [AMD 2021.1：STREAM 指令](https://docs.amd.com/r/2021.1-English/ug1399-vitis-hls/pragma-HLS-stream)
