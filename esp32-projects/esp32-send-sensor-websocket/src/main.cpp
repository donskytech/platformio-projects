#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <WebSocketsClient.h>

int sensorVal;
const int ANALOG_READ_PIN = 36; // or A0
const int RESOLUTION = 12;      // Could be 9-12

// Enter your User ID and Password for your wifi connection
const char *SSID = "<YOUR_WIFI_SSID>";
const char *PASSWORD = "<YOUR_WIFI_PASSWORD>";

WebSocketsClient webSocket;

void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // server address, port and URL - ws://192.168.100.121:1880/ws/readings
  webSocket.begin("192.168.100.121", 1880, "/ws/readings");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  delay(1000);
}
unsigned long previousMillis = 0;
const int interval = 1000;

void loop()
{
  analogReadResolution(RESOLUTION);
  // Read and print the sensor pin value
  sensorVal = analogRead(ANALOG_READ_PIN);
  // Serial.print(sensorVal);
  // Serial.print(" \n");

  webSocket.loop();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time we send the last reading
    previousMillis = currentMillis;

    char buffer[5];
    sprintf(buffer, "%d", sensorVal);
    // send sensor readings thru WebSocket
    webSocket.sendTXT(buffer);
  }
}