# M05 AXU9EGB PS 上板测试程序

2026-10-10 最新结果：用户在修复AUX复位的v22硬件上运行V1.2，11个用例全部通过。
实际代码/ELF/bitstream/XSA与串口日志已归档 `../../reports/board_test_v22_20261010/`。
以下“代码已准备/尚未运行”等为准备阶段记录。

状态：代码已准备，未由助手编译或上板运行。用户已完成 Hello World 上板验证。
目标：Vivado/Vitis 2021.1、AXU9EGB、板级工程 m05_board_v21、IP 包装 2.1、
核心 build_config=0x20010011、standalone/psu_cortexa53_0/64-bit、UART0/115200。

2026-10-10：用户编译 V1 成功，V1.1 TRACE 上板停在首次 AP_CTRL 读取
`0xA0000000 begin`，尚未启动 M05；输出缓存初始化/Flush 已完成。
目前不是 payload 比较失败，控制总线响应故障尚待定位。
V1.2 新增 PS 侧 PL0 时钟、GPIO 复位、PCAP DONE、FPD reset、隔离状态打印，
以及启动时对 pl_resetn0 做一次复位/释放。此前 V1.1 保存在 `.c.bak` 文件，勿作为 C 源导入。
V1.2 尚未由用户编译/运行，不表示已解决。

本次核对 XSA 生成的 psu_init.c：psu_init() 不调用
psu_ps_pl_reset_config_data()；FSBL 的 xfsbl_handoff.c 在特定 reset/JTAG 分支中调用
该独立步骤，不能只凭 FSBL banner 判断板上复位已释放。
startup 使用 XSA 中 GPIO bank5 bit31 的 pl_resetn0，保留其他 GPIO 位，
没有重新初始化 DDR，也没有修改 PL 隔离或 PMU 电源控制寄存器。
PS 状态满足只是必要条件：MMCM locked 未对软件暴露；隔离/互连等仍可能导致读取不返回。
如仍停在 AP_CTRL，下一步使用这些状态排查启动流程/隔离，并视情况加入 ILA。

## 导入 Vitis

现有工作区：`C:/Users/87767/Desktop/m05_vitis/`。
现有应用：`m05_board_test`；保持 `platform.c`、`platform.h`、`lscript.ld`。

1. 将原 `src/helloworld.c` 备份到 src 之外，供恢复使用。
2. 在 Vitis 中打开 `src/helloworld.c`，把全文替换为本目录 `m05_board_test.c` 的内容。
   保持文件名 helloworld.c 即可。
3. 将本目录 `xembedding_v2_hw.h` 复制到应用 `src` 目录。
4. 不要同时添加另一个包含 main 的 .c 文件，否则会出现重复 main。
5. 保存，右键应用 Build Project。把编译输出提交核对后，再运行硬件。

保持已有链接脚本：本次读取的 MEMORY 中 DDR_LOW 为 0..0x7FEFFFFF，
`.bss (NOLOAD)`、heap、stack 均由链接器分配到该区；PMU 保留区 0x7FF00000 起不使用。
静态 table 预留 272269312 字节；没有 malloc 或写死 DDR 缓冲区地址。
编译输出 bss 增大约 272 MB 是预期结果。ELF 中该区域为 NOLOAD，文件本身不因此增大约260MiB。
启动代码清零 .bss，UART 第一条消息之前可能有数秒等待。
本次仅用 DDR_LOW，未验证 DDR_HIGH 或完整 4 GiB 内存。

## 运行条件

- 程序只能运行于已初始化的上述 PS 硬件平台；保持 Hello World 验证成功的 JTAG 启动方式。
- 运行 M05 测试时，运行配置必须加载本次 XSA 中的板级 bitstream，并初始化 PS/DDR。
  Hello World 不访问 M05，因此它的成功不能证明 PL 中已配置正确的 M05。
- 如果运行配置显示 Program FPGA/bitstream，确认勾选且指向 m05_board_v21 的 system_wrapper.bit。
  仅手动下载 bitstream 不能替代 PS 初始化。
- 串口 COM9、115200、8N1、无流控；先连接终端再启动。
- 仅启动一个任务；禁用中断和 auto_restart；在新任务启动前读取全部结果快照。

## 用例与预期结果

程序生成与 token ID、列索引相关的 16-bit 原始位模式；M05 必须按位复制。
完整词表地址布局保留，但只初始化八个被用例访问的行，不载入 Qwen 权重。
64 个 token 用例覆盖首尾 ID、65535/65536、重复 ID 和非排序访问。

11 个用例：prefill 单行、decode 末行、prefill 64 行、64 个重复 token、
非法中间 token（已写前缀一行）、错误后 decode 恢复、错误 ABI、decode 两行、
valid_rows=65、flags=1、最终 decode 恢复。
每个用例检查 status、processed_rows、build、CRC=0、实际计数基本约束，
逐项比较有效输出，检查首尾 guard 和未写区域保持哨兵，检查 IDs 未变。
最后核对所有初始化词表行未被改写。错误任务前缀仅用于验证，不用于下游推理。

缓存顺序：CPU 写 IDs/词表/输出哨兵 → Flush + dsb → 启动设备 →
轮询 done → 读取稳定结果快照 → Invalidate 输出 + dsb → CPU 对比。
输出 guard/数据边界均为 64 字节对齐，减少缓存行共享问题。

计数输出用 `0x高32位低32位`，避免 xil_printf 对 64-bit 十进制格式支持问题。
正常 N 行：write_beats=N*112；read_beats=N*112+N（IDs 和词表合计）。
非法 token：允许 ID burst 预取，read_beats 范围包含已处理前缀与实际拒绝的 ID。
参数校验失败：读写拍数和 active 应为 0；total 非零，表示错误返回同样花费周期。
perf_cycles_compute 是至少一个 payload 握手的周期数，不是浮点计算次数。
上板检查计数规律；逐周期准确性已有独立 RTL 仿真参考，软件不能观测每个 AXI 握手。

最终预期：

```text
ALL 11 M05 BOARD CASES PASSED; selected table rows unchanged.
```

未通过时打印 `[FAIL]` 并停止。等待完成有 5 秒软件超时，但 AXI-Lite 单次总线访问
若完全无响应，CPU 可能阻塞在 MMIO 指令，不能由轮询超时恢复。
超时或总线故障后不要继续下一任务；保存日志，重置/重新启动再排查。
本测试证明的是合成数据查表搬运及测试地址范围功能，不等同于 Qwen 数值验证或完整 DDR 压力测试。
