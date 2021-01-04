/*
  RGBLed.h - Library for easy use of RGB leds.
  Created by Primoz Flander 06.11.2020.
  Released into the public domain.
*/

#ifndef RGBLed_H
#define RGBLed_H
#include <Arduino.h>
class RGBLed
{
private:
    bool inverse = false;
    int redLedPin;
    int greenLedPin;
    int blueLedPin;
    int brightnessRed = 0;
    int brightnessGreen = 0;
    int brightnessBlue = 0;
    int ledTurnOnSpeed = 8;
    int ledTipAssistanceColor = 180;
    int ledFingerAssistanceColor = 300;
    int ledSimpleAssistanceColor = 240;
    int ledOkColor = 120;
    int ledNokColor = 0;

public:
    RGBLed(int redLedPin = 9, int greenLedPin = 10, int blueLedpin = 11, bool inverse = false);
    void init();
    void on();
    void off();
    void setLedColorHsv(double h, double s = 1, double v = 1);
    void setLedColorHsvWithTransition(double h, double s = 1, double v = 1);
    void setOutput(int r, int g, int b);
    void increaseRedBrightness(int value = 1);
    void decreaseRedBrightness(int value = 1);
    void increaseGreenBrightness(int value = 1);
    void decreaseGreenBrightness(int value = 1);
    void increaseBlueBrightness(int value = 1);
    void decreaseBlueBrightness(int value = 1);
    void setRedBrightness(int value);
    void setGreenBrightness(int value);
    void setBlueBrightness(int value);
    void setLedTurnOnSpeed(int value);
    int getLedTurnOnSpeed();
    void setLedTipAssistanceColor(int value);
    int getLedTipAssistanceColor();
    void setLedFingerAssistanceColor(int value);
    int getLedFingerAssistanceColor();
    void setLedSimpleAssistanceColor(int value);
    int getLedSimpleAssistanceColor();
    void setLedOkColor(int value);
    int getLedOkColor();
    void setLedNokColor(int value);
    int getLedNokColor();
    void displayOkColor();
    void displayNokColor();
    void displaySimpleAssistanceColor();
    void displayTipAssistanceColor();
    void displayFingerAssistanceColor();
    void displayNoColor();
    void ledTest();
    void blinkOk();
    void blinkNok();
};
#endif
