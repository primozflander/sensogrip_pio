#pragma once

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "MPU6050.h"
#include "FlashBLE.h"
#include "Led.h"
#include "RGBLed.h"
#include "TipSensor.h"
#include "FingerSensor.h"
#include "BatterySensor.h"
#include "Statistics.h"
#include "RangeAI.h"

union data_stream
{
    struct __attribute__((packed))
    {
        uint16_t values[12];
    };
    uint8_t bytes[12 * sizeof(uint16_t)];
};

union configuration_state
{
    struct __attribute__((packed))
    {
        uint16_t values[15];
    };
    uint8_t bytes[15 * sizeof(uint16_t)];
};

inline union data_stream dataStream;
inline union configuration_state configurationState;
inline BLEService sensoGripService("1111");
inline BLEBoolCharacteristic calibrateChar("2014", BLEWrite);
inline BLEBoolCharacteristic minutesPassedInUseChar("2017", BLEWrite);
inline BLEBoolCharacteristic minutesPassedInRangeChar("2018", BLEWrite);
inline BLECharacteristic dataStreamChar("3004", BLERead | BLENotify, sizeof dataStream.bytes);
inline BLECharacteristic configurationStateChar("3005", BLERead | BLEWrite, sizeof configurationState.bytes);
inline MPU6050 mpu(Wire);
inline TipSensor tipSensor(A0, 100, 70);
inline FingerSensor fingerSensor(A1, 100, 70);
inline BatterySensor batteryLevel(A2);
inline RGBLed rgbLed(9, 10, 11, false);
inline Led onboardLedR(22, true);
inline Led onboardLedG(23, true);
inline Led onboardLedB(24, true);
inline Led ledBuiltIn(LED_BUILTIN);
inline Led ledPwr(LED_PWR);
inline FlashBLE Flash;
inline Statistics Stats;
inline RangeAI tipRangeAI(20, 500);
inline RangeAI fingerRangeAI(20, 500);
inline const int SNZ_PWR = 4;
inline const int SERIAL_SEND_INTERVAL = 50;
inline const int BLE_SEND_INTERVAL = 100;
inline const int SLEEP_TIMER = 300;
inline const int LED_FEEDBACK_INTERVAL = 10;
inline const int STATS_SAVE_INTERVAL = 60000;
inline long timingStart = 0;
inline long timingStop = 0;
inline bool positiveFeedback = true;
inline bool aiRangeAssistance = false;
inline bool angleCorrection = true;
inline int ledAssistance = 4;
inline int tipPressureReleaseDelay = 80;