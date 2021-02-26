/*
  FingerSensor.cpp - Library for easy use of finger sensors.
  Created by Primoz Flander 26.02.2021.
  Released into the public domain.
*/

#include "FingerSensor.h"
FingerSensor::FingerSensor(int pin, int referenceValue, int referenceRange) : Sensor::Sensor(pin, referenceValue, referenceRange)
{
}

int FingerSensor::getSensorValue()
{
    // sensogrip 3V, 2k
    long input = map(analogRead(pin), 0, 1023, 1, 2999);
    long output = ((input * 50) / (2999 - input) - offset) * outputCorrectionFactor;
    return output = (int)(!inverse) ? output : -output;
}

int FingerSensor::getValue()
{
    return (int)max(filteredValue, 0);
}
