#pragma once
#include <Arduino.h>
#include "GlobalVariables.h"

void isBatteryOk();
void initIO();
void saveConfigurationToFlash();
void readConfigurationFromFlash();
void updateStatistics();
void powerOffFunctionality();
void sleepToSavePower();
void disableUART();
