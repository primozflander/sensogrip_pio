#pragma once

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "MPU6050.h"
#include "FlashBLE.h"
#include "Led.h"
#include "RGBLed.h"
#include "Sensor.h"
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
inline BLEIntCharacteristic refTipValueChar("2001", BLEWrite);
inline BLEIntCharacteristic refTipRangeChar("2002", BLEWrite);
inline BLEIntCharacteristic refFingerValueChar("2003", BLEWrite);
inline BLEIntCharacteristic refFingerRangeChar("2004", BLEWrite);
inline BLEBoolCharacteristic feedbackChar("2005", BLEWrite);
inline BLEIntCharacteristic ledAssistanceTypeChar("2006", BLEWrite);
inline BLEIntCharacteristic tipPressureReleaseDelayChar("2007", BLEWrite);
inline BLEIntCharacteristic ledTurnOnSpeedChar("2008", BLEWrite);
inline BLEIntCharacteristic ledTipAssistanceColorChar("2009", BLEWrite);
inline BLEIntCharacteristic ledFingerAssistanceColorChar("2010", BLEWrite);
inline BLEIntCharacteristic ledSimpleAssistanceColorChar("2011", BLEWrite);
inline BLEIntCharacteristic ledOkColorChar("2012", BLEWrite);
inline BLEIntCharacteristic ledNokColorChar("2013", BLEWrite);
inline BLEBoolCharacteristic calibrateChar("2014", BLEWrite);
inline BLEBoolCharacteristic aiChar("2015", BLEWrite);
inline BLEBoolCharacteristic angleCorrectionChar("2016", BLEWrite);
inline BLEBoolCharacteristic minutesPassedInUseChar("2017", BLEWrite);
inline BLEBoolCharacteristic minutesPassedInRangeChar("2018", BLEWrite);
inline BLEStringCharacteristic fastStreamChar("3000", BLERead | BLENotify, 32);
inline BLEStringCharacteristic slowStreamChar("3001", BLERead | BLENotify, 32);
inline BLEStringCharacteristic configurationChar("3002", BLERead, 32);
inline BLEStringCharacteristic configuration2Char("3003", BLERead, 32);
inline BLECharacteristic dataStreamChar("3004", BLERead | BLENotify, sizeof dataStream.bytes );
inline BLECharacteristic configurationStateChar("3005", BLERead | BLENotify, sizeof configurationState.bytes );

inline MPU6050 mpu(Wire);
inline Sensor tipSensor(A0, 0, 100, 70);
inline Sensor fingerSensor(A1, 1, 500, 350);
inline Sensor batteryLevel(A2, 2, 0, 100);
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
inline const int SLOW_BLE_SEND_INTERVAL = 1000;
inline const int SLEEP_TIMER = 300;
inline const int LED_FEEDBACK_INTERVAL = 10;
inline const int STATS_SAVE_INTERVAL = 60000;
inline long timingStart = 0;
inline long timingStop = 0;
inline bool positiveFeedback = true;
inline bool aiRangeAssistance = false;
inline bool angleCorrection = true;
inline int ledAssistance = 1;
inline int tipPressureReleaseDelay = 80;