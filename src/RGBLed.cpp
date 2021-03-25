#include "RGBLed.h"
RGBLed::RGBLed(int redLedPin, int greenLedPin, int blueLedpin, bool inverse)
{
    this->redLedPin = redLedPin;
    this->greenLedPin = greenLedPin;
    this->blueLedPin = blueLedpin;
    this->inverse = inverse;
    init();
}

void RGBLed::init()
{
    pinMode(redLedPin, OUTPUT);
    pinMode(greenLedPin, OUTPUT);
    pinMode(blueLedPin, OUTPUT);
}

void RGBLed::on()
{
    if (!inverse)
    {
        setRedBrightness(255);
        setGreenBrightness(255);
        setBlueBrightness(255);
    }
    else
    {
        setRedBrightness(0);
        setGreenBrightness(0);
        setBlueBrightness(0);
    }
}

void RGBLed::off()
{
    if (!inverse)
    {
        setRedBrightness(0);
        setGreenBrightness(0);
        setBlueBrightness(0);
    }
    else
    {
        setRedBrightness(255);
        setGreenBrightness(255);
        setBlueBrightness(255);
    }
}

void RGBLed::setLedColorHsv(double h, double s, double v)
{
    double r = 0;
    double g = 0;
    double b = 0;
    int i = (int)floor(h / 60.0);
    double f = h / 60.0 - i;
    double pv = v * (1 - s);
    double qv = v * (1 - s * f);
    double tv = v * (1 - s * (1 - f));
    switch (i)
    {
    case 0:
        r = v;
        g = tv;
        b = pv;
        break;
    case 1:
        r = qv;
        g = v;
        b = pv;
        break;
    case 2:
        r = pv;
        g = v;
        b = tv;
        break;
    case 3:
        r = pv;
        g = qv;
        b = v;
        break;
    case 4:
        r = tv;
        g = pv;
        b = v;
        break;
    case 5:
        r = v;
        g = pv;
        b = qv;
        break;
    }
    int red = constrain((int)255 * r, 0, 255);
    int green = constrain((int)255 * g, 0, 255);
    int blue = constrain((int)255 * b, 0, 255);
    setOutput(red, green, blue);
}

void RGBLed::setLedColorHsvWithTransition(double h, double s, double v)
{
    double r = 0, g = 0, b = 0;
    int i = (int)floor(h / 60.0);
    double f = h / 60.0 - i;
    double pv = v * (1 - s);
    double qv = v * (1 - s * f);
    double tv = v * (1 - s * (1 - f));
    switch (i)
    {
    case 0:
        r = v;
        g = tv;
        b = pv;
        break;
    case 1:
        r = qv;
        g = v;
        b = pv;
        break;
    case 2:
        r = pv;
        g = v;
        b = tv;
        break;
    case 3:
        r = pv;
        g = qv;
        b = v;
        break;
    case 4:
        r = tv;
        g = pv;
        b = v;
        break;
    case 5:
        r = v;
        g = pv;
        b = qv;
        break;
    }
    int red = constrain((int)255 * r, 0, 255);
    int green = constrain((int)255 * g, 0, 255);
    int blue = constrain((int)255 * b, 0, 255);
    int rOut = brightnessRed, gOut = brightnessGreen, bOut = brightnessBlue;
    if (brightnessRed != red || brightnessGreen != green || brightnessBlue != blue)
    {
        if (brightnessRed < red)
            rOut = min(brightnessRed + ledTurnOnSpeed, 255);
        if (brightnessRed > red)
            rOut = max(brightnessRed - ledTurnOnSpeed, 0);
        if (brightnessGreen < green)
            gOut = min(brightnessGreen + ledTurnOnSpeed, 255);
        if (brightnessGreen > green)
            gOut = max(brightnessGreen - ledTurnOnSpeed, 0);
        if (brightnessBlue < blue)
            bOut = min(brightnessBlue + ledTurnOnSpeed, 255);
        if (brightnessBlue > blue)
            bOut = max(brightnessBlue - ledTurnOnSpeed, 0);
        setOutput(rOut, gOut, bOut);
    }
}

void RGBLed::setOutput(int r, int g, int b)
{
    setRedBrightness(r);
    setGreenBrightness(g);
    setBlueBrightness(b);
}

