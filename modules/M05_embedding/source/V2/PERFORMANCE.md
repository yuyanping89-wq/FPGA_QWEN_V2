# M05 V2 实际硬件性能计数

## 实现状态

最新进展（2026-10-09）：用户已完成计数器版本普通顶层综合；0 errors、0 critical warnings、
6 条 Synth 8-589 警告。LUT=3720、FF=5842、BRAM=4 RAMB36 + 1 RAMB18、DSP=0。
综合后 WNS=+7.229 ns、WHS=-0.086 ns，接口延迟约束未补齐，尚不能记录时序通过。
接下来使用独立 OOC 工程验证内部实现，见 [OOC_IMPLEMENTATION.md](OOC_IMPLEMENTATION.md)。
其余下文按时间记录实现和行为仿真的历史。

2026-10-08：用户已确认 build_config 核心版本通过；本地最新 HLS 联合仿真报告为
17:42:04、Verilog Pass。该报告对应 HLS 核心 `0x10010011`，没有实际性能计数器。

本轮新增可综合的 `rtl/m05_perf_counters.v`，并用 `scripts/prepare_perf_rtl.py`
将计数器接入 HLS 生成 RTL 的**独立副本**。已准备目录：`build/embedding_v2_perf/`。
助手仅执行文件准备、格式/连接静态核对，未运行编译、综合、仿真或板测。
2026-10-09 用户已运行 Vivado 2021.1 行为级仿真，8 项计数器单元检查、13 项 M05 集成任务
全部通过，包括真实 AXI-Lite 读回与独立逐拍统计的比较。证据见 reports/perf_rtl_20261009。
新增硬件尚待综合和时序检查，不沿用原核的资源估计或 7.300 ns 估计。

## 为什么计数器放在 RTL 层

HLS C++ 的 `table[...]` 或 `hidden_out[...]` 访问由工具变成 AXI 硬件。
实际传输还会受到突发拆分、适配器缓存、DDR 等待和背压影响。
因此不能把 C++ 循环自增当成时钟周期，也不能把访问次数当成外部 AXI 已握手次数。
本实现直接观察生成 RTL 的外部 AXI R/W 通道，并把测量值接回现有控制寄存器。

```text
HLS 生成的 M05 ─── AXI ─── DDR
    │               │
    │ 启动/完成       │ RVALID & RREADY / WVALID & WREADY
    └──────┬────────┘
           ↓
     m05_perf_counters
           ↓ 完成时同时锁存
     原 s_axi_ctrl 的四个 perf_* 结果寄存器 → PS
```

没有增加外部 AXI 接口、DDR 缓冲区或软件函数参数。
计数器使用同一个 `ap_clk` 和同步低有效复位 `ap_rst_n`，不跨时钟域。

## 固定计数口径

| 字段 | 本次定义 |
| --- | --- |
| `perf_cycles_total` | 从接受本次启动的上升沿 S 到采样完成的上升沿 D，包含两端，计数 D-S+1；包括纯等待和写响应等待 |
| `perf_cycles_compute` | 上述窗口内，IDs 读、词表读、输出写任一数据通道完成握手的周期数；一拍多路活动只加 1 |
| `perf_read_beats` | IDs 的 RVALID&&RREADY 次数 + 词表的 RVALID&&RREADY 次数；同一拍两路都成功则加 2 |
| `perf_write_beats` | 输出口 WVALID&&WREADY 次数；无论 WLAST 是否为 1，每个数据 beat 都计数 |

本模块没有浮点计算，因此将“核心活跃周期”明确定义为**数据搬运活跃周期**。
这个指标不包括纯地址握手、B 响应、无外部传输的内部状态推进或 FIFO 内部搬运。
它不能理解成完整 datapath busy 时间或两个读写拍数的和。此口径需随 M05 交付给 M02/M15。

- 所有计数器是 64-bit 无符号整数，溢出按模 2^64 回绕。
- START 取自当前生成 FSM 的 `ap_start && ap_CS_fsm_state1`，不会把保持数千拍的 ap_start
  电平误当成连续启动。工具版本/FSM 改变时准备脚本会拒绝不认识的结构。
- 接收到新任务后工作计数器从零开始，已完成任务的快照保持到下次完成或复位。
- 在完成边沿之前组合给出包含该边沿的最终值，控制块在采样 ap_done 的同一边沿锁存四个值。
  不引入“done 已置位，计数晚一拍”的读取窗口。
- 错误返回同样产生快照；非法参数的传输数应为零，总周期仍非零。
- 复位清零并中止本次测量，不伪造任务完成。
- 目前计数始终开启，正常使用仍传 `flags=0`；不因此宣称支持公共 PERF flag。
- ID 端口每 beat 为 4 字节，表/输出每 beat 为 16 字节。合计 read_beats 不能统一乘 16。
- 计的是数据通道接受的 beat；不把地址请求次数、burst 数或写响应数当成 write_beats。
  即使外部返回 AXI 错误，已经握手的流量仍被计入；现有核的 AXI 错误处理另属验收范围。

## 数值直觉（理想的正常调用）

若一次成功处理 T 行、IDs 口每个 ID 一拍且无额外取数：

```text
表读取 = 112 × T beats
ID 读取 = T beats
perf_read_beats = 113 × T
perf_write_beats = 112 × T
```

因此 T=1 时通常为读 113、写 112；T=64 时为读 7232、写 7168。
这些只是核对预期，计数器的实现没有写入这些公式，最终值来自实际握手。
非法 token 前后可能存在适配器取数行为，尤其不能仅靠 processed_rows 倒推真实 ID 读拍数。
加入 AXI 等待时，total 应增大；仅增加纯等待的情况下，数据 beat 数应保持不变。
相同 beat 数下，读写重叠不同可能改变 compute 活跃周期数。

