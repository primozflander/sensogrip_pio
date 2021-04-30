#pragma once
#include "Arduino.h"
#include "Wire.h"

#define CONFIG_REGISTER 0x1a
#define GYRO_CONFIG_REGISTER 0x1b
#define ACCEL_CONFIG_REGISTER 0x1c
#define MOT_THR_REGISTER 0x1f
#define MOT_DUR_REGISTER 0x20
#define INT_ENABLE_REGISTER 0x38
#define INT_PIN_CFG_REGISTER 0x37
#define ADDR 0x68
#define SMPLRT_DIV_REGISTER 0x19
#define CONFIG_REGISTER 0x1a
#define GYRO_CONFIG_REGISTER 0x1b
#define ACCEL_CONFIG_REGISTER 0x1c
#define PWR_MGMT_1_REGISTER 0x6b
#define PWR_MGMT_2_REGISTER 0x6c
#define GYRO_OUT_REGISTER 0x43
#define ACCEL_OUT_REGISTER 0x3b
#define RAD_2_DEG 57.29578
#define CALIB_OFFSET_NB_MES 5000
#define TEMP_LSB_2_DEGREE 340.0
#define TEMP_LSB_OFFSET 12412.0
#define DEFAULT_GYRO_COEFF 0.98

class MPU6050
{
private:
    TwoWire *wire;
    float gyro_lsb_to_degsec, acc_lsb_to_g;
    float gyroXoffset, gyroYoffset, gyroZoffset;
    float accXoffset, accYoffset, accZoffset;
    float temp, accX, accY, accZ, gyroX, gyroY, gyroZ;
    float angleAccX, angleAccY;
    float angleX, angleY, angleZ;
    long preInterval;
    float filterGyroCoef;

public:
    MPU6050(TwoWire &w);
    byte begin(int gyro_config_num = 1, int acc_config_num = 0);
    byte writeData(byte reg, byte data);
    byte readData(byte reg);
    void calcOffsets(bool is_calc_gyro = true, bool is_calc_acc = true);
    void calcGyroOffsets() { calcOffsets(true, false); }; // retro-compatibility with v1.0.0
    void calcAccOffsets() { calcOffsets(false, true); };  // retro-compatibility with v1.0.0
    byte setGyroConfig(int config_num);
    byte setAccConfig(int config_num);
    void setGyroOffsets(float x, float y, float z);
    void setAccOffsets(float x, float y, float z);
    void setFilterGyroCoef(float gyro_coeff);
    void setFilterAccCoef(float acc_coeff);
    float getGyroXoffset() { return gyroXoffset; };
    float getGyroYoffset() { return gyroYoffset; };
    float getGyroZoffset() { return gyroZoffset; };
    float getAccXoffset() { return accXoffset; };
    float getAccYoffset() { return accYoffset; };
    float getAccZoffset() { return accZoffset; };
    float getFilterGyroCoef() { return filterGyroCoef; };
    float getFilterAccCoef() { return 1.0 - filterGyroCoef; };
    float getTemp() { return temp; };
    float getAccX() { return accX; };
    float getAccY() { return accY; };
    float getAccZ() { return accZ; };
    float getGyroX() { return gyroX; };
    float getGyroY() { return gyroY; };
    float getGyroZ() { return gyroZ; };
    float getAccAngleX() { return angleAccX; };
    float getAccAngleY() { return angleAccY; };
    float getAngX() { return angleX; };
    float getAngY() { return angleY; };
    float getAngZ() { return angleZ; };
    void fetchData();
    void update();
    void setAccWakeUp();
};