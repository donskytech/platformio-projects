#include <Arduino.h>
#include <EEPROM.h>
#include <elapsedMillis.h>
#include "OLED_Displayer.h"

const int OLED_WIDTH = 128;
const int OLED_HEIGHT = 64;
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
OLEDDisplayer oledDisplayer(OLED_WIDTH, OLED_HEIGHT, display);

// Sum of all the  coins inseted
int totalAmount = 0;
volatile bool updateDisplay = false;
volatile bool coinInserted = false;

elapsedMillis timer;
elapsedMillis countDownTimer;
long interval_timer = 10000;
bool countDownStarted = false;

const uint8_t PIN = 19;

const uint8_t RELAY_PIN = 13;

void turnOnRelay();
void turnOffRelay();

void IRAM_ATTR isr()
{
  coinInserted = true;
  updateDisplay = true;
}

void setup()
{
  Serial.begin(9600);
  attachInterrupt(PIN, isr, FALLING);

  oledDisplayer.begin();
  pinMode(RELAY_PIN, OUTPUT);
  turnOffRelay();

  Serial.println("Begin coin counter...");
}

void loop()
{
  if (coinInserted)
  {
    coinInserted = false;
    totalAmount = totalAmount + 1;
    oledDisplayer.displayText("Total :", totalAmount, 2);
    if (totalAmount > 0 && !countDownStarted)
    {
      countDownStarted = true;
      timer = 0;
      countDownTimer = timer;
      turnOnRelay();
    }
  }
  if (totalAmount > 0 && countDownStarted)
  {
    if (timer >= interval_timer * totalAmount)
    {
      Serial.println("Time is up!");
      turnOffRelay();
      countDownStarted = false;
      totalAmount = 0;
      oledDisplayer.displayText("Time Left", "0", 2);
    }
  }

  if (countDownTimer >= 1000)
  {
    if (countDownStarted)
    {
      countDownTimer = 0;
      long timeLeft = ((interval_timer * totalAmount) - timer) / 1000;
      Serial.print("timeLeft :");
      Serial.println(timeLeft);
      oledDisplayer.displayText("Time Left", timeLeft, 2);
    }
  }
}

void turnOnRelay()
{
  digitalWrite(RELAY_PIN, HIGH);
}
void turnOffRelay()
{
  digitalWrite(RELAY_PIN, LOW);
}