#pragma once
#include <Arduino.h>
class Statistics
{
private:
    unsigned long updateInterval = 1000;
    unsigned long previousMillis = 0;
    unsigned int minutesPassedPoweredOn = 0;
    unsigned int minutesPassedInUse = 0;
    unsigned int minutesPassedInRange = 0;

public:
    Statistics();
    void init();
    void update(int tipSensorValue, int fingerSensorValue, bool tipSensorInRange, bool fingerSensorInRange);
    void setMinutesInUse(unsigned long minutesInUse);
    unsigned int getMinutesInUse();
    void setMinutesInRange(unsigned long minutesInRange);
    unsigned int getMinutesInRange();
    void setMinutesPoweredOn(unsigned long minutesPoweredOn);
    unsigned int getMinutesPoweredOn();
};
