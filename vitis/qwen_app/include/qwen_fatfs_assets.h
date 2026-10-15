#ifndef QWEN_FATFS_ASSETS_H
#define QWEN_FATFS_ASSETS_H

#include "qwen_infer.h"

typedef struct {
    char root[96]; /* Example: "0:/qwen_model" */
} QwenFatfsAssetContext;

QwenStatus qwen_fatfs_assets_init(QwenFatfsAssetContext *context,
                                  const char *root,
                                  QwenAssetProvider *provider);

#endif
