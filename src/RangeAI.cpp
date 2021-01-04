/*
  RangeAI.cpp - Library for calculating optimal sensor range.
  Created by Primoz Flander 27.10.2020.
  Released into the public domain.
*/
#include "RangeAI.h"
RangeAI::RangeAI(int minRange, int maxRange, int sensorThreshold)
{
    this->minRange = minRange;
    this->maxRange = maxRange;
    this->sensorThreshold = sensorThreshold;
    init();
}

void RangeAI::init()
{
    range = (minRange + maxRange) / 2;
}

void RangeAI::execute(float sensorValue, float reference)
{
    sensor = sensor * filterPar + sensorValue * (1 - filterPar);
    upperRange = reference + range;
    lowerRange = max(int(reference - range), sensorThreshold + 5);
    if (sensorValue > sensorThreshold)
    {
        if ((sensor > lowerRange) && (sensor < upperRange))
        {
            if (range > minRange)
                range -= 0.1;
        }
        else
        {
            if (range < maxRange)
                range += 0.2;
        }
    }
}

int RangeAI::getRange()
{
    return (int)range;
}

int RangeAI::getUpperRange()
{
    return (int)upperRange;
}

int RangeAI::getLowerRange()
{
    return (int)lowerRange;
}

void RangeAI::showData()
{
    Serial.print(sensor);
    Serial.print(" ");
    Serial.print(lowerRange);
    Serial.print(" ");
    Serial.print(upperRange);
    Serial.print(" ");
    Serial.println(0);
}