## 两种产物必须区分

| 产物 | build_config | perf 字段 |
| --- | --- | --- |
| 原 HLS C++ / HLS 直接导出的核心 | `0x10010011` | 仍为占位 0，用于原 C/RTL 功能回归 |
| 本轮准备的带计数器 RTL | `0x20010011` | 实际计数；修订号从 1 改为 2 |

带计数器版本仅改变高四位 MODULE_REVISION，其余搬运、burst、时钟编码不变。
`perf_build_profile.json` 固定记录口径，准备脚本生成的 manifest 还记录输入/输出 RTL SHA256。
配置编码仍需课题组统一确认。本轮没有补齐启动前身份寄存器。

**直接在 Vitis HLS 中点击 C/RTL Co-Simulation 或 Export RTL，不会自动得到带计数器版本。**
不能把带计数器顶层直接覆盖到 HLS 仿真目录：原 C golden 的 perf 为 0，且 build_config 为
0x10010011，与带计数器硬件不同。计数器版本需要单独的 RTL 验证。

## 文件准备与后续使用

当前 `build/embedding_v2_perf/rtl/` 已由助手准备，包含完整 HLS 核及连接好的计数器。
同目录的 `drivers/` 复制自当前生成驱动，四个性能寄存器的地址没有变化。
没有写入 HLS 的 syn、sim 或 impl 目录。

HLS 核心日后重新综合时，在 V2 目录运行以下文件准备命令（只生成文件）：

```powershell
python scripts/prepare_perf_rtl.py --output build/embedding_v2_perf_next
```

脚本只接受新的输出目录，不覆盖旧产物；输入工具版本、FSM、位宽、结果常量或寄存器映射
不符合当前适配约定时会报错，需审查后更新适配器。不要通过删除校验来跳过不匹配。
manifest 中的 HLS 源码 hash 是文件准备时的追溯记录，不替代重新综合的必要性。

### 在 Vivado 2021.1 中打开

1. 打开 Vivado 2021.1。
2. 使用 **Tools → Run Tcl Script**，选择 `V2/scripts/open_perf_project.tcl`。
3. 脚本创建 `build/embedding_v2_perf/vivado_project/m05_embedding_perf.xpr`，
   加入带计数器 RTL 和 10 ns 时钟约束，顶层为 `embedding_v2`。
4. 该脚本只建工程，不运行综合或仿真。后续再次打开时直接打开 `.xpr`，不要重复创建。

若使用其他准备目录，可在 Vivado 的 Tcl Console 中先设置 `m05_perf_dir` 为该绝对路径，
再 source 此脚本。这里明确指 **Vivado**，不要求在 Vitis HLS GUI 中寻找 Tcl Console。
2026-10-09 已补充并由用户运行 8 项计数器单元检查和 13 个完整 M05 任务，全部通过。
在当前工程通过 Tools → Run Tcl Script 执行 `scripts/add_perf_tests.tcl` 加入测试，
再由用户运行 Behavioral Simulation。详细覆盖和预期输出见 [PERF_TESTS.md](PERF_TESTS.md)。
新增模块必须重新综合检查时序，原 HLS 资源/时序数字不包括这些 64-bit 计数器。
此目录是 RTL 集成产物，尚未封装成正式 component.xml IP 包。

## PS 读取

寄存器仍为：

| 字段 | 低 32 bit | 高 32 bit |
| --- | --- | --- |
| total | 0x08c | 0x090 |
| compute | 0x0a4 | 0x0a8 |
| read_beats | 0x0bc | 0x0c0 |
| write_beats | 0x0d4 | 0x0d8 |

单任务调用顺序：关闭 auto_restart → 配置参数/flags=0 → Start → 带超时等待 Done →
读取状态、行数、build_config 及四个计数 → 将结果与调用方保存的 job_id 一起记录 → 下一任务。
读取 64-bit 值的两次 32-bit 访问期间不能由另一个线程启动新任务；也不要开启 auto_restart
后直接读取这一组寄存器，否则新一次完成可能覆盖快照。
监测模块内部锁存 job_id 供波形关联；当前没有额外 PS 可读的 job_id 快照寄存器，
PS 必须保留本次提交的 job_id 到结果收集结束。

已有 HLS 生成驱动可直接读取：

```c
/* Call only after observing Done, with auto_restart disabled and no new Start. */
u64 cycles = XEmbedding_v2_Get_result_perf_cycles_total(&ip);
u64 active = XEmbedding_v2_Get_result_perf_cycles_compute(&ip);
u64 reads  = XEmbedding_v2_Get_result_perf_read_beats(&ip);
u64 writes = XEmbedding_v2_Get_result_perf_write_beats(&ip);
u32 build  = XEmbedding_v2_Get_result_build_config(&ip); /* 0x20010011 */
```

总计数采用包含启动和完成两个采样边沿的口径。若要和严格的“两个边沿时间差”比较，
应使用 `(cycles - 1) × 时钟周期`；按监测窗口占用拍数计时则为 `cycles × 时钟周期`。
比较不同版本时必须使用相同边界定义，不要求它与 HLS 报告的延迟数逐项相等。

## 协议依据

AXI 数据在时钟上升沿 VALID 与 READY 同时有效时完成传输。
参考 [Arm AMBA AXI/ACE 协议](https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/IHI0022H_amba_axi_protocol_spec.pdf)。
