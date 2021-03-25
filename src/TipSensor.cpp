#include "TipSensor.h"
TipSensor::TipSensor(int pin, int referenceValue, int referenceRange) : Sensor::Sensor(pin, referenceValue, referenceRange)
{
}

void TipSensor::setPitch(float angle)
{
    pitch = (angle < 90) ? angle : (180 - angle);
    pitch = constrain(pitch, 30.0, 89.9);
}

int TipSensor::getValue()
{
    float output = filteredValue / sin(pitch * PI / 180);
    return (int)max(output, 0);
}
