# M07 / M11 / M12 V2 集成包（2026-10-09，含 v2b burst 优化版）

**2026-10-13 当前整机维护版本：** M07/M11 源码已更新为 4-row 资源优化版本，build_config `0x40008041`。
活动交付包在 `ip_v2_resource_fit_20261012/m07`、`m11`；当前整机控制地址为 `0xB0050000`、`0xB0060000`。
当前 flags 为 CRC/PERF/STRICT mask `0xB`，公共性能由 RTL 实测。M07/M11 OOC 综合分别为 28,770 / 19,999 LUT。
详细变化、性能取舍和重新构建说明见 [RESOURCE_FIT_20261013.md](RESOURCE_FIT_20261013.md)。
下面的 `ip/`、`hardware/`、旧地址、8-row 性能及板测记录保留为历史基线，不能作为新整机测试结果。

本包包含集成 V2 三个模块所需的全部代码、IP、硬件产物、权重和测试数据。
详细结果和集成指南见 `docs/m07_m11_m12_v2_report.md`。

| 模块 | 功能 | HLS 顶层 | AXI-Lite 基址 |
|---|---|---|---|
| M07 | QKV Projection | `qkv_projection_v2_hls` | `0xA0000000` |
| M11 | Attention Output Projection | `attention_out_v2_hls` | `0xA0010000` |
| M12 | Residual Add | `residual_add_v2_hls` | `0xA0020000` |

## 状态与版本

本包包含两个版本，接口、寄存器表和权重布局完全相同，PS 侧代码无需修改即可切换：

| 版本 | 目录 | 状态 |
|---|---|---|
| **v2b（主版本，burst 优化）** | `ip/`、`hardware/` | 已通过 host 逐位比对、CSim、RTL CoSim 和整板实现（WNS 1.35 ns）；**尚未上板** |
| v2（板测通过的回退版） | `ip_v2_board_verified/`、`hardware_v2_board_verified/` | AXU9EG 板测正确性 PASS；但实测比 A53 慢 6–9 倍 |

v2b 预计性能（需板测确认）：M11/M07 每行约 1.0/1.3 ms，Prefill 8 行时间与 1 行相同；M12 每行约 2–3 µs。
v2 和 CPU 的对比数据见 `docs/m07_m11_m12_v2_report.md` 第 4 节。

## 目录

```text
include/          fpga_abi_v2.h（统一 ABI）、fpga_axi128.h
hls/M07_*/ M11_*/ M12_*/   HLS 源码、v2 testbench、HLS 脚本、manifest
ip/M07_*/ M11_*/ M12_*/    v2b 的 Vivado IP（component.xml + .zip），可直接加入 ip_repo
ip_v2_board_verified/      v2 的 IP（板测通过的回退版本）
hardware/         v2b 的 system_wrapper.bit / .xsa 及报告
hardware_v2_board_verified/  v2 的 bit / xsa（已修复复位，板测通过）
                  两个目录都包含 psu_init.tcl（两版内容相同）
                  psu_init.tcl、ps_config.tcl、address_map.txt、时序/资源/DRC 报告
                  integrate_m07_m11_m12_v2_axu9eg.tcl（从本包重建整板工程）
reports/M07|M11|M12/       v2b 的 HLS CSynth 和 CoSim 报告
ps_runtime/       PS 侧 C 驱动（fpga_hls_driver.c 等，只做过 mock 测试）
orin_runtime/     Orin 侧 Python 协议/客户端（CommonJobV2）
board_test/       板测程序 main.c、编好的 ELF、XSCT 脚本、板测结果
tools/            pack_v2_weights.py（权重打包 + golden 生成）、test_m07_m11_m12_v2.py
tests/            C++ golden 测试、真实权重 golden 校验、v2b 硬件顶层逐位比对 test_v2_burst_tops.cpp
weights/          Qwen2.5-0.5B 24 层已打包权重（BF16 LE）+ manifest.json + golden/
docs/             总结报告、集成说明、两份接口规范原文
```

## 权重文件（`weights/`）

