/*
  Project: SensoGripBLE
  Created: 08.09.2020
  Updated: 07.01.2021
  Author:  Primoz Flander
  Arduino board: Sensogrip NINA-B306
  
  ARDUINO I/O   /     DESCRIPTION
  A0            ->     Tip pressure sensor
  A1            ->     Finger pressure sensor
  A2            ->     Battery level measurement
  LED_R         ->     D9
  LED_G         ->     D10
  LED_B         ->     D11
  MPU6050       ->     I2C (A4, A5), INT (D2)
*/

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "MPU6050.h"
#include "FlashBLE.h"
#include "Led.h"
#include "RGBLed.h"
#include "Sensor.h"
#include "Statistics.h"
#include "RangeAI.h"
#include "DebugUtils.h"
#include "FunctionPrototypes.h"
#include "GlobalVariables.h"

void setup()
{
    // while (!Serial);
    initIO();
    //saveConfigurationToFlash();
    readConfigurationFromFlash();
    ledPwr.off();
}

void loop()
{
    timingStart = millis();
    readSensors();
    showLedFeedback();
    sendAndReceiveBLEData();
    //sendSensorData();
    updateStatistics();
    sleepToSavePower();
    powerOffFunctionality();
    //timingStop = millis();
    //Serial.println(timingStop - timingStart);
}

void initIO()
{
    Serial.begin(115200);
    if (!BLE.begin())
    {
        DEBUG_PRINTLN("starting BLE failed!");
    }
    mpu.initialize();
    BLEconfig();
    configureSensors();
}

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
    mpu.execute();
    if (angleCorrection)
        tipSensor.setPitch(mpu.getAngX());
}

void sendSensorData()
{
    static unsigned long previousSendMillis = 0;
    if (millis() - previousSendMillis >= sendInterval)
    {
        String payload = String(millis()) + " " + String(tipSensor.getValue()) + " " + String(fingerSensor.getValue()) + " " + String(batteryLevel.getFilteredValue()) + " " + String(mpu.getAngX());
        Serial.println(payload);
        previousSendMillis += sendInterval;
    }
}

void showLedFeedback()
{
    static int ledFeedbackCounter = 0;
    static unsigned long previousLedFeedbackMillis = 0;
    if (millis() - previousLedFeedbackMillis >= ledFeedbackInterval)
    {
        if (aiRangeAssistance)
            calculateOptimalRange();
        if (tipSensor.isInRange() && fingerSensor.isInRange())
        {
            if (positiveFeedback)
                rgbLed.displayOkColor();
            else
                rgbLed.displayNoColor();
            ledFeedbackCounter += 10;
            ledFeedbackCounter = min(ledFeedbackCounter, tipPressureReleaseDelay);
        }
        else if (tipSensor.isUnderRange() && ledFeedbackCounter > 0)
        {
            ledFeedbackCounter -= 1;
        }
        else
        {
            if (positiveFeedback && !tipSensor.isInRange() && !fingerSensor.isInRange())
                rgbLed.displayNoColor();
            else if (!positiveFeedback && !tipSensor.isInRange() && !fingerSensor.isInRange())
                rgbLed.displayNokColor();
            ledFeedbackCounter = 0;
        }
        if (ledFeedbackCounter <= 0)
            showLedAssistance();
        previousLedFeedbackMillis += ledFeedbackInterval;
    }
}

