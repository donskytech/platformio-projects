#include "OLED_Displayer.h"

OLEDDisplayer::OLEDDisplayer(int w, int h, Adafruit_SSD1306 oledDisplay) : width(w), height(h), display(oledDisplay)
{
}
void OLEDDisplayer::displayText(const char *text, const char *text2, uint8_t textSize)
{
    display.clearDisplay();
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);
    display.setCursor(0, 0);
    display.write(text);
    if (strcmp(text2, "") != 0)
    {
        display.setCursor(0, 32);
        display.write(text2);
    }

    display.display();
}
void OLEDDisplayer::displayText(const char *text, uint8_t amount, uint8_t textSize)
{
    display.clearDisplay();
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);

    // Start at top-left corner
    display.setCursor(0, 0);
    display.write(text);

    char buffer[10];
    sprintf(buffer, "%d", amount);
    display.setCursor(0, 32);
    display.write(buffer);

    display.display();
}

void OLEDDisplayer::begin()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
    this->clearOLEDDisplay();
}

void OLEDDisplayer::clearOLEDDisplay()
{
    display.clearDisplay();
    display.display();
}
