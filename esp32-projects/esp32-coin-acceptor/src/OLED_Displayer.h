#ifndef OLED_DISPLAYER
#define OLED_DISPLAYER
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDDisplayer
{
private:
    int width;
    int height;
    Adafruit_SSD1306 display;

public:
    OLEDDisplayer(int, int, Adafruit_SSD1306);
    void begin();
    void displayText(const char *text1, const char *text2 = "", uint8_t textSize = 1);
    void displayText(const char *text1, uint8_t amount, uint8_t textSize = 1);
    void clearOLEDDisplay();
};
#endif