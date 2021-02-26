#include <Arduino.h>
#include <ArduinoBLE.h>
#include "MPU6050.h"
#include "FlashBLE.h"
#include "Led.h"
#include "RGBLed.h"
//#include "Sensor.h"
#include "TipSensor.h"
#include "FingerSensor.h"
#include "BatterySensor.h"
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
    // ledPwr.off();
}

void loop()
{
    //timingStart = millis();
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
    BLEconfig();
    configureSensors();
    mpu.initialize();
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
        tipSensor.setPitch(mpu.getAngY());
}

void sendSensorData()
{
    static unsigned long previousSendMillis = 0;
    if (millis() - previousSendMillis >= SERIAL_SEND_INTERVAL)
    {
        String payload = String(millis()) + " " + String(tipSensor.getValue()) + " " + String(fingerSensor.getValue()) + " " + String(batteryLevel.getFilteredValue()) + " " +
        String(mpu.getAngY()) + " " + String(abs(mpu.getGyroX()) + abs(mpu.getGyroY()) + abs(mpu.getGyroZ()));
        Serial.println(payload);
        previousSendMillis += SERIAL_SEND_INTERVAL;
    }
}

void showLedFeedback()
{
    static int ledFeedbackCounter = 0;
    static unsigned long previousLedFeedbackMillis = 0;
    if (millis() - previousLedFeedbackMillis >= LED_FEEDBACK_INTERVAL)
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
        previousLedFeedbackMillis += LED_FEEDBACK_INTERVAL;
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
    sensoGripService.addCharacteristic(dataStreamChar);
    sensoGripService.addCharacteristic(configurationStateChar);
    sensoGripService.addCharacteristic(testChar);
    BLE.addService(sensoGripService);
    BLE.advertise();
    DEBUG_PRINTLN("BLE ready");
}

void sendBLEData()
{
    static unsigned long previousBLEMillis = 0;
    if (millis() - previousBLEMillis >= BLE_SEND_INTERVAL)
    {
        char sendBuffer[32];
        String payload = String(millis()) + "," + String(tipSensor.getValue()) + "," + String(fingerSensor.getValue()) + "," + 
        String((int)mpu.getAngY()) + "," + String((int)(abs(mpu.getGyroX()) + abs(mpu.getGyroY()) + abs(mpu.getGyroZ()))) + "\0";
        payload.toCharArray(sendBuffer, 32);
        fastStreamChar.writeValue(sendBuffer);

        dataStream.values[0] = millis() / 100;
        dataStream.values[1] = tipSensor.getValue();
        dataStream.values[2] = fingerSensor.getValue();
        dataStream.values[3] = (int)mpu.getAngY();
        dataStream.values[4] = (int)(abs(mpu.getGyroX()) + abs(mpu.getGyroY()) + abs(mpu.getGyroZ()));
        dataStream.values[5] = batteryLevel.getFilteredValue();
        dataStream.values[6] = Stats.getMinutesInRange();
        dataStream.values[7] = Stats.getMinutesInUse();
        dataStream.values[8] = tipSensor.getUpperRange();
        dataStream.values[9] = tipSensor.getLowerRange();
        dataStream.values[10] = fingerSensor.getUpperRange();
        dataStream.values[11] = fingerSensor.getLowerRange();
        dataStreamChar.writeValue(dataStream.bytes, sizeof dataStream.bytes);
        previousBLEMillis += BLE_SEND_INTERVAL;
    }

    static unsigned long previousSlowBLEMillis = 0;
    if (millis() - previousSlowBLEMillis >= SLOW_BLE_SEND_INTERVAL)
    {
        char sendBuffer[32];
        String payload = String(batteryLevel.getFilteredValue()) + "," + String(Stats.getMinutesInRange()) + "," + String(Stats.getMinutesInUse()) + "," + 
        String(tipSensor.getUpperRange() - tipSensor.getRefRange()) + "," + String(fingerSensor.getUpperRange() - fingerSensor.getRefRange()) + "\0";
        payload.toCharArray(sendBuffer, 32);
        slowStreamChar.writeValue(sendBuffer);
        previousSlowBLEMillis += SLOW_BLE_SEND_INTERVAL;
    }
}