| 文件 | 形状 | 用途 |
|---|---|---|
| `layerNN_m07_wqkv.bin` | `[36][896][32]` | M07 `w_qkv`（q/k/v_proj 按输出维拼接后，再按 W_OG32_I_L32_V2 打包） |
| `layerNN_m07_bqkv.bin` | `[1152]` | M07 `bias_qkv` |
| `layerNN_m11_wo.bin` | `[28][896][32]` | M11 `weight`（o_proj） |

需要重新生成时运行：

```bash
python tools/pack_v2_weights.py <Qwen2.5-0.5B-Instruct 目录> weights --layers 0-23 --golden-layer 0 --golden-rows 1 8
```

`weights/golden/layer00_rows{1,8}_*.bin` 是第 0 层的输入输出对照数据：

- M07：`x` → `q`、`k`、`v`
- M11：`attn` → `o`
- M12：`a=resid`、`b=o`、`role=0` → `y`

这些数据可以直接拷到板上 DDR，用来和 kernel 输出逐位比较。

## 快速验证（不需要板子）

```bash
g++ -std=c++11 -O2 -ffp-contract=off -I include tests/test_m07_m11_m12_v2.cpp \
    hls/M07_qkv_projection/qkv_kernel.cpp hls/M11_attention_out_proj/attention_output_kernel.cpp \
    hls/M12_residual_add/residual_add_kernel.cpp -o t && ./t              # M07_M11_M12_V2_PASS
g++ -std=c++11 -O2 -ffp-contract=off -I include -I tests tests/check_real_weight_golden.cpp \
    hls/M07_qkv_projection/qkv_kernel.cpp hls/M11_attention_out_proj/attention_output_kernel.cpp \
    hls/M12_residual_add/residual_add_kernel.cpp -o g && ./g weights      # REAL_WEIGHT_GOLDEN_PASS
PYTHONPATH=. python tools/test_m07_m11_m12_v2.py                         # M07_M11_M12_V2_HOST_PASS
g++ -std=c++11 -O2 -ffp-contract=off -I include tests/test_v2_burst_tops.cpp     hls/M07_qkv_projection/qkv_kernel.cpp hls/M11_attention_out_proj/attention_output_kernel.cpp     hls/M12_residual_add/residual_add_kernel.cpp -o b && ./b weights      # V2_BURST_HOST_PASS
```

## 上板（AXU9EG，Vitis 2021.1）

```bash
xsct board_test/smoke_v2_control_plane.tcl hardware/system_wrapper.bit hardware/psu_init.tcl
xsct board_test/run_v2_board_test.tcl hardware/system_wrapper.bit \
     board_test/m07_m11_m12_v2_baremetal.elf hardware/psu_init.tcl result.txt
```

`run_v2_board_test.tcl` 里写死了 nm 的路径 `D:/Vitis/2021.1/...`，换机器时需要修改。
如果 JTAG 报 `DAP status 0x30000021`，需要给板卡断电重启。

## 重建硬件（可选）

```bash
vivado -mode batch -source hardware/integrate_m07_m11_m12_v2_axu9eg.tcl -tclargs <本包根目录> <输出目录> 0 v2b   # 用 v2 时把 v2b 换成 v2
```

## 集成要点

1. 三个模块共用 `common_job_v2` 字段。Decode 时 `valid_rows=1`；Prefill 时每块 1..64 行。
2. 每次调用的步骤：
   - 写入 64-bit 指针（先写低 32 位到 `off`，再写高 32 位到 `off+4`）和各个参数；
   - 对输入输出做 cache flush，然后写 `ap_ctrl=1`；
   - 轮询 `ap_done`，完成后对输出做 invalidate；
   - 检查 `status==0`，并且 `processed_rows==valid_rows`。

   参考实现见 `board_test/main.c` 中的 `run_m07/m11/m12()`。
3. M07 的输出是 head-major 布局：Q 为 `[14][T][64]`，K、V 为 `[2][T][64]`。
4. M12 的 `y` 不能和 `a`、`b` 重叠；M11 的 `y` 不能和 `x` 重叠。
5. 全部 24 层的打包权重约 85 MB，可以常驻在 DDR 中。
