#pragma once
#include <Arduino.h>
#include "GlobalVariables.h"

void isBatteryOk();
void onCharging();
String stringSegToFloat(String data, char separator, int index);
void calibrateIMU();
void initIO();
void saveConfigurationToFlash();
void readConfigurationFromFlash();
void updateStatistics();
void powerOffFunctionality();
void sleepToSavePower();
void disableUART();
