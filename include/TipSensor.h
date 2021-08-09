#pragma once
#include <Arduino.h>
#include "Sensor.h"
class TipSensor : public Sensor
{
private:
    float pitch = 90;
    // int tempUpperRange;
    // int tempLowerRange;
public:
    TipSensor(int pin, int referenceValue, int referenceRange);
    void setPitch(float angle);
    float getPitch();
    int getValue();
    // void setTempUpperRange(int value);
    // void setTempLowerRange(int value);
    // int getTempUpperRange();
    // int getTempLowerRange();
};