#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "SPIFFS.h"
#include "automation.h"

// Enter your User ID and Password for your wifi connection
const char *ssid = "donsky";
const char *password = "donsky";

AsyncWebServer server(80);

// Import our Automation Proto Class
AutomationProto proto;

// Processor for index page template
String indexPageProcessor(const String &var)
{
    String status = "";
    if (var == "TUBE_TEMPLATE_STATUS")
    {
        if (proto.isTubeOn())
        {
            status = "checked";
        }
    }
    else if (var == "BULB_TEMPLATE_STATUS")
    {
        if (proto.isBulbOn())
        {
            status = "checked";
        }
    }
    else if (var == "SOCKET_TEMPLATE_STATUS")
    {
        if (proto.isSocketOn())
        {
            status = "checked";
        }
    }
    else if (var == "DIMMER_VALUE")
    {
        status = String(proto.getDimmerValue());
        Serial.print("Dimmer Value: ");
        Serial.println(status);
    }
    return status;
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void setup()
{

    Serial.begin(115200);
    Serial.println("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    //initialize our automation proto project
    proto.init();
    

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Requesting index page...");
        request->send(SPIFFS, "/index.html", "text/html", false, indexPageProcessor);
    });

    // Route to load entireframework.min.css file
    server.on("/css/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/css/entireframework.min.css", "text/css");
    });

    // Route to load toggle-switchy.css file
    server.on("/css/toggle-switchy.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/css/toggle-switchy.css", "text/css");
    });

    // Route to load custom.css file
    server.on("/css/custom.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/css/custom.css", "text/css");
    });

    // Route to load custom.js file
    server.on("/js/custom.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/js/custom.js", "text/javascript");
    });

    // server toggle request for "tube"
    server.on("/toggle/tube", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<100> data;
        if (request->hasParam("status"))
        {
            String status = request->getParam("status")->value();
            bool tubeStatus = (status == "true");
            proto.setTubeStatus(tubeStatus);
            data["success"] = "true";
            data["current_tube_status"] = proto.isTubeOn();
        }
        else
        {
            data["isError"] = "true";
            data["error_description"] = "No tube status parameter was sent by the client!";
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(data, *response);
        request->send(response);
    });

    // server toggle request for "bulb"
    server.on("/toggle/bulb", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<100> data;
        if (request->hasParam("status"))
        {
            String status = request->getParam("status")->value();
            bool bulbStatus = (status == "true");
            proto.setBulbStatus(bulbStatus);
            data["success"] = "true";
            data["current_bulb_status"] = proto.isBulbOn();
        }
        else
        {
            data["isError"] = "true";
            data["error_description"] = "No bulb status parameter was sent by the client!";
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(data, *response);
        request->send(response);
    });

    // server toggle request for "socket"
    server.on("/toggle/socket", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<100> data;
        if (request->hasParam("status"))
        {
            String status = request->getParam("status")->value();
            bool socketStatus = (status == "true");
            proto.setSocketStatus(socketStatus);
            data["success"] = "true";
            data["current_socket_status"] = proto.isSocketOn();
        }
        else
        {
            data["isError"] = "true";
            data["error_description"] = "No socket status parameter was sent by the client!";
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(data, *response);
        request->send(response);
    });

    // server toggle request for "dimmer"
    server.on("/dimmer/change", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<100> data;
        if (request->hasParam("value"))
        {
            String value = request->getParam("value")->value();
            proto.setDimmerValue(value.toInt());
            data["success"] = "true";
            data["current_dimmer_value"] = proto.getDimmerValue();
        }
        else
        {
            data["isError"] = "true";
            data["error_description"] = "No dimmer value parameter was sent by the client!";
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(data, *response);
        request->send(response);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop()
{
}

