# 当前维护交付：M08 RoPE / M10 GQA Attention ABI V2

源码仍为本目录 `rope.cpp` / `attn.cpp`。已按当前整机适配 AXI-Lite 公共结果、flags、CRC、实测性能、绝对位置和固定 stride 全层 KV Cache。

完整说明和寄存器表见 [README_V2_INTEGRATION.md](README_V2_INTEGRATION.md)。
活动包在 `ip_v2_resource_fit_20261012/m08` / `m10`，工程副本在 `vivado/ip_repo/m08_rope_v2` / `m10_attention_v2`。
最终 source / component hashes、检查状态和数据偏移见 `module_manifest_v2.json`；报告在 `reports_v2_20261013`。

CSim、数值边界、PS MMIO、公用计数器 RTL 和两模块 control RTL cosim 均通过。
两模块各 3 个有效计算任务的 datapath RTL cosim 通过，包含 RoPE 常规 cos/sin、因果 GQA、最后一层 Cache、非均匀 softmax、CRC 开关和尾输出保护。
最大尺寸及所有数值边界的 datapath RTL 回归、整机 FPGA 板测待完成。Vivado 硬件构建状态见工程 `RESOURCE_FIT_M08_M10_20261013.md`。

其余原始工程、脚本和产物保留作为历史交付；当前整机使用上述包及工程根目录的维护构建入口。
