#pragma once
#include <Arduino.h>
class Led
{
private:
    bool inverse = false;
    int pin;
    int brightness = 0;
    bool state = 0;
    unsigned long previousMillis = 0;

public:
    Led(int pin = 13, bool inverse = false);
    void init();
    void on();
    void off();
    void blinkMs(unsigned int blinkInterval);
    void brightnessUp();
    void brightnessDown();
    void setBrightness(int value);
    int getBrightness();
};
