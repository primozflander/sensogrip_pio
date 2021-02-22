/*
  Sensor.cpp - Library for easy use of analog sensors.
  Created by Primoz Flander 06.11.2020.
  Released into the public domain.
*/

#include "Sensor.h"
long Sensor::calculateMovingAverage(int input)
{
    long average = 0;
    total = total - readings[readIndex];
    readings[readIndex] = input;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % numAvgReadings;
    average = total / numAvgReadings;
    return average;
}

float Sensor::calculateFilteredValue(int input)
{
    filteredValue = filteredValue * filterPar + (float)input * (1 - filterPar);
    return filteredValue;
}

Sensor::Sensor(int pin, int sensorType, int referenceValue, int referenceRange, unsigned long updateInterval)
{
    this->pin = pin;
    this->sensorType = sensorType;
    this->referenceValue = referenceValue;
    this->referenceRange = referenceRange;
    this->updateInterval = updateInterval;
    init();
}

void Sensor::init()
{
    upperRange = referenceValue + referenceRange;
    lowerRange = referenceValue - referenceRange;
    for (int i = 0; i < numAvgReadings; i++)
        readings[i] = 0;
    nrf_saadc_oversample_set(NRF_SAADC_OVERSAMPLE_8X);
}

void Sensor::calibrate()
{
    int input = 0;
    int sensorMin = 5000;
    int sensorMax = 0;
    offset = 0;
    for (int i = 0; i <= 1000; i++)
    {
        switch (sensorType)
        {
        case 1:
            input = getFsrValue();
            break;
        case 2:
            input = getBatteryValue();
            break;
        default:
            input = getSensorValue();
            break;
        }
        if (input > sensorMax)
            sensorMax = input;
        if (input < sensorMin)
            sensorMin = input;
    }
    offset = (sensorMax + sensorMin) / 2;
}

int Sensor::getFsrValue()
{
    // long input = map(analogRead(pin), 0, 1023, 1, 3299);
    // long output = ((input * 100) / (3299 - input) - offset);
    // return output = (int)(!inverse) ? output : -output;
    // sensogrip 3V, 2k
    long input = map(analogRead(pin), 0, 1023, 1, 2999);
    long output = ((input * 50) / (2999 - input) - offset) * outputCorrectionFactor;
    return output = (int)(!inverse) ? output : -output;
}

int Sensor::getSensorValue()
{
    int output = (analogRead(pin) - offset) * outputCorrectionFactor;
    return output = (int)(!inverse) ? output : -output;
}

int Sensor::getBatteryValue()
{   
    // int output = map(analogRead(pin), 465, 620, 0, 100);
    // // AR_INTERNAL2V4: 2.4 V reference (internal 0.6 V reference with 4x gain)
    // // int output = map(analogRead(pin), 639, 916, 0, 100);
    // output = constrain(output, 0, 100);
    // return output = (int)(!inverse) ? output : -output;
    // sensogrip 100% -> 4.1V , 0% -> 3.2V
    analogReference(AR_INTERNAL2V4);
    int output = map(analogRead(pin), 680, 870, 0, 100);
    analogReference(AR_VDD);
    output = constrain(output, 0, 100);
    return output = (int)(!inverse) ? output : -output;
}

int Sensor::getPitchCorrectedValue(int pitch)
{
    float output = filteredValue / sin(pitch * PI / 180);
    return (int)output;
}

void Sensor::update()
{
    if (millis() - previousMillis >= updateInterval)
    {
        int inputValue = 0;
        switch (sensorType)
        {
        case 1:
            inputValue = getFsrValue();
            break;
        case 2:
            inputValue = getBatteryValue();
            break;
        default:
            inputValue = getSensorValue();
            break;
        }
        filteredValue = calculateFilteredValue(inputValue);
        average = calculateMovingAverage(inputValue);
        previousMillis += updateInterval;
    }
}

bool Sensor::isInRange()
{
    if ((getValue() < (upperRange)) && (getValue() > (lowerRange)))
        return true;
    else
        return false;
}

bool Sensor::isUnderRange()
{
    if (getValue() < (lowerRange))
        return true;
    else
        return false;
}

bool Sensor::isOverRange()
{
    if (getValue() > (upperRange))
        return true;
    else
        return false;
}

int Sensor::getMedian()
{
    int swapper;
    int analogValues[numAvgReadings];
    std::copy(std::begin(readings),std::end(readings),std::begin(analogValues));
    for (int out = 0; out < numAvgReadings; out++)
    {
        for (int in = out; in < (numAvgReadings - 1); in++)
        {
            if (analogValues[in] > analogValues[in + 1])
            {
                swapper = analogValues[in];
                analogValues[in] = analogValues[in + 1];
                analogValues[in + 1] = swapper;
            }
        }
    }
    return analogValues[numAvgReadings / 2];
}

int Sensor::getValue()
{
    float output = filteredValue / sin(pitch * PI / 180);
    return output = (int)max(output, 0);
}

int Sensor::getFilteredValue()
{
    return filteredValue;
}

int Sensor::getAverage()
{
    return average;
}

int Sensor::getRefValue()
{
    return referenceValue;
}

int Sensor::getRefRange()
{
    return referenceRange;
}

int Sensor::getUpperRange()
{
    return upperRange;
}

int Sensor::getLowerRange()
{
    return lowerRange;
}

int Sensor::getOffset()
{
    return offset;
}

float Sensor::getOutputCorrectionFactor()
{
    return outputCorrectionFactor;
}

void Sensor::setFilterPar(float par)
{
    filterPar = par;
}

void Sensor::setSensorType(int type)
{
    sensorType = type;
}

void Sensor::setUpdateInterval(int intervalMs)
{
    updateInterval = intervalMs;
}

void Sensor::setInverse(bool inverseOutput)
{
    inverse = inverseOutput;
}

void Sensor::setRefValue(int reference)
{
    referenceValue = reference;
}

void Sensor::setRefRange(int range)
{
    referenceRange = range;
    upperRange = referenceValue + range;
    lowerRange = referenceValue - range;
}

void Sensor::setUpperRange(int range)
{
    upperRange = range;
}

void Sensor::setLowerRange(int range)
{
    lowerRange = range;
}

void Sensor::setPitch(float angle)
{
    pitch = (angle < 90) ? angle : (180 - angle);
    pitch = constrain(pitch, 30.0, 89.9);
}

void Sensor::setOffset(int sensorOffset)
{
    offset = sensorOffset;
}

void Sensor::setOutputCorrectionFactor(float factor)
{
    outputCorrectionFactor = factor;
}
