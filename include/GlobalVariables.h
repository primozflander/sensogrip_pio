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
#include "DebugUtils.h"
#include "LedFeedbackFunctions.h"
#include "BleFunctions.h"
#include "SensorFunctions.h"
#include "SystemFunctions.h"

#define DISPLAYED_DEVICE_NAME "SensoGrip#dev"
#define HARDWARE_VERSION 3

union data_stream
{
    struct __attribute__((packed))
    {
        uint16_t timeStamp;
        uint16_t tipSensorValue;
        uint16_t fingerSensorValue;
        uint16_t angle;
        uint16_t speed;
        uint16_t batteryLevel;
        uint16_t minutesInRange;
        uint16_t minutesInUse;
        uint16_t tipSensorUpperRange;
        uint16_t tipSensorLowerRange;
        uint16_t fingerSensorUpperRange;
        uint16_t fingerSensorLowerRange;
        float_t accX;
        float_t accY;
        float_t accZ;
        float_t gyroX;
        float_t gyroY;
        float_t gyroZ;
    };
    uint8_t bytes[12 * sizeof(uint16_t) + 6 * sizeof(float_t)];
};

union configuration_state
{
    struct __attribute__((packed))
    {
        uint16_t tipSensorUpperRange;
        uint16_t tipSensorLowerRange;
        uint16_t fingerSensorUpperRange;
        uint16_t fingerSensorLowerRange;
        uint16_t feedback;
        uint16_t ledAssistance;
        uint16_t aiRangeAssistance;
        uint16_t angleCorection;
        uint16_t tipPressureReleaseDelay;
        uint16_t ledTurnOnSpeed;
        uint16_t ledSimpleAssistanceColor;
        uint16_t ledTipAssistanceColor;
        uint16_t ledFingerAssistanceColor;
        uint16_t ledOkColor;
        uint16_t ledNokColor;
        uint16_t hwVersion;
        uint16_t tipSensorCorrectionFactor;
        uint16_t fingerSensorCorrectionFactor;

    };
    uint8_t bytes[18 * sizeof(uint16_t)];
};

inline union data_stream dataStream;
inline union configuration_state configurationState;
inline BLEService sensoGripService("1111");
inline BLEBoolCharacteristic calibrateChar("2014", BLEWrite);
inline BLEBoolCharacteristic calibrateIMUChar("2015", BLEWrite);
inline BLEBoolCharacteristic minutesPassedInUseChar("2017", BLEWrite);
inline BLEBoolCharacteristic minutesPassedInRangeChar("2018", BLEWrite);
inline BLECharacteristic dataStreamChar("3004", BLERead | BLENotify, sizeof dataStream.bytes);
inline BLECharacteristic configurationStateChar("3005", BLERead | BLEWrite, sizeof configurationState.bytes);
inline MPU6050 mpu(Wire);
inline TipSensor tipSensor(A0, 170, 30);
inline FingerSensor fingerSensor(A1, 170, 30);
inline BatterySensor batteryLevel(A2);
inline RGBLed rgbLed(9, 10, 11, false);
inline Led ledBuiltIn(LED_BUILTIN);
inline Led ledPwr(LED_PWR);
inline FlashBLE Flash;
inline Statistics Stats;
inline RangeAI tipRangeAI(20, 500);
inline RangeAI fingerRangeAI(20, 500);
inline const int SNZ_PWR = 4;
inline const int SERIAL_SEND_INTERVAL = 50;
inline const int BLE_SEND_INTERVAL = 100;
inline const int SLEEP_TIMER = 600;
inline const int LED_FEEDBACK_INTERVAL = 10;
inline const int STATS_SAVE_INTERVAL = 60000;
inline long loopStartTime = 0;
inline long loopEndTime = 0;
inline bool isPositiveFeedback = true;
inline bool isAiRangeAssisted = false;
inline bool isAngleCorrected = true;
inline int ledFeedbackType = 4;
inline int tipPressureReleaseDelay = 80;