void showLedAssistance()
{
    switch (ledAssistance)
    {
    case 1:
        if (!tipSensor.isInRange() != !fingerSensor.isInRange())
        {
            rgbLed.displaySimpleAssistanceColor();
        }
        break;
    case 2:
        if (tipSensor.isInRange() && !fingerSensor.isInRange())
        {
            rgbLed.displayTipAssistanceColor();
        }
        if (!tipSensor.isInRange() && fingerSensor.isInRange())
        {
            rgbLed.displayFingerAssistanceColor();
        }
        break;
    default:
        if (!tipSensor.isInRange() != !fingerSensor.isInRange())
        {
            rgbLed.displayNoColor();
        }
        break;
    }
}

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
    BLE.setLocalName("SensoGrip");
    BLE.setAdvertisedService(sensoGripService);
    sensoGripService.addCharacteristic(refTipValueChar);
    sensoGripService.addCharacteristic(refTipRangeChar);
    sensoGripService.addCharacteristic(refFingerValueChar);
    sensoGripService.addCharacteristic(refFingerRangeChar);
    sensoGripService.addCharacteristic(feedbackChar);
    sensoGripService.addCharacteristic(ledAssistanceTypeChar);
    sensoGripService.addCharacteristic(tipPressureReleaseDelayChar);
    sensoGripService.addCharacteristic(ledTurnOnSpeedChar);
    sensoGripService.addCharacteristic(ledTipAssistanceColorChar);
    sensoGripService.addCharacteristic(ledFingerAssistanceColorChar);
    sensoGripService.addCharacteristic(ledSimpleAssistanceColorChar);
    sensoGripService.addCharacteristic(ledOkColorChar);
    sensoGripService.addCharacteristic(ledNokColorChar);
    sensoGripService.addCharacteristic(fastStreamChar);
    sensoGripService.addCharacteristic(slowStreamChar);
    sensoGripService.addCharacteristic(minutesPassedInUseChar);
    sensoGripService.addCharacteristic(minutesPassedInRangeChar);
    sensoGripService.addCharacteristic(calibrateChar);
    sensoGripService.addCharacteristic(aiChar);
    sensoGripService.addCharacteristic(angleCorrectionChar);
    sensoGripService.addCharacteristic(configurationChar);
    sensoGripService.addCharacteristic(configuration2Char);
    BLE.addService(sensoGripService);
    BLE.advertise();
    DEBUG_PRINTLN("BLE ready");
}

void sendBLEData()
{
    static unsigned long previousBLEMillis = 0;
    if (millis() - previousBLEMillis >= BLEInterval)
    {
        char sendBuffer[32];
        String payload = String(millis()) + "," + String(tipSensor.getValue()) + "," + String(fingerSensor.getValue()) + "," + String((int)mpu.getAngX()) + "\0";
        payload.toCharArray(sendBuffer, 32);
        fastStreamChar.writeValue(sendBuffer);
        previousBLEMillis += BLEInterval;
    }

    static unsigned long previousSlowBLEMillis = 0;
    if (millis() - previousSlowBLEMillis >= slowBLEInterval)
    {
        char sendBuffer[32];
        String payload = String(batteryLevel.getFilteredValue()) + "," + String(Stats.getMinutesInRange()) + "," + String(Stats.getMinutesInUse()) + "," + String(tipSensor.getUpperRange() - tipSensor.getRefRange()) + "," + String(fingerSensor.getUpperRange() - fingerSensor.getRefRange()) + "\0";
        payload.toCharArray(sendBuffer, 32);
        slowStreamChar.writeValue(sendBuffer);
        previousSlowBLEMillis += slowBLEInterval;
    }
}

