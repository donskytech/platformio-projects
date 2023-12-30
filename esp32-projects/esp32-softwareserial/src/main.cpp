#include <Arduino.h>
#include <SoftwareSerial.h>

// Software Serial object to interface with my SIM900
SoftwareSerial sim900(5, 18);

void updateSerial();

void setup()
{
  // Set Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  // Begin serial communication with Arduino and SIM900
  sim900.begin(9600);

  Serial.println("Initialize SIM900");

  // SIM900 Handshake AT commands
  sim900.println("AT");
  updateSerial();
  // Signal Quality Test
  sim900.println("AT+CSQ");
  updateSerial();
  // Check SIM
  sim900.println("AT+CCID");
  updateSerial();
  // Check network registration
  sim900.println("AT+CREG?");
  updateSerial();
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    sim900.write(Serial.read()); // From Serial Port to Software Serial
  }
  while (sim900.available())
  {
    Serial.write(sim900.read()); // From Software Serial to Serial Port
  }
}