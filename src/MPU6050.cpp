#include "MPU6050.h"
MPU6050::MPU6050(TwoWire &w)
{
    wire = &w;
    setFilterGyroCoef(DEFAULT_GYRO_COEFF);
    setGyroOffsets(0, 0, 0);
    setAccOffsets(0, 0, 0);
}

byte MPU6050::begin(int gyro_config_num, int acc_config_num)
{
    writeData(SMPLRT_DIV_REGISTER, 0x00);
    writeData(CONFIG_REGISTER, 0x00);
    // writeData(GYRO_CONFIG, 0x08);
    // writeData(ACCEL_CONFIG, 0x00);
    // writeData(PWR_MGMT_1, 0x01);
    // writeData(PWR_MGMT_2, 0x80);
    setGyroConfig(gyro_config_num);
    setAccConfig(acc_config_num);
    byte status = writeData(PWR_MGMT_1_REGISTER, 0x01);
    this->update();
    angleX = this->getAccAngleX();
    angleY = this->getAccAngleY();
    preInterval = millis();
    return status;
}

byte MPU6050::writeData(byte reg, byte data)
{
    wire->beginTransmission(ADDR);
    wire->write(reg);
    wire->write(data);
    byte status = wire->endTransmission();
    return status; // 0 if success
}

byte MPU6050::readData(byte reg)
{
    wire->beginTransmission(ADDR);
    wire->write(reg);
    wire->endTransmission(true);
    wire->requestFrom(ADDR, 1);
    byte data = wire->read();
    return data;
}

byte MPU6050::setGyroConfig(int config_num)
{
    byte status;
    switch (config_num)
    {
    case 0: // range = +- 250 °/s
        gyro_lsb_to_degsec = 131.0;
        status = writeData(GYRO_CONFIG_REGISTER, 0x00);
        break;
    case 1: // range = +- 500 °/s
        gyro_lsb_to_degsec = 65.5;
        status = writeData(GYRO_CONFIG_REGISTER, 0x08);
        break;
    case 2: // range = +- 1000 °/s
        gyro_lsb_to_degsec = 32.8;
        status = writeData(GYRO_CONFIG_REGISTER, 0x10);
        break;
    case 3: // range = +- 2000 °/s
        gyro_lsb_to_degsec = 16.4;
        status = writeData(GYRO_CONFIG_REGISTER, 0x18);
        break;
    default: // error
        status = 1;
        break;
    }
    return status;
}

byte MPU6050::setAccConfig(int config_num)
{
    byte status;
    switch (config_num)
    {
    case 0: // range = +- 2 g
        acc_lsb_to_g = 16384.0;
        status = writeData(ACCEL_CONFIG_REGISTER, 0x00);
        break;
    case 1: // range = +- 4 g
        acc_lsb_to_g = 8192.0;
        status = writeData(ACCEL_CONFIG_REGISTER, 0x08);
        break;
    case 2: // range = +- 8 g
        acc_lsb_to_g = 4096.0;
        status = writeData(ACCEL_CONFIG_REGISTER, 0x10);
        break;
    case 3: // range = +- 16 g
        acc_lsb_to_g = 2048.0;
        status = writeData(ACCEL_CONFIG_REGISTER, 0x18);
        break;
    default: // error
        status = 1;
        break;
    }
    return status;
}

void MPU6050::setGyroOffsets(float x, float y, float z)
{
    gyroXoffset = x;
    gyroYoffset = y;
    gyroZoffset = z;
}

void MPU6050::setAccOffsets(float x, float y, float z)
{
    accXoffset = x;
    accYoffset = y;
    accZoffset = z;
}

void MPU6050::setFilterGyroCoef(float gyro_coeff)
{
    if ((gyro_coeff < 0) or (gyro_coeff > 1))
    {
        gyro_coeff = DEFAULT_GYRO_COEFF;
    }
    filterGyroCoef = gyro_coeff;
}

void MPU6050::setFilterAccCoef(float acc_coeff)
{
    setFilterGyroCoef(1.0 - acc_coeff);
}

