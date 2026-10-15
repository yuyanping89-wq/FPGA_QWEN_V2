# 寄存器与调用约定

地址见interface_contract.json和drivers/xembedding_v2_hw.h，C++结构体偏移不是寄存器偏移。
AP_CTRL 0x00：bit0 ap_start（任务接受时由控制器清除）、bit1 ap_done（读清除）、bit2 ap_idle（实时状态）、bit3 ap_ready、bit7 auto_restart。使用单任务模式，auto_restart=0。
GIE bit0中断总使能；IER bit0 done/bit1 ready使能；ISR bit0/1 toggle-on-write。当前板测轮询，中断未验证。
所有参数寄存器存储复位为0；结果存储与valid复位为0。AP_CTRL idle/ready是实时核心信号，不能把整个AP_CTRL读值认定固定0。
64位地址/计数分为低、高32位。RESULT_*_CTRL的bit0为valid，读清除；结果DATA读值保留至下次更新或复位。
build_config在任务中产生，不能在上电复位后立即把0解释成模块缺失。没有完整module_id/ABI/capabilities只读启动探测寄存器。

PS准备DDR和64字节对齐缓冲区，检查范围、容量、layout、generation和不重叠；刷新table和IDs的脏缓存，先清理输出脏缓存，再写参数/start。
等待done（带PS超时），再读结果、失效输出缓存；status=0时才把ACT_TH_V2描述符交给M06。任何错误均丢弃整次输出。
非法token可能已写合法前缀，processed_rows描述完整前缀行数；它不是有效地址。M05不分配缓冲区，也不直接调用M06。
Prefill允许T=1..64，Decode仅T=1；hidden=896，vocab=151936，ABI=2，flags只能0。
状态码：0成功，1非法ABI，2非法模式/维度，7非法token，11不支持flags；4空地址仅C仿真检查，不能当作真实硬件地址保护。
最低数据对齐IDs4字节、词表/输出16字节，系统按64字节缓存合同分配。BF16不转换、不计算、原始16bit逐bit复制；每行896元素/1792字节/112个128bit字。
perf_total：接受启动至完成的采样边沿数（含两端）；perf_compute：任一IDs R/词表R/输出W握手的周期数；read_beats是两个读口握手之和；write_beats是输出W握手数。
启动后保持全部参数与buffer ownership不变，读完结果后才启动下一任务。超时后不能直接复用输出或继续下一任务，应协调系统复位恢复。
