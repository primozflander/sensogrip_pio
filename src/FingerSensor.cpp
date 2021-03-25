#include "FingerSensor.h"
FingerSensor::FingerSensor(int pin, int referenceValue, int referenceRange) : Sensor::Sensor(pin, referenceValue, referenceRange)
{
}

int FingerSensor::getSensorValue()
{
    // sensogrip 3V, 2k
    // long input = map(analogRead(pin), 0, 1023, 1, 2999);
    // long output = ((input * 50) / (2999 - input) - offset) * outputCorrectionFactor;
    // return output = (int)(!inverse) ? output : -output;

    // sensogrip 3V, 2k
    double output = (50 * exp(((analogRead(pin) - offset) + 1.55) / 86.91) - 50) * outputCorrectionFactor;
    return output = (int)(!inverse) ? output : -output;
}

int FingerSensor::getValue()
{
    return (int)max(filteredValue, 0);
}
