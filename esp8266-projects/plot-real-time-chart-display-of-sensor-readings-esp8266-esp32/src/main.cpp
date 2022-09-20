
/*
  Title:  Plot Real-time Chart display of Sensor Readings – ESP8266/ESP32
  Description:  Displays sensor readings in real time
  Author: donsky
  For:    www.donskytech.com
  Date:   September 20, 2022
*/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <LittleFS.h>
#endif
#include <ESPAsyncWebServer.h>


/*
  Replace the SSID and Password according to your wifi
*/
const char *ssid = "<REPLACE_WITH_YOUR_WIFI_SSID>";
const char *password = "<REPLACE_WITH_YOUR_WIFI_PASSWORD>";


// Webserver and Websockets setup
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

// LDR Pin
static int sensorVal = 0;
const int ANALOG_READ_PIN = A0; // or A0

// Function to handle request that is not serve by our web server
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// Callback function for our websocket message
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    // client connected
    Serial.println("Client connected....");
    os_printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    // client disconnected
    os_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    // error was received from the other end
    os_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    // pong message was received (in response to a ping request maybe)
    os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    // do nothing as client is not sending message to server
    os_printf("ws[%s][%u] data received\n", server->url(), client->id());
  }
}

// function to read LDR value
void readLDRValue()
{

  // Read 
  int tempSensorVal = analogRead(ANALOG_READ_PIN);

  // Check if value read is different then send a websocket message to the client
  if (tempSensorVal != sensorVal)
  {
    Serial.println(tempSensorVal);
    sensorVal = tempSensorVal;

    // send message to clients when Connected
    webSocket.printfAll(std::to_string(sensorVal).c_str());

    // adding a little delay
    delay(10);
  }
}

void setup()
{

  Serial.begin(115200);
  Serial.println("Starting the Real-time Chart display of Sensor Readings ..");

  // Begin LittleFS for ESP8266 or SPIFFS for ESP32
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // attach AsyncWebSocket
  webSocket.onEvent(onEvent);
  server.addHandler(&webSocket);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  // Route for root index.css
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.css", "text/css"); });

  // Route for root entireframework.min.css
  server.on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/entireframework.min.css", "text/css"); });

  // Route for root index.js
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", "text/javascript"); });

  server.onNotFound(notFound);

  // Start the server
  server.begin();
}

void loop()
{
  // Read the LDR values continously
  readLDRValue();
}