#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "LittleFS.h"
#endif
#include <ESPAsyncWebServer.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <ArduinoJson.h>

// Change this to your network SSID
const char *ssid = "<CHANGE TO YOUR SSID>";
const char *password = "<CHANGE TO YOUR SSID PASSWORD>";

// AsyncWebserver runs on port 80 and the asyncwebsocket is initialize at this point also
AsyncWebServer server(80);
AsyncEventSource events("/events");

MFRC522DriverPinSimple ss_pin(5); // Configurable, see typical pin layout above.
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
// MFRC522DriverI2C driver{}; // Create I2C driver.
MFRC522 mfrc522{driver}; // Create MFRC522 instance.

// Buzzer and LED Pins
const int buzzerPin = 32;
const int ledPin = 33;

// The current tag being processed
String tagContent = "";
String currentUID = "";

// Interval before we process the same RFID
int INTERVAL = 3000;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// List of valid RFID codes
String validRFID[2] = {"04 DC 69 2B", "A9 33 26 B5"};

// Forward declaration of function
void notFound(AsyncWebServerRequest *request);
void sendMessage();
void initOutputControl();
void showAllowed();
void showNotAllowed();

void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC for debugging.
  while (!Serial)
    ; // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).

  // configure buzzer and led pin
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Connect to your wifi
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

  // setup ......
  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if (client->lastId())
    {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    } });

  // attach AsyncEventSource
  server.addHandler(&events);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("Requesting index page...");
              request->send(SPIFFS, "/index.html", "text/html", false); });
  server.serveStatic("/css", SPIFFS, "/css/");
  server.serveStatic("/js", SPIFFS, "/js/");

  // On Not Found
  server.onNotFound(notFound);

  // Start server
  server.begin();

  mfrc522.PCD_Init();                                     // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); // Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards.
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  initOutputControl();

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    tagContent.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tagContent.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  tagContent.trim();
  tagContent.toUpperCase();

  currentMillis = millis();
  // If current UID is not equal to current UID..meaning new card, then validate if it has access
  if (currentUID != tagContent)
  {
    currentUID = tagContent;
  }
  else
  {
    // If it is the same RFID UID then we wait for interval to pass before we process the same RFID
    if (currentMillis - previousMillis <= INTERVAL)
    {
      tagContent = "";
      return;
    }
  }

  if (tagContent != "")
  {
    sendMessage();
    previousMillis = currentMillis;
    tagContent = "";
  }
}

void initOutputControl()
{
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  delay(1000);
}
void showAllowed()
{
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
}

void showNotAllowed()
{
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

// Function called when resource is not found on the server
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void sendMessage()
{
  // Declare a buffer to hold the result
  char output[128];
  StaticJsonDocument<64> doc;

  bool isValid = false;
  for (size_t i = 0; i < 2; i++)
  {
    if (validRFID[i].equals(tagContent))
    {
      isValid = true;
      break;
    }
  }

  doc["status"] = isValid;
  doc["rfid_tag_id"] = tagContent;

  serializeJson(doc, output);
  events.send(output);

  if (isValid)
    showAllowed();
  else
    showNotAllowed();
}
