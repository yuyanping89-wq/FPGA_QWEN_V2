# M05 性能计数器 RTL 测试

编写及用户验证日期：2026-10-09。用户在 Vivado 2021.1 XSim 运行成功，
**8 项计数器单元检查和 13 个 M05 集成任务全部通过**。
助手仅审阅和归档日志，没有运行编译、仿真或综合。
测试对象是带计数器的 `build_config=0x20010011` RTL；原 HLS C 测试不包含这些计数器。

证据归档：`reports/perf_rtl_20261009/user_vivado_console.log`，附当前 RTL/TB 哈希的 validation.json。
decode baseline：total=272、active=220、reads=113、writes=112；
同一 decode 加等待：total=858、active=223、reads=113、writes=112。
Prefill T=64：total=15345、active=13943、reads=7232、writes=7168。
三个非法尺寸任务均为 total=3、active/read/write=0。
最终输出 `M05 PERFORMANCE RTL TEST PASSED`，在 625190 ns 调用 `$finish`。
界面末尾的 “ran for 21ms” 是配置的运行时长，实际测试已提前结束；不是任务实际耗时。
新增硬件仍需综合和时序检查，这些是行为级仿真结果。

## 在当前已打开的 Vivado 工程中操作

1. 保持 `m05_embedding_perf` 工程打开，不必重新创建。
2. **Tools → Run Tcl Script**，选择 `V2/scripts/add_perf_tests.tcl`。
3. 脚本把四个 `.sv` 文件加到 Simulation Sources，并设置仿真顶层为 `tb_m05_perf`。
   Design Sources 的设计顶层仍为 `embedding_v2`。脚本不会运行编译、仿真或综合。
4. 点击 **Run Simulation → Run Behavioral Simulation**，不需要先 Run Synthesis。
5. 查看底部 Tcl Console 的输出，等待最终 PASS 或 FAIL。默认运行时长设为 21 ms 仿真时间，
   测试在全部结束时调用 `$finish`，卡死则在 20 ms 调用 `$fatal`。
   若已有仿真会话，应先关闭旧会话，再重新启动以编译新源文件。

成功时应看到：

```text
ALL 8 COUNTER UNIT CASES PASSED
...
[PASS][M05] decode baseline total=... active=... reads=113 (ids=1 table=112) writes=112
[PASS][M05] same decode with AXI waits total=... active=... reads=113 (ids=1 table=112) writes=112
...
ALL 13 M05 INTEGRATION CASES PASSED
M05 PERFORMANCE RTL TEST PASSED
```

日志顺序可能穿插：计数器单元测试和完整 M05 测试使用同一时钟并行运行，各自有独立复位。
仅出现 8 项单元 PASS 不够，必须看到完整测试的最后一行。
`$finish` 是正常结束；`$fatal`、`[FAIL]`、编译错误或超时均不算通过。
不要直接套用旧 HLS 仿真的周期数，本测试的 AXI 内存模型及等待模式不同。

若 GUI 因设置原因只运行了 1000 ns，可在已经进入仿真的 **Vivado Tcl Console** 输入 `run all`。
这是启动后继续运行仿真的命令，不是配置命令。

## 测试文件

| 文件 | 用途 |
| --- | --- |
| `tb/m05_perf_tb_pkg.sv` | 测试地址、模型尺寸、独立 BF16 数据模式生成 |
| `tb/m05_axi_memory_models.sv` | IDs/词表读模型和输出写模型，支持可控等待和写响应延迟 |
| `tb/m05_perf_unit_test.sv` | 8 项计数器单元检查，由总测试顶层实例化 |
| `tb/tb_m05_perf.sv` | AXI-Lite 驱动、真实 M05 顶层、逐拍参考统计、13 个任务及结果回读 |

## 8 项计数器单元检查

1. 复位清零。
2. 纯等待的三拍，总周期为 3，活跃和传输为 0。
3. 无任务时的握手输入或孤立 done 不改变快照。
4. 同拍双读加 2、同时读写只算一拍活跃、VALID 无 READY 不计数、READY 无 VALID 不计数；
   完成边沿的传输也计入；运行中改变输入 job_id 不影响已锁存的任务编号。
   特别检查完成边沿前快照已经可供控制寄存器同拍采样。
