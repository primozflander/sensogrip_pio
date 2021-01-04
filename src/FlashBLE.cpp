/*
  FlashBLE.cpp - Library for using Arduino BLE flash storage.
  Created by Primoz Flander 27.10.2020.
  Released into the public domain.
*/

#include "FlashBLE.h"
FlashBLE::FlashBLE()
{
    init();
}

void FlashBLE::init()
{
}

int FlashBLE::clear()
{
    res = kv_reset("/kv/");
    return err_code(res);
}

int FlashBLE::put(String key, String value)
{
    String keyString = "/kv/" + String(key);
    const char *keyChar = keyString.c_str();
    const char *valueChar = value.c_str();
    res = kv_set(keyChar, valueChar, strlen(valueChar), 0);
    return res;
}

String FlashBLE::get(String key)
{
    String output;
    String keyString = "/kv/" + String(key);
    const char *keyChar = keyString.c_str();
    res = kv_get_info(keyChar, &info);
    char *kv_first_value_out = new char[info.size + 1];
    memset(kv_first_value_out, 0, info.size + 1);
    res = kv_get(keyChar, kv_first_value_out, info.size, &actual_size);
    return output = (res == 0) ? String(kv_first_value_out) : "error";
}
