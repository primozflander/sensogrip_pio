#include "BleFunctions.h"

void sendAndReceiveBLEData()
{
    BLEDevice central = BLE.central();
    if (central)
    {
        if (central.connected())
        {
            sendBLEData();
            getBLEData();
        }
    }
}

void BLEconfig()
{
    BLE.setLocalName(DISPLAYED_DEVICE_NAME);
    BLE.setAdvertisedService(sensoGripService);
    sensoGripService.addCharacteristic(minutesPassedInUseChar);
    sensoGripService.addCharacteristic(minutesPassedInRangeChar);
    sensoGripService.addCharacteristic(calibrateChar);
    sensoGripService.addCharacteristic(dataStreamChar);
    sensoGripService.addCharacteristic(configurationStateChar);
    BLE.addService(sensoGripService);
    BLE.advertise();
    DEBUG_PRINTLN("BLE ready");
}

void sendBLEData()
{
    static unsigned long previousBLEMillis = 0;
    if (millis() - previousBLEMillis >= BLE_SEND_INTERVAL)
    {
        dataStream.timeStamp = millis() / 100;
        dataStream.tipSensorValue = tipSensor.getValue();
        dataStream.fingerSensorValue = fingerSensor.getValue();
        dataStream.angle = (int)mpu.getAngY();
        dataStream.speed = (int)(abs(mpu.getGyroX()) + abs(mpu.getGyroY()) + abs(mpu.getGyroZ()));
        dataStream.batteryLevel = batteryLevel.getMedian();
        dataStream.minutesInRange = Stats.getMinutesInRange();
        dataStream.minutesInUse = Stats.getMinutesInUse();
        dataStream.tipSensorUpperRange = tipSensor.getUpperRange();
        dataStream.tipSensorLowerRange = tipSensor.getLowerRange();
        dataStream.fingerSensorUpperRange = fingerSensor.getUpperRange();
        dataStream.fingerSensorLowerRange = fingerSensor.getLowerRange();
        dataStream.accX = mpu.getAccX();
        dataStream.accY = mpu.getAccY();
        dataStream.accZ = mpu.getAccZ();
        dataStream.gyroX = mpu.getGyroX();
        dataStream.gyroY = mpu.getGyroY();
        dataStream.gyroZ = mpu.getGyroZ();
        dataStreamChar.writeValue(dataStream.bytes, sizeof dataStream.bytes);
        previousBLEMillis = millis();
    }
}

