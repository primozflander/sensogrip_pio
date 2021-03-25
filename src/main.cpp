#include <Arduino.h>
#include "GlobalVariables.h"


void setup()
{
    // while (!Serial);
    initIO();
    //saveConfigurationToFlash();
    readConfigurationFromFlash();
    // ledPwr.off();
}

void loop()
{
    //timingStart = millis();
    readSensors();
    showLedFeedback();
    sendAndReceiveBLEData();
    //sendSensorData();
    updateStatistics();
    sleepToSavePower();
    //powerOffFunctionality();
    //timingStop = millis();
    //Serial.println(timingStop - timingStart);
}