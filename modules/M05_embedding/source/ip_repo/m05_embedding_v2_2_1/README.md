# 包装修订 2.1：待用户运行封装与集成检查

2.0 在板级 Validate Design 中出现固定时钟频率和 AXI USER 位宽错误，不能作为已完成集成验证的版本。
2.1 保留全部核心 RTL 和寄存器，去掉不用的外部 USER 信号（核心 RUSER/BUSER 接零），
允许外部时钟频率传播。M05 综合时序目标仍是 10 ns，不能据此允许任意更高频率。
build_config 仍代表原核心 0x20010011；包装修订由 VLNV 2.1 和文件 hash 区分。
本版本尚未仿真、综合、板测，不能继承 2.0 的封装通过结论。
在 Vivado 关闭工程后，运行本目录 package_m05_ip.tcl，再关闭封装工程，运行 V2/scripts/create_m05_board.tcl。

以下为接口/操作说明（已有核心验证结果仅适用于核心）：

# M05 V2 IP 封装与使用

## 本次产物与状态

封装对象是带实际性能计数器的 RTL：ABI=2，任务完成后 build_config=0x20010011。
VLNV：`llm-fpga.local:user:m05_embedding_v2:2.1`。
顶层为 `m05_embedding_v2_ip`，内部实例 `core` 使用原 `embedding_v2`。

准备脚本将 19 个核心 RTL 文件与性能版本 manifest 和 OOC 源文件逐一核对 SHA256 后复制。
新增顶层只做固定宽度连线：省略 AXI4 不使用的 WID，六个 LOCK 输出取 HLS 两位信号的低位。
原核心发出的访问都是普通访问（LOCK=0）；不增加寄存器或流水级。
核心 RTL、寄存器地址、查表逻辑和计数口径不变。
新增封装顶层尚未运行仿真或综合，原核心的 PASS 不等同于新 IP 已完成验证。

准备好的目录是 `V2/ip_repo/m05_embedding_v2_2_1/`；执行 Vivado 脚本成功后才会有
`component.xml` 和 IP Catalog 条目。目前交付状态以实际日志为准。
`module_manifest.json` 记录准备时状态，后续封装执行记录在 `packaging_run.txt`，
不将脚本完成提示当作仿真或时序验收。

## 用户操作：Vivado 2021.1

1. 保存当前 OOC 工程，然后选择 **File > Close Project**，保持 Vivado 开着。
2. 在 Tcl Console 执行（当前电脑路径）：

   ```tcl
   source C:/Users/87767/Desktop/work/program/llm-fpga/LLM_FPGA-main/V2/ip_repo/m05_embedding_v2_2_1/package_m05_ip.tcl
   ```

3. 脚本创建单独的封装工程，登记源文件、接口、寄存器、时钟和复位，调用 IP Packager，
   并加入当前工程的 IP Catalog。它不执行综合、实现或仿真。
4. 正常完成时打印 `M05 IP PACKAGE CREATED`。还需要检查此前的 ERROR/CRITICAL WARNING
   及 `ipx::check_integrity` 诊断；将输出反馈后再进行下一阶段。
5. 点击左侧 **IP Catalog**，搜索 `m05_embedding_v2` 或 `M05 Token Embedding`。
   应看到 **M05 Token Embedding V2 with Performance Counters**。

不要对这一封装工程直接 Generate Bitstream。上板需要下一步建立 PS、DDR、AXI、时钟/复位等系统。
本次也不需要重跑 HLS Export RTL；该导出不会自动包含这里后加的性能计数器。

脚本拒绝覆盖已有 component.xml。若执行中报错，保留控制台和文件进行定位，不要盲目反复执行
或删除原 OOC/性能工程。若只因原工程未关闭而报错，关闭原工程后可重新执行。

## IP 的接口

| 接口 | 方向/协议 | 固定宽度与用途 |
|---|---|---|
| m_axi_ids | 主 AXI4 | 地址 64 bit，数据 32 bit；读取 token IDs |
| m_axi_weight | 主 AXI4 | 地址 64 bit，数据 128 bit；读取词表 |
| m_axi_out_r | 主 AXI4 | 地址 64 bit，数据 128 bit；写出 embedding |
| s_axi_ctrl | 从 AXI4-Lite | 地址 9 bit，数据 32 bit；参数、控制、结果寄存器 |
| ap_clk | 输入 | 当前目标 100 MHz，各 AXI 接口共用该时钟 |
| ap_rst_n | 输入 | 低有效，按核心要求在 ap_clk 域同步释放复位 |
| interrupt | 输出 | 高电平中断；可以先使用 PS 轮询完成状态 |

