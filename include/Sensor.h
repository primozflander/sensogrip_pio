/*
  Sensor.h - Library for easy use of analog sensors.
  Created by Primoz Flander 06.11.2020.
  Released into the public domain.
*/

#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>

class Sensor
{
private:
#define numAvgReadings 100
    bool inverse = false;
    int pin;
    int sensorType = 0;
    int offset = 0;
    int inputValue = 0;
    int readings[numAvgReadings];
    int readIndex = 0;
    int referenceValue;
    int referenceRange;
    int upperRange;
    int lowerRange;
    float pitch = 90;
    long average = 0;
    long total = 0;
    float filterPar = 0.9;
    float filteredValue = 0;
    unsigned long updateInterval = 10;
    unsigned long previousMillis = 0;
    long calculateMovingAverage(int input);
    float calculateFilteredValue(int input);

public:
    Sensor(int pin, int sensorType = 0, int referenceValue = 500, int referenceRange = 250, unsigned long updateInterval = 10);
    void init();
    void calibrate();
    int getFsrValue();
    int getSensorValue();
    int getBatteryValue();
    int getPitchCorrectedValue(int pitch);
    void update();
    bool isInRange();
    bool isUnderRange();
    bool isOverRange();
    int getValue();
    int getFilteredValue();
    int getAverage();
    int getRefValue();
    int getRefRange();
    int getUpperRange();
    int getLowerRange();
    int getOffset();
    void setFilterPar(float par);
    void setSensorType(int type);
    void setUpdateInterval(int intervalMs);
    void setInverse(bool inverseOutput);
    void setRefValue(int reference);
    void setRefRange(int range);
    void setUpperRange(int range);
    void setLowerRange(int range);
    void setPitch(float angle);
    void setOffset(int sensorOffset);
};
#endif
