
/*
  Title:  ESP8266 Conveyor Counter Using Webserver
  Author: donsky
  For:    www.donskytech.com
  Date:   September 10, 2022
*/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

/*
  Replace the SSID and Password according to your wifi
*/
const char *ssid = "<YOUR_WIFI_SSID>";
const char *password = "<YOUR_PASSWORD>";

// Webserver and Websockets setup
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// IR Sensor Pin
int IRSensor = D2;
int LED = LED_BUILTIN;

String localIPAddress = "";

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

// Template Processor
String indexPageProcessor(const String &var)
{
  return localIPAddress;
}

void setup()
{

  Serial.begin(115200);

  // Initialize Filesystem LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("Cannot load LittleFS Filesystem!");
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
  localIPAddress = WiFi.localIP().toString();
  Serial.print("IP Address: ");
  Serial.println(localIPAddress);

  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", String(), false, indexPageProcessor); });

  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.css", "text/css"); });

  server.on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/entireframework.min.css", "text/css"); });

  server.onNotFound(notFound);

  server.begin();

  pinMode(IRSensor, INPUT); // IR Sensor pin INPUT
  pinMode(LED, OUTPUT);     // LED Pin Output
}

static int counter = 0;
static bool isTriggered = false;
static long lastTriggered = 0;
const long WAIT_FOR_NEXT_TRIGGER = 1000;

void loop()
{

  int sensorStatus = digitalRead(IRSensor);
  if (sensorStatus == 1)
  {
    digitalWrite(LED, HIGH);
    isTriggered = false;
  }
  else
  {
    if (!isTriggered)
    {
      long timeElapsed = millis() - lastTriggered;
      Serial.printf("timeElapsed :: %u\n", timeElapsed);
      if (timeElapsed < WAIT_FOR_NEXT_TRIGGER)
      {
        // To avoid multiple consecutive signal coming from the IR sensor
        return;
      }

      isTriggered = true;
      counter++;
      digitalWrite(LED, LOW);
      Serial.printf("counter :: %u\n", counter);
      ws.printfAll("%u", counter);
      lastTriggered = millis();
    }
  }
  // cleanup websocket clients
  ws.cleanupClients();
}