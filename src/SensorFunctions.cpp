#include "SensorFunctions.h"

void configureSensors()
{
    pinMode(SNZ_PWR, OUTPUT);
    digitalWrite(SNZ_PWR, HIGH);
    rgbLed.ledTest();
}

void readSensors()
{
    tipSensor.update();
    fingerSensor.update();
    batteryLevel.update();
    mpu.update();
    if (isAngleCorrected)
        tipSensor.setPitch(mpu.getAngY());
    else
    {
        tipSensor.setPitch(90);
    }
}

void sendSensorData()
{
    static unsigned long previousSendMillis = 0;
    if (millis() - previousSendMillis >= SERIAL_SEND_INTERVAL)
    {
        String payload = String(millis()) + " " + String(tipSensor.getValue()) + " " + String(fingerSensor.getValue()) + " " + String(batteryLevel.getMedian()) + " " +
                         String(mpu.getAngY()) + " " + String(abs(mpu.getGyroX()) + abs(mpu.getGyroY()) + abs(mpu.getGyroZ()));
        Serial.println(payload);
        previousSendMillis += SERIAL_SEND_INTERVAL;
    }
}