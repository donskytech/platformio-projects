#include "Arduino.h"
#include "PCF8575.h"

// Set i2c address
PCF8575 pcf8575(0x20);

void setup()
{
  Serial.begin(9600);

  pcf8575.pinMode(P0, INPUT);
  pcf8575.pinMode(P1, OUTPUT);

  pcf8575.begin();
}

void loop()
{
  uint8_t val = pcf8575.digitalRead(P0);
  if (val == HIGH)
  {
    pcf8575.digitalWrite(P1, HIGH);
  }
  else
  {
    pcf8575.digitalWrite(P1, LOW);
  }

  delay(50);
}
