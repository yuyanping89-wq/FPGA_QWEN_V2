#ifndef M05_COMMON_RESULT_V2_H
#define M05_COMMON_RESULT_V2_H

#include <stdint.h>

// 按项目 V2 文档定义的逻辑结果字段。
// 集成时应与全项目统一头文件合并，保持字段名称、类型和语义一致。
// 此结构体的 C++ 内存布局不代表 AXI-Lite 寄存器地址布局。
// embedding_v2 的 DISAGGREGATE/INTERFACE 指令将各字段映射为控制口的输出寄存器。
// 具体寄存器偏移及附加 valid 寄存器需查看综合报告和导出的驱动。
struct common_result_v2 {
    uint32_t status_code;         // 执行状态；非 0 时下游不得使用本次输出。
    uint32_t processed_rows;      // 实际成功处理的行数。
    uint64_t perf_cycles_total;   // 实际总周期，包括 AXI 等待。
    uint64_t perf_cycles_compute; // 活跃周期，具体计数定义待实现阶段确定。
    uint64_t perf_read_beats;     // 实际 AXI 读拍数，需说明统计端口及宽度。
    uint64_t perf_write_beats;    // 实际 AXI 写拍数。
    uint32_t output_crc;          // 有效输出的 CRC；未启用时为 0。
    uint32_t build_config;        // 综合配置标识，不随本次任务改变。
};

#endif
