# M14 V2 独立 Decode 验收与 TCP 加载（2026-10-10）

## 结论与范围

独立 Decode、penalty ON、生产 H8/V1 配置的完整 55-run campaign 和严格离线复核
**PASS**。8-step 生产/debug bit-exact、4 类负例、受控 golden-failure 计时、30 次
独立生产性能和 1,000 次连续调用通过。已完成网口大权重上传与错误路径板测。

这不是整体模型或全部设计计划的最终验收：Prefill/penalty OFF 等性能矩阵、HP2 APM、
公共 ABI 去重/冻结、M06 V2 原生 BF16 trace 和全系统端到端集成仍待完成。本轮未修改
生产 HLS/PL、原 BF16 资产、冻结的数值 test ELF 或其 board package。

## 板卡恢复与网络加载

恢复工作时 DDR 读仍报 DAP status 0x3C000021/0x30000021。用户主电源断电重启后
PS/PL 初始化恢复。PC 有线为 192.168.8.12/24，路由优先使用有线；未改 IP/Wi-Fi。

| 上传 | 包含逐块+整份 DDR CRC32 | MiB/s | 结果 |
| --- | ---: | ---: | --- |
| 首次 BF16 TCP 上传 | 8.3877663 s | 30.9565432 | PASS，独立 sentinel PASS |
| 修复版，错误路径后整份重传 | 8.3596202 s | 31.0607712 | PASS，独立 sentinel PASS |
| 默认冻结包复用检查 | 8.3338715 s | 31.1567379 | PASS，sentinel + step_00 生产/debug PASS |

大小 272,269,312 bytes；canonical BF16 SHA256
`4e96b0df6d274768cbb7e72404011853d23349999b658dc2f4dfb3c431ea223f`；全 DDR CRC32
`7acfde43`。端到端 elapsed 包含 TCP 连接、块回应和最终全 DDR 校验，主机发送前
SHA256 扫描、PS 初始化/小 ELF 下载和额外 JTAG sentinel 时间另计。
相对 30 分钟 JTAG 大文件阶段约 215 倍；不是整次初始化的加速比。

### 接收/发送修复与 6 项错误路径

初版完整上传通过，但 7-byte 小分段合法请求后主机等待回应失败。专用诊断窗口显示
header/payload 完整、next=64、CRC 正确，reply 已 queued 却未确认。修复为每次网络循环
对已 queued 回应重试 tcp_output，容忍 ERR_MEM/ERR_BUF，不重复 tcp_write。修复后同一
分段测试通过。驱动 TX ring 暂满是与此行为一致的解释；没有用连接重试掩盖上传失败。
单客户端新连接在旧 FIN 处理之前可被拒绝，测试工具仅在 BEGIN 之前 STATUS 握手重试。

1. 坏 payload CRC 返回 status=3、next=0、ready=false。
2. 7-byte 分段合法 BEGIN/DATA 成功，重复 DATA 序号拒绝。
3. 超过 1 MiB 的 DATA 拒绝。
4. 未完整上传就 COMMIT 拒绝。
5. payload 中途断线，下一连接 STATUS 显示未就绪。
6. 所有块正确、整份 expected CRC 故意错一位：COMMIT 返回 status=5 和真实 DDR CRC，
   STATUS 仍未就绪。随后完整正确重传通过。

初始失败、复现诊断、修复与最终 PASS 日志均保留；有效证据为
`board_runs/20261010_tcp_fault_and_reload/fault_tests_fixed.json`、`reload_fixed.json`、
`sentinels_fixed.log`。上板 loader 已归档到 `tcp_loader_package_20261010/`，
ELF 881,008 bytes，SHA256
`74fa41c0210448fa36d23a4999d2679591bac92f0eee5f1101e305d0ca0d3742`。
`standalone/program_board_tcp.ps1` 默认校验并使用此包；JTAG 大权重脚本作为显式回退保留。
默认包复用的网口日志为 `board_runs/20261010_104740_405_tcp_weight/`，随后生产/debug
run 为 `20261010_104804_857_step_00` 与 `20261010_104807_761_step_00`；生产目录以
`20261010_frozen_step00_production.log` 的 M14_BOARD_STEP_DIR marker 为准。

## 数值、错误与稳定性

campaign：`board_runs/20261010_104006_168_acceptance/campaign.json`。
`summary.json` 严格复核 55 个独立目录、job ID、build/capability、mailbox terminal state、
token/score bits/CRC、计数器和 raw logits SHA256，failures=[]。

- step_00..07 共 8 个生产和 8 个 debug，全量 151,936 logits/step 逐位一致；
  token/score bits/CRC 全部符合修正的相邻归约树 golden。
- 生产无 logits 事务，weight/dot beats=17,016,832，read beats=17,018,131，
  history words=4,748，vocab rows=151,936；debug logits write beats=37,984。
- BAD_ABI/BAD_FLAGS/BAD_DIMENSION/NUMERIC_ERROR：status=1/11/2/9，未处理 rows/weight。
- wrong-golden-CRC 受控失败：FAILED/driver=7，硬件 status=0，正确结果及正 wall ticks
  保留。该 intentional failure 不计入成功性能样本。
- 1,000 次循环中每次检查结果/CRC/token/job 合同，最终 job ID=1000，结果正确；
  最终 mailbox/counter 合同通过，wall time=224.8792944 s。每次周期不单独导出；
  不把单个最终 IP cycle 当作 1,000 个独立性能样本。
- aggregate_cross_check.log 二次严格数值复核 PASS。旧失败记录仍保留并明确排除；
  聚合 latency 包含不同阶段，仅独立 campaign 的 performance 阶段用于以下统计。

## 30 次独立性能统计

每次独立启动/完成、repeat_count=1，之前 3 次 warmup 不进入分布。

| 口径 | Median | P95（nearest rank） | Max |
| --- | ---: | ---: | ---: |
| IP，100 MHz 推导 | 224.868020 ms | 224.868020 ms | 224.868140 ms |
| A53 wall timer | 224.886284 ms | 224.886629 ms | 224.886719 ms |

IP cycle min/max=22,486,802/22,486,814；正常 memory wait 可出现 12-cycle 差异，
work/数值不漂移。旧 11.09 s 对 IP median 比值为 49.3178x（旧 RTT 与新 IP 的跨口径
比较，非同口径端到端声明）。A53 timer 为 33,333,000 counts/s，包含 submit/wait/verify
循环，不含主机 JTAG 启动与结果搬运或权重上传。

原 100 MHz 实现 WNS +3.649 ns、WHS +0.010 ns，DRC error=0，引用既有 2021.1
post-route 报告；本轮不宣称重新实现。内部 AXI 拍计数不是外部 APM/protocol monitor。

## 复用与下一阶段

操作步骤见 `../standalone/TCP_WEIGHT_LOADING.md`；断电后 DDR 清空，优先网口重载。
完整 campaign 入口仍为 `standalone/run_acceptance.ps1`，不会重新加载权重。
昨日 PAUSED campaign 原样保留，STOP_REQUESTED 已归档为 resumed_20261010。

下一阶段先补其余模式与 HP2 外部监测、公共 ABI 和 M06 V2 真实 trace 门槛，再整体集成。
当前仓库检索到公共 ABI 设计文档，未找到独立的公共 V2 ABI 头文件或 M06 V2 BF16 生成
trace；不能把旧 FP16 转 BF16 的模块输入称为全系统 V2 数据。

