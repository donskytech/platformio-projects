#include <Arduino.h>
#include <Servo.h>

const int BUFFER_SIZE = 256;
char receivedMessage[BUFFER_SIZE];
bool messageComplete = false;

// LED PIN Setup
int ledStatus = LOW;
const int LED_PIN = 2;

// RGB LED PIN Setup
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
int currentColor = 0;

// Servo PinSetup
int servoPin = 6;
Servo servoMotor;
int currentServoPosition = 0;

//  Forward Declaration
void processMessage(const char *message);
void setColor(int red, int green, int blue);
void moveServo(int position);

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(1000); // Set a timeout for Serial.readBytesUntil
  // Initialize other setup tasks if needed
  pinMode(LED_PIN, OUTPUT);
  // RGB LED
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  servoMotor.attach(servoPin, 550, 2650);
  moveServo(0);
}

void loop()
{
  if (Serial.available() > 0 && !messageComplete)
  {
    // Read the incoming characters until newline character is received
    int bytesRead = Serial.readBytesUntil('\n', receivedMessage, BUFFER_SIZE - 1);

    if (bytesRead > 0)
    {
      receivedMessage[bytesRead] = '\0'; 
      Serial.print("Received message: ");
      Serial.println(receivedMessage);

      // Process received message
      processMessage(receivedMessage);

      // Reset the buffer and flag for the next message
      memset(receivedMessage, 0, sizeof(receivedMessage));
      messageComplete = true;
    }
  }

  // Reset the messageComplete flag after performing tasks
  if (messageComplete)
  {
    messageComplete = false;
    digitalWrite(LED_PIN, ledStatus);

    if (currentColor == 1)
    {
      setColor(255, 0, 0);
    }
    else if (currentColor == 2)
    {
      setColor(0, 255, 0);
    }
    else if (currentColor == 3)
    {
      setColor(0, 0, 255);
    }
    moveServo(currentServoPosition);
    delay(100);
  }
}

void processMessage(const char *message)
{
  // Check if the message starts with "RGB"
  if (strncmp(message, "RGB-", 4) == 0)
  {
    // Process RGB command
    currentColor = atoi(message + 4); // Extract color value after "RGB-"
  }
  // Check if the message starts with "LED"
  else if (strncmp(message, "LED-", 4) == 0)
  {
    // Process LED command
    bool ledState = atoi(message + 4); // Extract LED state after "LED-"
  }
  // Check if the message starts with "SERVO"
  else if (strncmp(message, "SERVO-", 6) == 0)
  {
    // Process SERVO command
    int servoPosition = atoi(message + 6); // Extract servo position after "SERVO-"
    currentServoPosition = servoPosition;
  }
  // Unknown command
  else
  {
    Serial.println("Unknown command");
  }
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void moveServo(int position)
{
  // Set the servo position
  servoMotor.write(position);

  // Wait for some time
  delay(15);
}