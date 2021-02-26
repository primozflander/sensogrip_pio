/*
  BatterySensor.h - Library for easy use of battery sensors.
  Created by Primoz Flander 26.02.2021.
  Released into the public domain.
*/

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