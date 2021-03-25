#pragma once
#include <Arduino.h>
class RangeAI
{
private:
    int minRange;
    int maxRange;
    int sensorThreshold;
    float sensor = 0;
    float range;
    float upperRange = 0;
    float lowerRange = 0;
    const float filterPar = 0.99;

public:
    RangeAI(int minRange, int maxRange, int sensorThreshold = 10);
    void init();
    void execute(float sensorValue, float reference);
    int getRange();
    int getUpperRange();
    int getLowerRange();
    void showData();
};
