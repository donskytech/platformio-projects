#include <Arduino.h>

int sensorValue;
const int ANALOG_IN_PIN = 36;
 
void setup()
{
  Serial.begin(9600); 
  delay(1000);
}
 
void loop()
{
  sensorValue = analogRead(ANALOG_IN_PIN);
  Serial.print(sensorValue); 
  Serial.print(" \n"); 
  delay(100); 
}