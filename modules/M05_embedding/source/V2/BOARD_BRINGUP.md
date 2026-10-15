# M05 AXU9EGB 上板准备

最新状态（2026-10-10）：用户使用 v22 bitstream 完成全部11个独立上板用例。
AP_CTRL首次读取恢复；正常查表、64行、错误处理/恢复、输出guards与性能计数规律均通过。
日志和实际软件/硬件文件归档：`reports/board_test_v22_20261010/`，详见 RESULT.md。
本次板测使用合成数据、DDR_LOW和V1.2启动复位步骤；不表示真实Qwen/全DDR/系统集成验证完成。
下方“待运行/尚未确认”均为此前阶段的历史记录。

2026-10-10 15:23 最新结果：用户完成 v22 布局布线，routed WNS=+3.726 ns、
WHS=+0.010 ns、WPWS=+2.062 ns，失败端点均0，DRC violations=0。
实际生成的 reset_100m netlist 确认 C_AUX_RESET_HIGH=1。
归档 `reports/board_impl_v22_20261010/`；下一步用户生成 v22 bitstream、
导出含 bitstream 的新 XSA，再更新 Vitis 平台和运行配置。尚未确认故障上板消除。

2026-10-10 最新执行结果：用户已成功创建 v22，Validate Design 完成，
`board_config_report.txt` 确认 C_EXT_RESET_HIGH=0、C_AUX_RESET_HIGH=1。
复位极性断言通过，日志无 ERROR/CRITICAL WARNING；证据归档
`reports/board_bd_v22_20261010/`。下一步用户运行 v22 综合与实现。
尚未确认新 bitstream/XSA 生成或修复上板通过。

## 最新定位：v21 辅助复位被永久拉有效（2026-10-10）

用户上板程序停在首次 AP_CTRL 读取，尚未启动 M05。v21 生成 RTL 明确显示
aux_reset_in 为 ACTIVE_LOW、C_AUX_RESET_HIGH=0，实际接常量0，导致互连和M05持续复位。
证据与修复说明：`reports/board_aux_reset_fault_20261010.md`。
已修正 `scripts/create_m05_board.tcl`，显式 C_AUX_RESET_HIGH=1，输出新目录
`build/m05_board_v22/`，Validate后核对辅助/外部/debug复位极性。
待用户创建、综合、实现、生成新bitstream/XSA并更新Vitis硬件平台；尚未验证修复上板通过。
先前v21时序/DRC通过仍是历史事实，但不能标为板级功能通过。

## 最新结果：板级实现通过（2026-10-10）

用户已完成 `build/m05_board_v21/project/m05_board.xpr` 的综合和布局布线。
报告归档：`reports/board_impl_v21_20261010/`。

- Routed timing：WNS +3.726 ns，WHS +0.010 ns，WPWS +2.062 ns；失败端点均为 0。
- check_timing 各类别为 0，所有用户指定的时序约束满足。
- Fully Routed，20348 条可布线网络全部完成，routing errors=0，routed DRC violations=0。
- Methodology 有一项 CLKC-56 Advisory（MMCM 输入经全局时钟缓冲、未指定 LOC）；保留记录。
- 布局后整个测试系统资源：LUT 10167、FF 14975；不是 M05 单核资源。
- M05 时钟约 100 MHz；路由时序报告为 100.005 MHz，BD 传播元数据为 99.998999 MHz，
  两者是工具时钟模型/精度差异，均不是板上实测频率。

下一步由用户点击 Generate Bitstream。尚未确认 bitstream、XSA、PS 测试 ELF 生成，
尚未下载本设计或验证实际 DDR/M05 功能。助手仅阅读并归档报告，没有执行 FPGA 工具。
以下章节为历史记录，其中“等待运行”等描述以本节最新状态为准。

## 最新结果：包装 2.1 与板级 BD 检查已完成

2026-10-10 用户运行成功：包装 Integrity passed，板级 Validate Design 完成，
生成 system_wrapper，日志没有 ERROR/CRITICAL WARNING。证据见 `reports/board_bd_v21_20261010/`。
M05 实际传播时钟为 99998999 Hz（约 99.999 MHz）；复位派生极性检查通过。
三个主口均映射低地址 2 GiB、高地址 2 GiB；控制基地址 0xA0000000。
HP0 AWUSER/ARUSER 位宽警告对应生成 system.v 中两个 PS 输入明确接 1'b0，未悬空。
控制互连 Low-area 模式不支持 WRAP；本测试的软件须用 Device/MMIO 属性进行普通寄存器访问。
这次不是此前 RUSER/WUSER 的 SmartConnect 数据位宽错误，那些错误已不再出现。

下一步用户在 `m05_board_v21/project/m05_board.xpr` 中运行 Run Synthesis，
完成后检查 ERROR/CRITICAL WARNING 和资源结果。尚未确认综合、实现、bitstream 或板测通过。

以下保留修复和创建过程。

## 2026-10-10 板级 DRC 修复（以本节操作为准）

