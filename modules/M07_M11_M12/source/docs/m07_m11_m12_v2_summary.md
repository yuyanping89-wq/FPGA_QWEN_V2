# M07 / M11 / M12 V2 版本总结

## 1. 版本范围

V2 版本统一实现以下三个 FPGA 推理模块：

| 模块 | 功能 | 仓库目录 | V2 顶层函数 |
| --- | --- | --- | --- |
| M07 | QKV Projection | `module/04_qkv_projection` | `qkv_projection_v2` / `qkv_projection_v2_hls` |
| M11 | Attention Output Projection | `module/10_attention_out_proj` | `attention_out_v2` / `attention_out_v2_hls` |
| M12 | Residual Add | `module/11_residual_add` | `residual_add_v2` / `residual_add_v2_hls` |

V2 严格按照统一接口文档和统一集成设计执行，保留 Prefill/Decode 双模式。

## 2. 统一 ABI

核心接口定义位于：

- `include/fpga_abi_v2.h`
- `include/fpga_axi128.h`

统一作业字段包括：

- `run_mode`
- `valid_rows`
- `hidden_size`
- `layout_id`
- `abi_version`
- `job_id`
- `flags`
- `common_result_v2`

协议约束：

- Prefill 支持 `1..64` 行。
- Decode 必须使用 `valid_rows=1`。
- 外部数据统一使用 BF16。
- HLS 外部 AXI 数据口统一为 128 bit，每拍 8 个 BF16。
- 地址、范围和 buffer alias 由 PS/M03 descriptor 管理。
- M12 输出 buffer 不允许与输入 buffer 重叠。

`fpga_axi128.h` 在 HLS 环境使用 `ap_uint<128>`，在 Host CSim 环境使用兼容的 128-bit 结构体，保证同一套 kernel 可以进行板前 CSim 验证。

## 3. M07 QKV Projection

代码位置：

- `module/04_qkv_projection/hls/qkv_kernel.hpp`
- `module/04_qkv_projection/hls/qkv_kernel.cpp`

功能和布局：

- 输入 `x`。
- 输入 packed Q/K/V 权重 `w_qkv`。
- 输入 `bias_qkv`。
- 输出 `q`、`k`、`v`。
- 权重布局为 `W_OG32_I_L32_V2`。
- Q/K/V 输出保持 head-major 布局。
- Q、K、V 使用独立 AXI Master 访问口。

M07 的 6 路 AXI Master：

```text
gmem0: x
gmem1: w_qkv
gmem2: bias_qkv
gmem3: q
gmem4: k
gmem5: v
```

## 4. M11 Attention Output Projection

代码位置：

- `module/10_attention_out_proj/hls/attention_output_kernel.hpp`
- `module/10_attention_out_proj/hls/attention_output_kernel.cpp`

功能和布局：

- 输入形状为 `[T,896]`。
- 输入 packed `Wo` 权重。
- 输出形状为 `[T,896]`。
- BF16 输入和输出，FP32 累加。
- `OUT_PAR=8`，输出通道按 8 路展开。

M11 的 3 路 AXI Master：

```text
gmem0: x
gmem1: weight
gmem2: y
```

## 5. M12 Residual Add

代码位置：

- `module/11_residual_add/hls/residual_add_kernel.hpp`
- `module/11_residual_add/hls/residual_add_kernel.cpp`

功能和布局：

- 输入 `a`、`b`。
- 输出 `y`。
- 输入和输出形状均为 `[T,896]`。
- 支持 `residual_role`。
- 输出不能与任意输入 buffer 重叠。
- 主计算循环综合结果达到 `II=1`。

M12 的 3 路 AXI Master：

```text
gmem0: a
gmem1: b
gmem2: y
```

## 6. AXI-Lite 结果接口

V2 的结果字段通过 AXI-Lite 输出寄存器暴露，不再通过 DDR 写入短的 `result_words` 结构。

统一结果字段包括：

- `result_status_code`
- `result_processed_rows`
- `result_perf_cycles_total`
- `result_perf_cycles_compute`
- `result_perf_read_beats`
- `result_perf_write_beats`
- `result_output_crc`
- `result_build_config`

对应 PS 驱动：

- `ps_runtime/include/fpga_hls_driver.h`
- `ps_runtime/src/fpga_hls_driver.c`
- `ps_runtime/src/fpga_buffers.c`
- `ps_runtime/src/ip_registry.c`

驱动包含 AXI-Lite 寄存器访问、64-bit 地址配置、启动/完成轮询、超时处理以及 cache flush/invalidate 路径。

