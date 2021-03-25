#pragma once
#include <Arduino.h>
#include "Sensor.h"
class FingerSensor : public Sensor
{
public:
    FingerSensor(int pin, int referenceValue, int referenceRange);
    int getSensorValue();
    int getValue();
};