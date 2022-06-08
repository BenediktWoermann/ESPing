// Include Guard
#ifndef WEBSERVER2_H
#define WEBSERVER2_H


#include <Arduino.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

extern const char* ssid;
extern const char* password;
extern WebServer server;
extern bool ota_flag;
extern uint16_t time_elapsed;
extern float temperature;
extern char* brawlInfoOnServer;
extern const int utcOffsetInSeconds;
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

extern long colorBG;
extern long colorFG;
extern int activeMode;
extern int activeBackground;
enum modes {OFFLINE, CLEAR, TIME, WEATHER, BRAWLSTARS, SLIDESHOW, SNAKE, PONG};
enum backgrounds {RAINBOW, STATIC, CLOUD};

bool wifiSetup();
void serverSetup();


#endif // WEBSERVER2_H