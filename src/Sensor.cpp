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

Sensor::Sensor(int pin, int referenceValue, int referenceRange, unsigned long updateInterval)
{
    this->pin = pin;
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
        input = getSensorValue();
        if (input > sensorMax)
            sensorMax = input;
        if (input < sensorMin)
            sensorMin = input;
    }
    offset = (sensorMax + sensorMin) / 2;
}

int Sensor::getSensorValue()
{
    int output = (analogRead(pin) - offset) * outputCorrectionFactor;
    return output = (int)(!inverse) ? output : -output;
}

void Sensor::update()
{
    if (millis() - previousMillis >= updateInterval)
    {
        inputValue = getSensorValue();
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
    std::copy(std::begin(readings), std::end(readings), std::begin(analogValues));
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
    return (int)max(inputValue,0);
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

void Sensor::setOffset(int sensorOffset)
{
    offset = sensorOffset;
}

void Sensor::setOutputCorrectionFactor(float factor)
{
    outputCorrectionFactor = factor;
}
