# PS GEM3 / Marvell PHY 网络适配交付

完整操作步骤见 [JTAG 运行、SD 权重与无 UART 上板测试教程](D:/FPGA_PRO/FPGA_QWEN/docs/Qwen_V2_上板测试教程_SD启动_无UART_20261015.md)。
上板前先运行 `board_preflight.py`；历史交付成功不代表后续重新生成 BSP 后补丁仍然保留。

日期：2026-10-15。当前工程 `D:/FPGA_PRO/FPGA_QWEN/vitis/qwen_ps_app`，
参考工程 `D:/FPGA_PRO/rms_kv_cache_workspace`。

## 先做的 diff

完整记录在 `build-review/network_marvell_20261015/DIFF_BEFORE.md`，
lwIP 文件级清单为同目录 `lwip_diff_summary.json`，逐行 .diff 同时保留。

| 项目 | 差异 | 处理 |
|---|---|---|
| platform_zynqmp.c / platform.h | 完全相同 | 保留 |
| platform_config.h | include 与宏排列不同，均使用 GEM3 0xFF0E0000 | 保留 |
| main.c | 原 Qwen 入口默认 IP 为 192.168.1.10；本次按要求改为参考工程使用的 192.168.8.190 | 保留 Qwen 推理入口，仅更新静态 IPv4 与网关 |
| xemacpsif_physpeed.c | 参考明确识别 Marvell，并为未知 PHY 提供 Generic 检查 | 迁移；补齐 Marvell 超时与诊断 |
| lwipopts.h | 5 项网络缓冲配置不同 | 通过 BSP 配置迁移 |
| GEM 时钟分频 | 当前 12/1、60/1、60/10；参考 8/1、40/1、50/8 | 保留当前 XSA 生成值 |

Marvell 原始初始化函数在两边相同。迁移参考工程的厂商识别后，
只有 MDIO ID1=0x0141 才执行 Marvell 私有页配置；其他厂商按参考选择相应路径。
参考中 ZCU102 的 I2C 扩展器复位代码依赖该板专用宏和引脚，不适用于当前配置。

## 实际修改

1. 应用域 BSP 的网络参数：
   mem_size=524288、memp_n_tcp_seg=512、pbuf_pool_size=512、tcp_snd_buf=32768、tcp_wnd=32768。
   xilffs use_lfn=2、stdin/stdout=none 保留。
2. Marvell 初始化沿用参考顺序：page 2 / MAC control 配置 RGMII TX/RX 延迟；
   回到 page 0 设置 10/100/1000 广告、Pause、downshift；BMCR 启动协商并软件复位。
   新增 MDIO 错误检查、最多 5 秒等待复位清除、最多 30 秒等待协商及链路完成；
   读取 Marvell 状态中的链路/速度，不把无链路或保留速度值误报成 10 Mbps。
3. 新增网络专用 `qwen_phy_debug` 快照，无需 UART；状态更新后清理其 64-byte Cache 行。
4. 用当前 BSP 的头文件重新编译整个应用域 BSP，同步当前生成的 liblwip4.a / libxil.a /
   libxilffs.a 与头文件到平台导出目录，再强制重编并链接应用 ELF。
5. 使用当前平台 FSBL、原有选定 bitstream、新 ELF 生成 BOOT.BIN，保持原来的三分区启动流程。

原网络迁移时，32 个应用文件在 PHY 适配构建期间均受哈希保护。本次 IP 更新只修改 `main.c` 的
静态地址和网关；算子运行框架、MMIO 驱动、性能采集、TCP 服务与协议保持原样。
没有迁移参考工程的算子、opcode 或服务器；没有直接使用参考工程二进制库或时钟配置。
本次 ELF/BOOT 与离线预检记录见 `delivery_manifest.json`。

## 可以使用的产物

- ELF：`vitis/qwen_ps_app/qwen_app/Debug/qwen_app.elf`
- 独立打包镜像（本次 JTAG 上板流程不使用）：`vitis/qwen_ps_app/qwen_app_system/Debug/sd_card/BOOT.BIN`
- 实际打包 BIF：`vitis/qwen_ps_app/network_support/network_boot.bif`

