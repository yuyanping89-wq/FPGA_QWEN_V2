# M14 V2 数值差异闭环与恢复门禁（2026-10-09）

## 结论

step_00 的逐位差异来自 Python 最后 8 lane 的归约顺序错误。修正 reference 后，
归档实板的 151,936 个 raw logits 全部 exact，相同 SHA256、score bits 和 CRC；
无需放宽任何数值门槛，也无需修改生产 HLS/RTL 或重建 bitstream。
当前独立模块合同为 `bf16_rne_h8_interleave16_fp32_adjacent_tree_v2`，见
`../NUMERIC_CONTRACT.md`。整体公共 ABI 仍需单模块验收之后再统一。

## 根因与可复现证据

16-way partial 的归约两端均按距离 8、4、2、1 执行；差异仅在最后 8 lane：

- 原 Python：先 0+4、1+5、2+6、3+7，再按距离 2、1 合并；
- 已有 C++/RTL：先 0+1、2+3、4+5、6+7，再合并相邻 pair。

`scripts/diagnose_reduction.py` 保持输入、产品和 interleave 累加不变，只替换最终
lane tree，就把 exact_count 从 72,260 提升到 151,936，最大绝对误差降为 0。
候选并未由板上数据生成，而是直接由 BF16 权重与 hidden 重新计算。

| 项目 | 修正后的 reference / 归档实板 |
| --- | --- |
| raw SHA256 | `d7ada7553c6ff9bc48899776732e8269ca791874363cf64cc17a4aebe1245acc` |
| token | 104198 |
| score bits | 1101012901 |
| CRC | 2154663276 |
| exact / absolute / relative / ULP | 全部逐位一致，所有误差为 0 |

原比较文件保留；新比较位于 `numeric_20261009/corrected_logits_comparison.json`，
根因候选统计位于 `numeric_20261009/reduction_diagnosis.json`。

新增反例的 BF16 lane products 为 `[2^24,1,-2^24,1,0,0,0,0]`：相邻树输出 1，
旧树输出 2。另一个 token 分数为 1.5，因此旧 reference 还会选错 token。Python
和 HLS testbench 对正确树输出、selected token/score bits/CRC 作精确检查。

## 离线验证

- Python：5 个数值用例 + 3 个验收脚本用例，共 8/8 通过。
- Vitis HLS 2021.1 CSim：PASS、0 errors；证据 `numeric_20261009/csim.log`。
- RTL CoSim：7/7、C pre/post-check PASS；仍使用交接中已记录的 `hidden_size_r`
  向量别名恢复，证据 `numeric_20261009/cosim_recovered.log` 与 `xsim.log`。
- 修正的 8-step golden 已全部重新计算，token 均不变；权重/hidden/history 哈希
  不变，ASSET_VERIFY=PASS steps=8；全部旧 raw/processed/result 和两个旧 manifest
  保存至 `numeric_20261009/legacy_golden/`，附 SHA256SUMS。
- SOURCE_CONTRACT=PASS arguments=33 axilite_ports=34。
- ELF 重新构建 PASS，required symbols 完整且零 undefined symbol。
- 本地 mock XSCT 回归证明：FAILED mailbox + 错误 raw hash 仍先保存 per-run JSON
  和一次聚合结果，计数器 22,486,841 完整保留，然后明确失败。此回归未操作板卡，
  证据 `numeric_20261009/result_capture_test.log`。

CSim/CoSim 的本轮新增反例为 compact 32×64 配置；生产尺寸的全量数值证据来自
现有 step_00 实板归档。这不能替代另外 7 步的生产尺寸实板验证。

## 测试框架修复

`main.c` 的运行失败与 golden 失败路径，在发布 FAILED 前保存结束时间和 wall_ticks。
前置初始化/host validation 失败不属于运行计时；运行失败也保存 timer frequency。
新 ELF 尚未上板，wall_ticks 的真实计时结果仍待验证。

`run_step.ps1` 在 transport/hash/数值失败前先解析 mailbox，保存
`mailbox_decoded.json` 并追加聚合记录；错误信息包含具体失败类型与 run directory。
新增 ELF 包哈希检查，所有 runner 默认使用 `board_package_20261009`，同时支持
显式 `-PackageDir`。

`compare_board_results.py` 默认精确检查 score bits/CRC，要求 DONE、driver_status=0
及正确工作量；失败记录不进入性能分布。旧全零记录与旧 golden 的诊断 FAIL 均
显式标记 `excluded_from_acceptance`，原硬件结果保留，不重写为成功。

## 当前构建包

| 资产 | SHA256 |
| --- | --- |
| bit | `d6223bcf8b36f5cbe2032c4aa6e75f248438fbe9faa4e6aae36ee41951a736ef` |
| XSA | `df88b0a2ae6b6a99d00bf6cd0f38970e4a08356f3bd8bdedd170a8a1902c8a9c` |
| 新 ELF | `cf22f2cc8b8c812a1848fc2f475ed67afce589c440bed34940d3b0429a3cfabd` |
| 新 map | `a99c7dc56f6aa357ae77fd2fe7a4f45382ad12dd862485c3c5fe195bd949e6e0` |

新 ELF 281,336 bytes，text/data/bss=49,160/2,064/20,796；LOAD file/memory=
0xe048/0x13100。当前包为 `board_package_20261009/`；20260930 原包完整保留。

## 板卡与网络现场

2021.1 XSCT 在受限环境内 connect 因临时目录权限失败；在允许的本地环境重跑，
枚举 PS TAP/PMU/PL，读取 0x40000000 时返回
`AXI AP transaction error, DAP status 0x3C000021`。后一次只读检查枚举到
`DAP (AXI AP transaction error, DAP status 0x30000021)`，无可用 PSU target。
因此不能确认 DDR 权重 sentinel，也不能声称 PL 保持原配置；未 stop/reset/program
或下载任何资产。证据为 `board_runs/20261009_retained_state*.log`。

用户询问以太网后只读检查：Realtek 有线 Up/1 Gbps，IPv4=192.168.0.12/24；
旧网络程序的 FPGA 地址为 192.168.8.190/24。有线与其不同子网，现有路由把该
目标交给 Wi-Fi 192.168.8.110。实际 ping 无板卡 reply，无 ARP；一次 host-unreachable
来自电脑 Wi-Fi，不能当作 FPGA reply。本轮 M14 standalone 没有 lwIP/以太网服务，
采用 JTAG mailbox，因此 ping 不通不是模块数值/性能验收结论，也不是 DAP 错误根因。
没有修改电脑 IP、路由或网络程序。

## 下一步

按交接要求，获得用户对状态变更的确认后，先确认主供电/JTAG，再执行 PS 初始化、
PL 配置与固定 DDR 权重加载（272,269,312 bytes，约 30–31 分钟）。匹配 bit/XSA/
ELF 已准备，DDR 地址布局不变。先验证 sentinel，再用新 ELF 做 step_00 生产与
logits 回归，继而 8-step、30 次独立稳态采样、1,000 次稳定性、4 类负向测试。
30 次性能分布应采集 30 个独立任务，不能把 RepeatCount=30 的单个最终周期值
误报为 30 个样本。全模块验收通过前不开始整体集成。
