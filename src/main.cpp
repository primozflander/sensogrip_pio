#include <Arduino.h>
#include "GlobalVariables.h"

void setup()
{
    // while (!Serial);
    initIO();
    //tipSensor.setOutputCorrectionFactor(2.44);
    // fingerSensor.setOutputCorrectionFactor(0.56);
    //saveConfigurationToFlash();
    readConfigurationFromFlash();
}

void loop()
{
    //onCharging();
    loopStartTime = millis();
    Serial.println(String(mpu.getAngX()) + " " + String(mpu.getAngY()) + " " + String(mpu.getAngZ()));
    readSensors();
    showLedFeedback();
    sendAndReceiveBLEData();
    //sendSensorData();
    updateStatistics();
    //powerOffFunctionality();
    sleepToSavePower();
}