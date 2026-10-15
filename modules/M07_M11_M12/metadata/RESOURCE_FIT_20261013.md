# M07 / M11 资源优化维护记录

当前源码位于原 `include`、`hls/M07_qkv_projection`、`hls/M11_attention_out_proj`；没有移动交付目录。
当前工程 `vivado/proj/qwen/qwen.xpr` 的 M07/M11 控制地址为 `0xB0050000` / `0xB0060000`。
M12 保留原活动实现。本次仅维护 M07/M11。

## 实现变化

- 原生 128-bit 数据、8 BF16 lanes、OG32 权重布局和 weight stream II=1 保留。
- 行共享参数 `OG32_ROW_PAR=4`，动态处理 Prefill 1..64 / Decode 1；build_config `0x40008041`。
- 输出 FP32 缓冲绑定 BRAM；将最终 BF16 转换放在 8-lane 输出写入循环，M07 bias 在投影结束后相加。
- 累加仍按输入索引顺序进行，CRC 覆盖有效输出，flags 接受 mask `0xB`。
- 打包 Verilog 公共计数器实测 native AXI data handshakes 和 accepted-start 到 done 周期，PERF 关闭为 0，未测得的 compute-active 为 0。

## 资源和性能

| 模块 | 原整机 Optimized 总 LUT | 新 OOC Synthesized 总 LUT | 新 FF | 新 DSP |
|---|---:|---:|---:|---:|
| M07 | 61,404 | 28,770 | 35,547 | 175 |
| M11 | 38,898 | 19,999 | 23,926 | 158 |

报告阶段不同，最终以整机 Routed 资源为准。
Decode 仍为一次权重 pass；Prefill 四行共享一次 pass，64 行从原 8 次增加到 16 次。
不能将 II=1 当作完整 Prefill 吞吐不变的保证。HLS `row_blocks` max tripcount 已修正为 16；Verilog 硬件内容一致性检查通过，仅描述性延迟 metadata 更新。

## 构建、包与验证

HLS 2021.1 的 Windows makefile 不能可靠处理本目录名称的括号，使用工程根目录的
`hls/m07_m11_m12_v2_verified` 作为相同源码镜像，`tools/build_resource_fit_hls.tcl` 作为维护构建入口。
活动包在本目录 `ip_v2_resource_fit_20261012`，复制到 `vivado/ip_repo/m07_qkv_projection` / `m11_attention_out`。
旧交付 IP、bitstream / XSA、golden、权重及测试记录保留；不据此宣称新版本已板测。

CSim 检查非零 M07 bias、尾块、有效行、flags、独立 CRC 和对应参考结果，通过。
完整新 RTL datapath cosim 与板测待完成；整机资源/时序及硬件产物见 `vivado/proj/qwen/RESOURCE_FIT_M08_M10_20261013.md`。
