/*
  FingerSensor.h - Library for easy use of finger sensors.
  Created by Primoz Flander 26.02.2021.
  Released into the public domain.
*/

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