/*
  MPU6050.cpp - Library for easy use of sensorfusion on LSM9DS1 IMU.
  Created by Primoz Flander 06.11.2020.
  Released into the public domain.
*/

#include "MPU6050.h"
MPU6050::MPU6050(TwoWire &w, int i2cAddress)
{
    wire = &w;
    if (i2cAddress == ADDRESS_LOW || i2cAddress == ADDRESS_HIGH)
    {
        address = i2cAddress;
    }
    else
    {
        address = ADDRESS_LOW;
    }
}

static float wrap(float angle)
{
    while (angle > +180)
        angle -= 360;
    while (angle < -180)
        angle += 360;
    return angle;
}

static float angle_average(float wa, float a, float wb, float b)
{
    return wrap(wa * a + wb * (a + wrap(b - a)));
}

void MPU6050::initialize()
{
    wire->begin();
    this->baseInititalize();
}

void MPU6050::baseInititalize()
{
    filterAccelCoeff = DEFAULT_ACCEL_COEFF;
    filterGyroCoeff = DEFAULT_GYRO_COEFF;
    this->registerWrite(SMPLRT_DIV, 0x00);
    this->registerWrite(CONFIG, 0x00);
    this->registerWrite(GYRO_CONFIG, 0x08);
    this->registerWrite(ACCEL_CONFIG, 0x00);
    this->registerWrite(PWR_MGMT_1, 0x01);
    angX = 0;
    angY = 0;
    angZ = 0;
    intervalStart = millis();
}

void MPU6050::execute()
{
    this->updateRawAccel();
    this->updateRawGyro();
    accX = (float)(rawAccX)*ACCEL_TRANSFORMATION_NUMBER;
    accY = (float)(rawAccY)*ACCEL_TRANSFORMATION_NUMBER;
    accZ = (float)(rawAccZ)*ACCEL_TRANSFORMATION_NUMBER;
    gyroX = (float)(rawGyroX - gyroXOffset) * GYRO_TRANSFORMATION_NUMBER;
    gyroY = (float)(rawGyroY - gyroYOffset) * GYRO_TRANSFORMATION_NUMBER;
    gyroZ = (float)(rawGyroZ - gyroZOffset) * GYRO_TRANSFORMATION_NUMBER;
    angAccX = wrap((atan2(accY, sqrt(accZ * accZ + accX * accX))) * RAD_TO_DEG);
    angAccY = wrap((-atan2(accX, sqrt(accZ * accZ + accY * accY))) * RAD_TO_DEG);
    dt = (millis() - intervalStart) * 0.001;
    angGyroX = wrap(angGyroX + gyroX * dt);
    angGyroY = wrap(angGyroY + gyroY * dt);
    angGyroZ = wrap(angGyroZ + gyroZ * dt);
    angX = angle_average(filterAccelCoeff, angAccX, filterGyroCoeff, angX + gyroX * dt);
    angY = angle_average(filterAccelCoeff, angAccY, filterGyroCoeff, angY + gyroY * dt);
    angZ = angGyroZ;
    intervalStart = millis();
}

void MPU6050::updateRawAccel()
{
    wire->beginTransmission(address);
    wire->write(ACCEL_XOUT_H);
    wire->endTransmission(false);
    wire->requestFrom(address, 6, (int)true);
    rawAccX = wire->read() << 8;
    rawAccX |= wire->read();
    rawAccY = wire->read() << 8;
    rawAccY |= wire->read();
    rawAccZ = wire->read() << 8;
    rawAccZ |= wire->read();
}

void MPU6050::updateRawGyro()
{
    wire->beginTransmission(address);
    wire->write(GYRO_XOUT_H);
    wire->endTransmission(false);
    wire->requestFrom(address, 6, (int)true);
    rawGyroX = wire->read() << 8;
    rawGyroX |= wire->read();
    rawGyroY = wire->read() << 8;
    rawGyroY |= wire->read();
    rawGyroZ = wire->read() << 8;
    rawGyroZ |= wire->read();
}

void MPU6050::registerWrite(byte registerAddress, byte data)
{
    wire->beginTransmission(address);
    wire->write(registerAddress);
    wire->write(data);
    wire->endTransmission();
}

void MPU6050::calibrate()
{
    for (int i = 0; i < DISCARDED_MEASURES; i++)
    {
        this->updateRawAccel();
        this->updateRawGyro();
        delay(2);
    }
    float sumGyroX = 0;
    float sumGyroY = 0;
    float sumGyroZ = 0;
    for (int i = 0; i < CALIBRATION_MEASURES; i++)
    {
        this->updateRawAccel();
        this->updateRawGyro();
        sumGyroX += this->getRawGyroX();
        sumGyroY += this->getRawGyroY();
        sumGyroZ += this->getRawGyroZ();
        delay(2);
    }
    sumGyroX /= CALIBRATION_MEASURES;
    sumGyroY /= CALIBRATION_MEASURES;
    sumGyroZ /= CALIBRATION_MEASURES;
    this->setGyroOffsets(sumGyroX, sumGyroY, sumGyroZ);
}

void MPU6050::setGyroOffsets(float x, float y, float z)
{
    gyroXOffset = x;
    gyroYOffset = y;
    gyroZOffset = z;
}

void MPU6050::setFilterAccCoeff(float coeff)
{
    filterAccelCoeff = coeff;
}

void MPU6050::setFilterGyroCoeff(float coeff)
{
    filterGyroCoeff = coeff;
}

void MPU6050::setAccWakeUp()
{
    this->registerWrite(PWR_MGMT_1, 0b00001000);
    this->registerWrite(PWR_MGMT_2, 0b00000111);
    this->registerWrite(ACCEL_CONFIG, 0x00);
    this->registerWrite(CONFIG, 0x00);
    this->registerWrite(INT_ENABLE, 0x40);
    this->registerWrite(INT_PIN_CFG, 128);
    this->registerWrite(MOT_THR, 10);
    this->registerWrite(MOT_DUR, 0x01);
    delay(100);
    this->registerWrite(ACCEL_CONFIG, 0x07);
    this->registerWrite(PWR_MGMT_2, 0x47);
    this->registerWrite(PWR_MGMT_1, 0x20);
}
