#pragma once
#include <Arduino.h>
#include "Sensor.h"
class BatterySensor : public Sensor
{
public:
    BatterySensor(int pin);
    int getSensorValue();
    int getValue();
};