5. 紧接着的任务重新计数。
6. 工作期间复位，中止任务并清零。
7. 被复位中止的任务随后出现 done 时不产生旧任务快照。
8. 复位后的正常恢复。

## 13 个完整 M05 任务

| 顺序 | 任务 | 检查重点 |
| --- | --- | --- |
| 1 | Decode T=1，词表末行，无额外插入等待 | 基础搬运和读写计数 |
| 2 | 同一 Decode，加入读写等待及延迟 B 响应 | 拍数正确，总周期严格大于任务 1 |
| 3 | Prefill T=64，首尾、重复、非顺序 ID | 大批次、burst、结果逐 BF16 位比较 |
| 4 | Prefill T=36，加入等待 | 不同长度和背压 |
| 5 | 第一个 token 为 UINT32_MAX | processed_rows=0，禁止输出请求 |
| 6 | 第 64 个 token 为 vocab_size | 前 63 行正确，失败行和后续区域不写 |
| 7 | 中间 token 非法 | 前 1 行正确 |
| 8 | 连续三次 token 错误后 Prefill T=64 | 各级恢复，无任务残留 |
| 9 | valid_rows=0 | 参数拒绝，无读写数据握手 |
| 10 | valid_rows=65 | 参数拒绝，无读写数据握手 |
| 11 | Decode T=2 | 参数拒绝，无读写数据握手 |
| 12 | 连续三次参数错误后 Decode | 正常恢复，快照更新 |
| 13 | 完整 IP 复位后 Prefill T=1 | 四个控制寄存器清零后重新工作 |

每个任务通过实际 AXI-Lite 写入所有地址和参数，启动，带超时轮询完成，
读取 status/processed_rows/build_config 和四个 64-bit 计数。
四个计数器的 valid 位也检查“完成后置位、读取后清零”，并检查完成快照保持稳定。
单次任务期间关闭 auto_restart，未提交下一任务前才读完所有结果。

参考总周期通过独立时钟序号相减（包含起止两个边沿）获得；参考读写次数观察 AXI 外部握手，
不读取计数模块的内部工作寄存器。两者与完成时的值及 AXI-Lite 读回值分别比较。
任务边界仍引用当前 HLS 顶层的开始/完成信号；另检查每次显式启动恰好对应一次完成。

## 内存模型和范围

这是模拟内存，不使用开发板、PS 软件或真实 DDR 控制器。
词表维度仍是 151936×896，按读地址即时生成 BF16 位模式，省去几 GB 的序列化向量。
输出仅分配 64 行，并为未写区域设置哨兵。对写地址、写拍、重复写入、WSTRB、WLAST、
burst 范围、4 KiB 边界及等待期间 W 数据保持进行检查。
输入/词表写请求、输出读请求一旦出现即报错。
IDs、表和输出都使用大于 4 GiB 的模拟地址，检查地址高位在本 RTL 模型中的传递。

每个端口仅支持一个在途 burst，与当前设计相符。读模型即使 gap=0 仍存在模型固有的调度间隔，
因此测试不代表峰值带宽；它不是完整 AXI VIP、随机背压覆盖或板级地址可达性验证。
没有覆盖所有 AXI 错误响应、任意 outstanding 配置、auto_restart 并发读快照、
64-bit 计数器溢出或整个 M05 在任意数据传输位置复位。中途复位目前在计数器单元层覆盖。
完整 M05 只验证任务之间的复位恢复。

## 发回结果

发送 Tcl Console 中最后约 40 行，包括最终 PASS。
若失败，发送第一处 `[FAIL]` / `ERROR` 及上下文。编译/展开失败时，
发送报错指出的 `xvlog.log` / `elaborate.log` 内容。
仿真日志通常位于工程的 `m05_embedding_perf.sim/sim_1/behav/xsim/`。

RTL 仿真通过后，再进行新增硬件的综合和时序检查，原 HLS 的资源估计不包括计数器。

工具参考：[AMD Vivado Logic Simulation UG900](https://docs.amd.com/r/en-US/ug900-vivado-logic-simulation/Vivado-Simulator-Simulation-Options)。
