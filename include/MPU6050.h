/*
  MPU6050.cpp - Library for easy use of IMU MPU5060.
  Created by Primoz Flander 13.11.2020.
  Released into the public domain.
*/

#ifndef MPU6050_H
#define MPU6050_H
#include "Arduino.h"
#include "Wire.h"
#define ADDRESS_LOW 0x68
#define ADDRESS_HIGH 0x69
#define ADDRESS ADDRESS_LOW
//Configuration registers
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
#define SIGNAL_PATH_RESET 0x68
#define MOT_THR 0x1F
#define MOT_DUR 0x20
#define MOT_DETECT_CTRL 0x69
#define INT_ENABLE 0x38
#define INT_STATUS 0x3A
#define INT_PIN_CFG 0x37
//Data registers
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
//Macros
#define ACCEL_TRANSFORMATION_NUMBER 0.00006103515
#define GYRO_TRANSFORMATION_NUMBER 0.01525878906
#define DEFAULT_ACCEL_COEFF 0.02f
#define DEFAULT_GYRO_COEFF 0.98f
#define DISCARDED_MEASURES 100
#define CALIBRATION_MEASURES 5000
#define CHECKING_MEASURES 50
#define ACCEL_PREOFFSET_MAGIC_NUMBER 8
#define GYRO_PREOFFSET_MAGIC_NUMBER 4

class MPU6050
{
private:
    TwoWire *wire;
    int address;
    float gyroXOffset, gyroYOffset, gyroZOffset;
    int16_t rawAccX, rawAccY, rawAccZ, rawGyroX, rawGyroY, rawGyroZ;
    float accX, accY, accZ, gyroX, gyroY, gyroZ;
    long intervalStart;
    float dt;
    float angGyroX, angGyroY, angGyroZ, angAccX, angAccY;
    float filterAccelCoeff, filterGyroCoeff;
    float angX, angY, angZ;
    void baseInititalize();

public:
    MPU6050(TwoWire &w, int i2cAddress = ADDRESS);
    void initialize();
    void execute();
    void updateRawAccel();
    void updateRawGyro();
    void registerWrite(byte registerAddress, byte data);
    void calibrate();
    float getGyroXOffset() { return gyroXOffset; };
    float getGyroYOffset() { return gyroYOffset; };
    float getGyroZOffset() { return gyroZOffset; };
    void setGyroOffsets(float x, float y, float z);
    int16_t getRawAccX() { return rawAccX; };
    int16_t getRawAccY() { return rawAccY; };
    int16_t getRawAccZ() { return rawAccZ; };
    int16_t getRawGyroX() { return rawGyroX; };
    int16_t getRawGyroY() { return rawGyroY; };
    int16_t getRawGyroZ() { return rawGyroZ; };
    float getAccX() { return accX; };
    float getAccY() { return accY; };
    float getAccZ() { return accZ; };
    float getGyroX() { return gyroX; };
    float getGyroY() { return gyroY; };
    float getGyroZ() { return gyroZ; };
    float getAngAccX() { return angAccX; };
    float getAngAccY() { return angAccY; };
    float getAngGyroX() { return angGyroX; };
    float getAngGyroY() { return angGyroY; };
    float getAngGyroZ() { return angGyroZ; };
    float getAngX() { return angX; };
    float getAngY() { return angY; };
    float getAngZ() { return angZ; };
    float getFilterAccCoeff() { return filterAccelCoeff; };
    float getFilterGyroCoeff() { return filterGyroCoeff; };
    void setFilterAccCoeff(float coeff);
    void setFilterGyroCoeff(float coeff);
    void setAccWakeUp();
};
#endif
