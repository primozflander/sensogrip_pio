/*
  TipSensor.h - Library for easy use of tip sensors.
  Created by Primoz Flander 26.02.2021.
  Released into the public domain.
*/

#pragma once
#include <Arduino.h>
#include "Sensor.h"
class TipSensor : public Sensor
{
private:
    float pitch = 90;
public:
    TipSensor(int pin, int referenceValue, int referenceRange);
    void setPitch(float angle);
    int getValue();
};