#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

/*
  The resolution of the PWM is 8 bit so the value is between 0-255
  We will set the speed between 200 to 255.
*/
enum speedSettings
{
  SLOW = 200,
  NORMAL = 225,
  FAST = 255
};

class Car
{
private:
  // Motor 1 connections
  int in1 = 16;
  int in2 = 17;
  // Motor 2 connections
  int in3 = 32;
  int in4 = 33;

  // PWM Setup to control motor speed
  const int SPEED_CONTROL_PIN_1 = 21;
  const int SPEED_CONTROL_PIN_2 = 22;
  const int freq = 30000;
  const int channel_0 = 0;
  const int channel_1 = 1;
  const int resolution = 8;

  speedSettings currentSpeedSettings;

public:
  Car()
  {
    // Set all pins to output
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(SPEED_CONTROL_PIN_1, OUTPUT);
    pinMode(SPEED_CONTROL_PIN_2, OUTPUT);

    // Set initial motor state to OFF
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

    //Attach Pin to Channel
    ledcAttachPin(SPEED_CONTROL_PIN_1, channel_0);
    ledcAttachPin(SPEED_CONTROL_PIN_2, channel_1);

    //Set the PWM Settings
    ledcSetup(channel_0, freq, resolution);
    ledcSetup(channel_1, freq, resolution);

    // initialize default speed to SLOW
    setCurrentSpeed(speedSettings::NORMAL);
  }
  void turnLeft()
  {
    Serial.println("car is turning left...");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

    setMotorSpeed();
  }
  void turnRight()
  {
    Serial.println("car is turning right...");
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    setMotorSpeed();
  }
  void moveForward()
  {
    Serial.println("car is moving forward...");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    setMotorSpeed();
  }
  void moveBackward()
  {
    setMotorSpeed();
    Serial.println("car is moving backward...");
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

  }
  void stop()
  {
    Serial.println("car is stopping...");
    // Turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }

  void setMotorSpeed()
  {
    // change the duty cycle of the speed control pin connected to the motor
    Serial.print("Speed Settings: ");
    Serial.println(currentSpeedSettings);
    ledcWrite(channel_0, currentSpeedSettings);
    ledcWrite(channel_1, currentSpeedSettings);
  }

  void setCurrentSpeed(speedSettings newSpeedSettings)
  {
    Serial.println("car is changing speed...");
    currentSpeedSettings = newSpeedSettings;
  }

  speedSettings getCurrentSpeed()
  {
    return currentSpeedSettings;
  }
};

// Change this to your network SSID
const char *ssid = "<CHANGE THIS TO YOUR SSID>";
const char *password = "<CHANGE THIS TO YOUR PASSWORD>";

// AsyncWebserver runs on port 80 and the asyncwebsocket is initialize at this point also
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Our car
Car car;

void sendCarCommand(const char *command)
{
  // command could be either "left", "right", "forward" or "reverse" or "stop"
  // or speed settingg "slow-speed", "normal-speed", or "fast-speed"
  if (strcmp(command, "left") == 0)
  {
    car.turnLeft();
  }
  else if (strcmp(command, "right") == 0)
  {
    car.turnRight();
  }
  else if (strcmp(command, "up") == 0)
  {
    car.moveForward();
  }
  else if (strcmp(command, "down") == 0)
  {
    car.moveBackward();
  }
  else if (strcmp(command, "stop") == 0)
  {
    car.stop();
  }
  else if (strcmp(command, "slow-speed") == 0)
  {
    car.setCurrentSpeed(speedSettings::SLOW);
  }
  else if (strcmp(command, "normal-speed") == 0)
  {
    car.setCurrentSpeed(speedSettings::NORMAL);
  }
  else if (strcmp(command, "fast-speed") == 0)
  {
    car.setCurrentSpeed(speedSettings::FAST);
  }
}

// Processor for index page template
String indexPageProcessor(const String &var)
{
  String status = "";
  if (var == "SPEED_SLOW_STATUS")
  {
    if (car.getCurrentSpeed() == speedSettings::SLOW)
    {
      status = "checked";
    }
  }
  else if (var == "SPEED_NORMAL_STATUS")
  {
    if (car.getCurrentSpeed() == speedSettings::NORMAL)
    {
      status = "checked";
    }
  }
  else if (var == "SPEED_FAST_STATUS")
  {
    if (car.getCurrentSpeed() == speedSettings::FAST)
    {
      status = "checked";
    }
  }
  return status;
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    // client->printf("Hello Client %u :)", client->id());
    // client->ping();
  }

  case WS_EVT_DISCONNECT:
  {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  }

  case WS_EVT_DATA:
  {
    //data packet
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
      //the whole message is in a single frame and we got all of it's data
      if (info->opcode == WS_TEXT)
      {
        data[len] = 0;
        char *command = (char *)data;
        sendCarCommand(command);
      }
    }
  }

  case WS_EVT_PONG:
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }

  case WS_EVT_ERROR:
  {
    // Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  }
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

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("Requesting index page...");
              request->send(SPIFFS, "/index.html", "text/html", false, indexPageProcessor);
            });

  // Route to load entireframework.min.css file
  server.on("/css/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/css/entireframework.min.css", "text/css"); });

  // Route to load custom.css file
  server.on("/css/custom.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/css/custom.css", "text/css"); });

  // Route to load custom.js file
  server.on("/js/custom.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/js/custom.js", "text/javascript"); });

  // On Not Found
  server.onNotFound(notFound);

  // Start server
  server.begin();
}

void loop()
{

}


