#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/*
  Replace the SSID and Password according to your wifi
*/
const char *ssid = "<YOUR_WIFI_SSID>";
const char *password = "<YOUR_WIFI_PASSWORD>";

// Your MQTT broker ID
const char *mqttBroker = "192.168.100.22";
const int mqttPort = 1883;
// const char *mqttUserName = "user";
// const char *mqttUserPassword = "password";

// MQTT topics
const char *publishTopic = "temperature";
const char *subscribeTopic = "led";

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
// WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, mqttBroker, mqttPort);
// Adafruit_MQTT_Client mqtt(&client, mqttBroker, mqttPort, mqttUserName, mqttUserPassword);

// Setup a subscriber
Adafruit_MQTT_Subscribe ledControl = Adafruit_MQTT_Subscribe(&mqtt, subscribeTopic, MQTT_QOS_1);

// Setup a publisher
Adafruit_MQTT_Publish temperatureControl = Adafruit_MQTT_Publish(&mqtt, publishTopic, MQTT_QOS_1);

unsigned long lastMsg = 0;
const int READ_CYCLE_TIME = 3000;

void ledCallback(char *message, uint16_t len)
{
  char messageBuffer[40];
  snprintf(messageBuffer, sizeof(messageBuffer), "LED status is :: %s, len :: %u", message, len);
  Serial.println(messageBuffer);
  if (strcmp(message, "ON") == 0)
  {
    Serial.println("Turning ON LED");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    Serial.println("Turning OFF LED");
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void setupWifi()
{
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void connectToMQTTServer()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000); // wait 10 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void setup()
{
  // Set Serial Monitor Baud rate
  Serial.begin(115200);
  // Configure LED
  pinMode(LED_BUILTIN, OUTPUT);
  // Connect to Wifi
  setupWifi();
  // Randomize seed
  randomSeed(micros());
  // Set MQTT callback function
  ledControl.setCallback(&ledCallback);
  // Setup MQTT subscription for time feed.
  mqtt.subscribe(&ledControl);
}

void loop()
{
  // Connect to MQTT
  connectToMQTTServer();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(10000);

  // publish random temperature readings
  unsigned long now = millis();
  if (now - lastMsg > READ_CYCLE_TIME)
  {
    lastMsg = now;

    //  Publish MQTT messages
    int randomTemp = random(0, 50);
    if (!temperatureControl.publish(randomTemp))
    {
      Serial.println(F("Failed to send MQTT Message"));
    }
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  // if (!mqtt.ping())
  // {
  //   mqtt.disconnect();
  // }
}
