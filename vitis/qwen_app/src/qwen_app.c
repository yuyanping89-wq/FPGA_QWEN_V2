#include "qwen_app.h"
#include "qwen_lwip_server.h"

#include <string.h>

static QwenModel g_model;

QwenStatus qwen_app_boot(struct netif *netif, uint16_t tcp_port,
                         const QwenAssetProvider *assets,
                         QwenServiceHook network_pump, void *network_context)
{
    QwenStatus status;
    memset(&g_model, 0, sizeof(g_model));
    qwen_platform_set_service_hook(network_pump, network_context);
    status = qwen_model_load(assets, &g_model);
    if (status != QWEN_OK) return status;
    return qwen_lwip_server_start(netif, tcp_port, &g_model);
}

void qwen_app_process(void)
{
    qwen_lwip_server_process();
}

const QwenModel *qwen_app_model(void)
{
    return &g_model;
}
