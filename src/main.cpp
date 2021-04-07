#include <Arduino.h>
#include "GlobalVariables.h"

void setup()
{
    // while (!Serial);
    
    initIO();
    // saveConfigurationToFlash();
    readConfigurationFromFlash();
}

void loop()
{
    onCharging();
    readSensors();
    showLedFeedback();
    sendAndReceiveBLEData();
    //sendSensorData();
    updateStatistics();
    sleepToSavePower();
    powerOffFunctionality();
}