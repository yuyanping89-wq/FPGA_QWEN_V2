# M14 V2 网口加载权重

2026-10-10 状态：独立 A53 loader 已通过完整 BF16 网口传输、每块和全 DDR CRC32、
独立 sentinel 及 6 项错误路径实板验证。错误路径后完整重传为 **8.3596202 秒、
31.0607712 MiB/s**（272,269,312 bytes，DDR CRC32 `7acfde43`）。
当前 V2 测试 ELF 仍使用 JTAG mailbox；它启动后没有 lwIP，ping/TCP 不再响应属于预期。

## 下次使用步骤

PC 有线地址设为 `192.168.8.12/24`，接板卡 PS 网口；板卡 loader 固定
`192.168.8.190/24`，TCP 端口 `1414`。无需改 Wi-Fi 或默认路由。

在项目根目录 PowerShell 执行：

```powershell
$pythonExe = 'C:\Users\ben\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
# 新上电：PS init + 原 PL bit + 小 loader ELF 走 JTAG，大权重走 TCP。
& .\module_14_lm_head_v2\standalone\program_board_tcp.ps1

# 若当前 PL/DDR 已初始化，且没有板测运行，可只替换 A53 程序：
# & .\module_14_lm_head_v2\standalone\program_board_tcp.ps1 -RetainHardware

# 加载/全部校验 PASS 后，替换回数值验收 ELF，DDR 权重保留。
& .\module_14_lm_head_v2\standalone\run_step.ps1 -Step step_00 -PythonExe $pythonExe
& .\module_14_lm_head_v2\standalone\run_step.ps1 -Step step_00 -WriteLogits -PythonExe $pythonExe
```

默认使用已板测冻结包 `reports/tcp_loader_package_20261010/` 中的 loader ELF 和
`psu_init.tcl`，启动前验证 ARTIFACTS.sha256，首次使用无需重新编译 BSP。
如需修改源码，可运行 `standalone/build_tcp_loader.ps1` 仅本机构建，产物位于
`standalone/build/tcp_loader/m14_tcp_loader.elf`；验证新版本后另建带 ARTIFACTS.sha256
的新包，通过 `-LoaderPackageDir` 指定，保留旧包。
它使用同一冻结 XSA
生成的独立 lwIP BSP，不覆盖既有 standalone BSP、测试 ELF 或冻结板测包。BSP 使用
既有 AXU9EGB generic PHY patch `ref/xemacpsif_physpeed.c`；平台定时器/中断文件复用
`realFpga/vitis/workspace/lm_head_net_diag/src/platform*`（保留 Xilinx 许可证）。
代码/数据/堆限制在低 256 MiB，避开从 `0x40000000` 开始的权重区。

`program_board_tcp.ps1` 保存 start log、upload.json、进度 log、sentinel log 和 loader
哈希到 `reports/board_runs/*_tcp_weight/`。失败即停止，不继续调用 IP，不自动回退
30 分钟 JTAG 传输。旧 `program_board.ps1` 仅保留为明确选择的 JTAG 回退工具。

## 校验与协议

- 主机发送前检查 canonical 文件长度 `272269312` 和 manifest 的 SHA256
  `4e96b0df6d274768cbb7e72404011853d23349999b658dc2f4dfb3c431ea223f`，不接受 FP16。
- 每块最多 1 MiB，校验 payload CRC32 后写固定 DDR 地址；flush/invalidate 后从 DDR
  回读计算 CRC32，再返回确认。单连接、顺序 offset/sequence，客户端逐块等待回应。
- COMMIT 用 64 KiB 切片重新扫描整个 DDR，期间继续服务 lwIP 定时器；最终 CRC32
  必须等于整份主机文件 CRC32，才置 ready。CRC32 是传输/存储错误校验，板端没有 SHA256。
- PC 再查询 ready，并通过 JTAG 独立校验首尾 16 bytes；loader 不直接启动 LM Head IP。
- 不完整、越界、重复/乱序、非法 header、断线、60 秒无活动或 CRC 错误都会拒绝当前上传。
  不支持断点续传。错误后整份重传，未 COMMIT 的权重不得用于推理。

线格式为 little-endian 32-byte `<8I>`：magic=`M14W`、version=1、op、seq、offset、
payload size、固定 total、CRC32。op：BEGIN=1/DATA=2/COMMIT=3/STATUS=4；response
op OR `0x80000000`，offset 表示 next，size 字段改为 status（0 成功），CRC 字段为回读值。
非法 header 会断开，合法 DATA 的 CRC 错误返回失败状态。服务只面向直连实验网。

## 实测速率与口径

历史同体积旧网口服务曾测得 `7.6917031 s` / `33.758 MiB/s`
（`realFpga/DEBUG_LOG.md`，旧 FP16 应用），与约 30–31 分钟 JTAG 相比约 240 倍。
新 loader 增加逐块和全 DDR 校验，必须以 `upload.json` 的端到端 elapsed/throughput
为准，不能承诺复现旧速度。1 Gbit/s 的约 2.18 秒只是无开销理论下限。

2026-10-10 修复版实测 8.36 秒，含逐块和整份 DDR 校验，约为 30 分钟 JTAG 的 215 倍。
`reports/board_runs/20261010_tcp_fault_and_reload/reload_fixed.json` 为实测数据。
IP 测试期间不需要重新上传权重。

## 错误路径回归与诊断

在没有 IP 测试运行时，启动 loader 后可执行：

```powershell
& 'C:\Users\ben\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' `
  .\module_14_lm_head_v2\standalone\test_tcp_loader_board.py --full-commit-fault `
  --report .\module_14_lm_head_v2\reports\board_runs\tcp_faults.json
```

该脚本会故意使已加载权重失效；之后必须完整重传。6 项包括坏块 CRC、7-byte 分段
合法帧及重复序号拒绝、超大块、截断 COMMIT、payload 中断线、全 DDR CRC 不匹配。
全 DDR 测试发送所有正确块但故意把 expected CRC 改一位，确认 COMMIT 返回真实回读 CRC
和 status=5、ready=false。2026-10-10 修复版全部 PASS。

初版在细分小包测试中出现回应停留在发送队列的问题。必须在 `tcp_write` 成功后继续
调用 `tcp_output`，重试 ERR_MEM/ERR_BUF，直到 sent 回调确认；不得重复 tcp_write 回应。
单客户端旧连接的 FIN 可能晚于新连接 SYN，负例工具只在发送 BEGIN 前做 STATUS 握手重试。
上传本身不跳过失败。

loader 在 `0x51001000..0x510013FF` 保存最近 16 条协议事件，与 weight/mailbox 分离。
`scripts/read_tcp_diagnostics.tcl <binary_dump>` 可只读保存诊断。每条 16 个 u32：
seq/event/detail/header_used/payload_used/op/wire_seq/size/next/active/ready/
reply_pending/reply_queued/commit_pending/crc/reserved。故障初始证据与修复后的日志均保留。

2026-10-09 的“编译完成、未上板”状态已被本次实测替代；旧暂停报告作为历史证据保留。

