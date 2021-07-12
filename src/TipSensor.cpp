#include "TipSensor.h"
TipSensor::TipSensor(int pin, int upperRange, int lowerRange) : Sensor::Sensor(pin, upperRange, lowerRange)
{
}

void TipSensor::setPitch(float angle)
{
    pitch = (angle < 90) ? angle : (180 - angle);
    pitch = constrain(pitch, 30.0, 89.9);
}

float TipSensor::getPitch()
{
    return pitch;
}

int TipSensor::getValue()
{
    float output = filteredValue / sin(pitch * PI / 180);
    return (int)max(output, 0);
}

// void TipSensor::setTempUpperRange(int value)
// {
//     tempUpperRange = value;
// }

// void TipSensor::setTempLowerRange(int value)
// {
//     tempLowerRange = value;
// }

// int TipSensor::getTempUpperRange()
// {
//     return tempUpperRange;
// }

// int TipSensor::getTempLowerRange()
// {
//     return tempLowerRange;
// }