本次测试由 Vitis/JTAG 初始化 PS、下载 bitstream 并运行 ELF；SD 卡根目录仅保留 `qwen_model/` 权重，
无需复制 BOOT.BIN。默认应用仍为 **192.168.8.190:5000**，PC 应使用同网段地址，
例如 192.168.8.100 / 255.255.255.0，网关 192.168.8.1。

## 没有 UART 时如何判断网口卡在哪里

在 Vitis 调试器中加载这次 ELF 的符号，观察 `qwen_phy_debug`：

| stage | 含义 |
|---:|---|
| 0 | 尚未进入 PHY 配置；可能停在网卡/MDIO 探测之前 |
| 1 | 读取 PHY ID1/ID2 |
| 2 | 配置 Marvell RGMII、广告和 downshift |
| 3 | 等待 BMCR 软件复位清除 |
| 4 | 等待自协商与链路建立 |
| 5 | 链路已解析，查看 link_mbps（10 / 100 / 1000） |
| 6 | 失败，查看 error、last_reg、mdio_status、bmcr、bmsr、phy_status |

error：0 正常；1 MDIO 读失败；2 MDIO 写失败；3 复位超时；4 协商超时；
5 无链路/速度无法解析；6 PHY ID 无效。
phy_address 是实际 MDIO 地址，phy_id1 / phy_id2 是实际读回值。
该交付没有连接开发板，不能预先断言实际 PHY 的 ID2 或协商速度。

同时观察应用原有 `qwen_debug_state`：stage=3 是 SD 模型装载，stage=4 是模型/TCP 服务就绪。
**应用 stage=4 不替代物理链路检查**；还应确认 PHY stage=5、网线连接与 PC 子网正确。
模型约 1 GB 装载完成前，TCP 5000 不监听；网络初始化已完成时仍可先尝试 ping。

若 PHY ID 为全 0/全 F、MDIO 读取失败或灯始终不亮，下一步检查板卡电源、PHY 硬复位脚、
参考时钟与网线连接。此包执行的是参考 Marvell MDIO 软件复位，没有猜测板级 GPIO/I2C 引脚。

## 避免以后重新生成 BSP 覆盖补丁

补丁保存在工程自己的 network_support 下，没有修改全局 Xilinx SDK。
Vitis 的 Regenerate BSP 会重新产生默认 PHY 源码，之后从仓库根目录执行：

```powershell
python vitis/qwen_ps_app/network_support/rebuild_network.py
```

它依次完成 BSP 网络配置与重新生成、安装本地 PHY 补丁、BSP 编译、导出库同步、
应用重编链接、BOOT.BIN 打包，并检查已有应用源码未被改变。
若 BSP 配置已经生成，可使用 `--skip-config`。
每个子步骤的失败都会停止后续流程，日志写入 `build-review/network_marvell_20261015`。
在 Vitis 中刷新工程和重新打开平台设置，避免用未刷新的旧配置覆盖本次网络参数。

## 已完成的验证与边界

- 迁移前完整 diff 与备份。
- 生产 Marvell/厂商分发函数的 MDIO 仿真：1000/100/10、RGMII 位配置、复位/协商超时、
  MDIO 读写失败、无链路、保留速度、未知厂商不写 Marvell 私有寄存器、无效 ID。
- 当前 BSP 重建成功，三个导出库与本地 BSP 逐字节一致。
- 应用强制重编链接成功，ELF 含 qwen_phy_debug 符号。
- Bootgen 生成成功；再次读取 BOOT.BIN 的镜像/分区头成功，包含当前 qwen_app.elf 分区。
- 所有原应用文件哈希不变。

BSP 重建存在三条原 Xilinx SDK 警告（xemacps BD 变量、xipipsu 未使用标签），
不来自本次 PHY 补丁；应用重编无警告。未在本次工作中修改这些 SDK 文件。
**尚未进行真实开发板的灯、ping、PHY 自协商和 TCP 推理测试**，不能把模拟通过当作物理链路已修复。

日志：bsp_build.log、app_build.log、marvell_mdio_test.log、bootgen.log、bootgen_read.log。
旧 ELF/BOOT.BIN、应用源码与配置备份在同目录 baseline 下。