void getBLEData()
{
    if (feedbackChar.written())
    {
        positiveFeedback = feedbackChar.value();
        Flash.put("positiveFeedback", String(positiveFeedback));
        rgbLed.off();
        updateConfigurationChar();
        DEBUG_PRINTLN("feedback written: " + String(feedbackChar.value()));
    }
    if (aiChar.written())
    {
        aiRangeAssistance = aiChar.value();
        updateConfigurationChar();
        DEBUG_PRINTLN("ai written: " + String(aiChar.value()));
    }
    if (calibrateChar.written())
    {
        tipSensor.calibrate();
        fingerSensor.calibrate();
        Flash.put("tipSensorOffset", String(tipSensor.getOffset()));
        Flash.put("fingerSensorOffset", String(fingerSensor.getOffset()));
        DEBUG_PRINTLN("calibration written: " + String(tipSensor.getOffset()) + "," + String(fingerSensor.getOffset()));
    }
    if (ledAssistanceTypeChar.written())
    {
        ledAssistance = ledAssistanceTypeChar.value();
        Flash.put("ledAssistance", String(ledAssistance));
        rgbLed.off();
        updateConfigurationChar();
        DEBUG_PRINTLN("ledAssistance written: " + String(ledAssistanceTypeChar.value()));
    }
    if (refTipValueChar.written())
    {
        tipSensor.setRefValue(refTipValueChar.value());
        Flash.put("refTipValue", String(tipSensor.getRefValue()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refTipValue written: " + String(refTipValueChar.value()));
    }
    if (refTipRangeChar.written())
    {
        tipSensor.setRefRange(refTipRangeChar.value());
        Flash.put("refTipRange", String(tipSensor.getRefRange()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refTipRange written: " + String(refTipRangeChar.value()));
    }
    if (refFingerValueChar.written())
    {
        fingerSensor.setRefValue(refFingerValueChar.value());
        Flash.put("refFingerValue", String(fingerSensor.getRefValue()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refFingerValue written: " + String(refFingerValueChar.value()));
    }
    if (refFingerRangeChar.written())
    {
        fingerSensor.setRefRange(refFingerRangeChar.value());
        Flash.put("refFingerRange", String(fingerSensor.getRefRange()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refFingerRange written: " + String(refFingerRangeChar.value()));
    }
    if (tipPressureReleaseDelayChar.written())
    {
        tipPressureReleaseDelay = tipPressureReleaseDelayChar.value();
        Flash.put("tipPressureReleaseDelay", String(tipPressureReleaseDelay));
        updateConfigurationChar();
        DEBUG_PRINTLN("tipPressureReleaseDelay written: " + String(tipPressureReleaseDelayChar.value()));
    }
    if (ledTurnOnSpeedChar.written())
    {
        rgbLed.setLedTurnOnSpeed(ledTurnOnSpeedChar.value());
        Flash.put("ledTurnOnSpeed", String(rgbLed.getLedTurnOnSpeed()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledTurnOnSpeed written: " + String(ledTurnOnSpeedChar.value()));
    }
    if (ledTipAssistanceColorChar.written())
    {
        rgbLed.setLedTipAssistanceColor(ledTipAssistanceColorChar.value());
        Flash.put("ledTipAssistanceColor", String(rgbLed.getLedTipAssistanceColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledTipAssistanceColor written: " + String(ledTipAssistanceColorChar.value()));
    }
    if (ledFingerAssistanceColorChar.written())
    {
        rgbLed.setLedFingerAssistanceColor(ledFingerAssistanceColorChar.value());
        Flash.put("ledFingerAssistanceColor", String(rgbLed.getLedFingerAssistanceColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledFingerAssistanceColor written: " + String(ledFingerAssistanceColorChar.value()));
    }
    if (ledSimpleAssistanceColorChar.written())
    {
        rgbLed.setLedSimpleAssistanceColor(ledSimpleAssistanceColorChar.value());
        Flash.put("ledSimpleAssistanceColor", String(rgbLed.getLedSimpleAssistanceColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledSimpleAssistanceColor written: " + String(ledSimpleAssistanceColorChar.value()));
    }
    if (ledOkColorChar.written())
    {
        rgbLed.setLedOkColor(ledOkColorChar.value());
        Flash.put("ledOkColor", String(rgbLed.getLedOkColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledOkColor written: " + String(ledOkColorChar.value()));
    }
    if (ledNokColorChar.written())
    {
        rgbLed.setLedNokColor(ledNokColorChar.value());
        Flash.put("ledNokColor", String(rgbLed.getLedNokColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledNokColor written: " + String(ledNokColorChar.value()));
    }
    if (minutesPassedInUseChar.written())
    {
        Stats.setMinutesInUse(minutesPassedInUseChar.value());
        Flash.put("minutesInUse", String(Stats.getMinutesInUse()));
        DEBUG_PRINTLN("minutesInUse reset");
    }
    if (minutesPassedInRangeChar.written())
    {
        Stats.setMinutesInRange(minutesPassedInRangeChar.value());
        Flash.put("minutesInRange", String(Stats.getMinutesInRange()));
        DEBUG_PRINTLN("minutesInRange reset");
    }
    if (angleCorrectionChar.written())
    {
        angleCorrection = angleCorrectionChar.value();
        Flash.put("angleCorrection", String(angleCorrectionChar.value()));
        updateConfigurationChar();
        DEBUG_PRINTLN("angleCorrection: " + String(angleCorrectionChar.value()));
    }
}

void saveConfigurationToFlash()
{
    Flash.put("positiveFeedback", String(positiveFeedback));
    Flash.put("ledAssistance", String(ledAssistance));
    Flash.put("refTipValue", String(tipSensor.getRefValue()));
    Flash.put("refTipRange", String(tipSensor.getRefRange()));
    Flash.put("tipSensorOffset", String(tipSensor.getOffset()));
    Flash.put("refFingerValue", String(fingerSensor.getRefValue()));
    Flash.put("refFingerRange", String(fingerSensor.getRefRange()));
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
    Flash.put("angleCorrection", String(angleCorrection));
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
        positiveFeedback = ("1" == Flash.get("positiveFeedback")) ? true : false;
        ledAssistance = Flash.get("ledAssistance").toInt();
        tipSensor.setRefValue(Flash.get("refTipValue").toInt());
        tipSensor.setRefRange(Flash.get("refTipRange").toInt());
        tipSensor.setOffset(Flash.get("tipSensorOffset").toInt());
        fingerSensor.setRefValue(Flash.get("refFingerValue").toInt());
        fingerSensor.setRefRange(Flash.get("refFingerRange").toInt());
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
        angleCorrection = Flash.get("angleCorrection").toInt();
        DEBUG_PRINTLN("Configuration read from flash");
        updateConfigurationChar();
    }
}

void updateConfigurationChar()
{
    char sendBuffer[32];
    String payload = String(tipSensor.getRefValue()) + "," + String(tipSensor.getRefRange()) + "," + String(fingerSensor.getRefValue()) + "," + String(fingerSensor.getRefRange()) + "," + String(positiveFeedback) + "," + String(ledAssistance) + "," + String(aiRangeAssistance) + "\0";
    payload.toCharArray(sendBuffer, 32);
    configurationChar.writeValue(sendBuffer);
    //sendBuffer[32];
    payload = String(angleCorrection) + "," + String(tipPressureReleaseDelay) + "," + String(rgbLed.getLedTurnOnSpeed()) + "," + String(rgbLed.getLedSimpleAssistanceColor()) + "," + String(rgbLed.getLedTipAssistanceColor()) + "," + String(rgbLed.getLedFingerAssistanceColor()) + "," + String(rgbLed.getLedOkColor()) + "," + String(rgbLed.getLedNokColor()) + "\0";
    payload.toCharArray(sendBuffer, 32);
    configuration2Char.writeValue(sendBuffer);
    DEBUG_PRINTLN("Configuration char updated");
}

void updateStatistics()
{
    Stats.update(tipSensor.getValue(), fingerSensor.getValue(), tipSensor.isInRange(), fingerSensor.isInRange());
    static unsigned long previousStatsMillis = 0;
    if (millis() - previousStatsMillis >= statsSaveInterval)
    {
        Flash.put("minutesPoweredOn", String(Stats.getMinutesPoweredOn()));
        Flash.put("minutesInUse", String(Stats.getMinutesInUse()));
        Flash.put("minutesInRange", String(Stats.getMinutesInRange()));
        previousStatsMillis += statsSaveInterval;
        DEBUG_PRINTLN("Statistics saved to flash");
    }
}

void calculateOptimalRange()
{
    tipRangeAI.execute(tipSensor.getValue(), tipSensor.getRefValue());
    tipSensor.setUpperRange(tipRangeAI.getUpperRange());
    tipSensor.setLowerRange(tipRangeAI.getLowerRange());
    fingerRangeAI.execute(fingerSensor.getValue(), fingerSensor.getRefValue());
    fingerSensor.setUpperRange(fingerRangeAI.getUpperRange());
    fingerSensor.setLowerRange(fingerRangeAI.getLowerRange());
}

void powerOffFunctionality()
{
    static int secondsToSleepCounter = 0;
    static unsigned long previousSleepMillis = 0;
    if (millis() - previousSleepMillis >= 1000)
    {
        if (fingerSensor.getValue() < 15)
            secondsToSleepCounter++;
        else
            secondsToSleepCounter = 0;
        previousSleepMillis += 1000;
    }
    if (secondsToSleepCounter > sleepTimer)
    {
        digitalWrite(SNZ_PWR, LOW);
        onboardLedR.off();
        onboardLedG.off();
        onboardLedB.off();
        rgbLed.off();
        ledBuiltIn.off();
        ledPwr.off();
        BLE.end();
        mpu.setAccWakeUp();
        nrf_gpio_cfg_sense_input(P1_11, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
        // nrf_gpio_cfg_sense_input(P0_19, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);  // sensogrip board D2
        DEBUG_PRINTLN("Power off");
        NRF_POWER->SYSTEMOFF = 1;
    }
}

void sleepToSavePower()
{
    long diff = millis() - timingStart;
    if (diff < 10)
        delay(10 - diff);
    // DEBUG_PRINTLN(String("loop ms: ") + diff);
}