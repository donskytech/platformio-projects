#include <Arduino.h>

enum COMMON_TYPE
{
  COMMON_ANODE = 1,
  COMMON_CATHODE = 0
};
byte SEVEN_SEMENT_TYPE = COMMON_CATHODE;

const int PIN_COUNT = 8;

const byte A_PIN = D2;
const byte B_PIN = D3;
const byte C_PIN = D6;
const byte D_PIN = D5;
const byte E_PIN = D4;
const byte F_PIN = D1;
const byte G_PIN = D0;
const byte P_PIN = D7;

int segmentPins[]{A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, F_PIN, G_PIN, P_PIN};

// count from 0-9
const byte DISPLAY_COUNT = 10;
const byte SEGMENT_COUNT = 8;
const byte COL_COUNT = 8;
int seven_segment[DISPLAY_COUNT][COL_COUNT]{
    {1, 1, 1, 1, 1, 1, 0, 0}, // Display 0
    {0, 1, 1, 0, 0, 0, 0, 0}, // Display 1
    {1, 1, 0, 1, 1, 0, 1, 0}, // Display 2
    {1, 1, 1, 1, 0, 0, 1, 0}, // Display 3
    {0, 1, 1, 0, 0, 1, 1, 0}, // Display 4
    {1, 0, 1, 1, 0, 1, 1, 0}, // Display 5
    {1, 0, 1, 1, 1, 1, 1, 0}, // Display 6
    {1, 1, 1, 0, 0, 0, 0, 0}, // Display 7
    {1, 1, 1, 1, 1, 1, 1, 0}, // Display 8
    {1, 1, 1, 1, 0, 1, 1, 0}  // Display 9
};

void turnOffDisplay()
{
  // initialize all pins to off
  for (size_t i = 0; i < PIN_COUNT; i++)
  {
    digitalWrite(segmentPins[i], SEVEN_SEMENT_TYPE);
  }
}

void setup()
{
  // Initialize the serial monitor baud rate
  Serial.begin(115200);

  // set all pins to output
  for (size_t i = 0; i < PIN_COUNT; i++)
  {
    pinMode(segmentPins[i], OUTPUT);
  }
}

void loop()
{
  turnOffDisplay();

  // put your main code here, to run repeatedly:
  for (size_t iCtr = 0; iCtr < DISPLAY_COUNT; iCtr++)
  {
    for (size_t i = 0; i < COL_COUNT; i++)
    {
      digitalWrite(segmentPins[i], seven_segment[iCtr][i]);
    }
    delay(1000);
  }
}