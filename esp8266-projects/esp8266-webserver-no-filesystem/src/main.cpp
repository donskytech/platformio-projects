
/*
  Title:  ESP8266 Webserver - No Filesystem
  Description:  An ESP8266 webserver that hardcodes all web content and not using any file system
  Author: donsky
  For:    www.donskytech.com
  Date:   September 14, 2022
*/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

/*
  Replace the SSID and Password according to your wifi
*/
const char *ssid = "<REPLACE_WITH_YOUR_WIFI_SSID>";
const char *password = "<REPLACE_WITH_YOUR_WIFI_PASSWORD>";

// Webserver
AsyncWebServer server(80);

String PARAM_MESSAGE = "status";

const int LED_PIN = LED_BUILTIN;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// Prepare the HTML String
String prepareHTML()
{
  return "<!DOCTYPE html>"
         "<html>"
         "  <head>"
         "    <meta charset=\"UTF-8\" />"
         "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />"
         "    <title>ESP8266 LittleFS Webserver</title>"
         "    <link rel=\"stylesheet\" href=\"index.css\" />"
         "    <link rel=\"stylesheet\" href=\"entireframework.min.css\" />"
         "    <script src=\"index.js\"></script>"
         "  </head>"
         "  <body>"
         "    <nav class=\"nav\" tabindex=\"-1\" onclick=\"this.focus()\">"
         "      <div class=\"container\">"
         "        <a class=\"pagename current\" href=\"#\">www.donskytech.com</a>"
         "        <a href=\"#\">One</a>"
         "        <a href=\"#\">Two</a>"
         "        <a href=\"#\">Three</a>"
         "      </div>"
         "    </nav>"
         ""
         "    <div class=\"container\">"
         "      <div class=\"hero\">"
         "        <h1 class=\"title\">ESP8266 LittleFS Webserver</h1>"
         "        <div class=\"toggle-div\">"
         "            <p class=\"label\">Toggle Switch</p>"
         "            <input type=\"checkbox\" id=\"switch\" onclick=\"toggleButtonSwitch()\"/><label for=\"switch\">Toggle</label>"
         "        </div>"
         "      </div>"
         "    </div>"
         "  </body>"
         "</html>";
}

// Prepare the index.css
String prepareIndexCSS()
{
  return "/******** mincss **********/"
         ".hero {"
         "    background: #eee;"
         "    padding: 20px;"
         "    border-radius: 10px;"
         "    margin-top: 1em;"
         "    text-align: center;"
         "}"
         ".hero h1 {"
         "    margin-top: 0;"
         "    margin-bottom: 0.3em;"
         "    color: red;"
         "}"
         "/****** Custom toggle Switch**********/"
         "/* https://codepen.io/alvarotrigo/pen/abVPyaJ */"
         "/*********************/"
         ".toggle-div{"
         "    display: flex;"
         "    justify-content: center;"
         "    align-items: center;"
         "    margin-top: 5em;"
         "}"
         ".label{"
         "    font-size: 1.5em;"
         "    font-weight: bolder;"
         "    margin-right: 0.5em;"
         "    /* color: blue */"
         "}"
         "/* Styles for the toggle switch */"
         "input[type=checkbox] {"
         "    height: 0;"
         "    width: 0;"
         "    visibility: hidden;"
         "  }"
         "  "
         "  label {"
         "    cursor: pointer;"
         "    text-indent: -9999px;"
         "    width: 150px;"
         "    height: 60px;"
         "    background: grey;"
         "    display: block;"
         "    border-radius: 100px;"
         "    position: relative;"
         "  }"
         "  "
         "  label:after {"
         "    content: \"\";"
         "    position: absolute;"
         "    top: 5px;"
         "    left: 5px;"
         "    width: 70px;"
         "    height: 50px;"
         "    background: #fff;"
         "    border-radius: 90px;"
         "    transition: 0.3s;"
         "  }"
         "  "
         "  input:checked + label {"
         "    background: #bada55;"
         "  }"
         "  "
         "  input:checked + label:after {"
         "    left: calc(100% - 5px);"
         "    transform: translateX(-100%);"
         "  }"
         "  "
         "  label:active:after {"
         "    width: 130px;"
         "  }";
}

