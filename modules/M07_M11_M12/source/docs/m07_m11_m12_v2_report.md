# M07 / M11 / M12 V2 总结报告（2026-10-09）

## 1. 结论一览

| 项目 | 状态 |
|---|---|
| 统一 ABI v2（Prefill/Decode 双模式） | 完成，三个模块使用同一套字段和状态码 |
| C++ / Python golden、CSim、CSynth | PASS |
| Vivado BD → 综合 → 布局布线 → bitstream/XSA | PASS，WNS 3.988 ns、WHS 0.010 ns，0 未布线 |
| **真实板卡数值正确性**（AXU9EG） | **PASS**（共 3 次运行，与 CPU 逐个 BF16 比较） |
| **真实板卡性能** | **已实测：目前比 A53 CPU 慢**（M12 约 5.9 倍，M11/M07 约 9 倍），原因已定位，见第 4 节 |
| **Burst 优化版 v2b** | 已实现；host 逐位比对、CSim、RTL CoSim 均 PASS；预计比 CPU 快约 5–50 倍，**待上板确认**，见 4.4 节 |

一句话：**接口和正确性已经可以用于集成；性能优化版 v2b 已完成离线验证，等上板实测。**

## 2. 模块与接口

| 架构编号 | 功能 | 仓库目录 | HLS 顶层 | AXI-Lite 基址 |
|---|---|---|---|---|
| M07 | QKV Projection | `module/04_qkv_projection` | `qkv_projection_v2_hls` | `0xA0000000` |
| M11 | Attention Output Projection | `module/10_attention_out_proj` | `attention_out_v2_hls` | `0xA0010000` |
| M12 | Residual Add | `module/11_residual_add` | `residual_add_v2_hls` | `0xA0020000` |

公共约定（`include/fpga_abi_v2.h`）：

- `abi_version=2`；`run_mode`：0=Prefill（`valid_rows` 1..64），1=Decode（`valid_rows` 必须为 1）。
- `hidden_size=896`；外部数据统一使用 BF16 little-endian，内部用 FP32 累加，输出时 RNE 舍入为 BF16。
- 权重布局 `layout_id=6`（`W_OG32_I_L32_V2`），排布为 `[ceil(O/32)][I][32]`。
- M07 输出 Q `[14][T][64]`、K/V `[2][T][64]`，均为 head-major；M11/M12 的输入输出为 `[T][896]`。
- 结果通过 AXI-Lite 寄存器读回（`result_status_code`、`result_processed_rows` 等），不写 DDR。
- 地址、对齐和 alias 由 PS 侧的 buffer descriptor 负责检查。

### 2.1 AXI-Lite 寄存器表（来自生成的 `*_hw.h`）

| 字段 | M07 | M11 | M12 |
|---|---|---|---|
| ap_ctrl | 0x00 | 0x00 | 0x00 |
| 指针（各 64 bit） | x 0x10, w_qkv 0x1c, bias 0x28, q 0x34, k 0x40, v 0x4c | x 0x10, weight 0x1c, y 0x28 | a 0x10, b 0x1c, y 0x28 |
| run_mode | 0x58 | 0x34 | 0x34 |
| valid_rows | 0x60 | 0x3c | 0x3c |
| hidden_size | 0x68 | 0x44 | 0x44 |
| layout_id / residual_role | 0x70 | 0x4c | 0x4c（role） |
| abi_version | 0x78 | 0x54 | 0x54 |
| job_id | 0x80 | 0x5c | 0x5c |
| flags | 0x88 | 0x64 | 0x64 |
| result_status_code | 0x90 | 0x6c | 0x6c |
| result_processed_rows | 0xa0 | 0x7c | 0x7c |
| perf_cycles_total / compute | 0xb0 / 0xc8 | 0x8c / 0xa4 | 0x8c / 0xa4 |
| perf_read / write_beats | 0xe0 / 0xf8 | 0xbc / 0xd4 | 0xbc / 0xd4 |
| output_crc / build_config | 0x110 / 0x120 | 0xec / 0xfc | 0xec / 0xfc |

## 3. 板级验证

