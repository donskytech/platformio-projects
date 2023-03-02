#include <Arduino.h>
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 32 // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
// #define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// Change this to point to your Wifi Credentials
const char *ssid = "<REPLACE_ME_WITH_YOUR_SSID>";
const char *password = "<REPLACE_ME_WITH_YOUR_PASSWORD>";

// Your MQTT broker ID
const char *mqttBroker = "192.168.100.22";
const int mqttPort = 1883;

// MQTT topics
const char *temperatureTopic = "sensor/dht22/temperature";
const char *humidityTopic = "sensor/dht22/humidity";

// MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE (5)

// Cycle time
unsigned long previousMillis = 0;
const int interval = 2000;

// Callback function whenever an MQTT message is received
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++)
  {
    Serial.print(message += (char)payload[i]);
  }
  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("MQTT Broker connected!");
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishMessage(const char *topic, float value)
{
  char msgBuffer[MSG_BUFFER_SIZE];
  snprintf(msgBuffer, MSG_BUFFER_SIZE, "%g", value);
  Serial.printf("Publishing to topic :: %s, value :: %s", topic, msgBuffer);
  Serial.println("");
  client.publish(topic, msgBuffer);
}

// Connect to Wifi
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
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

void setup()
{
  Serial.begin(115200);
  // Setup the wifi
  setup_wifi();
  // setup the mqtt server and callback
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(callback);

  dht.begin();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time we send the last reading
    previousMillis = currentMillis;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temperatureInC = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float temperatureInF = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperatureInC) || isnan(temperatureInF))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    publishMessage(temperatureTopic, temperatureInC);
    publishMessage(humidityTopic, humidity);
  }
}