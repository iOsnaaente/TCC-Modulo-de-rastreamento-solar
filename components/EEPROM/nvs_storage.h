
#ifndef NVS_OPERATIONS_H_
#define NVS_OPERATIONS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nvs_flash.h"
#include "nvs.h"

#define NVS_NAMESPACE "storage"

bool write_to_nvs(const char *key, const void *data, size_t size);
bool read_from_nvs(const char *key, void *data, size_t size);

#endif