平台：AXU9EG（`xczu9eg-ffvb1156-2-i`），PL 时钟 100 MHz；12 路 128-bit AXI Master 经 SmartConnect 接到 `S_AXI_HP3_FPD`。

### 3.1 修过的硬件问题

首轮板测超时的原因：`proc_sys_reset` 的 `aux_reset_in`（低有效）接到了常量 0，导致 SmartConnect 和三个 IP 一直处于复位状态。
修复位置是 `vivado/integrate_m07_m11_m12_v2_axu9eg.tcl`（改接 `const_one`）。修复后控制面 smoke 读到三个 IP 的 `ap_ctrl=0x4`（ap_idle）。

### 3.2 正确性用例（全部 PASS）

| 阶段码 | 用例 |
|---|---|
| 1201 / 1208 | M12 Decode 1 行（role 0）/ Prefill 8 行（role 1） |
| 1299 | M12 非法 Decode（rows=2）→ status=2、processed_rows=0、输出不被改写 |
| 1101 / 1108 | M11 Decode 1 行 / Prefill 8 行 |
| 701 / 708 | M07 Decode 1 行 / Prefill 8 行 |

每个用例都和 CPU 结果逐个 BF16 精确比较，并检查输出末尾之后的 guard 元素没有被改写。

## 4. 实测性能（`vitis/v2_board_perf_result.txt`）

测量方法：

- FPGA 时间：写 `ap_start` 到读到 `ap_done`，跑 5 次取最小值（5 次结果差异小于 0.01%）。
- "含 cache"：再加上 flush 和 invalidate 的时间。
- CPU 基线：单核 A53 @ 1.33 GHz，`-O2`，对 row-major 权重做标量 BF16→FP32 计算，跑 3 次取最小值，输出与 FPGA 逐位一致。

| 用例 | FPGA kernel | FPGA 含 cache | A53 CPU | FPGA/CPU 加速比 |
|---|---|---|---|---|
| M12 Decode 1 行 | 73.7 µs | 75.2 µs | 12.5 µs | **0.17×** |
| M12 Prefill 8 行 | 572.7 µs | 577.1 µs | 98.7 µs | 0.17× |
| M11 Decode 1 行 | 59.66 ms | 59.76 ms | 6.68 ms | **0.11×** |
| M11 Prefill 8 行 | 477.3 ms | 477.4 ms | 53.4 ms | 0.11× |
| M07 Decode 1 行 | 77.30 ms | 77.43 ms | 8.60 ms | **0.11×** |
| M07 Prefill 8 行 | 618.4 ms | 618.5 ms | 68.9 ms | 0.11× |

### 4.1 为什么慢：已定位

所有 `*_v2_hls` 顶层都把 128-bit AXI 指针 `reinterpret_cast` 成 16-bit `data_t*`，然后在计算循环里**逐个元素访问 DDR**：

- 例如 M11 每次迭代读 1 个 x 和 8 个权重，地址不连续，HLS 无法推断出 burst。
- 结果是每次访问都要承受一次完整的 DDR 往返延迟。

实测和理论周期的对比（100 MHz 下）：

| 模块 | 理论（II=1） | 实测 | 平均每次迭代 |
|---|---|---|---|
| M11（每行 896×896/8 = 100,352 次迭代） | 1.0 ms | 59.7 ms | 约 59 周期 |
| M07（每行 1152×896/8 = 129,024 次迭代） | 1.29 ms | 77.3 ms | 约 60 周期 |
| M12（每个元素） | 约 1 周期 | 约 8.2 周期 | — |

另外，耗时与行数严格成线性关系（8 行正好是 1 行的 8 倍），说明 Prefill 也没有复用权重：每一行都重新从 DDR 读一遍完整的权重。

**之前的总结里写的"8 路并行、128-bit AXI、II=1"只是 pragma 的设定，真实硬件上并没有生效。** 这也是此前没有实测数据时不能宣称加速的原因。

### 4.2 与 V1 的对比

V1 的 IP 不在当前 bitstream 中，**V1 从未在板上测过，所以没有 V2 相对 V1 的实测加速比**。

