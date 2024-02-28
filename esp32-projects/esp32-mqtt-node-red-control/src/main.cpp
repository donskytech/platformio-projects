#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Update these with your Wi-Fi and MQTT broker details
const char *ssid = "<CHANGE_TO_YOUR_WIFI_SSID>";
const char *password = "<CHANGE_TO_YOUR_WIFI_PASSWORD>";
const char *mqtt_server = "<CHANGE_TO_YOUR_MQTT_SERVER_IP>";

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqttToggleLEDTopic = "toggle-led";
const char *mqttRGBColorSetTopic = "rgb-color-set";
const char *mqttServoPositionSetTopic = "servo-position-set";

// LED PIN Setup
const int LED_PIN = 32;

// RGB LED PIN Setup
const int redPin = 33;
const int greenPin = 25;
const int bluePin = 26;

// Servo PinSetup
int const servoPin = 27;
Servo servoMotor;
int pos = 0;

//  Forward Declaration
void processMessage(const char *topic, const char *message);
void moveServo(int position);
void setupWifi();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void reconnect();
void handleLedMessage(String payload);
void handleRgbMessage(String payload);
void handleServoMessage(String payload);

void setup()
{
  Serial.begin(9600);
  // Setup WiFi connection details
  setupWifi();
  // Setup MQTT Connection
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  // Initialize other setup tasks if needed
  pinMode(LED_PIN, OUTPUT);
  // RGB LED
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  servoMotor.setPeriodHertz(50);           // standard 50 hz servo
  servoMotor.attach(servoPin, 500, 2450); // attaches the servo on pin 18 to the servo object
  moveServo(0);

}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  // for (pos = 0; pos <= 180; pos += 1)
  // { // goes from 0 degrees to 180 degrees
  //   // in steps of 1 degree
  //   servoMotor.write(pos); // tell servo to go to position in variable 'pos'
  //   delay(20);             // waits 15ms for the servo to reach the position
  // }
  // for (pos = 180; pos >= 0; pos -= 1)
  // {                        // goes from 180 degrees to 0 degrees
  //   servoMotor.write(pos); // tell servo to go to position in variable 'pos'
  //   delay(20);             // waits 15ms for the servo to reach the position
  // }
}

void setupWifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{

  String payloadStr = "";
  for (int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }

  Serial.print("Received message on topic '");
  Serial.print(topic);
  Serial.print("': ");
  Serial.println(payloadStr);

  if (strcmp(topic, mqttToggleLEDTopic) == 0)
  {
    handleLedMessage(payloadStr);
  }
  else if (strcmp(topic, mqttRGBColorSetTopic) == 0)
  {
    handleRgbMessage(payloadStr);
  }
  else if (strcmp(topic, mqttServoPositionSetTopic) == 0)
  {
    handleServoMessage(payloadStr);
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
      client.subscribe(mqttToggleLEDTopic);
      client.subscribe(mqttRGBColorSetTopic);
      client.subscribe(mqttServoPositionSetTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void handleLedMessage(String payload)
{
  if (payload.equals("true"))
  {
    digitalWrite(LED_PIN, HIGH);
  }
  else if (payload.equals("false"))
  {
    digitalWrite(LED_PIN, LOW);
  }
}

void handleRgbMessage(String payload)
{
  if (payload.equals("red"))
  {
    analogWrite(redPin, 255);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }
  else if (payload.equals("green"))
  {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 0);
  }
  else if (payload.equals("blue"))
  {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 255);
  }
}

void handleServoMessage(String payload)
{
  int angle = payload.toInt();
  moveServo(angle);
}

void moveServo(int position)
{
  // Set the servo position
  servoMotor.write(position);

  // Wait for some time
  delay(20);
}