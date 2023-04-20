/*
  Title: Control your Arduino IoT projects with a MongoDB database
  Author: donsky (www.donskytech.com)
*/
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// SSID and Password
const char *ssid = "<PLACE_YOUR_SSID_HERE>";
const char *password = "<PLACE_YOUR_PASSWORD_HERE>";

/**** NEED TO CHANGE THIS ACCORDING TO YOUR SETUP *****/
// The REST API endpoint - Change the IP Address
const char *base_rest_url = "http://192.168.100.22:5000/";

WiFiClient client;
HTTPClient http;

// Read interval
unsigned long previousMillis = 0;
const long readInterval = 5000;
// LED Pin
const int LED_PIN = 23;
// Relay Pins
const int RELAY_PIN_1 = 19;
const int RELAY_PIN_2 = 18;
const int RELAY_PIN_3 = 5;
const int RELAY_PIN_4 = 17;

char dhtObjectId[30];
#define DHTPIN 32 // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
// Struct to read DHT22 readings
struct DHT22Readings
{
  float temperature;
  float humidity;
};
// Struct to represent our LED  record
struct LED
{
  char sensor_id[10];
  char description[20];
  char location[20];
  bool enabled;
  char type[20];
  char status[10];
};
// Struct to represent our RELAY record
struct RELAY
{
  char sensor_id[10];
  char description[20];
  char location[20];
  bool enabled;
  char type[20];
  char in1[10];
  char in2[10];
  char in3[10];
  char in4[10];
};
// Size of the JSON document. Use the ArduinoJSON JSONAssistant
const int JSON_DOC_SIZE = 384;
// HTTP GET Call
StaticJsonDocument<JSON_DOC_SIZE> callHTTPGet(const char *sensor_id)
{
  char rest_api_url[200];
  // Calling our API server
  sprintf(rest_api_url, "%sapi/sensors?sensor_id=%s", base_rest_url, sensor_id);
  Serial.println(rest_api_url);

  http.useHTTP10(true);
  http.begin(client, rest_api_url);
  http.addHeader("Content-Type", "application/json");
  http.GET();

  StaticJsonDocument<JSON_DOC_SIZE> doc;
  // Parse response
  DeserializationError error = deserializeJson(doc, http.getStream());

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return doc;
  }

  http.end();
  return doc;
}
// Extract LED records
LED extractLEDConfiguration(const char *sensor_id)
{
  StaticJsonDocument<JSON_DOC_SIZE> doc = callHTTPGet(sensor_id);
  if (doc.isNull() || doc.size() > 1)
    return {}; // or LED{}
  for (JsonObject item : doc.as<JsonArray>())
  {

    const char *sensorId = item["sensor_id"];      // "led_1"
    const char *description = item["description"]; // "This is our LED"
    const char *location = item["location"];       // "Inside the bedroom"
    bool enabled = item["enabled"];                // true
    const char *type = item["type"];               // "toggle"
    const char *status = item["status"];           // "HIGH"

    LED ledTemp = {};
    strcpy(ledTemp.sensor_id, sensorId);
    strcpy(ledTemp.description, description);
    strcpy(ledTemp.location, location);
    ledTemp.enabled = enabled;
    strcpy(ledTemp.type, type);
    strcpy(ledTemp.status, status);

    return ledTemp;
  }
  return {}; // or LED{}
}
// Extract Relay record
RELAY extractRelayConfiguration(const char *sensor_id)
{
  StaticJsonDocument<JSON_DOC_SIZE> doc = callHTTPGet(sensor_id);
  if (doc.isNull() || doc.size() > 1)
    return {}; // or RELAY{}
  for (JsonObject item : doc.as<JsonArray>())
  {
    const char *sensorId = item["sensor_id"];      // "relay_1"
    const char *description = item["description"]; // "This is our Relay"
    const char *location = item["location"];       // "Outside the garage"
    bool enabled = item["enabled"];                // true
    const char *type = item["type"];               // "toggle"

    JsonObject values = item["values"];
    const char *valuesIn1 = values["in1"]; // "LOW"
    const char *valuesIn2 = values["in2"]; // "LOW"
    const char *valuesIn3 = values["in3"]; // "HIGH"
    const char *valuesIn4 = values["in4"]; // "HIGH"

    RELAY relayTemp = {};
    strcpy(relayTemp.sensor_id, sensorId);
    strcpy(relayTemp.description, description);
    strcpy(relayTemp.location, location);
    relayTemp.enabled = enabled;
    strcpy(relayTemp.type, type);
    strcpy(relayTemp.in1, valuesIn1);
    strcpy(relayTemp.in2, valuesIn2);
    strcpy(relayTemp.in3, valuesIn3);
    strcpy(relayTemp.in4, valuesIn4);

    return relayTemp;
  }
  return {}; // or RELAY{}
}
// Send DHT22 readings using HTTP PUT
void sendDHT22Readings(const char *objectId, DHT22Readings dhtReadings)
{
  char rest_api_url[200];
  // Calling our API server
  sprintf(rest_api_url, "%sapi/sensors/%s", base_rest_url, objectId);
  Serial.println(rest_api_url);

  // Prepare our JSON data
  String jsondata = "";
  StaticJsonDocument<JSON_DOC_SIZE> doc;
  JsonObject readings = doc.createNestedObject("readings");
  readings["temperature"] = dhtReadings.temperature;
  readings["humidity"] = dhtReadings.humidity;

  serializeJson(doc, jsondata);
  Serial.println("JSON Data...");
  Serial.println(jsondata);

  http.begin(client, rest_api_url);
  http.addHeader("Content-Type", "application/json");

  // Send the PUT request
  int httpResponseCode = http.PUT(jsondata);
  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  }
  else
  {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    http.end();
  }
}