HLS 的三个主接口均保留完整读写通道，封装元数据相应为 READ_WRITE；实际业务方向见上表。
`m_axi_out_r` 是生成器的名称，对应文档中的输出接口。64-bit 地址不代表板上存在 2^64 字节 DDR；
PS 必须提供实际可访问的 DDR 物理地址。接口宽度固定，不提供任意修改位宽的 GUI 参数。

## PS 调用约定

- PS 在 DDR 准备 token_ids（每项 4 字节）、完整逻辑词表（272269312 字节）和输出缓冲区
  （至少 valid_rows × 1792 字节）；IDs 至少 4 字节对齐，词表/输出至少 16 字节对齐。
- 写入三个 64-bit DDR 地址和其余标量参数。词表元素为 BF16，每个 128-bit 数据字包含 8 个元素。
- `run_mode=0` 时 valid_rows 为 1..64；`run_mode=1` 时必须为 1。
  hidden_size=896、vocab_size=151936、abi_version=2、flags=0。
- 根据实际 PS 内存属性进行缓存维护，确保 PL 可见输入、PS 可见输出；这由 PS 程序负责。
- 关闭 auto_restart；写 ap_start，等待完成，读取状态、有效行数及计数结果，然后才能启动下一任务。
- 错误可能已经写出合法前缀，PS 不应把该任务的部分输出送入 M06。
- CRC 功能未实现，output_crc=0；build_config 在任务完成后读取，不是启动身份探测寄存器。

寄存器完整位级语义以 `drivers/xembedding_v2_hw.h` 为准。
控制寄存器 AP_CTRL=0x000，token_ids=0x010，table_r=0x01c，hidden_out=0x028，
run_mode=0x034，valid_rows=0x03c，hidden_size=0x044，vocab_size=0x04c，
abi_version=0x054，job_id=0x05c，flags=0x064。
64-bit 参数的高 32 位紧随低 32 位；偏移均相对于系统分配的控制接口基地址。

结果低地址：status=0x06c、processed_rows=0x07c、total_cycles=0x08c、
compute_cycles=0x0a4、read_beats=0x0bc、write_beats=0x0d4、CRC=0x0ec、build_config=0x0fc。
四个计数器为 64-bit，其余结果为 32-bit。结果 valid 寄存器具有读清除语义，详见驱动头。
compute_cycles 统计存在 IDs 读、词表读或输出写数据握手的周期，不能解读为纯计算单元忙周期。

随附驱动是 HLS C 源码，供后续 PS 工程手动集成；此轮未制作或验证 Vitis BSP 自动绑定。
因此不能保证 Vitis 自动生成 `XEmbedding_v2` 配置表；后续结合 XSA 设置控制基地址并初始化驱动。

## 交付与验证边界

- `hdl/`：完整核心与新增封装顶层；不依赖外部 HLS 工程目录。
- `drivers/`：寄存器定义和 PS 驱动源码。
- `component.xml`、`xgui/`：用户运行 Vivado 封装后生成的 IP 描述及 GUI。
- `module_manifest.json`、`core_manifest.json`：配置、版本、来源、验证范围。
- `SHA256SUMS.prepared`：准备阶段文件清单；不覆盖之后由 Vivado 生成的 component.xml/xgui。
- `package_m05_ip.tcl`、`package_data.tcl`：随包保留的封装脚本与固定端口/寄存器元数据。
- `_packaging_project/`：操作工程，不是 IP 源码交付所必需。

其他 Vivado 工程通过 **Settings > IP > Repository > Add** 添加 `V2/ip_repo`。
复制给其他同学时复制整个 `m05_embedding_v2_2_1` 目录（可排除 `_packaging_project`），
不能只发 component.xml。

原核心已由用户完成行为仿真和独立综合/OOC 分析。OOC 内部 setup/hold 裕量为
+2.289 ns / +0.010 ns，依赖该分析上下文；完整报告仍有接口边界 hold 违例（-1.930 ns）。
本包不启用原 OOC 的输入输出延迟、时钟来源或布线位置约束，由集成系统统一提供。
封装成功只表示可分发、可加入 Catalog；封装边界回归、系统连接检查、集成时序和板测仍待进行。
此包是当前可集成候选版，尚不等同于课题组全部 V2 交付要求完成。

## 重新准备源文件（当前已由助手完成文件准备后不必再执行）

在 `V2/scripts/prepare_ip_package.py` 所在仓库执行 Python 脚本即可生成目录。
脚本只做文件准备，不启动 FPGA 工具；目标目录必须不存在。需更换核心版本时先保留已交付目录，
更新版本及来源记录后再生成，禁止用旧 HLS impl 目录覆盖已加入计数器的 RTL。