void MPU6050::calcOffsets(bool is_calc_gyro, bool is_calc_acc)
{
    if (is_calc_gyro)
    {
        setGyroOffsets(0, 0, 0);
    }
    if (is_calc_acc)
    {
        setAccOffsets(0, 0, 0);
    }
    float ag[6] = {0, 0, 0, 0, 0, 0}; // 3*acc, 3*gyro
    for (int i = 0; i < CALIB_OFFSET_NB_MES; i++)
    {
        this->fetchData();
        ag[0] += accX;
        ag[1] += accY;
        ag[2] += (accZ - 1.0);
        ag[3] += gyroX;
        ag[4] += gyroY;
        ag[5] += gyroZ;
        delay(1); // wait a little bit between 2 measurements
    }
    if (is_calc_acc)
    {
        accXoffset = ag[0] / CALIB_OFFSET_NB_MES;
        accYoffset = ag[1] / CALIB_OFFSET_NB_MES;
        accZoffset = ag[2] / CALIB_OFFSET_NB_MES;
    }
    if (is_calc_gyro)
    {
        gyroXoffset = ag[3] / CALIB_OFFSET_NB_MES;
        gyroYoffset = ag[4] / CALIB_OFFSET_NB_MES;
        gyroZoffset = ag[5] / CALIB_OFFSET_NB_MES;
    }
    Serial.println(String(accXoffset) + " " + String(accYoffset) + " " + String(accZoffset) + " " + String(gyroXoffset) + " " + String(gyroYoffset) + " " + String(gyroZoffset));
}

void MPU6050::fetchData()
{
    wire->beginTransmission(ADDR);
    wire->write(ACCEL_OUT_REGISTER);
    wire->endTransmission(false);
    wire->requestFrom((int)ADDR, 14);
    int16_t rawData[7]; // [ax,ay,az,temp,gx,gy,gz]
    for (int i = 0; i < 7; i++)
    {
        rawData[i] = wire->read() << 8;
        rawData[i] |= wire->read();
    }
    accX = ((float)rawData[0]) / acc_lsb_to_g - accXoffset;
    accY = ((float)rawData[1]) / acc_lsb_to_g - accYoffset;
    accZ = ((float)rawData[2]) / acc_lsb_to_g - accZoffset;
    temp = (rawData[3] + TEMP_LSB_OFFSET) / TEMP_LSB_2_DEGREE;
    gyroX = ((float)rawData[4]) / gyro_lsb_to_degsec - gyroXoffset;
    gyroY = ((float)rawData[5]) / gyro_lsb_to_degsec - gyroYoffset;
    gyroZ = ((float)rawData[6]) / gyro_lsb_to_degsec - gyroZoffset;
}

void MPU6050::update()
{
    this->fetchData();
    float sgZ = (accZ >= 0) - (accZ < 0);
    angleAccX = atan2(accY, sgZ * sqrt(accZ * accZ + accX * accX)) * RAD_2_DEG;
    angleAccY = -atan2(accX, sqrt(accZ * accZ + accY * accY)) * RAD_2_DEG;
    unsigned long Tnew = millis();
    float dt = (Tnew - preInterval) * 1e-3;
    preInterval = Tnew;
    angleX = (filterGyroCoef * (angleX + gyroX * dt)) + ((1.0 - filterGyroCoef) * angleAccX);
    angleY = (filterGyroCoef * (angleY + gyroY * dt)) + ((1.0 - filterGyroCoef) * angleAccY);
    angleZ += gyroZ * dt;
}

void MPU6050::setAccWakeUp()
{
    writeData(PWR_MGMT_1_REGISTER, 0b00001000);
    writeData(PWR_MGMT_2_REGISTER, 0b00000111);
    writeData(ACCEL_CONFIG_REGISTER, 0x00);
    writeData(CONFIG_REGISTER, 0x00);
    writeData(INT_ENABLE_REGISTER, 0x40);
    writeData(INT_PIN_CFG_REGISTER, 128);
    writeData(MOT_THR_REGISTER, 10);
    writeData(MOT_DUR_REGISTER, 0x01);
    delay(100);
    writeData(ACCEL_CONFIG_REGISTER, 0x07);
    writeData(PWR_MGMT_2_REGISTER, 0x47);
    writeData(PWR_MGMT_1_REGISTER, 0x20);
}