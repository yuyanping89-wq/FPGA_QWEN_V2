# M08 / M10 V2 接口与工程交付说明

更新日期：2026-10-13。原始源码位置保持不变：`rope.cpp`、`rope.h`、`attn.cpp`、`attn.h`。
原交付文件已备份到工程根目录的 `build-review/resource_fit_20261012/baseline/m08_m10_original_source.zip`。

## 接口检查结论

原交付不能直接接入当前整机：结果通过 DDR 指针返回，性能字段使用算法估算，RoPE 位置与历史 cache 访问需要适配。
现维护版本改为 V2 的独立 AXI-MM 算子和 AXI-Lite 结果寄存器；原逻辑函数仅保留为 CSim 兼容入口。

| 检查项 | M08 | M10 |
|---|---|---|
| 综合顶层 | `rope_v2_hls` | `attention_v2_hls` |
| VLNV | `xilinx.com:hls:rope_v2_hls:2.0` | `xilinx.com:hls:attention_v2_hls:2.0` |
| PS 控制基址 | `0xB0090000` | `0xB00A0000` |
| AXI 数据口 | 6 个原生 128-bit master | 4 个原生 128-bit master |
| 有效行 | Prefill 1..64；Decode 1 | Prefill 1..64；Decode 1 |
| ABI / job | `abi_version=2`；接收 `job_id` | 相同 |
| 输出结果 | AXI-Lite 展开 `common_result_v2` | 相同 |
| 支持 flags | CRC / PERF / STRICT，mask `0xB` | 相同 |
| 不支持的 flags | snapshot bit 2 和其他保留位返回 `BAD_FLAGS`，不写输出 | 相同 |
| 外部数值 | BF16，使用共用 RNE / canonical signed quiet NaN 转换 | 相同；点积、softmax、V 加权累加为 FP32 |
| build_config | `0x30010011` | `0x30008011` |

只有控制寄存器不同，公共 flags 和性能语义与 M05–M12 及已对齐的 M14 一致。没有声明 debug snapshot、在线 softmax 或原地计算 capability。

## 布局、位置与 Cache

M08 的 Q / Qrot 是 `[14, valid_rows, 64]` 的 `Q_HTD_V2`；K / Krot 是 `[2, valid_rows, 64]` 的 `KV_HTD_V2`。
cos / sin 分别是 `[max_position, 64]` 的 `ROPE_SD_V2`。第 row 行使用绝对位置 `position_base + row`，按照前后 32 维的 rotate-half 规则计算。
同一块内的每个 head 使用 `valid_rows` 为 stride，不能传入以固定 64 行 padding 的张量。

M10 的 Q 输入仍是 `[14, valid_rows, 64]`；context 输出是 `[valid_rows, 896]` 的 `ACT_TH_V2`，可直接交给 M11。
K / V 指针必须是整个 `[24, 2, 2048, 64]` Cache 的起始地址，每个 tensor 12,582,912 bytes。
硬件使用固定 2048 的 head stride，自行加入 layer 和 KV head 偏移；不能传入某层局部指针或按 `cache_len` 压缩的 Cache。
GQA 映射是 Q head 0..6 -> KV head 0，Q head 7..13 -> KV head 1。

`causal=1` 时第 row 行读取 key 0..`query_pos_base+row`，同时包含历史块和当前块已写入的位置。
`causal=0` 时读取完整 `cache_len`。PS 在 M09 完成后才启动 M10；M10 不更新软件的已提交 Cache 长度。
`scale` 必须为有限正 FP32；当前模型使用 `1/sqrt(64)=0.125`。
softmax 使用减去最大值后计算 exp 和归一化，只保存一个 query 的 2048 项片上 scores / exps，不输出完整 attention 矩阵。

## 控制寄存器

所有偏移都相对于模块控制基址，来自最终 HLS `*_hw.h`。指针占 64 bit，低字在表中偏移，高字在偏移 +4。
本工程数据 buffer 位于低于 4 GiB 的 PL DDR 窗口，高字写 0。`0x000` 是标准 HLS AP_CTRL。

| M08 偏移 | 参数 | M10 偏移 | 参数 |
|---:|---|---:|---|
| 0x010 | q_raw | 0x010 | q_rot |
| 0x01C | k_raw | 0x01C | k_cache |
| 0x028 | cos_table | 0x028 | v_cache |
| 0x034 | sin_table | 0x034 | attn_out |
| 0x040 | q_rot | 0x040 | run_mode |
| 0x04C | k_rot | 0x048 | layer_id |
| 0x058 | run_mode | 0x050 | query_pos_base |
| 0x060 | valid_rows | 0x058 | valid_rows |
| 0x068 | position_base | 0x060 | cache_len |
| 0x070 | max_position | 0x068 | scale（FP32 原始位） |
| | | 0x070 | causal |

| 两模块公共偏移 | 字段 | 宽度 |
|---:|---|---:|
| 0x078 | abi_version | 32 |
| 0x080 | job_id | 32 |
| 0x088 | flags | 32 |
| 0x090 | result_status_code | 32 |
| 0x0A0 | result_processed_rows | 32 |
| 0x0B0 | result_perf_cycles_total | 64 |
| 0x0C8 | result_perf_cycles_compute | 64 |
| 0x0E0 | result_perf_read_beats | 64 |
| 0x0F8 | result_perf_write_beats | 64 |
| 0x110 | result_output_crc | 32 |
| 0x120 | result_build_config | 32 |

结果含 HLS ap_vld 控制寄存器，完整地址表在生成驱动头文件中。PS 等待 ap_done 后读结果；不要在 IP 运行时更新参数，也不要开启 auto-restart。

