#include <Arduino.h>

int sensorVal;
const int ANALOG_READ_PIN = 36; // or A0
const int RESOLUTION = 12; // Could be 9-12
 
void setup()
{
  Serial.begin(115200); 
  analogReadResolution(RESOLUTION);
}
 
void loop()
{
  
  //Read and print the sensor pin value
  sensorVal = analogRead(ANALOG_READ_PIN);
  Serial.print(sensorVal); 
  Serial.print(" \n"); 
  //sleep for some time before next read
  delay(100); 
}