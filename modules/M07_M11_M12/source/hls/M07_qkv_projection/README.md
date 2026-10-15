# M07 当前 V2 交付实现

活动 top 为 `qkv_projection_v2_hls`，VLNV `xilinx.com:hls:qkv_projection_v2_hls:3.1`，core revision `2026101202`，build_config `0x40008041`。

PS 控制基址为 `0xB0050000`；全部数据 master 位于 PL DDR `0x80000000..0xBFFFFFFF`。

外部 BF16、内部 FP32；128-bit 数据口，每拍 8 个 BF16；主权重 MAC II=1。Prefill 按 4 行共享权重，接受 1..64 行；Decode 只接受 1 行。输出暂存 FP32 BRAM，再转换成 BF16。Bias 在完整投影之后相加。

CRC/PERF/STRICT 支持 mask `0xB`；snapshot/reserved flags 拒绝。CRC 是有效 BF16 输出低字节在前的 IEEE CRC32；PERF 是实际 RTL 周期和原生 AXI 数据握手拍数，不可独立测量的 compute/stall 返回 0。PS 在任何 MMIO 写入前检查完整地址范围、64-byte 对齐和输出重叠。

当前编译入口（在工程根目录执行）：

```powershell
vitis_hls -f tools/build_resource_fit_hls.tcl -tclargs m07 csim final
vitis_hls -f tools/build_resource_fit_hls.tcl -tclargs m07 synth final
```

构建脚本会自动将原目录的 C/C++ 源码和头文件同步到无括号路径 `hls/m07_m11_m12_v2_verified`。活动交付包在原模块根目录 `ip_v2_resource_fit_20261012`，工程仓库由显式列表指定。

非零数据/bias、CRC 和尾块 CSim、公共计数器 RTL、PS MMIO 测试已通过；完整算子 RTL datapath 和板级推理尚未验证。64 行 Prefill 的权重 pass 从 8 增为 16，吞吐不能视为与旧版相同。

历史脚本和报告保留；当前版本使用上述构建入口。最终整机资源、时序及 XSA 状态见 `vivado/proj/qwen/RESOURCE_FIT_M08_M10_20261013.md`。
