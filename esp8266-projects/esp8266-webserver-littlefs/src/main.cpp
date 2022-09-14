
/*
  Title:  ESP8266 Webserver - No Filesystem
  Description:  An ESP8266 webserver that uses LittleFS to load web content
  Author: donsky
  For:    www.donskytech.com
  Date:   September 14, 2022
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
const char *ssid = "<REPLACE_WITH_YOUR_WIFI_SSID>";
const char *password = "<REPLACE_WITH_YOUR_WIFI_PASSWORD>";


// Webserver
AsyncWebServer server(80);

String PARAM_MESSAGE = "status";

const int LED_PIN = LED_BUILTIN;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void toggleLED(String status)
{
  if (status == "ON")
    digitalWrite(LED_PIN, LOW);
  else
    digitalWrite(LED_PIN, HIGH);
}

void setup()
{

  Serial.begin(115200);
  Serial.println("Starting the LittleFS Webserver..");

  // Begin LittleFS
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

  // LED PIN
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

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

  // Respond to toggle event
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        String status;
        if (request->hasParam(PARAM_MESSAGE)) {
            status = request->getParam(PARAM_MESSAGE)->value();
            if(status == "ON"){
              toggleLED("ON");
            }else{
              toggleLED("OFF");
            }
        } else {
            status = "No message sent";
        }
        request->send(200, "text/plain", "Turning Built In LED : " + status); });

  server.onNotFound(notFound);

  server.begin();
}

void loop()
{
}