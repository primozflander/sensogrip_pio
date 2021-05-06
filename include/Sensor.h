#pragma once
#include <Arduino.h>
class Sensor
{
private:
#define numAvgReadings 100
    int readings[numAvgReadings];
    int inputValue = 0;
    int readIndex = 0;
    int referenceValue;
    int referenceRange;
    int upperRange;
    int lowerRange;
    long average = 0;
    long total = 0;
    float filterPar = 0.5;
    unsigned long updateInterval;
    unsigned long previousMillis = 0;
    long calculateMovingAverage(int input);
    float calculateFilteredValue(int input);

protected:
    bool inverse = false;
    int pin;
    int offset = 0;
    float filteredValue = 0;
    float outputCorrectionFactor = 1;

public:
    Sensor(int pin, int referenceValue = 100, int referenceRange = 50, unsigned long updateInterval = 10);
    void init();
    void calibrate();
    virtual int getSensorValue();
    void update();
    bool isInRange();
    bool isUnderRange();
    bool isOverRange();
    virtual int getValue();
    int getFilteredValue();
    int getAverage();
    int getMedian();
    int getRefValue();
    int getRefRange();
    int getUpperRange();
    int getLowerRange();
    int getOffset();
    float getOutputCorrectionFactor();
    int getRawValue();
    void setFilterPar(float par);
    void setUpdateInterval(int intervalMs);
    void setInverse(bool inverseOutput);
    void setRefValue(int reference);
    void setRefRange(int range);
    void setUpperRange(int range);
    void setLowerRange(int range);
    void setOffset(int sensorOffset);
    void setOutputCorrectionFactor(float factor);
};