## 7. HLS 构建与 IP 导出

HLS 脚本：

- `module/04_qkv_projection/hls/run_hls_axu9eg_v2_2021.tcl`
- `module/10_attention_out_proj/hls/run_hls_axu9eg_v2_2021.tcl`
- `module/11_residual_add/hls/run_hls_axu9eg_v2_2021.tcl`

关键配置：

```text
m_axi_max_widen_bitwidth = 128
M07/M11 OUT_PAR = 8
```

最新导出的 IP 目录：

- `module/04_qkv_projection/hls/m07_v2/f/impl/ip`
- `module/10_attention_out_proj/hls/m11_v2/f/impl/ip`
- `module/11_residual_add/hls/m12_v2/f/impl/ip`

为规避 Vivado 2021.1 在 2026 年日期 revision 上的 Tcl signed integer 溢出，当前 IP revision 为：

```text
M07: 2026092807
M11: 2026092808
M12: 2026092809
```

## 8. Vivado Block Design

集成脚本：

- `vivado/integrate_m07_m11_m12_v2.tcl`

严格接口版 memory SmartConnect 结构：

```text
M07: 6 个 AXI Master
M11: 3 个 AXI Master
M12: 3 个 AXI Master
总计: 12 个 AXI Master
```

AXI VIP 用作板前 DDR 替代模型，AXI-Lite VIP 用于模拟 PS 寄存器访问。

最新输出目录：

- `vivado/bd_out7`
- `vivado/bd_out7/m07_m11_m12_v2_axi_vip.xsa`

该版本已通过 Vivado 2021.1 的 `validate_bd_design`，并成功生成 XSA。

旧的 `bd_out4`、`bd_out5`、`bd_out6` 保留为历史生成物，不作为当前 V2 严格接口版的最终集成结果。

AXI-VIP smoke bench 和脚本：

- `vivado/m07_m11_m12_v2_axi_vip_tb.sv`
- `vivado/run_m07_m11_m12_v2_axi_vip_sim.tcl`

控制寄存器地址由 SmartConnect 映射为：

```text
M07: 0x0000_0000
M11: 0x0001_0000
M12: 0x0002_0000
```

## 9. 已完成验证

已完成或通过：

- M07 CSim
- M07 CSynth
- M11 CSim
- M11 CSynth
- M12 CSim
- M12 CSynth
- M11 reduced CoSim
- M12 full-size CoSim
- C++ kernel golden test
- Python/NumPy golden test
- PS driver mock test
- Vivado Block Design validation
- IP repack 和 XSA 导出

测试代码位置：

- `tests/test_m07_m11_m12_v2.cpp`
- `tools/test_m07_m11_m12_v2.py`
- `ps_runtime/tests/test_v2_driver.c`

已通过的关键输出：

```text
M07_M11_M12_V2_PASS
M07_M11_M12_V2_HOST_PASS
FPGA_HLS_DRIVER_MOCK_PASS
M07_M11_M12_V2_BD_READY
```

## 10. 加速情况

相比旧版本，V2 已使用以下 FPGA 资源优化：

- M07/M11 的 8 路输出并行。
- accumulator 完全分区。
- lane 循环完全展开。
- M12 主计算循环达到 `II=1`。
- 128-bit AXI 宽口，每拍搬运 8 个 BF16。
- packed weight 布局，减少无效搬运和地址计算。
- M07 的 Q/K/V 使用独立访问口。
- BF16 数据格式配合 FP32 累加。

当前综合估计约为：

```text
M07 Fmax: 136.99 MHz，权重循环实际 II 约 8
M11 Fmax: 136.99 MHz，权重循环实际 II 约 3
M12 Fmax: 136.99 MHz，主计算循环 II=1
```

M07/M11 当前主要瓶颈是 packed weight 的 AXI/存储端口数量，因此 8 路展开不等于端到端 8 倍加速。后续可继续做 weight banking、BRAM tile/cache、weight prefetch 和 dataflow。

## 11. 内存和显存说明

当前流程不加载完整 LLM 模型，不需要 GPU 显存。主要消耗 CPU、系统内存和磁盘：

- 单个 Vivado 进程工作集约 1.3 GB。
- 三个 Vivado 任务并行时可能占用数 GB 系统内存，因此建议串行执行。
- HLS CSim、CSynth、IP 导出和 BD 验证均可在没有 FPGA 板的情况下完成。
- 生成的仿真、综合和 IP 文件会占用磁盘空间。