## Flags、CRC 与性能语义

- bit 0 CRC：IEEE CRC32，反射多项式 `0xEDB88320`，初始值 `0xFFFFFFFF`，最终取反。按 BF16 内存顺序，每元素低 byte 先。
  M08 的 CRC 连续覆盖全部 Qrot，再覆盖全部 Krot；M10 覆盖全部 context。不包含 padding。
- bit 1 PERF：打包后的 Verilog 实测 accepted-start 到 ap_done 的周期数，包含首尾采样边沿、AXI 等待和可选 CRC。
  read / write beats 累加所有原生 master 上的 `RVALID && RREADY` / `WVALID && WREADY`，不计 AR/AW/B。
  CRC 的输出读回也计入 read beats。全部数据口原生宽度是 128 bit，因此此处一个数据 beat 为 16 bytes。
- 没有独立 compute-active 信号，`perf_cycles_compute=0` 表示未测得；不再用算法工作量冒充实测周期。
  关闭 PERF 或使用不支持的 flags 时四个公共计数结果为 0，覆盖前一事务的快照。
  合法 flags 下的其他参数错误可以报告实际 start-to-error-completion 周期。
- bit 3 STRICT：统一 PS 驱动在启动前检查 64-byte 对齐、完整 buffer 范围和输出 alias，且不依赖该 bit 才启用。
  当前 HLS 内核不进行物理窗口检查；独立使用 PL IP 时，调用者必须执行同样检查。CSim 的 STRICT 分支只检查主机指针对齐和 alias。

CSim 没有 RTL 时钟和 AXI 握手，性能结果为 0。重新运行 HLS export 会覆盖 RTL 后处理，必须重新注入公共计数器再打包，不能直接使用未经注入的 HLS IP。

## 验证证据与边界

- CSim 覆盖 1 / 9 / 64 行、绝对 RoPE 位置、末位置 2047、尾输出不越界、独立 CRC、错误 ABI / flags / 参数和输出 alias。
- M10 覆盖 layer 23、完整 2048 历史、跨块 query base、causal / noncausal、GQA 分组；与独立 double softmax reference 比较，BF16 误差阈值为 `0.006 + 0.006*abs(reference)`。
- 数值转换独立检查全部 65,536 个 BF16 编码，并检查 FP32 ties-to-even、Inf、NaN、signed zero、下溢和舍入溢出。
- PS MMIO 测试覆盖寄存器参数、PL DDR 范围、64-byte 对齐、输出重叠、错误 build_config、IP error 和有界超时。
- 公共性能计数器 RTL 仿真覆盖多端口同时握手、首尾边沿、清零、复位及非法 flags。
- M08、M10 的 control RTL co-simulation 均通过，覆盖 snapshot / snapshot+PERF 拒绝、错误 ABI 和错误模式；报告在 `reports_v2_20261013`。此项不覆盖正常计算 datapath。
- 两模块各 3 个小规模有效任务的 datapath RTL co-simulation 通过：M08 包含 3 行位置 17、末位置 2047、常规 cos/sin；M10 包含因果 Prefill、layer 23 Decode 和独立 double 参考的非均匀 softmax。验证输出 layout、CRC 读回、关闭 CRC 后清零及尾输出保护。
- 当前 revision 3 将 M08 的 Q/K CRC 读回分别绑定到独立模板实例，修正 HLS 2021.1 复用带 AXI 指针的 CRC 子函数时生成重叠读口选择条件的问题；主运算和公共寄存器偏移保持现有定义。
- 工程检查逐项对比原 BD：原实例、外部端口、连线、时钟复位、约束和原地址均保留；只新增 M08、M10 和一个数据 SmartConnect。

正常 RTL 用例覆盖小规模有效计算，未覆盖最大尺寸和全部数值边界的 RTL 回归，也未进行 FPGA 板测。
当前待完成两算子真实 AXI stalled traffic、板级 Cache 可见性、整机数值和 tokens/s 测量。最终综合、布局布线、bitstream / XSA 状态以工程根目录的 `build-vivado/qwen_full_v2_resource_fit_20261012` 和整机交付记录为准。

## 复现与 Vitis 使用

在工程根目录运行：

```powershell
vitis_hls -f tools/build_resource_fit_hls.tcl -tclargs m08 verify final
vitis_hls -f tools/build_resource_fit_hls.tcl -tclargs m10 verify final
```

小规模有效计算 RTL 复现使用 `tools/prepare_attention_datapath_cosim.py m08 crc3` 和
`tools/run_attention_datapath_cosim.tcl m08 crc3`（M10 同理），在隔离项目中运行。
若该隔离项目已存在，直接运行后者即可；不会重置交付 HLS 项目。

export 与公共计数器注入 / Vivado 打包流程见 `tools/package_resource_fit_ip.py`。
生成的 IP 在 `ip_v2_resource_fit_20261012/m08` 和 `m10`；工程的活动仓库分别为 `vivado/ip_repo/m08_rope_v2` 和 `m10_attention_v2`。
旧包保存在 `_previous` / Vivado `_backup` 中，只有显式仓库列表中的包参与当前工程。

PS 端真实驱动位于 `vitis_ps_lwip/src/qwen_placeholders.c`（为兼容手动导入的 Vitis 工程保留文件名）。
在手动创建的 lwIP standalone 工程导入该源包，使用包含 M08 / M10 的新 XSA，按源码包 README 配置 BSP 和权重文件。
应用通过 CDMA staging RoPE 表、权重和张量到 PL DDR，按照 M08 -> M09 -> M10 调度。