// Get DHT22 ObjectId
void getDHT22ObjectId(const char *sensor_id)
{
  StaticJsonDocument<JSON_DOC_SIZE> doc = callHTTPGet(sensor_id);
  if (doc.isNull() || doc.size() > 1)
    return;
  for (JsonObject item : doc.as<JsonArray>())
  {
    Serial.println(item);
    const char *objectId = item["_id"]["$oid"]; // "dht22_1"
    strcpy(dhtObjectId, objectId);

    return;
  }
  return;
}

// Read DHT22 sensor
DHT22Readings readDHT22()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperatureInC = dht.readTemperature();
  // // Read temperature as Fahrenheit (isFahrenheit = true)
  // float temperatureInF = dht.readTemperature(true);

  return {temperatureInC, humidity};
}
// Convert HIGH and LOW to Arduino compatible values
int convertStatus(const char *value)
{
  if (strcmp(value, "HIGH") == 0)
  {
    Serial.println("Setting LED to HIGH");
    return HIGH;
  }
  else
  {
    Serial.println("Setting LED to LOW");
    return LOW;
  }
}
// Set our LED status
void setLEDStatus(int status)
{
  Serial.printf("Setting LED status to : %d", status);
  Serial.println("");
  digitalWrite(LED_PIN, status);
}
// Set our Relay status
void setRelayStatus(int pin, int status)
{
  Serial.printf("Setting Relay %d status to : %d", pin, status);
  Serial.println("");
  digitalWrite(pin, status);
}
// Turn off all relays during startup
void turnOffAllRelay()
{
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);
  digitalWrite(RELAY_PIN_4, HIGH);
}

void setup()
{
  Serial.begin(9600);
  for (uint8_t t = 2; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

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
  //  Start DHT Sensor readings
  dht.begin();
  //  Get the ObjectId of the DHT22 sensor
  getDHT22ObjectId("dht22_1");
  // Setup LED
  pinMode(LED_PIN, OUTPUT);
  // Setup relay
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);
  turnOffAllRelay();
}

void loop()
{

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= readInterval)
  {
    // save the last time
    previousMillis = currentMillis;

    Serial.println("---------------");
    // Read our configuration for our LED
    LED ledSetup = extractLEDConfiguration("led_1");
    Serial.println(ledSetup.sensor_id);
    Serial.println(ledSetup.description);
    Serial.println(ledSetup.location);
    Serial.println(ledSetup.enabled);
    Serial.println(ledSetup.type);
    Serial.println(ledSetup.status);
    setLEDStatus(convertStatus(ledSetup.status)); // Set LED value

    Serial.println("---------------");
    // Read our configuration for our RELAY
    RELAY relaySetup = extractRelayConfiguration("relay_1");
    Serial.println(relaySetup.sensor_id);
    Serial.println(relaySetup.description);
    Serial.println(relaySetup.location);
    Serial.println(relaySetup.enabled);
    Serial.println(relaySetup.type);
    Serial.println(relaySetup.in1);
    Serial.println(relaySetup.in2);
    Serial.println(relaySetup.in3);
    Serial.println(relaySetup.in4);
    // Set Relay Status
    setRelayStatus(RELAY_PIN_1, convertStatus(relaySetup.in1));
    setRelayStatus(RELAY_PIN_2, convertStatus(relaySetup.in2));
    setRelayStatus(RELAY_PIN_3, convertStatus(relaySetup.in3));
    setRelayStatus(RELAY_PIN_4, convertStatus(relaySetup.in4));

    Serial.println("---------------");
    // Send our DHT22 sensor readings
    // Locate the ObjectId of our DHT22 document in MongoDB
    Serial.println("Sending latest DHT22 readings");
    DHT22Readings readings = readDHT22();
    // Check if any reads failed and exit early (to try again).
    if (isnan(readings.humidity) || isnan(readings.temperature))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Serial.print("Temperature :: ");
    Serial.println(readings.temperature);
    Serial.print("Humidity :: ");
    Serial.println(readings.humidity);
    sendDHT22Readings(dhtObjectId, readings);
  }
}