void RGBLed::increaseRedBrightness(int value)
{
    if (brightnessRed < 255)
        brightnessRed += value;
    setRedBrightness(brightnessRed);
}

void RGBLed::decreaseRedBrightness(int value)
{
    if (brightnessRed > 0)
        brightnessRed -= value;
    setRedBrightness(brightnessRed);
}

void RGBLed::increaseGreenBrightness(int value)
{
    if (brightnessGreen < 255)
        brightnessGreen += value;
    setGreenBrightness(brightnessGreen);
}

void RGBLed::decreaseGreenBrightness(int value)
{
    if (brightnessGreen > 0)
        brightnessGreen -= value;
    setGreenBrightness(brightnessGreen);
}

void RGBLed::increaseBlueBrightness(int value)
{
    if (brightnessBlue < 255)
        brightnessBlue += value;
    setBlueBrightness(brightnessBlue);
}

void RGBLed::decreaseBlueBrightness(int value)
{
    if (brightnessBlue > 0)
        brightnessBlue -= value;
    setBlueBrightness(brightnessBlue);
}

void RGBLed::setRedBrightness(int value)
{
    brightnessRed = constrain(value, 0, 255);
    if (!inverse)
        analogWrite(redLedPin, brightnessRed);
    else
        analogWrite(redLedPin, (255 - brightnessRed));
}

void RGBLed::setGreenBrightness(int value)
{
    brightnessGreen = constrain(value, 0, 255);
    if (!inverse)
        analogWrite(greenLedPin, brightnessGreen);
    else
        analogWrite(greenLedPin, (255 - brightnessGreen));
}

void RGBLed::setBlueBrightness(int value)
{
    brightnessBlue = constrain(value, 0, 255);
    if (!inverse)
        analogWrite(blueLedPin, brightnessBlue);
    else
        analogWrite(blueLedPin, (255 - brightnessBlue));
}

void RGBLed::setLedTurnOnSpeed(int value)
{
    ledTurnOnSpeed = value;
}

int RGBLed::getLedTurnOnSpeed()
{
    return ledTurnOnSpeed;
}

void RGBLed::setLedTipAssistanceColor(int value)
{
    ledTipAssistanceColor = constrain(value, 0, 359);
}

int RGBLed::getLedTipAssistanceColor()
{
    return ledTipAssistanceColor;
}

void RGBLed::setLedFingerAssistanceColor(int value)
{
    ledFingerAssistanceColor = constrain(value, 0, 359);
}

int RGBLed::getLedFingerAssistanceColor()
{
    return ledFingerAssistanceColor;
}

void RGBLed::setLedSimpleAssistanceColor(int value)
{
    ledSimpleAssistanceColor = constrain(value, 0, 359);
}

int RGBLed::getLedSimpleAssistanceColor()
{
    return ledSimpleAssistanceColor;
}

void RGBLed::setLedOkColor(int value)
{
    ledOkColor = constrain(value, 0, 359);
}

int RGBLed::getLedOkColor()
{
    return ledOkColor;
}

void RGBLed::setLedNokColor(int value)
{
    ledNokColor = constrain(value, 0, 359);
}

int RGBLed::getLedNokColor()
{
    return ledNokColor;
}

void RGBLed::displayOkColor()
{
    setLedColorHsvWithTransition(ledOkColor);
}

void RGBLed::displayNokColor()
{
    setLedColorHsvWithTransition(ledNokColor);
}

void RGBLed::displaySimpleAssistanceColor()
{
    setLedColorHsvWithTransition(ledSimpleAssistanceColor);
}

void RGBLed::displayTipAssistanceColor()
{
    setLedColorHsvWithTransition(ledTipAssistanceColor);
}

void RGBLed::displayFingerAssistanceColor()
{
    setLedColorHsvWithTransition(ledFingerAssistanceColor);
}

void RGBLed::displayNoColor()
{
    setLedColorHsvWithTransition(0, 0, 0);
}

void RGBLed::ledTest()
{
    on();
    delay(500);
    off();
}

void RGBLed::blinkOk()
{
    setGreenBrightness(255);
    delay(100);
    setGreenBrightness(0);
    delay(100);
}

void RGBLed::blinkNok()
{
    setRedBrightness(255);
    delay(100);
    setRedBrightness(0);
    delay(100);
}
