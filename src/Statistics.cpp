#include "Statistics.h"
Statistics::Statistics()
{
    init();
}

void Statistics::init()
{
}

void Statistics::update(int tipSensorValue, int fingerSensorValue, bool tipSensorInRange, bool fingerSensorInRange)
{
    if (millis() - previousMillis >= updateInterval)
    {
        minutesPassedPoweredOn++;
        minutesPassedInUse += (tipSensorValue > 25) ? 1 : 0;
        minutesPassedInRange += (tipSensorInRange && fingerSensorInRange) ? 1 : 0;
        previousMillis += updateInterval;
    }
}

void Statistics::setMinutesInUse(unsigned long minutesInUse)
{
    minutesPassedInUse = minutesInUse;
}

unsigned int Statistics::getMinutesInUse()
{
    return minutesPassedInUse;
}

void Statistics::setMinutesInRange(unsigned long minutesInRange)
{
    minutesPassedInRange = minutesInRange;
}

unsigned int Statistics::getMinutesInRange()
{
    return minutesPassedInRange;
}

void Statistics::setMinutesPoweredOn(unsigned long minutesPoweredOn)
{
    minutesPassedPoweredOn = minutesPoweredOn;
}

unsigned int Statistics::getMinutesPoweredOn()
{
    return minutesPassedPoweredOn;
}
