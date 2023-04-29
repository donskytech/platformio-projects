/*
  Project: IR Sensor Counter
  Author: donsky
  For: www.donskytech.com
  Date: April 30, 2023
*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

const int IR_Sensor = 33;

volatile uint64_t count = 0;
volatile uint64_t currentCount = -1;

unsigned long currentTime = 0;
unsigned long lastReadTime = 0;
unsigned int intervalDelay = 1000;

void IRAM_ATTR isr()
{
  currentTime = millis();
  // IR Sensor is noisy so we add a debounce mechanism here
  if (currentTime - lastReadTime > intervalDelay)
  {
    count++;
    lastReadTime = currentTime;
  }
}

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// Check your I2C LCD Address
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Function to print to LCD
void printToLCD(const String &message, uint8_t column, uint8_t row, bool isClear)
{
  if (isClear)
  {
    lcd.clear();
  }
  // set cursor to  column,  row
  lcd.setCursor(column, row);
  if (message.length() == 0)
  {
    lcd.setCursor(0, 1);
    for (int n = 0; n < 16; n++)
    {
      lcd.print(" ");
    }
  }
  else
  {
    lcd.print(message);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(IR_Sensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_Sensor), isr, FALLING);

  lcd.init();
  lcd.clear();
  lcd.backlight(); // Make sure backlight is on
  printToLCD("Current Count:", 0, 0, false);
  delay(1000);
}
void loop()
{
  Serial.printf("Count :: %lld\r\n", count);
  if (currentCount != count)
  {
    currentCount = count;
    char buffer[50];
    ltoa(count, buffer, 10);
    // Print a message to the LCD.
    printToLCD(buffer, 0, 1, false);
  }
}