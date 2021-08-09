#include <Arduino.h>
#include "GlobalVariables.h"

void setup()
{
    // while (!Serial);
    initIO();
    //saveConfigurationToFlash();
    readConfigurationFromFlash();
    tipSensor.setOutputCorrectionFactor(2);
    fingerSensor.setOutputCorrectionFactor(0.5);
}

void loop()
{
    //onCharging();
    loopStartTime = millis();
    readSensors();
    showLedFeedback();
    sendAndReceiveBLEData();
    // sendSensorData();
    updateStatistics();
    powerOffFunctionality();
    sleepToSavePower();
}