#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include "Dht.h"

const char *ssid = "<CHANGE_TO_MATCH_YOUR_SSID>";
const char *password = "<CHANGE_TO_MATCH_YOUR_PASSWORD>";

// Change to match your REST Server IP Address example: http://192.168.100.22:5000/api/sensors
const char *serverName = "http://<REST_SERVER_IP>:5000/api/sensors";

#define DHT22_PIN 23 // Digital pin connected to the DHT sensor
#define DHT11_PIN 22 // Digital pin connected to the DHT sensor

// Initialize DHT sensor.
DHT dht22(DHT22_PIN, DHT22);
DHT dht11(DHT11_PIN, DHT11);

// Time interval before saving next readings
elapsedMillis timeElapsed;
unsigned long intervalSaveRequest = 10000;

// Forward declaration of saving sensor readings
void saveSensorRecord(const char *, DHT);

void setup()
{
  // Set Baud Rate
  Serial.begin(115200);

  // Begin WiFi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Begin communicating with DHT sensors
  dht22.begin();
  dht11.begin();
}

void loop()
{
  if (timeElapsed >= intervalSaveRequest)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Saving sensor records...");
      // Save sensor records, first parameter is the name or position of the sensor
      saveSensorRecord("dht-garage", dht22);
      saveSensorRecord("dht-front", dht11);
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    timeElapsed = 0;
  }
}

void saveSensorRecord(const char *sensor_id, DHT dht)
{

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float dhtHumidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float dhtTempInC = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float dhtTempInF = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(dhtHumidity) || isnan(dhtTempInC) || isnan(dhtTempInF))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float dhtHiF = dht.computeHeatIndex(dhtTempInF, dhtHumidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float dhtHiC = dht.computeHeatIndex(dhtTempInC, dhtHumidity, false);

  WiFiClient client;
  HTTPClient http;
  //  Begin connection with your REST server
  http.begin(client, serverName);
  //  Set content type to JSON
  http.addHeader("Content-Type", "application/json");

  // Write JSON of sensor readings
  StaticJsonDocument<200> doc;
  doc["sensor_id"] = sensor_id;
  doc["temperature_in_c"] = dhtTempInC;
  doc["temperature_in_f"] = dhtTempInF;
  doc["humidity"] = dhtHumidity;
  doc["heat_index_in_c"] = dhtHiC;
  doc["heat_index_in_f"] = dhtHiF;
  String requestBody;
  serializeJson(doc, requestBody);

  // HTTP Post to REST server
  int httpResponseCode = http.POST(requestBody);

  if (httpResponseCode > 0)
  {

    String response = http.getString(); // Get the response to the request

    Serial.println(httpResponseCode); // Print return code
    Serial.println(response);         // Print request answer
  }
  else
  {

    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
