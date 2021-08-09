#include "FingerSensor.h"
FingerSensor::FingerSensor(int pin, int upperRange, int lowerRange) : Sensor::Sensor(pin, upperRange, lowerRange)
{
}

int FingerSensor::getSensorValue()
{
    double output = (50 * exp(((analogRead(pin) - offset) + 1.55) / 86.91) - 50) * outputCorrectionFactor;
    return output = (int)(!inverse) ? output : -output;
}

int FingerSensor::getValue()
{
    return (int)max(filteredValue, 0);
}

// void FingerSensor::setTempUpperRange(int value)
// {
//     tempUpperRange = value;
// }

// void FingerSensor::setTempLowerRange(int value)
// {
//     tempLowerRange = value;
// }

// int FingerSensor::getTempUpperRange()
// {
//     return tempUpperRange;
// }

// int FingerSensor::getTempLowerRange()
// {
//     return tempLowerRange;
// }
