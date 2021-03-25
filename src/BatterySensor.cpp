#include "BatterySensor.h"
BatterySensor::BatterySensor(int pin) : Sensor::Sensor(pin)
{
}

int BatterySensor::getSensorValue()
{
    // 100% -> 4.1V , 0% -> 3.2V
    analogReference(AR_INTERNAL2V4);
    int output = map(analogRead(pin), 680, 870, 0, 100);
    analogReference(AR_VDD);
    output = constrain(output, 0, 101);
    return output = (int)(!inverse) ? output : -output;
}

int BatterySensor::getValue()
{
    return (int)max(filteredValue, 0);
}
