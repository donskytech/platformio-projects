#include "QRCode_Displayer.h"

QRCodeDisplayer::QRCodeDisplayer(int w, int h, Adafruit_SSD1306 oledDisplay) : width(w), height(h), display(oledDisplay)
{
}
void QRCodeDisplayer::displayText(const char *text, const char *text2, uint8_t textSize)
{
    display.clearDisplay();
    display.setTextSize(textSize);       // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.cp437(true);                 // Use full 256 char 'Code Page 437' font

    display.setCursor(0, 0);
    display.write(text);
    if (strcmp(text2, "") != 0)
    {
        display.setCursor(0, 32);
        display.write(text2);
    }

    display.display();
}
void QRCodeDisplayer::displayText(const char *text, uint8_t amount, uint8_t textSize)
{
    display.clearDisplay();
    display.setTextSize(textSize);       // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
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

void QRCodeDisplayer::begin()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
    this->clearOLEDDisplay();
}

void QRCodeDisplayer::clearOLEDDisplay()
{
    display.clearDisplay();
    display.display();
}