void getBLEData()
{
    if (feedbackChar.written())
    {
        positiveFeedback = constrain(feedbackChar.value(), 0, 1);
        Flash.put("positiveFeedback", String(positiveFeedback));
        rgbLed.off();
        updateConfigurationChar();
        DEBUG_PRINTLN("feedback written: " + String(feedbackChar.value()));
    }
    if (aiChar.written())
    {
        aiRangeAssistance = constrain(aiChar.value(), 0, 1);
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
        ledAssistance = constrain(ledAssistanceTypeChar.value(), 0, 2);
        Flash.put("ledAssistance", String(ledAssistance));
        rgbLed.off();
        updateConfigurationChar();
        DEBUG_PRINTLN("ledAssistance written: " + String(ledAssistanceTypeChar.value()));
    }
    if (refTipValueChar.written())
    {
        tipSensor.setRefValue(constrain(refTipValueChar.value(), 0, 5000));
        Flash.put("refTipValue", String(tipSensor.getRefValue()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refTipValue written: " + String(refTipValueChar.value()));
    }
    if (refTipRangeChar.written())
    {
        tipSensor.setRefRange(constrain(refTipRangeChar.value(), 0, 5000));
        Flash.put("refTipRange", String(tipSensor.getRefRange()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refTipRange written: " + String(refTipRangeChar.value()));
    }
    if (refFingerValueChar.written())
    {
        fingerSensor.setRefValue(constrain(refFingerValueChar.value(), 0, 5000));
        Flash.put("refFingerValue", String(fingerSensor.getRefValue()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refFingerValue written: " + String(refFingerValueChar.value()));
    }
    if (refFingerRangeChar.written())
    {
        fingerSensor.setRefRange(constrain(refFingerRangeChar.value(), 0, 5000));
        Flash.put("refFingerRange", String(fingerSensor.getRefRange()));
        updateConfigurationChar();
        DEBUG_PRINTLN("refFingerRange written: " + String(refFingerRangeChar.value()));
    }
    if (tipPressureReleaseDelayChar.written())
    {
        tipPressureReleaseDelay = constrain(tipPressureReleaseDelayChar.value(), 0, 500);
        Flash.put("tipPressureReleaseDelay", String(tipPressureReleaseDelay));
        updateConfigurationChar();
        DEBUG_PRINTLN("tipPressureReleaseDelay written: " + String(tipPressureReleaseDelayChar.value()));
    }
    if (ledTurnOnSpeedChar.written())
    {
        rgbLed.setLedTurnOnSpeed(constrain(ledTurnOnSpeedChar.value(), 0, 500));
        Flash.put("ledTurnOnSpeed", String(rgbLed.getLedTurnOnSpeed()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledTurnOnSpeed written: " + String(ledTurnOnSpeedChar.value()));
    }
    if (ledTipAssistanceColorChar.written())
    {
        rgbLed.setLedTipAssistanceColor(constrain(ledTipAssistanceColorChar.value(), 0, 359));
        Flash.put("ledTipAssistanceColor", String(rgbLed.getLedTipAssistanceColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledTipAssistanceColor written: " + String(ledTipAssistanceColorChar.value()));
    }
    if (ledFingerAssistanceColorChar.written())
    {
        rgbLed.setLedFingerAssistanceColor(constrain(ledFingerAssistanceColorChar.value(), 0, 359));
        Flash.put("ledFingerAssistanceColor", String(rgbLed.getLedFingerAssistanceColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledFingerAssistanceColor written: " + String(ledFingerAssistanceColorChar.value()));
    }
    if (ledSimpleAssistanceColorChar.written())
    {
        rgbLed.setLedSimpleAssistanceColor(constrain(ledSimpleAssistanceColorChar.value(), 0, 359));
        Flash.put("ledSimpleAssistanceColor", String(rgbLed.getLedSimpleAssistanceColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledSimpleAssistanceColor written: " + String(ledSimpleAssistanceColorChar.value()));
    }
    if (ledOkColorChar.written())
    {
        rgbLed.setLedOkColor(constrain(ledOkColorChar.value(), 0, 359));
        Flash.put("ledOkColor", String(rgbLed.getLedOkColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledOkColor written: " + String(ledOkColorChar.value()));
    }
    if (ledNokColorChar.written())
    {
        rgbLed.setLedNokColor(constrain(ledNokColorChar.value(), 0, 359));
        Flash.put("ledNokColor", String(rgbLed.getLedNokColor()));
        updateConfigurationChar();
        DEBUG_PRINTLN("ledNokColor written: " + String(ledNokColorChar.value()));
    }
    if (minutesPassedInUseChar.written())
    {
        //Stats.setMinutesInUse(minutesPassedInUseChar.value());
        Stats.setMinutesInUse(0);
        Flash.put("minutesInUse", String(Stats.getMinutesInUse()));
        DEBUG_PRINTLN("minutesInUse reset");
    }
    if (minutesPassedInRangeChar.written())
    {
        //Stats.setMinutesInRange(minutesPassedInRangeChar.value());
        Stats.setMinutesInRange(0);
        Flash.put("minutesInRange", String(Stats.getMinutesInRange()));
        DEBUG_PRINTLN("minutesInRange reset");
    }
    if (angleCorrectionChar.written())
    {
        angleCorrection = constrain(angleCorrectionChar.value(), 0, 1);
        Flash.put("angleCorrection", String(angleCorrectionChar.value()));
        updateConfigurationChar();
        DEBUG_PRINTLN("angleCorrection: " + String(angleCorrectionChar.value()));
    }
    if (testChar.written())
    {
        testChar.readValue(test.bytes, sizeof test.bytes);
        for ( int i = 0; i < 11; i++ )
        {
          Serial.print( "Sensor (" );
          Serial.print( i );
          Serial.print( "): " );
          Serial.println(test.values[i] );
        }
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

    configurationState.values[0] = tipSensor.getRefValue() + tipSensor.getRefRange();
    configurationState.values[1] = tipSensor.getRefValue() - tipSensor.getRefRange();
    configurationState.values[2] = fingerSensor.getRefValue() + fingerSensor.getRefRange();
    configurationState.values[3] = fingerSensor.getRefValue() - fingerSensor.getRefRange();
    configurationState.values[4] = positiveFeedback;
    configurationState.values[5] = ledAssistance;
    configurationState.values[6] = aiRangeAssistance;
    configurationState.values[7] = angleCorrection;
    configurationState.values[8] = tipPressureReleaseDelay;
    configurationState.values[9] = rgbLed.getLedTurnOnSpeed();
    configurationState.values[10] = rgbLed.getLedSimpleAssistanceColor();
    configurationState.values[11] = rgbLed.getLedTipAssistanceColor();
    configurationState.values[12] = rgbLed.getLedFingerAssistanceColor();
    configurationState.values[13] = rgbLed.getLedOkColor();
    configurationState.values[14] = rgbLed.getLedNokColor();
    configurationStateChar.writeValue(configurationState.bytes, sizeof configurationState.bytes);

    test.values[0] = tipSensor.getRefValue() + tipSensor.getRefRange();
    test.values[1] = tipSensor.getRefValue() - tipSensor.getRefRange();
    test.values[2] = fingerSensor.getRefValue() + fingerSensor.getRefRange();
    test.values[3] = fingerSensor.getRefValue() - fingerSensor.getRefRange();
    test.values[4] = positiveFeedback;
    test.values[5] = ledAssistance;
    test.values[6] = aiRangeAssistance;
    test.values[7] = angleCorrection;
    test.values[8] = tipPressureReleaseDelay;
    test.values[9] = rgbLed.getLedTurnOnSpeed();
    test.values[10] = rgbLed.getLedSimpleAssistanceColor();
    test.values[11] = rgbLed.getLedTipAssistanceColor();
    test.values[12] = rgbLed.getLedFingerAssistanceColor();
    test.values[13] = rgbLed.getLedOkColor();
    test.values[14] = rgbLed.getLedNokColor();
    testChar.writeValue(test.bytes, sizeof test.bytes);
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
        if (tipSensor.getValue() < 15)
            secondsToSleepCounter++;
        else
            secondsToSleepCounter = 0;
        previousSleepMillis += 1000;
    }
    if (secondsToSleepCounter > SLEEP_TIMER || batteryLevel.getValue() < 0)
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
        //nrf_gpio_cfg_sense_input(P1_11, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
        nrf_gpio_cfg_sense_input(P0_19, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);  // sensogrip board D2
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

void disableUART()
{
    NRF_UARTE0->TASKS_STOPTX = 1;
    NRF_UARTE0->TASKS_STOPRX = 1;
    NRF_UARTE0->ENABLE = 0;
    NRF_UART0->TASKS_STOPTX = 1;
    NRF_UART0->TASKS_STOPRX = 1;
    NRF_UART0->ENABLE = 0;
}