- **按 HLS 综合估计**（假设理想内存）：以 M11 为例，V1 是 4 路输出、II=4，`input_loop` 3603 周期 × 224 个 tile，约 8.1 ms/行；V2 是 8 路、II=3，约 3.0 ms/行。V2 约快 2.7 倍。
- **真实硬件上**：V1 的权重访问 `weight[(out+lane)*H+in]` 同样是逐元素、跨步的 DDR 访问，会遇到第 4.1 节的同一个瓶颈。预计 V1 在板上同样受 DDR 延迟限制，V2 相对 V1 的优势不会明显。
- V2 的主要收益在**接口层面**：统一的 ABI、Prefill/Decode 双模式、结果寄存器、packed 权重布局（为 burst 优化做了准备），以及经过板级验证的正确性。

### 4.3 优化方向（只改 HLS 内部，不改 ABI 和寄存器）

1. **按 128-bit beat 做 burst 读写**：外部口直接用 `fpga_axi128_t` 顺序访问。packed 布局下同一个 32-lane 组的 8 个输出在一个 beat 里正好连续，现在的布局已经适合 burst，只是代码没有这样写。
2. **把 x 行缓存到片上 BRAM**：896×BF16，每行只读一次。
3. **Prefill 复用权重**：按权重 tile 外层循环、行内层循环，权重只读一遍，供全部 `valid_rows` 使用。
4. **M12 改为每拍处理 8 个元素**（a、b、y 各一个 128-bit beat），理论约 112 周期/行，即约 1.1 µs。

按理论估算，优化后 M11/M07 每行约 1–1.3 ms（相对 CPU 约 5–7 倍），M12 每行约 1–2 µs。这些是估算值，需要重新跑 HLS 和板测才能确认。

### 4.4 Burst 优化版（v2b，已完成离线验证，待上板）

上述优化已经实现，称为 **v2b**：ABI、寄存器表、权重布局都不变，AXI-Lite 寄存器表经 diff 与 v2 逐项相同。

**代码改动**

| 文件 | 内容 |
|---|---|
| `include/fpga_og32_linear_v2.h`（新） | M07/M11 共用的 packed 权重线性计算核心 |
| `module/04_qkv_projection/hls/qkv_kernel.cpp` | `qkv_projection_v2_hls` 改走 `qkv_burst()` |
| `module/10_attention_out_proj/hls/attention_output_kernel.cpp` | `attention_out_v2_hls` 改走 `attention_out_burst()` |
| `module/11_residual_add/hls/residual_add_kernel.cpp` | `residual_add_v2_hls` 改走 `residual_add_burst()` |

原有的标量参考函数 `*_v2()` 保留不动，作为 golden。

**实现要点**

- 权重整块顺序读：HLS 推断出 burst，M07 长 129024 拍、M11 长 100352 拍，都是 II=1。
- 输入行一次 burst 读入片上 `xbuf`。
- 8 行一组共用同一次权重读取，所以 Prefill 不再每行重读一遍权重。
- 同一个输出的 FP32 累加顺序与参考实现完全相同。用 4 级轮转寄存器隔开对同一个累加器的相邻更新，因此 II=1 时仍然逐位一致。
- M12 每拍从 a、b 各读 8 个元素、写出 8 个结果，三个 AXI 口都是 burst，II=1。
- m_axi 参数：`max_*_burst_length=64`，读 outstanding 8/16，`latency=64`。

**验证（全部 PASS）**

- `tests/test_v2_burst_tops.cpp`：rows 取 1/2/7/8/9/17/64，加上错误路径和真实第 0 层权重，与参考实现逐位一致 → `V2_BURST_HOST_PASS`。
- 三个模块的 CSim 通过。
- 缩小尺寸的 **RTL CoSim** 通过（H=64，随机数据，Decode 1 行 + Prefill 9 行）。脚本是各模块的 `run_hls_axu9eg_v2_burst_cosim_2021.tcl`。

**CSynth 资源（单个 IP）**

| 模块 | BRAM18K | DSP | FF | LUT | 64 行最大延迟 |
|---|---|---|---|---|---|
| M07 | 336 | 446 | 67k | 81k | 10.6 ms |
| M11 | 176 | 320 | 40k | 53k | 8.2 ms |
| M12 | 48 | 16 | 6k | 8k | 73 µs |

三个模块合计约占 LUT 52%、DSP 31%、BRAM 31%。

