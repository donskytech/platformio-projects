// /*
//   PROJECT NAME: ESP32 Keypad Doorlock With Database Interface
//   AUTHOR: Donsky
//   FOR: www.donskytech.com
// */

// #include <Arduino.h>
// #include <Keypad.h>
// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>
// #include <Wire.h>

// /**** NEED TO CHANGE THIS ACCORDING TO YOUR SETUP *****/
// // The REST API endpoint - Change the IP Address
// const char *base_rest_url = "http://192.168.100.22:5000/";

// // SSID and Password
// const char *ssid = "donsky-4thFloor";
// const char *password = "donsky982";

// /**** -- END -- *****/

// // Keypad columns and rows
// #define ROW_NUM 4    // four rows
// #define COLUMN_NUM 4 // four columns

// // Length of Keycode
// const int KEYCODE_SIZE = 6;

// char keys[ROW_NUM][COLUMN_NUM] = {
//     {'1', '2', '3', 'A'},
//     {'4', '5', '6', 'B'},
//     {'7', '8', '9', 'C'},
//     {'*', '0', '#', 'D'}};

// // GPIO Pins for rows
// byte pin_rows[ROW_NUM] = {19, 18, 5, 17};
// // GPIO Pins for columns
// byte pin_column[COLUMN_NUM] = {13, 14, 15, 16};

// Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// // Length of Keycode + '\0' char
// char input_keypad_code[KEYCODE_SIZE + 1];

// bool isComplete = false;
// byte ctr = 0;

// bool is_authenticated = false;

// bool parseJson(String jsonDoc)
// {
//     StaticJsonDocument<256> doc;

//     DeserializationError error = deserializeJson(doc, jsonDoc);

//     if (error)
//     {
//         Serial.print("deserializeJson() failed: ");
//         Serial.println(error.c_str());
//         return false;
//     }
//     int size = doc.size();
//     Serial.printf("JSON Array Size :: %d", size);
//     if (size == 0)
//         return false;
//     else
//         return true;
// }

// bool checkStudentId(String student_id)
// {

//     char rest_api_url[200];
//     // Calling our API server
//     sprintf(rest_api_url, "%sapi/validate-student/%s", base_rest_url, student_id.c_str());
//     Serial.printf("API URL=%s", rest_api_url);

//     HTTPClient http;

//     Serial.print("[HTTP] begin...\n");
//     http.begin(rest_api_url);

//     Serial.print("[HTTP] GET...\n");
//     // start connection and send HTTP header
//     int httpCode = http.GET();

//     // httpCode will be negative on error
//     if (httpCode > 0)
//     {
//         // HTTP header has been send and Server response header has been handled
//         Serial.printf("[HTTP] GET... code: %d\n", httpCode);

//         // file found at server
//         if (httpCode == HTTP_CODE_OK)
//         {
//             String payload = http.getString();
//             Serial.println(payload);
//             is_authenticated = parseJson(payload);
//             Serial.printf("isPresent :: %s\n", is_authenticated ? "Student ID is Valid!" : "Invalid Student ID");
//         }
//     }
//     else
//     {
//         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//     }

//     http.end();
//     return is_authenticated;
// }

// void clearDataArray()
// {
//     int index = 0;
//     while (input_keypad_code[index] != '\0')
//     {
//         input_keypad_code[index] = 0;
//         index++;
//     }
// }

// void keypadEvent(KeypadEvent key)
// {
//     switch (keypad.getState())
//     {
//     case PRESSED:
//         Serial.println(key);
//         if (key == '#')
//         {
//             Serial.println("Validating with the API...");
//             if (isComplete)
//             {
//                 Serial.println("Calling the API...");
//                 Serial.println(input_keypad_code);
//                 bool isValid = checkStudentId(input_keypad_code);
//                 if (isValid)
//                 {
//                     Serial.println("Valid Student ID!");
//                     // printToLCD("Correct keycode!", 0, 1, false);
//                     // delay(LCD_TIME_DELAY);
//                     // openLock();
//                 }
//                 else
//                 {
//                     Serial.println("Invalid Student ID!");
//                     // printToLCD("Invalid keycode!", 0, 1, false);
//                     // delay(LCD_TIME_DELAY);
//                     // closeLock();
//                 }
//                 delay(1000);
//                 // printToLCD("", 0, 1, false);
//                 ctr = 0;
//                 isComplete = false;
//                 clearDataArray();
//             }
//         }
//         else if (key == '*')
//         {
//             clearDataArray();
//             Serial.print("After clearing...");
//             Serial.println(input_keypad_code);
//             // printToLCD(input_keypad_code, 5, 1, false);
//             ctr = 0;
//             isComplete = false;
//         }
//         else
//         {
//             if (isComplete)
//             {
//                 // printToLCD("Max Length Over!", 0, 1, false);
//                 delay(1000);
//                 // clearLCDLineDisplay(SECOND_LCD_ROW);
//                 // printToLCD(input_keypad_code, 5, 1, false);
//                 return;
//             }
//             input_keypad_code[ctr] = key;
//             Serial.println(input_keypad_code);
//             // printToLCD(input_keypad_code, 5, 1, false);
//             ctr++;
//             if (ctr == KEYCODE_SIZE)
//             {
//                 Serial.println("6 digit keypad entered!");
//                 isComplete = true;
//             }
//         }
//         break;

//     case RELEASED:
//     case IDLE:
//     case HOLD:
//         break;
//     }
// }

// void setup()
// {
//     Serial.begin(9600);
//     keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
//     // Adjust the debounce accordingly
//     keypad.setDebounceTime(50);

//     // for (uint8_t t = 4; t > 0; t--)
//     // {
//     //   Serial.printf("[SETUP] WAIT %d...\n", t);
//     //   Serial.flush();
//     //   delay(1000);
//     // }

//     WiFi.begin(ssid, password);

//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("");
//     Serial.println("WiFi connected");
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());
// }

// void loop()
// {
//     // Just get the key char entered in our loop
//     keypad.getKey();
//     // Serial.println(key);
//     if (is_authenticated)
//     {
//         Serial.println("Scan your fingerprint...");
//     }
// }