#include "Led.h"
Led::Led(int pin, bool inverse)
{
    this->pin = pin;
    this->inverse = inverse;
    init();
}

void Led::init()
{
    pinMode(pin, OUTPUT);
    off();
}

void Led::on()
{
    if (!inverse)
    {
        digitalWrite(pin, HIGH);
        brightness = 255;
    }
    else
    {
        digitalWrite(pin, LOW);
        brightness = 255;
    }
}

void Led::off()
{
    if (!inverse)
    {
        digitalWrite(pin, LOW);
        brightness = 0;
    }
    else
    {
        digitalWrite(pin, HIGH);
        brightness = 0;
    }
}

void Led::blinkMs(unsigned int blinkInterval)
{
    if (millis() - previousMillis >= blinkInterval)
    {
        digitalWrite(pin, state);
        state = !state;
        previousMillis += blinkInterval;
    }
}

void Led::brightnessUp()
{
    if (brightness < 255)
        brightness++;
    if (!inverse)
        analogWrite(pin, brightness);
    else
        analogWrite(pin, (255 - brightness));
}

void Led::brightnessDown()
{
    if (brightness > 0)
        brightness--;
    if (!inverse)
        analogWrite(pin, brightness);
    else
        analogWrite(pin, (255 - brightness));
}

void Led::setBrightness(int value)
{
    brightness = value;
    if (!inverse)
        analogWrite(pin, brightness);
    else
        analogWrite(pin, (255 - brightness));
}

int Led::getBrightness()
{
    return brightness;
}