**预计性能（综合周期数 × 10 ns，需板测确认）**

| 用例 | 旧 v2 实测 | v2b 估算 | A53 CPU 实测 |
|---|---|---|---|
| M12 1 行 | 73.7 µs | 约 2–3 µs | 12.5 µs |
| M11 1 行 / 8 行 | 59.7 / 477 ms | 约 1.0 / 1.0 ms | 6.7 / 53.4 ms |
| M07 1 行 / 8 行 | 77.3 / 618 ms | 约 1.3 / 1.3 ms | 8.6 / 68.9 ms |

**构建**

- HLS 脚本：`run_hls_axu9eg_v2_burst_2021.tcl`，工程在 `m07_v2b`、`m11_v2b`、`m12_v2b`。IP revision 已固定为 2026100910/11/12。
- 整板：`integrate_m07_m11_m12_v2_axu9eg.tcl ... 0 v2b`，输出目录 `vivado/axu9eg_v2b_hw/`。

**整板实现结果**（`vivado/axu9eg_v2b_hw/`）：

- 时序：WNS 1.350 ns、WHS 0.010 ns、TNS/THS 0，所有时序约束都满足。
- 资源：LUT 106,841（39%）、FF 126,697（23%）、BRAM tile 94（10%）、DSP 783（31%）。
- DRC：0 error，只有 DSP 流水线类 warning/advisory。
- `psu_init.tcl` 与 v2 逐字节一致，现有板测 ELF 可以直接使用。

上板命令：

```bash
xsct vitis/smoke_v2_control_plane.tcl vivado/axu9eg_v2b_hw/system_wrapper.bit <psu_init.tcl>
xsct vitis/run_v2_board_test.tcl vivado/axu9eg_v2b_hw/system_wrapper.bit      vitis/workspace_v2/m07_m11_m12_v2_baremetal/Debug/m07_m11_m12_v2_baremetal.elf <psu_init.tcl> vitis/v2b_board_perf_result.txt
```

旧的 v2 IP 和 bitstream 都保留，可以随时回退。

## 5. 新代码位置

### 5.1 公共接口

| 文件 | 内容 |
|---|---|
| `include/fpga_abi_v2.h` | `common_job_v2` / `common_result_v2`、状态码、layout/dtype、`fpga_validate_job_v2()` |
| `include/fpga_axi128.h` | 128-bit beat 类型（HLS 用 `ap_uint<128>`，host 用结构体） |

### 5.2 HLS kernel（每个模块的 `hls/`）

| 模块 | kernel | testbench | HLS 脚本 | 导出 IP |
|---|---|---|---|---|
| M07 | `qkv_kernel.{hpp,cpp}` | `qkv_v2_tb.cpp` | `run_hls_axu9eg_v2_2021.tcl` | `hls/m07_v2/f/impl/ip` |
| M11 | `attention_output_kernel.{hpp,cpp}` | `attention_output_v2_tb.cpp` | 同上 | `hls/m11_v2/f/impl/ip` |
| M12 | `residual_add_kernel.{hpp,cpp}` | `residual_add_v2_tb.cpp` | 同上 | `hls/m12_v2/f/impl/ip` |

旧顶层（`qkv_proj`、`attention_output_projection`、`attention_residual_add`）都保留着，没有改动。

### 5.3 Vivado / 硬件

| 文件 | 内容 |
|---|---|
| `vivado/integrate_m07_m11_m12_v2_axu9eg.tcl` | AXU9EG 整板 BD + 实现脚本（已修复复位） |
| `vivado/axu9eg_v2_hw_fixed/system_wrapper.{bit,xsa}` | **当前有效**的硬件产物 |
| `vivado/axu9eg_v2_hw_fixed/*.rpt` | 时序、资源、DRC 报告 |
| `vivado/axu9eg_v2_hw/` | 旧产物，含复位 bug，**不要使用** |

### 5.4 PS 软件

