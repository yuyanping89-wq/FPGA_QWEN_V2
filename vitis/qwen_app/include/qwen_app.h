#ifndef QWEN_APP_H
#define QWEN_APP_H

#include "qwen_infer.h"
#include "lwip/netif.h"

QwenStatus qwen_app_boot(struct netif *netif, uint16_t tcp_port,
                         const QwenAssetProvider *assets,
                         QwenServiceHook network_pump, void *network_context);
void qwen_app_process(void);
const QwenModel *qwen_app_model(void);

#endif
