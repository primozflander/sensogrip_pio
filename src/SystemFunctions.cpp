#include "SystemFunctions.h"

enum color
{
    red = 0,
    yellow = 60,
    green = 120,
    cyan = 180,
    blue = 240,
    pink = 300,
};

void initIO()
{
    Wire.begin();
    mpu.begin();
    // calibrateIMU();
    isBatteryOk();
    Serial.begin(9600);
    if (!BLE.begin())
    {
        DEBUG_PRINTLN("starting BLE failed!");
    }
    batteryLevel.setUpdateInterval(75);
    BLEconfig();
    configureSensors();
}

String stringSegToFloat(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void calibrateIMU()
{
    float ax, ay, az, gx, gy, gz;
    mpu.calcOffsets(ax ,ay, az, gx, gy, gz);
    Serial.println("IMU offsets: " + String(ax) + " " + String(ay) + " " + String(az) + " " + String(gx) + " " + String(gy) + " " + String(gz));
    Flash.put("imuConfig", String(ax) + " " + String(ay) + " " + String(az) + " " + String(gx) + " " + String(gy) + " " + String(gz));
    Serial.println("IMU config written");
}

void isBatteryOk()
{
    if (!(batteryLevel.getSensorValue() > 0))
    {
        mpu.setAccWakeUp();
        nrf_gpio_cfg_sense_input(P0_19, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW); // sensogrip board D2
        NRF_POWER->SYSTEMOFF = 1;
    }
}

void onCharging()
{
    bool wasCharging = false;
    int currentColor = red;
    while (NRF_POWER->USBREGSTATUS == 3)
    {
        if (batteryLevel.getSensorValue() > 98)
        {
            rgbLed.setLedColorHsvWithTransition(green, 1.0, 0.1);
            currentColor = 120;
        }
        else if (batteryLevel.getSensorValue() < 95)
        {
            rgbLed.setLedColorHsvWithTransition(red, 1.0, 0.2);
            currentColor = 0;
        }
        else
        {
            rgbLed.setLedColorHsvWithTransition(currentColor, 1.0, 0.1);
        }
        wasCharging = true;
    }
    if (wasCharging)
    {
        __NVIC_SystemReset();
    }
}

void saveConfigurationToFlash()
{
    Flash.put("positiveFeedback", String(isPositiveFeedback));
    Flash.put("ledAssistance", String(ledFeedbackType));
    Flash.put("tipUpperRange", String(tipSensor.getUpperRange()));
    Flash.put("tipLowerRange", String(tipSensor.getLowerRange()));
    Flash.put("tipSensorOffset", String(tipSensor.getOffset()));
    Flash.put("fingerUpperRange", String(fingerSensor.getUpperRange()));
    Flash.put("fingerLowerRange", String(fingerSensor.getLowerRange()));
    Flash.put("fingerSensorOffset", String(fingerSensor.getOffset()));
    Flash.put("tipPressureReleaseDelay", String(tipPressureReleaseDelay));
    Flash.put("ledTurnOnSpeed", String(rgbLed.getLedTurnOnSpeed()));
    Flash.put("ledTipAssistanceColor", String(rgbLed.getLedTipAssistanceColor()));
    Flash.put("ledFingerAssistanceColor", String(rgbLed.getLedFingerAssistanceColor()));
    Flash.put("ledSimpleAssistanceColor", String(rgbLed.getLedSimpleAssistanceColor()));
    Flash.put("ledOkColor", String(rgbLed.getLedOkColor()));
    Flash.put("ledNokColor", String(rgbLed.getLedNokColor()));
    Flash.put("minutesPoweredOn", String(Stats.getMinutesPoweredOn()));
    Flash.put("minutesInUse", String(Stats.getMinutesInUse()));
    Flash.put("minutesInRange", String(Stats.getMinutesInRange()));
    Flash.put("angleCorrection", String(isAngleCorrected));
    Flash.put("tipSensorCorrectionFactor", String(tipSensor.getOutputCorrectionFactor()));
    Flash.put("fingerSensorCorrectionFactor", String(fingerSensor.getOutputCorrectionFactor()));
    Flash.put("imuConfig", "0 0 0 0 0 0");
    DEBUG_PRINTLN("Configuration saved to flash");
}

void readConfigurationFromFlash()
{
    if (Flash.get("positiveFeedback") == "error")
    {
        DEBUG_PRINTLN("Error reading Flash");
    }
    else
    {
        isPositiveFeedback = ("1" == Flash.get("positiveFeedback")) ? true : false;
        ledFeedbackType = Flash.get("ledAssistance").toInt();
        tipSensor.setUpperRange(Flash.get("tipUpperRange").toInt());
        tipSensor.setLowerRange(Flash.get("tipLowerRange").toInt());
        tipSensor.setOffset(Flash.get("tipSensorOffset").toInt());
        fingerSensor.setUpperRange(Flash.get("fingerUpperRange").toInt());
        fingerSensor.setLowerRange(Flash.get("fingerLowerRange").toInt());
        fingerSensor.setOffset(Flash.get("fingerSensorOffset").toInt());
        tipPressureReleaseDelay = Flash.get("tipPressureReleaseDelay").toInt();
        rgbLed.setLedTurnOnSpeed(Flash.get("ledTurnOnSpeed").toInt());
        rgbLed.setLedTipAssistanceColor(Flash.get("ledTipAssistanceColor").toInt());
        rgbLed.setLedFingerAssistanceColor(Flash.get("ledFingerAssistanceColor").toInt());
        rgbLed.setLedSimpleAssistanceColor(Flash.get("ledSimpleAssistanceColor").toInt());
        rgbLed.setLedOkColor(Flash.get("ledOkColor").toInt());
        rgbLed.setLedNokColor(Flash.get("ledNokColor").toInt());
        Stats.setMinutesPoweredOn(Flash.get("minutesPoweredOn").toInt());
        Stats.setMinutesInUse(Flash.get("minutesInUse").toInt());
        Stats.setMinutesInRange(Flash.get("minutesInRange").toInt());
        isAngleCorrected = Flash.get("angleCorrection").toInt();
        tipSensor.setOutputCorrectionFactor(Flash.get("tipSensorCorrectionFactor").toDouble());
        fingerSensor.setOutputCorrectionFactor(Flash.get("fingerSensorCorrectionFactor").toDouble());
        String imuConfig = Flash.get("imuConfig");
        mpu.setAccOffsets(stringSegToFloat(imuConfig, ' ', 0).toFloat(), stringSegToFloat(imuConfig, ' ', 1).toFloat(), stringSegToFloat(imuConfig, ' ', 2).toFloat());
        mpu.setGyroOffsets(stringSegToFloat(imuConfig, ' ', 3).toFloat(), stringSegToFloat(imuConfig, ' ', 4).toFloat(), stringSegToFloat(imuConfig, ' ', 5).toFloat());
        updateConfigurationChar();
        DEBUG_PRINTLN("Configuration read from flash");
        DEBUG_PRINTLN("imuConfig: " + String(imuConfig));
        DEBUG_PRINTLN("sensor correction factors: " + String(tipSensor.getOutputCorrectionFactor()) + " " + String(fingerSensor.getOutputCorrectionFactor()));
        
    }
}

void updateStatistics()
{
    Stats.update(tipSensor.getValue(), fingerSensor.getValue(), tipSensor.isInRange(), fingerSensor.isInRange());
    static unsigned long previousStatsMillis = 0;
    if (millis() - previousStatsMillis >= STATS_SAVE_INTERVAL)
    {
        Flash.put("minutesPoweredOn", String(Stats.getMinutesPoweredOn()));
        Flash.put("minutesInUse", String(Stats.getMinutesInUse()));
        Flash.put("minutesInRange", String(Stats.getMinutesInRange()));
        previousStatsMillis += STATS_SAVE_INTERVAL;
        DEBUG_PRINTLN("Statistics saved to flash");
    }
}

void powerOffFunctionality()
{
    static int secondsToSleepCounter = 0;
    static unsigned long previousSleepMillis = 0;
    if (millis() - previousSleepMillis >= 1000)
    {
        if ((abs(mpu.getGyroX()) + abs(mpu.getGyroY()) + abs(mpu.getGyroZ())) < 50)
        {
            secondsToSleepCounter++;
        }
        else
        {
            secondsToSleepCounter = 0;
        }
        previousSleepMillis = millis();
    }
    if (secondsToSleepCounter > SLEEP_TIMER)
    {
        digitalWrite(SNZ_PWR, LOW);
        rgbLed.off();
        ledBuiltIn.off();
        ledPwr.off();
        BLE.end();
        mpu.setAccWakeUp();
        nrf_gpio_cfg_sense_input(P0_19, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW); // sensogrip board D2
        DEBUG_PRINTLN("Power off");
        NRF_POWER->SYSTEMOFF = 1;
    }
}

void sleepToSavePower()
{
    long diff = millis() - loopStartTime;
    if (diff < 10)
        delay(10 - diff);
    // DEBUG_PRINTLN(String("loop ms: ") + diff);
}
