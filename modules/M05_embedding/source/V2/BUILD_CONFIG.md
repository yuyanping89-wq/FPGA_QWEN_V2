# M05 V2 综合配置标识

## 当前状态

2026-10-08：在已通过 38 项 C 回归（用户确认）和 12 项 RTL 回归的 DATAFLOW 基线上，
将 `result.build_config` 从占位值 0 改为 `0x10010011`。
用户随后确认该修改通过；最新本地 RTL 报告为 2026-10-08 17:42:04、Verilog Pass。
增加配置字段前的已通过基线保存在 `baselines/dataflow_12cases_pass_20261008/`；
该备份中的 build_config 仍为 0，不是本节 0x10010011 版本。

后续性能计数实现已加入独立 RTL 产物，配置为 `0x20010011`（revision=2），待单独 RTL 验证。
本文件的 `0x10010011` 仍描述 HLS 核心（revision=1）。两种产物及运行步骤见 [PERFORMANCE.md](PERFORMANCE.md)。

编码使用《PS_PL 全模块参数与接口字段说明书》第 4.2 节的建议定义。
第 28 节要求架构负责人冻结 lane、clock、numeric ID 映射；当前为 M05 模块内采用的配置，
集成前需与 M02/架构负责人统一，不能标记为已经全项目冻结。

## 解码表

| bit | 字段 | 编码值 | 本模块解释 |
| --- | --- | --- | --- |
| 0～3 | PAR_LANES_CODE | 1 | 128-bit 数据字包含 8 个 BF16，表示搬运宽度，不代表 8 个乘法器 |
| 4～9 | ROW_TILE | 1 | 读行/写行阶段各按一行推进；不是 valid_rows，也不是 FIFO 深度或最大批量 |
| 10～17 | MAIN_TILE | 64 | 表读和输出写接口的最大 burst 长度为 64 beats；一行 112 beats 可以拆分 |
| 18～21 | NUMERIC_IMPL_ID | 0 | 基准 BF16 原样复制，无浮点算术或转换 |
| 22～27 | CLOCK_CODE | 0 | 目标时钟 100 MHz / 10 ns；不是 HLS 估计周期 7.300 ns |
| 28～31 | MODULE_REVISION | 1 | 第一个显式编码的 M05 DATAFLOW 配置版本 |

```text
(1 << 28) | (0 << 22) | (0 << 18) | (64 << 10) | (1 << 4) | 1
= 0x10010011
```

定义位于 `include/m05_build_config.h`，所有字段均有位宽断言，避免溢出截断。
软件期望值必须对应实际综合的配置。修改总线宽度、burst 或时钟时，必须同步修改源码、
HLS 设置、配置头、测试期望值及 `build_profile.json`；这个字段不会自行测量时钟或推断设置。
FIFO 深度 64、ID 接口宽度 32 等未编码的信息记录在 profile 中。

## 返回时机与限制

- 每次调用在参数检查前设置该字段，因此成功、非法 ABI、非法尺寸、非法 flags、非法 token
  等返回路径均报告同一配置值。主机测试中的空 result 指针仍直接返回。
- 当前通过已有 `result.build_config` AXI-Lite 输出字段返回；没有新增函数参数。
- PS 应在本次任务完成后读取输出结果，具体偏移以本次综合生成的驱动为准。
- 这一步没有实现复位后、首次启动前即可读取的完整身份寄存器。不要把结果字段的常量赋值
  当作已实现 `module_id → ABI → capabilities → build_config → self_check` 启动检查。
- `build_profile.json` 是模块配置记录，尚不是完整交付 manifest 或验收证明。

## 验证步骤（由用户运行）

1. Test Bench 仅选择 `test_embedding_v2.cpp`，运行 C Simulation / Clean Build。
   预期仍为 `ALL 38 CHECKS PASSED`；有结果的调用均额外检查 `build_config=0x10010011`。
2. 切换到 `test_embedding_v2_cosim.cpp`，保持两个文件的 CFLAGS 和 CSIMFLAGS 为
   `-std=c++11 -O2 -Wno-unknown-pragmas`，运行 C Simulation / Clean Build。
   预期 `ALL 12 RTL-SUITABLE CASES PASSED`。
3. 本次设计源码和头文件已改变，必须重新 C Synthesis，再运行 C/RTL Co-Simulation。
   设置 Verilog / XSim / Trace None。必须看到最终 Verilog Pass。
4. 任务数量和 PASS 文本保持不变，新增检查嵌入原用例；失败信息会指出 build_config 不匹配。

完整词表联合仿真空间估算和操作说明见 `COSIM.md`。

## 性能计数的后续实现边界

四个 perf 字段目前仍为 0，占位值不能当成实际测量或解释为本次任务没有耗时。
普通 HLS C++ 中的自增操作无法直接证明每时钟计数；内存访问循环次数也不能证明
外部 AXI 已完成的握手次数。后续需要在 RTL 层观察任务开始/完成、RVALID/RREADY、
WVALID/WREADY，设计计数器和完成时快照，并与 PS 结果读取路径集成。
`perf_cycles_compute` 对查表模块的活跃周期口径也需要固定；不能直接将读拍数与写拍数相加
当成活跃周期，因为读写可能发生在同一时钟。
IDs 和词表端口分别为 32-bit / 128-bit，汇总 beat 数不应统一乘以 16 来推算字节数。
上述 RTL 计数与结果集成已在后续独立产物中实现，尚待 RTL 验证；HLS C++ 核心仍保留 0，
不能直接用 HLS C/RTL 仿真验收计数器，见 PERFORMANCE.md。
