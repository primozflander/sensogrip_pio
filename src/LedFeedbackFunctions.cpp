#include "LedFeedbackFunctions.h"

void showLedFeedback()
{
    enum feedback
    {
        noFeedback,
        allSensorsInRange,
        anySensorInRange,
        anySensorInRangeTwoColors,
        allSensorsInRangeWithOverpressureColor,
        negativeFeedback,
    };

    static unsigned long previousLedFeedbackMillis = 0;
    if (millis() - previousLedFeedbackMillis >= LED_FEEDBACK_INTERVAL)
    {
        if (isAiRangeAssisted)
            calculateOptimalRange();
        switch (ledFeedbackType)
        {
        case noFeedback:
            rgbLed.displayNoColor();
            break;
        case allSensorsInRange:
            showLedAllSensorsInRange();
            break;
        case anySensorInRange:
            showLedAnySensorInRange();
            break;
        case anySensorInRangeTwoColors:
            showLedAnySensorInRangeTwoColors();
            break;
        case allSensorsInRangeWithOverpressureColor:
            showLedAllSensorsInRangeWithOverpressureColor();
            break;
        case negativeFeedback:
            showLedNegativeFeedback();
            break;
        default:
            rgbLed.off();
            break;
        }
        previousLedFeedbackMillis += LED_FEEDBACK_INTERVAL;
    }
}

void showLedAllSensorsInRange()
{
    static int ledFeedbackCounter = 0;
    if (tipSensor.isInRange() && fingerSensor.isInRange())
    {
        ledFeedbackCounter += 5;
        ledFeedbackCounter = min(ledFeedbackCounter, tipPressureReleaseDelay);
        if (isPositiveFeedback)
            rgbLed.displayOkColor();
        else
            rgbLed.displayNoColor();
    }
    else if (tipSensor.isUnderRange() && ledFeedbackCounter > 0)
    {
        ledFeedbackCounter -= 1;
    }
    else
    {
        if (isPositiveFeedback)
            rgbLed.displayNoColor();
        else
            rgbLed.displayNokColor();
        ledFeedbackCounter = 0;
    }
}

void showLedAnySensorInRange()
{
    static int ledFeedbackCounter = 0;
    if (tipSensor.isInRange() && fingerSensor.isInRange())
    {
        ledFeedbackCounter += 5;
        ledFeedbackCounter = min(ledFeedbackCounter, tipPressureReleaseDelay);
        if (isPositiveFeedback)
            rgbLed.displayOkColor();
        else
            rgbLed.displayNoColor();
    }
    else if (tipSensor.isUnderRange() && ledFeedbackCounter > 0)
    {
        ledFeedbackCounter -= 1;
    }
    else if ((!tipSensor.isInRange() != !fingerSensor.isInRange()) && ledFeedbackCounter <= 0)
    {
        rgbLed.displaySimpleAssistanceColor();
    }
    else
    {
        if (isPositiveFeedback)
            rgbLed.displayNoColor();
        else
            rgbLed.displayNokColor();
        ledFeedbackCounter = 0;
    }
}

void showLedAnySensorInRangeTwoColors()
{
    static int ledFeedbackCounter = 0;
    if (tipSensor.isInRange() && fingerSensor.isInRange())
    {
        ledFeedbackCounter += 5;
        ledFeedbackCounter = min(ledFeedbackCounter, tipPressureReleaseDelay);
        if (isPositiveFeedback)
            rgbLed.displayOkColor();
        else
            rgbLed.displayNoColor();
    }
    else if (tipSensor.isUnderRange() && ledFeedbackCounter > 0)
    {
        ledFeedbackCounter -= 1;
    }
    else if ((tipSensor.isInRange() && !fingerSensor.isInRange()) && ledFeedbackCounter <= 0)
    {
        rgbLed.displayTipAssistanceColor();
    }
    else if ((!tipSensor.isInRange() && fingerSensor.isInRange()) && ledFeedbackCounter <= 0)
    {
        rgbLed.displayFingerAssistanceColor();
    }
    else
    {
        if (isPositiveFeedback)
            rgbLed.displayNoColor();
        else
            rgbLed.displayNokColor();
        ledFeedbackCounter = 0;
    }
}

void showLedAllSensorsInRangeWithOverpressureColor()
{

    static int ledFeedbackCounter = 0;
    if (tipSensor.isInRange() && fingerSensor.isInRange())
    {
        ledFeedbackCounter += 5;
        ledFeedbackCounter = min(ledFeedbackCounter, tipPressureReleaseDelay);
        rgbLed.displayOkColor();
    }
    else if (tipSensor.isUnderRange() && ledFeedbackCounter > 0)
    {
        ledFeedbackCounter -= 1;
    }
    else if (tipSensor.isOverRange() || fingerSensor.isOverRange())
    {
        rgbLed.displayNokColor();
        ledFeedbackCounter = 0;
    }
    else
    {
        rgbLed.displayNoColor();
        ledFeedbackCounter = 0;
    }
}

void showLedNegativeFeedback()
{
    static int ledFeedbackCounter = 0;
    if (tipSensor.isInRange() && fingerSensor.isInRange())
    {
        ledFeedbackCounter += 5;
        ledFeedbackCounter = min(ledFeedbackCounter, tipPressureReleaseDelay);
        rgbLed.displayNoColor();
    }
    else if (tipSensor.isUnderRange() && ledFeedbackCounter > 0)
    {
        ledFeedbackCounter -= 1;
    }
    else
    {
        rgbLed.displayNokColor();
        ledFeedbackCounter = 0;
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