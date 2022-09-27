#include "webserver2.h"

#include "main.h"
#include "weather.h"
#include "credentials.h"
#include "OTA.h"
#include "now.h"
#include "esp_now.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define DEBUGGING 1

// returns true if no error occured, else false gets returned
bool wifiSetup(){
    // WiFi.mode(WIFI_AP_STA);
    // WiFi.begin(SSID, PW);

    // uint8_t result = WiFi.waitForConnectResult();

    // if(esp_now_init() != ESP_OK){
    //     Telnet.println("ESP-NOW not initialized!");
    // }
    
    // if (result != WL_CONNECTED) {
    //     Telnet.println("Connection Failed!");
    //     delay(5000);
    //     return false;
    // }

    

    // // Register callback function for receiving data
    // esp_now_register_recv_cb(nowReceiveCb);

    // #if defined(DEBUGGING)
    // Telnet.println("Ready");
    // Telnet.print("IP address: ");
    // Serial.println(WiFi.localIP());
    // #endif

    // return true;
}

void handleModeRequest(){
    if (server.hasHeader("Mode")){
        if(server.header("Mode").equals("time")){
            activeMode = TIME;
        }
        else if(server.header("Mode").equals("weather")){
            activeMode = WEATHER;
        }else{
            activeMode = CLEAR;
        }
    }

    if (server.hasHeader("Background")){
        if(server.header("Background").equals("rainbow")){
            activeBackground = RAINBOW;
        }
        else if(server.header("Background").equals("static")){
            activeBackground = STATIC;
        }
        else if(server.header("Background").equals("cloud")){
            activeBackground = CLOUD;
        }
    }

    if (server.hasHeader("ColorBG")){
        char colorCharArray[7];
        colorCharArray[6] = '\0';
        server.header("ColorBG").toCharArray(colorCharArray, 7);
        colorBG = strtol(colorCharArray, NULL, 16);
        Telnet.print(colorBG);
    }

    if (server.hasHeader("ColorFG")){
        char colorCharArray[7];
        colorCharArray[6] = '\0';
        server.header("ColorFG").toCharArray(colorCharArray, 7);
        colorFG = strtol(colorCharArray, NULL, 16);
        Telnet.print(colorFG);
    }

    server.send(200,"text/plain", "Mode request received!");
}

void serverSetup(){
    server.on("/restart",[](){
        server.send(200,"text/plain", "Restarting...");
        delay(1000);
        ESP.restart();
    });

    server.on("/setflag",[](){
        server.send(200,"text/plain", "Setting flag...");
        ota_flag = true;
        time_elapsed = 0;
    });

    server.on("/weather", [](){
        server.send(200, "text/plain", String(temperature));
    });

    server.on("/time", [](){
        server.send(200, "text/plain", String(timeClient.getFormattedTime()));
    });

    server.on("/mac", [](){
        server.send(200, "text/plain", String(WiFi.macAddress()));
    });

    server.on("/mode", handleModeRequest);

    const char * headerkeys[] = {"Mode", "Background", "ColorBG", "ColorFG"} ;
    size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
    //ask server to track these headers
    server.collectHeaders(headerkeys, headerkeyssize );

    server.begin();
}