首轮 `create_m05_board.tcl` 已运行到 Validate Design，但失败：
时钟源传播为 99998999 Hz，而包装 2.0 固定为 100000000 Hz；SmartConnect 不接受
32/128-bit 数据通道上的 1-bit RUSER/WUSER；复位 C_EXT_RESET_HIGH 为只读传播参数。
另外自动地址分配把 DDR_HIGH 总线窗口设为 32 GiB，不能作为实际内存容量使用。

已准备独立包装 2.1，原包装 2.0 和失败工程保持原样。修复内容：

- 不固定输入时钟 FREQ_HZ，让频率随实际连接传播，仍以 10 ns 为核心时序目标。
- 三组主 AXI 的 USER 辅助信号不再对外引出；核心 RUSER/BUSER 接零，输出 USER 丢弃。
  HLS 适配器的 I_RUSER/I_BUSER 由 C_USER_VALUE=0 产生，本核心不消费外部 USER。
- 复位极性由 PS pl_resetn0 的 ACTIVE_LOW 属性传播，Validate 后检查派生值为 0。
- 每个数据主口显式映射 DDR_LOW 0x00000000/2GiB 和 DDR_HIGH 0x800000000/2GiB。
- 修正版工程放在 `V2/build/m05_board_v21/project/m05_board.xpr`。

用户操作（助手未执行 Vivado）：

1. 保存并关闭失败工程。
2. 运行新包装目录里的脚本：

```tcl
source C:/Users/87767/Desktop/work/program/llm-fpga/LLM_FPGA-main/V2/ip_repo/m05_embedding_v2_2_1/package_m05_ip.tcl
```

3. 如果封装成功，关闭封装工程，然后运行已修正的建工程脚本：

```tcl
close_project
source C:/Users/87767/Desktop/work/program/llm-fpga/LLM_FPGA-main/V2/scripts/create_m05_board.tcl
```

4. 提交日志，检查最新 Validate Design 与地址/时钟报告后再运行综合。

包装 2.1 未经用户运行，不能称为已修复验证通过。其查表核心和计数器未改，
build_config 仍为 0x20010011；包装变化通过 VLNV 2.1 与文件哈希追溯。
`prepare_ip_v21.py` 是文件准备脚本，不会运行 FPGA 工具。

以下为早期步骤记录，其中旧工程路径仅供追溯。

## 当前状态

2026-10-10 更新：用户成功运行配置检查脚本。报告已归档到 `reports/ps_config_20261010/`。
实际导出为 PS 参考时钟 33.333 MHz、DDR4/64-bit/单颗 16-bit/8192 Mbit、单 rank、
行地址 16 位、列地址 10 位、DDR 请求时钟 1200 MHz、CL=16/CWL=16、UART0 MIO42..43、
PL0 请求时钟 100 MHz。容量配置推算为 4 GiB，与课题组板级文档一致。
SUBPRESET1=Custom 是预设参数被进一步修改后的状态，不表示设置失败。
DP_AUDIO/RPLL 是启用显示外设后的共享时钟提醒，M05 最小工程不需要显示音频外设。
本次仅确认参数生成，尚未证明板上 DDR 初始化/读写成功；下一步建立 M05 最小板级设计。

- 用户确认实际开发板为 AXU9EGB；Vivado 2021.1 已通过 Digilent JTAG 识别 xczu9。
- 用户要求不复用 D:/mlp_residual 工程，本流程未使用它的任何配置。
- 用户下载的厂家资料位于 C:/Users/87767/Desktop/AXU9EG_AXU15EG-master。
- 当前仅核对/准备配置文件，尚未生成 M05 板级 bitstream、XSA 或 PS 测试 ELF；未执行板上初始化/测试。

## 配置来源与静态核对

厂家仓库：https://github.com/alinxalinx/AXU9EG_AXU15EG

仓库 README 标注 Vivado 2020.1；本机使用 2021.1。不能将来源于厂家的脚本直接等同于已验证的本板配置。
原始 ps_config.tcl、create_project.tcl、project_info.tcl 已按原字节复制到
`board/alinx_axu9eg_reference/`，来源及 SHA256 见该目录 source_manifest.json。
保留 create_project.tcl 仅供追溯，不直接运行它；它创建完整外围示例并使用 create_project -force。

已经确认的源文件内容：

| 项目 | 厂家源文件 | 本次判断 |
|---|---|---|
| FPGA | xczu9eg-ffvb1156-2-i | 与项目目标一致 |
| PS DDR 预设 | DDR4_MICRON_MT40A256M16GE_083E | 2021.1 本地 IP 包有同名预设；其初始单颗容量为 4096 Mbit |
| DDR 容量覆盖 | DEVICE_CAPACITY=8192 MBits | 厂家在预设后覆盖为单颗 8 Gbit，不能仅根据预设名称误判实际总容量 |
| DDR 行地址 | ROW_ADDR_COUNT=16 | 厂家明确覆盖 |
| DDR 写延迟 | CWL=16 | 在 create_project.tcl 中另外设置，复制 PS 配置时不能遗漏 |
| UART | UART0，MIO 42..43 | 来自厂家脚本；板端串口连接仍需后续确认 |
| PS DDR 总线 | 本地对应预设为 64 Bit、单颗 16 Bits | 与课题组文档的 PS DDR 64 bit/4 GiB 结构相符，需导出 Vivado 实际值确认 |
| PS 参考时钟 | 课题组板级文档记为约 33.333333 MHz | 厂家 ps_config 未显式设置，需检查展开后的实际值 |

