#ifndef QWEN_LWIP_SERVER_H
#define QWEN_LWIP_SERVER_H

#include "qwen_infer.h"

struct netif;

/* Raw TCP service; call process() from the no-OS main loop, outside callbacks. */
QwenStatus qwen_lwip_server_start(struct netif *netif, uint16_t port,
                                  const QwenModel *model);
void qwen_lwip_server_process(void);

#endif
