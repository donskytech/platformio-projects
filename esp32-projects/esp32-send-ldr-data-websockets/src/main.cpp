/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

static int sensorVal = 0;
const int ANALOG_READ_PIN = 36; // or A0
const int RESOLUTION = 12;      // Could be 9-12

// CHANGE THIS TO ADD YOUR WIFI USERNAME/PASSWORD
const char * WIFI_SSID = "donsky";
const char * WIFI_PASS = "donsky123";

//Initialize the JSON data we send to our websocket server
const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> doc;

#define USE_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  case WStype_PING:
  case WStype_PONG:
    break;
  }
}

void setup()
{
  //Set the baud rate
  USE_SERIAL.begin(115200);
  USE_SERIAL.printf("Begin websocket client program....");

  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

  // WiFi.disconnect();
  USE_SERIAL.printf("Connecting");
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    USE_SERIAL.printf(".");
    delay(100);
  }
  USE_SERIAL.printf("Connected!");

  // server address, port and URL
  webSocket.begin("192.168.100.22", 8080, "/sendSensorData");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  // webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  // // Set the resolution values
  // analogReadResolution(RESOLUTION);
}

void readLDRValue()
{

  // Read and print the sensor pin value
  int tempSensorVal = analogRead(ANALOG_READ_PIN);

  USE_SERIAL.println(tempSensorVal);

  // Check if value read is different then send a websocket message to the server
  if (tempSensorVal != sensorVal)
  {
    sensorVal = tempSensorVal;

    //send JSON message in this format {"value": 100}
    doc["value"] = tempSensorVal;

    // Declare a buffer to hold the result
    char output[50];

    serializeJson(doc, output);

    // send message to server when Connected
    webSocket.sendTXT(output);
  }

  // sleep for some time before next read
  delay(100);
}

// This will go into loop
void loop()
{
  readLDRValue();
  webSocket.loop();
}