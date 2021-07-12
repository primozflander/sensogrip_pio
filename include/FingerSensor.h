#pragma once
#include <Arduino.h>
#include "Sensor.h"
class FingerSensor : public Sensor
{
private:
    // int tempUpperRange;
    // int tempLowerRange;
public:
    FingerSensor(int pin, int upperRange, int lowerRange);
    int getSensorValue();
    int getValue();
    // void setTempUpperRange(int value);
    // void setTempLowerRange(int value);
    // int getTempUpperRange();
    // int getTempLowerRange();
};