// Prepare the entireframework.min.css
String prepareIndexMinCSS()
{
  return "/* Copyright 2014 Owen Versteeg; MIT licensed */body,textarea,input,select{background:0;border-radius:0;font:16px sans-serif;margin:0}.smooth{transition:all .2s}.btn,.nav a{text-decoration:none}.container{margin:0 20px;width:auto}label>*{display:inline}form>*{display:block;margin-bottom:10px}.btn{background:#999;border-radius:6px;border:0;color:#fff;cursor:pointer;display:inline-block;margin:2px 0;padding:12px 30px 14px}.btn:hover{background:#888}.btn:active,.btn:focus{background:#777}.btn-a{background:#0ae}.btn-a:hover{background:#09d}.btn-a:active,.btn-a:focus{background:#08b}.btn-b{background:#3c5}.btn-b:hover{background:#2b4}.btn-b:active,.btn-b:focus{background:#2a4}.btn-c{background:#d33}.btn-c:hover{background:#c22}.btn-c:active,.btn-c:focus{background:#b22}.btn-sm{border-radius:4px;padding:10px 14px 11px}.row{margin:1% 0;overflow:auto}.col{float:left}.table,.c12{width:100%}.c11{width:91.66%}.c10{width:83.33%}.c9{width:75%}.c8{width:66.66%}.c7{width:58.33%}.c6{width:50%}.c5{width:41.66%}.c4{width:33.33%}.c3{width:25%}.c2{width:16.66%}.c1{width:8.33%}h1{font-size:3em}.btn,h2{font-size:2em}.ico{font:33px Arial Unicode MS,Lucida Sans Unicode}.addon,.btn-sm,.nav,textarea,input,select{outline:0;font-size:14px}textarea,input,select{padding:8px;border:1px solid #ccc}textarea:focus,input:focus,select:focus{border-color:#5ab}textarea,input[type=text]{-webkit-appearance:none;width:13em}.addon{padding:8px 12px;box-shadow:0 0 0 1px #ccc}.nav,.nav .current,.nav a:hover{background:#000;color:#fff}.nav{height:24px;padding:11px 0 15px}.nav a{color:#aaa;padding-right:1em;position:relative;top:-1px}.nav .pagename{font-size:22px;top:1px}.btn.btn-close{background:#000;float:right;font-size:25px;margin:-54px 7px;display:none}@media(min-width:1310px){.container{margin:auto;width:1270px}}@media(max-width:870px){.row .col{width:100%}}@media(max-width:500px){.btn.btn-close{display:block}.nav{overflow:hidden}.pagename{margin-top:-11px}.nav:active,.nav:focus{height:auto}.nav div:before{background:#000;border-bottom:10px double;border-top:3px solid;content:'';float:right;height:4px;position:relative;right:3px;top:14px;width:20px}.nav a{padding:.5em 0;display:block;width:50%}}.table th,.table td{padding:.5em;text-align:left}.table tbody>:nth-child(2n-1){background:#ddd}.msg{padding:1.5em;background:#def;border-left:5px solid #59d}";
}

// Prepare the index.js
String prepareIndexJS()
{
  return "function toggleButtonSwitch(e) {"
         "  var switchButton = document.getElementById(\"switch\");"
         "  "
         "  var toggleValue = \"\";"
         "  if (switchButton.checked) {"
         "    console.log(\"On!\");"
         "    toggleValue = \"ON\";"
         "  } else {"
         "    console.log(\"Off!\");"
         "    toggleValue = \"OFF\""
         "  }"
         "  fetch( `/toggle?status=${toggleValue}`)"
         "  .then( response => {"
         "    console.log(response);"
         "  } )"
         "}";
}

void toggleLED(String status)
{
  if (status == "ON")
    digitalWrite(LED_PIN, LOW);
  else
    digitalWrite(LED_PIN, HIGH);
}

void setup()
{

  Serial.begin(115200);

  // Connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // LED PIN
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", prepareHTML()); });

  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/css", prepareIndexCSS()); });

  server.on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/css", prepareIndexMinCSS()); });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/javascript", prepareIndexJS()); });

  // Respond to toggle event
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        String status;
        if (request->hasParam(PARAM_MESSAGE)) {
            status = request->getParam(PARAM_MESSAGE)->value();
            if(status == "ON"){
              toggleLED("ON");
            }else{
              toggleLED("OFF");
            }
        } else {
            status = "No message sent";
        }
        request->send(200, "text/plain", "Turning Built In LED : " + status); });

  server.onNotFound(notFound);

  server.begin();
}

void loop()
{
}