void getBLEData()
{
    if (calibrateChar.written())
    {
        tipSensor.calibrate();
        fingerSensor.calibrate();
        Flash.put("tipSensorOffset", String(tipSensor.getOffset()));
        Flash.put("fingerSensorOffset", String(fingerSensor.getOffset()));
        DEBUG_PRINTLN("calibration written: " + String(tipSensor.getOffset()) + "," + String(fingerSensor.getOffset()));
    }
    if (minutesPassedInUseChar.written())
    {
        Stats.setMinutesInUse(0);
        Flash.put("minutesInUse", String(Stats.getMinutesInUse()));
        DEBUG_PRINTLN("minutesInUse reset");
    }
    if (minutesPassedInRangeChar.written())
    {
        Stats.setMinutesInRange(0);
        Flash.put("minutesInRange", String(Stats.getMinutesInRange()));
        DEBUG_PRINTLN("minutesInRange reset");
    }
    if (configurationStateChar.written())
    {
        configurationStateChar.readValue(configurationState.bytes, sizeof configurationState.bytes);
        tipSensor.setRefValue(constrain((configurationState.tipSensorUpperRange + configurationState.tipSensorLowerRange) / 2, 0, 5000));
        Flash.put("refTipValue", String(tipSensor.getRefValue()));
        DEBUG_PRINTLN("refTipValue written: " + String((configurationState.tipSensorUpperRange + configurationState.tipSensorLowerRange) / 2));
        tipSensor.setRefRange(constrain((configurationState.tipSensorUpperRange - configurationState.tipSensorLowerRange) / 2, 0, 5000));
        Flash.put("refTipRange", String(tipSensor.getRefRange()));
        DEBUG_PRINTLN("refTipRange written: " + String((configurationState.tipSensorUpperRange - configurationState.tipSensorLowerRange) / 2));
        fingerSensor.setRefValue(constrain((configurationState.fingerSensorUpperRange + configurationState.fingerSensorLowerRange) / 2, 0, 5000));
        Flash.put("refFingerValue", String(fingerSensor.getRefValue()));
        DEBUG_PRINTLN("refFingerValue written: " + String((configurationState.fingerSensorUpperRange + configurationState.fingerSensorLowerRange) / 2));
        fingerSensor.setRefRange(constrain((configurationState.fingerSensorUpperRange - configurationState.fingerSensorLowerRange) / 2, 0, 5000));
        Flash.put("refFingerRange", String(fingerSensor.getRefRange()));
        DEBUG_PRINTLN("refFingerRange written: " + String((configurationState.fingerSensorUpperRange - configurationState.fingerSensorLowerRange) / 2));
        isPositiveFeedback = constrain(configurationState.feedback, 0, 1);
        Flash.put("positiveFeedback", String(isPositiveFeedback));
        rgbLed.off();
        DEBUG_PRINTLN("feedback written: " + String(configurationState.feedback));
        //ledFeedbackType = constrain(configurationState.ledAssistance, 0, 5);
        ledFeedbackType = configurationState.ledAssistance;
        Flash.put("ledAssistance", String(ledFeedbackType));
        rgbLed.off();
        DEBUG_PRINTLN("ledFeedbackType written: " + String(configurationState.ledAssistance));
        isAiRangeAssisted = constrain(configurationState.aiRangeAssistance, 0, 1);
        DEBUG_PRINTLN("ai written: " + String(configurationState.aiRangeAssistance));
        if (isAiRangeAssisted != 1)
        {
            tipSensor.setUpperRange(tipSensor.getRefValue() + tipSensor.getRefRange());
            tipSensor.setLowerRange(tipSensor.getRefValue() - tipSensor.getRefRange());
            fingerSensor.setUpperRange(fingerSensor.getRefValue() + fingerSensor.getRefRange());
            fingerSensor.setLowerRange(fingerSensor.getRefValue() - fingerSensor.getRefRange());
            DEBUG_PRINTLN("tip and finger reseted: " + String(tipSensor.getUpperRange()) + " " + String(tipSensor.getLowerRange()) + " " + String(fingerSensor.getUpperRange()) + " " + String(fingerSensor.getLowerRange()));
        }
        isAngleCorrected = constrain(configurationState.angleCorection, 0, 1);
        Flash.put("angleCorrection", String(isAngleCorrected));
        DEBUG_PRINTLN("angleCorrection: " + String(configurationState.angleCorection));
        tipPressureReleaseDelay = constrain(configurationState.tipPressureReleaseDelay, 0, 500);
        Flash.put("tipPressureReleaseDelay", String(tipPressureReleaseDelay));
        DEBUG_PRINTLN("tipPressureReleaseDelay written: " + String(configurationState.tipPressureReleaseDelay));
        rgbLed.setLedTurnOnSpeed(constrain(configurationState.ledTurnOnSpeed, 0, 500));
        Flash.put("ledTurnOnSpeed", String(rgbLed.getLedTurnOnSpeed()));
        DEBUG_PRINTLN("ledTurnOnSpeed written: " + String(configurationState.ledTurnOnSpeed));
        rgbLed.setLedSimpleAssistanceColor(constrain(configurationState.ledSimpleAssistanceColor, 0, 359));
        Flash.put("ledSimpleAssistanceColor", String(rgbLed.getLedSimpleAssistanceColor()));
        DEBUG_PRINTLN("ledSimpleAssistanceColor written: " + String(configurationState.ledSimpleAssistanceColor));
        rgbLed.setLedTipAssistanceColor(constrain(configurationState.ledTipAssistanceColor, 0, 359));
        Flash.put("ledTipAssistanceColor", String(rgbLed.getLedTipAssistanceColor()));
        DEBUG_PRINTLN("ledTipAssistanceColor written: " + String(configurationState.ledTipAssistanceColor));
        rgbLed.setLedFingerAssistanceColor(constrain(configurationState.ledFingerAssistanceColor, 0, 359));
        Flash.put("ledFingerAssistanceColor", String(rgbLed.getLedFingerAssistanceColor()));
        DEBUG_PRINTLN("ledFingerAssistanceColor written: " + String(configurationState.ledFingerAssistanceColor));
        rgbLed.setLedOkColor(constrain(configurationState.ledOkColor, 0, 359));
        Flash.put("ledOkColor", String(rgbLed.getLedOkColor()));
        DEBUG_PRINTLN("ledOkColor written: " + String(configurationState.ledOkColor));
        rgbLed.setLedNokColor(constrain(configurationState.ledNokColor, 0, 359));
        Flash.put("ledNokColor", String(rgbLed.getLedNokColor()));
        DEBUG_PRINTLN("ledNokColor written: " + String(configurationState.ledNokColor));
        updateConfigurationChar();
    }
}

void updateConfigurationChar()
{
    configurationState.tipSensorUpperRange = tipSensor.getRefValue() + tipSensor.getRefRange();
    configurationState.tipSensorLowerRange = tipSensor.getRefValue() - tipSensor.getRefRange();
    configurationState.fingerSensorUpperRange = fingerSensor.getRefValue() + fingerSensor.getRefRange();
    configurationState.fingerSensorLowerRange = fingerSensor.getRefValue() - fingerSensor.getRefRange();
    configurationState.feedback = isPositiveFeedback;
    configurationState.ledAssistance = ledFeedbackType;
    configurationState.aiRangeAssistance = isAiRangeAssisted;
    configurationState.angleCorection = isAngleCorrected;
    configurationState.tipPressureReleaseDelay = tipPressureReleaseDelay;
    configurationState.ledTurnOnSpeed = rgbLed.getLedTurnOnSpeed();
    configurationState.ledSimpleAssistanceColor = rgbLed.getLedSimpleAssistanceColor();
    configurationState.ledTipAssistanceColor = rgbLed.getLedTipAssistanceColor();
    configurationState.ledFingerAssistanceColor = rgbLed.getLedFingerAssistanceColor();
    configurationState.ledOkColor = rgbLed.getLedOkColor();
    configurationState.ledNokColor = rgbLed.getLedNokColor();
    configurationStateChar.writeValue(configurationState.bytes, sizeof configurationState.bytes);
}