#include "weather.h"

#include <Arduino.h>
#include <ArduinoJson.h>

bool handleWeatherData(){
    // get new data every 10 minutes
    if(millis()/1000-lastWeatherQuery > 600){
        updateWeatherData();
        lastWeatherQuery = millis()/1000;
        return true;
    }
    return false;
}

void updateWeatherData(){
    if (weatherClient.connect(weatherServername, 80)) {  //starts client connection, checks for connection
        weatherClient.println("GET /data/2.5/weather?id="+cityID+"&units=metric&APPID="+apiKey);
        weatherClient.println("Host: api.openweathermap.org");
        weatherClient.println("User-Agent: ArduinoWiFi/1.1");
        weatherClient.println("Connection: close");
        weatherClient.println();
    } 
    else {
        return;
    }

    while(weatherClient.connected() && !weatherClient.available()) delay(1); //waits for data
    while (weatherClient.connected() || weatherClient.available()) { //connected or data available
        char c = weatherClient.read(); //gets byte from ethernet buffer
        weatherResult = weatherResult+c;
    }

    weatherClient.stop(); //stop client
    weatherResult.replace('[', ' ');
    weatherResult.replace(']', ' ');

    char jsonArray [weatherResult.length()+1];
    weatherResult.toCharArray(jsonArray,sizeof(jsonArray));
    jsonArray[weatherResult.length() + 1] = '\0';
    weatherResult = "";
    
    StaticJsonBuffer<1024> json_buf;
    JsonObject &root = json_buf.parseObject(jsonArray);
    if (!root.success())
    {
        return;
    }
    
    
    float read_temperature = root["main"]["temp"];
}