这里的“一致”仅指上述已核对项目，不是整个 AXU9EG 示例与 AXU9EGB 所有外设、板卡修订均兼容的结论。
官方在线手册 PDF 本轮访问未返回可解析正文，不能声称已经逐项重新核对该手册。
当前以项目文档的板级事实、厂家脚本和本地 2021.1 预设作初步交叉检查。
DDR 最终初始化是否成功仍要以后续板上结果为准。

## 第一步：在 Vivado 导出实际 PS 参数

1. 保存并关闭当前工程（File > Close Project），保留 Vivado 打开。
2. 在 Tcl Console 执行：

```tcl
source C:/Users/87767/Desktop/work/program/llm-fpga/LLM_FPGA-main/V2/scripts/inspect_axu9egb_ps.tcl
```

脚本新建 `V2/build/axu9egb_ps_review/project/axu9egb_ps_review.xpr`，
只放入 PS、应用厂家参数并补上其主脚本中的 CWL=16，导出 CONFIG 属性。
完成标记为 `M05 PS CONFIG REPORT CREATED`，报告位于
`V2/build/axu9egb_ps_review/ps_config_report.txt`。

这是配置检查工程，PS 的部分接口未连线是有意的；不要对它执行综合、实现或下载。
脚本不会连接/复位 JTAG，不会初始化 DDR，也不会修改当前板上设计。
助手未执行该 Tcl；需用户运行后检查是否有版本兼容错误及实际参数偏差。

## 后续步骤（尚未完成）

### 第二步：建立 M05 最小板级设计（脚本已准备，尚未执行）

注意：完整 PS 报告中 `PL0_REF_CTRL__ACT_FREQMHZ=96.968727`，而
`PL0_REF_CTRL__FREQMHZ=100` 只是请求值。上一阶段的“100 MHz”不能视作实际输出。
板级脚本根据 PS 实际计算频率配置 Clocking Wizard，请求输出 100 MHz；
输出时钟是否满足 M05 固定频率接口及实现时序，须检查 Validate Design 和后续实现报告。

1. 在 Vivado 保存并关闭配置检查工程（File > Close Project）。
2. Tcl Console 执行：

```tcl
source C:/Users/87767/Desktop/work/program/llm-fpga/LLM_FPGA-main/V2/scripts/create_m05_board.tcl
```

3. 正常结束打印 `M05 BOARD DESIGN CREATED`。先提交日志与生成的
   `V2/build/m05_board/board_config_report.txt` 进行审查，再开始综合。
4. 工程位置：`V2/build/m05_board/project/m05_board.xpr`；Block Design 名为 `system`。

结构：PS HPM0 FPD（32-bit）→控制 SmartConnect→M05 AXI-Lite；
M05 IDs/weight/output 三个主口→数据 SmartConnect→PS HP0（128-bit）→PS DDR。
Clocking Wizard 请求 100 MHz，连接 M05、两个互连与 PS AXI 端口时钟；
PS pl_resetn0 和时钟 locked 接入 Processor System Reset，同步释放下游复位。
PS UART0 保留用于裸机打印；网络、显示、USB、SD/QSPI 等外围在这个 JTAG 测试设计中关闭。
无 PL DDR/MIG，无用户 PL 外部 IO；PS DDR/MIO 使用芯片专用连接。

控制基地址暂定 0xA0000000，仅属于本测试系统；驱动需以后续 XSA/地址报告为准。
数据口只映射 PS DDR 段，实际可达范围见地址报告；M05 的 64-bit 地址不代表任意地址均有效。
M05 interrupt 本阶段不接，PS 使用轮询；无需在板级首次验证中启用中断。
现有 M05 IP 内部代码不变，新增顶层包装会随此板级工程一起综合；板级验证不能冒充此前已完成。

脚本不运行综合/实现/bitstream，也不访问 JTAG。若脚本出错，保留工程和日志定位，
不要直接重复 source（脚本会拒绝覆盖已有输出目录）。

### 后续待完成事项

1. 审查实际 DDR/参考时钟/UART 参数，确认板级适用性。
2. 建立独立 M05 工程，精简非必需外设，设置 100 MHz PL 时钟、同步复位。
3. PS 控制主口连接 M05 AXI-Lite；M05 三个主口经互连接 PS DDR 的 HP0，检查位宽与地址段。
4. 验证设计，用户执行综合/实现/bitstream 生成，导出 XSA。
5. 配套 PS 裸机测试程序，先确认串口和 DDR，再准备 IDs/词表测试行/输出区，运行 M05 并逐 bit 比对。
6. 用户通过 JTAG 加载并记录结果；此后才可报告上板通过。

M05 不使用 PL DDR，因此这个最小测试系统不需要为 PL DDR 建 MIG。
生成的测试数据只用于查表验证，不要求先部署完整 Qwen 推理软件。
