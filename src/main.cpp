/*------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to program the ESP8266 Over The Air 
  (OTA).
  https://youtu.be/gFK2EDNpIeM
  https://youtu.be/R3aB85PuOQhY

  Brawlstars API:
  https://developer.brawlstars.com/#/

  Open Weather Map API:
  https://openweathermap.org/api

------------------------------------------------------------------------------*/

#define DEBUGGING_ONLINE 1
#define DEBUGGING_OFFLINE 1

#pragma region include
  #include <Arduino.h>
  #include <FreeRTOS.h>
  #include <WiFi.h>
  #include <mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <WebServer.h>
  #include <ArduinoJson.h>
  #include <HTTPClient.h>
  #include <NTPClient.h>
  #include <Ticker.h>

  #include "main.h"
  #include "OTA.h"
  #include "weather.h"
  #include "util.h"
  #include "webserver2.h"
  #include "display.h"
  #include "background.h"
  #include "credentials.h"
#pragma endregion include

#pragma region taskVariables
TaskHandle_t ledHandle = NULL;
TaskHandle_t dataHandle = NULL;

void ledTask(void* parameter);
void dataTask(void* parameter);
#pragma endregion taskVariables

#pragma region globalVariables

// defined in webserver2.h
// enum modes {OFFLINE, CLEAR, TIME, WEATHER, BRAWLSTARS, SLIDESHOW, SNAKE, PONG};
// enum backgrounds {RAINBOW, STATIC};
long colorBG;
long colorFG;
int activeMode = CLEAR;
int activeBackground = RAINBOW;

Ticker blinkerSeconds;

const char* ssid = SSID;
const char* password = PW;

CRGB leds[ledQuantity];
CRGB* display[ledRows][ledColumns];
int lastTimeShown = -1;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// last time update at minute:
int lastTimeUpdate = 0;

const char* webPage;
char* notice;
const char htmlPage[]PROGMEM=R"=====(
  <!DOCTYPE html>
  <html>
  <body>
  <h3>Change info displayed at /mytrophies </h3>
  <FORM METHOD="POST"action="/changeBrawlInfo">
  <input type="text" name="myText" value="Write your Notice...">
  <input type="submit" value="Post Notice">
  </form>
  </body>
  </html>
  )=====";

char const * weatherServername = "api.openweathermap.org";
const String cityID = CITYID;
const String  apiKey = WEATHERAPIKEY;
int32_t lastWeatherQuery = -600;
String weatherResult = "";
float temperature = 0;

WebServer server(80);
WiFiClient weatherClient;

WiFiServer telnetServer(23);
WiFiClient Telnet;

bool ota_flag = false;
uint16_t time_elapsed = 0;

#pragma endregion globalVariables


void setupTasks(){
  xTaskCreatePinnedToCore(ledTask, "led", 4096, NULL, 1, &ledHandle, 0);
  xTaskCreatePinnedToCore(dataTask, "data", 4096, NULL, 1, &dataHandle, 1);
}

void setup() {
  setupTasks();

  if(wifiSetup()){
    serverSetup();
    setupOTA();
    setupTelnet();

    Serial.begin(115200);

    activeMode = CLEAR;
    activeBackground = RAINBOW;
    colorFG = 0xffffff;
    colorBG = 0x999999;


    timeClient.begin();
    timeClient.forceUpdate();  
    lastTimeUpdate = timeClient.getMinutes();
    // char* formattedDate = &timeClient.getFormattedDate()[0];
    // char* date = strtok(formattedDate, "T");
    // int day = atoi(strtok(date, "-"));
    // int month = atoi(strtok(NULL, "-"));
    // int year = atoi(strtok(NULL, "-"));
    // if((month>3 && month<10) || (month==3 && day+7-(1+getWeekday(day, month, year)%7)>31 || month==10) && (day+7-(getWeekday(day, month, year)%7)<31)) {
      timeClient.setTimeOffset(7200);
    // }
    // else {
    //   timeClient.setTimeOffset(3600);
    // }
  }
  

  setupLED();  

  FastLED.showColor(0x00ff00);
}

void ledTask(void* parameter){

  const TickType_t xFrequency = 10;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  for(;;){
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    if(activeMode != OFFLINE){
      handleOTA();
      handleTelnet();

      server.handleClient();
    }
    switch(activeBackground){
      case RAINBOW: 
                    {
                    int rainbowBrightness = ((colorBG%256)+((colorBG/256)%256)+(colorBG%256%256))/3;
                    rainbowbg(0.1, rainbowBrightness);
                    break;
                    }
      case STATIC:
                    staticbg(colorBG);
                    break;
      default:      
                    staticbg(0x000000);
                    break;
    }
    
    switch(activeMode){
      case OFFLINE:
                  animateWifiError(1,8, 0xff0000);
                  break;
      case TIME: 
                  writeTime(timeClient.getHours(), timeClient.getMinutes(), colorFG);
                  FastLED.show();
                  break;
      case WEATHER:
                  writeTemp(temperature, colorFG);
                  FastLED.show();
                  break;
      default:
                  FastLED.show();
                  break;
    }
  }

}

void dataTask(void* parameter) {

  const TickType_t xFrequency = 1000;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  for(;;){
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    switch(activeMode){
      case TIME: 
                  if((timeClient.getMinutes()+60 - lastTimeUpdate)%60 > 10){
                    timeClient.forceUpdate();  
                    lastTimeUpdate = timeClient.getMinutes();
                  }
                  break;
      case WEATHER:
                  handleWeatherData();
                  break;
      default:
                  break;
    }

    if(activeMode != OFFLINE){
      handleOTA();
      handleTelnet();

      server.handleClient();
    }
  }

}

void myPrintln(const char* toPrint){
  #if defined(DEBUGGING_ONLINE)
  Telnet.println(toPrint);
  #endif
  #if defined(DEBUGGING_OFFLINE)
  Serial.println(toPrint);
  #endif
}

void myPrint(const char* toPrint){
  #if defined(DEBUGGING_ONLINE)
  Telnet.print(toPrint);
  #endif
  #if defined(DEBUGGING_OFFLINE)
  Serial.print(toPrint);
  #endif
}

void loop(){}