## 12. 仍需进一步板级验证的内容

以下内容尚未被当前板测覆盖：

- 真实 AXI 带宽、burst 行为和单个 kernel 的硬件延迟（当前只测了整个测试程序的总耗时）。
- 更大的 Prefill 行数（当前板测为 1/8 行，ABI 允许 1..64）。
- 板级功耗。
- 与其他模块拼接后的端到端推理延迟和数值正确性。
- 真实硬件 TCP 服务稳定性。

## 13. 板测结果（2026-10-09）：PASS

### 13.1 首轮 TIMEOUT 的根因与修复

首轮板测停在阶段码 `1201`，并留下 `DAP status 0x30000021`。原因是 BD 中 `proc_sys_reset` 的复位连接错误：
`C_AUX_RESET_HIGH=0`（低有效），而 `aux_reset_in` 接到了常量 0，导致 aux 复位**一直有效**，
两个 SmartConnect 和三个 HLS IP 一直处于复位状态，AXI-Lite 访问无应答。所以这不是 kernel 数值错误。

修复位置：`vivado/integrate_m07_m11_m12_v2_axu9eg.tcl`，将 `aux_reset_in`（以及 `dcm_locked`）改接 `const_one`。
旧目录 `vivado/axu9eg_v2_hw/` 中的产物包含该错误，**不要再使用**。

### 13.2 修正版硬件产物

```text
vivado/axu9eg_v2_hw_fixed/system_wrapper.bit
vivado/axu9eg_v2_hw_fixed/system_wrapper.xsa
vivado/axu9eg_v2_hw_fixed/system_wrapper_timing_summary.rpt   WNS 3.988 ns, WHS 0.010 ns, TNS/THS 0
vivado/axu9eg_v2_hw_fixed/system_wrapper_utilization_impl.rpt
vivado/axu9eg_v2_hw_fixed/system_wrapper_drc.rpt              0 error（4 条 DSP PCIN tie-off warning）
```

HLS kernel 未改动，因此不需要重新跑 HLS。新工程生成的 `psu_init.tcl` 与原 Vitis 工作区中的版本逐字节一致，原 ELF 可以直接复用。

### 13.3 控制面 smoke

板卡断电重启后，`vitis/smoke_v2_control_plane.tcl` 的结果为：

```text
M07_AP_CTRL A0000000: 00000004   (ap_idle)
M11_AP_CTRL A0010000: 00000004
M12_AP_CTRL A0020000: 00000004
V2_SMOKE_RESULT=PASS
```

该脚本现在会明确输出 PASS/FAIL，并在第一次 AXI 访问失败时立即退出，避免 DAP 再次被卡死。

### 13.4 数值板测

`vitis/run_v2_board_test.tcl` 使用修正版 bitstream 和 `workspace_v2/.../m07_m11_m12_v2_baremetal.elf`，**连续运行两次均为 PASS**：

```text
V2_BOARD_RESULT=PASS target_elapsed_us=2378675   (vitis/v2_board_result_fixed.txt)
V2_BOARD_RESULT=PASS target_elapsed_us=2378650   (vitis/v2_board_result_fixed_run2.txt)
```

覆盖用例（每个用例都与 CPU golden 逐个 BF16 精确比较，并检查输出末尾之后的 guard 元素没有被改写）：

| 阶段码 | 模块 | 用例 |
|---|---|---|
| 1201 | M12 | Decode，valid_rows=1，residual_role=0 |
| 1208 | M12 | Prefill，valid_rows=8，residual_role=1 |
| 1299 | M12 | 非法 Decode（valid_rows=2）：须返回 status=2、processed_rows=0，输出不被改写 |
| 1101 / 1108 | M11 | Decode 1 行 / Prefill 8 行 |
| 701 / 708 | M07 | Decode 1 行 / Prefill 8 行（Q/K/V head-major 输出） |

注意：`target_elapsed_us`（约 2.38 s）包括测试数据生成和 CPU golden 计算，**不是 kernel 延迟**，不能作为加速比依据。
旧的失败记录保留在 `vitis/v2_board_result.txt`（TIMEOUT，使用的是有复位错误的旧 bitstream）。

Vivado 2021.1 的 AXI-VIP XSIM smoke bench 还可能受到自动生成 VIP package 名称差异影响；该问题属于工具/仿真工程命名问题，不改变 M07/M11/M12 的 kernel 算法、ABI 或已通过的 CSim、CSynth、CoSim、BD validation 结果。