| 文件 | 内容 |
|---|---|
| `ps_runtime/include/fpga_hls_driver.h`、`src/fpga_hls_driver.c` | 通用驱动 `fpga_hls_run_m07/m11/m12()`（只做过 mock 测试，见 6.2） |
| `ps_runtime/include/fpga_platform_config.h` | 三个 IP 的基址 |
| `ps_runtime/tests/test_v2_driver.c` | 驱动 mock 测试 |
| `orin_runtime/protocol.py`、`fpga_runtime_client.py` | Orin 端 `CommonJobV2` / `run_operator_v2` |

### 5.5 测试

| 文件 | 内容 |
|---|---|
| `tests/test_m07_m11_m12_v2.cpp` | C++ golden（直接调用 HLS 源码） |
| `tools/test_m07_m11_m12_v2.py` | NumPy golden（需要 `PYTHONPATH=Fpga_deploy_qwen25`） |
| `vitis/src/main.c` | **板测程序**：正确性 + 每个 kernel 的计时 + A53 基线 |
| `vitis/src/main_correctness_only.c.bak` | 加计时之前的板测程序备份 |
| `vitis/smoke_v2_control_plane.tcl` | 控制面 smoke（失败时立即退出） |
| `vitis/run_v2_board_test.tcl` | 下载并运行，从 ELF 自动解析符号地址，写出 PASS/FAIL 和性能数据 |
| `vitis/rebuild_v2_baremetal_perf.tcl` | 以 `-O2` 重新编译板测 ELF |

## 6. 后续集成指南

### 6.1 一个 Attention 块里的调用顺序

```text
x_norm ──M07──> Q[14][T][64], K[2][T][64], V[2][T][64]   (head-major，直接给 RoPE/KV-cache/Attention)
                   ... RoPE / KV cache / QK softmax / AV（其他模块） ...
attn[T][896] ──M11──> o[T][896]
residual[T][896] + o ──M12 (residual_role=0)──> h[T][896]
...MLP...
h + mlp_out ──M12 (residual_role=1)──> 下一层输入
```

- 同一层内三个模块共用同一个 `job_id`、`run_mode`、`valid_rows`。
- Decode 阶段 `valid_rows=1`；Prefill 时把 prompt 按 ≤64 行分块。
- M12 的 `y` 不能和 `a`、`b` 重叠，要用 ping-pong buffer。M11 的 `y` 也不能和 `x` 重叠。

### 6.2 每次调用 PS 侧必须做的事

1. 写入 64-bit 指针（低 32 位写在 `off`，高 32 位写在 `off+4`），然后写 run_mode、valid_rows、hidden_size、layout/role、abi_version=2、job_id、flags。
2. 对输入和输出 buffer 做 `Xil_DCacheFlushRange`，然后写 `ap_ctrl=1`。
3. 轮询 `ap_ctrl` bit1（ap_done），完成后对输出做 `Xil_DCacheInvalidateRange`。
4. 检查 `result_status_code==0` 且 `result_processed_rows==valid_rows`。

`vitis/src/main.c` 里的 `run_m07/m11/m12()` 就是按这个流程写的、在板上验证过的最小参考实现。
`ps_runtime/src/fpga_hls_driver.c` 实现了同样的流程（`fpga_hls_run_m07/m11/m12()`，结果读回到 `uint64_t result_words[8]`），但目前只在 host 上做过 mock 测试。集成时建议先在板上用驱动 API 把第 3.2 节的用例重跑一遍，再替换板测程序里的直接 MMIO。

### 6.3 权重准备（离线完成，放在 DDR 常驻）

- M07：把 `q_proj/k_proj/v_proj` 按输出维拼成 `[1152][896]`（Q 0..895、K 896..1023、V 1024..1151），bias 也按同样顺序拼成 `[1152]`，然后打包成 `[36][896][32]`。
- M11：`o_proj` `[896][896]` 打包成 `[28][896][32]`。
- 打包公式：`packed[(o/32)*I*32 + i*32 + (o%32)] = W[o][i]`。

### 6.4 集成前建议先完成

1. 完成第 4.3 节的 HLS 访存优化，重新导出 IP，重新构建 bitstream，然后用 `run_v2_board_test.tcl` 再测一遍。正确性和性能会一起输出。
2. 在板上验证 `fpga_hls_driver`，然后把板测程序切换到驱动 API。
3. 加入 64 行 Prefill 用例，测试 MAX_ROWS 边界。
