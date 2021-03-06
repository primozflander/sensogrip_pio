#pragma once
#include <Arduino.h>
#include "KVStore.h"
#include "kvstore_global_api.h"
#include "mbed.h"

class FlashBLE
{
#define EXAMPLE_KV_VALUE_LENGTH 64
#define EXAMPLE_KV_KEY_LENGTH 32
#define err_code(res) MBED_GET_ERROR_CODE(res)
private:
    int res = MBED_ERROR_NOT_READY;
    size_t actual_size = 0;
    kv_info_t info;

public:
    FlashBLE();
    void init();
    int clear();
    int put(String key, String value);
    String get(String key);
};
