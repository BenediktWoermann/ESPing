/*------------------------------------------------------------------------------
  ESP RainMaker demo:
  https://www.youtube.com/techiesms

  Open Weather Map API:
  https://openweathermap.org/api
------------------------------------------------------------------------------*/

#pragma region include
  #include <Arduino.h>
  #include "freertos/FreeRTOS.h"
  #include "RMaker.h"
  #include "esp_rmaker_scenes.h"
  #include <WiFi.h>
  #include "WiFiProv.h"
  #include <SimpleBLE.h>
  #include <WiFiUdp.h>
  #include <WebServer.h>
  #include <HTTPClient.h>
  #include <Ticker.h>

  #include "main.h"
  #include "util.h"
  #include "display.h"
  #include "background.h"
#pragma endregion include

#pragma region taskVariables
TaskHandle_t ledHandle = NULL;
TaskHandle_t dataHandle = NULL;
TaskHandle_t wifiHandle = NULL;

void ledTask(void* parameter);
void wifiTask(void* parameter);
#pragma endregion taskVariables

#pragma region globalVariables
void sysProvEvent(arduino_event_t *sys_event);
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx);
void setupRainMaker();
void rainMakerReset();

enum modes {OFFLINE, CLEAR, TIME, WEATHER};
enum backgrounds {RAINBOW, STATIC, CLOUD};
int activeMode = CLEAR;
int activeBackground = STATIC;
long colorFG = 0xffffff;
// dummy variable to avoid write errors, all occurences must be changed to new rainmaker variables hue, saturation, intensity, brightness
long colorBG = 0xFF8822;

// BLE Credentils
SimpleBLE ble;
const char *service_name = "Esping";
const char *pop = "1234567";

static uint8_t gpio_led = dataPin;
static uint8_t gpio_reset = 0;
bool powerState = false;
uint8_t hue = 0;
uint8_t saturation = 255;
uint8_t intensity = 255;
uint8_t brightness = 255;

bool wifi_connected = 0;
bool serviceSetup = 0;

//The framework provides some standard device types like switch, lightbulb, fan, temperature sensor.
static LightBulb esping("Esping", &gpio_led);


// const char* ssid = SSID;
// const char* password = PW;

CRGB leds[ledQuantity];
CRGB* display[ledRows][ledColumns];
int lastTimeShown = -1;

uint16_t time_elapsed = 0;

#pragma endregion globalVariables


void setupTasks(){
  xTaskCreatePinnedToCore(ledTask, "led", 4096, NULL, 3, &ledHandle, 1);
  xTaskCreatePinnedToCore(wifiTask, "wifi", 4096, NULL, 2, &wifiHandle, 0);
}

void setup() {
  Serial.begin(115200);
  setupRainMaker();
  setupLED();  
  delay(50);
  setupTasks();
}

void setupRainMaker(){
  //------------------------------------------- Declaring Node -----------------------------------------------------//
  Node my_node;
  my_node = RMaker.initNode("Woermi");


  //------------------------------------------- Adding Devices in Node -----------------------------------------------------//
  my_node.addDevice(esping);
  esping.addBrightnessParam(100);
  esping.addIntensityParam(100);
  esping.addHueParam(360);
  esping.addSaturationParam(100);
  esping.updateAndReportParam("Power", false);

  Param mode = Param("Mode", NULL, esp_rmaker_str("STATIC"), PROP_FLAG_READ | PROP_FLAG_WRITE);
  esp_rmaker_param_add_ui_type(mode.getParamHandle(), ESP_RMAKER_UI_DROPDOWN);
  static const char *strs[] = {"STATIC", "RAINBOW", "CLOUD"};
  esp_rmaker_param_add_valid_str_list(mode.getParamHandle(), strs, 3);
  esping.addParam(mode);

  // Enable OTA
  esp_rmaker_ota_enable(NULL, OTA_USING_PARAMS);
  // Enable Scenes
  esp_rmaker_scenes_enable();
  esping.addCb(write_callback);
  // Enable timezone service for scheduling
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();

  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
}

void ledTask(void* parameter){

  const TickType_t xFrequency = 10;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  unsigned long offlineTimestamp = 0;
  uint32_t offlineAnimationDuration = 5000;

  // set full brightness
	LEDS.setBrightness(255);

  for(;;){
    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    
    if(!powerState){
      staticbg(0,0,0);
      continue;
    }

    switch(activeBackground){
      case CLOUD:
                    {
                    cloudbg(0.15, 80);
                    break;
                    }
      case RAINBOW: 
                    {
                    rainbowbg(0.1);
                    break;
                    }
      case STATIC:
                    staticbg(hue, saturation, intensity);
                    break;
      default:      
                    staticbg(0, 0, 0);
                    break;
    }
    
    switch(activeMode){
      case OFFLINE:
                  // Esping is offline. Show Wifi Error Animation for some duration, then switch display off
                  if(offlineTimestamp != 0) offlineTimestamp = millis();
                  if(millis()-offlineTimestamp < offlineAnimationDuration){
                    animateWifiError(1,8, 0xff0000);
                  }else{
                    staticbg(0,0,0);
                  }
                  break;
      default:
                  break;
    }
    if(activeMode != OFFLINE){
      offlineTimestamp = 0;
    }
  }

}

void wifiTask(void* parameter) {

  const TickType_t xFrequency = 1000;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  for(;;){
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Try to reconnect when Wifi is lost
    if(WiFi.status() != WL_CONNECTED){
      WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
    }
  }

}

void loop(){
  rainMakerReset();
}

void rainMakerReset(){
  //-----------------------------------------------------------  Logic to Reset RainMaker

  // Read GPIO0 (external button to reset device
  if (digitalRead(gpio_reset) == LOW) { //Push button pressed
    Serial.printf("Reset Button Pressed!\n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      wifi_connected = 0;
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      wifi_connected = 0;
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    }
  }
}

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
#else
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
      printQR(service_name, pop, "softap");
#endif
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.printf("\nConnected to Wi-Fi!\n");
      wifi_connected = 1;
      delay(500);
      break;
    case ARDUINO_EVENT_PROV_CRED_RECV: {
        Serial.println("\nReceived Wi-Fi credentials");
        Serial.print("\tSSID : ");
        Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
        Serial.print("\tPassword : ");
        Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
        break;
      }
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(device_name, "Esping") == 0)
  {
    if (strcmp(param_name, "Power") == 0)
    {
      if(!val.val.b){
        powerState = false;
      }
      else {
        powerState = true;
      }
      param->updateAndReport(val);
    }
    if (strcmp(param_name, "Hue") == 0)
    {
      hue = map(val.val.i, 0, 360, 0, 255);
      activeBackground = STATIC;
      param_val_t newVal = val;
      newVal.val.s = "STATIC";
      param->updateAndReport(newVal);
    }
    if (strcmp(param_name, "Saturation") == 0)
    {
      saturation = map(val.val.i, 0, 100, 0, 255);
      activeBackground = STATIC;
      param_val_t newVal = val;
      newVal.val.s = "STATIC";
      param->updateAndReport(newVal);
    }
    if (strcmp(param_name, "Intensity") == 0)
    {
      intensity = map(val.val.i, 0, 100, 0, 255);
      param->updateAndReport(val);
    }
    if (strcmp(param_name, "Brightness") == 0)
    {
      brightness = map(val.val.i, 0, 100, 0, 255);
      FastLED.setBrightness(brightness);
      param->updateAndReport(val);
    }
    if (strcmp(param_name, "Mode") == 0)
    {
      if(strcmp(val.val.s, "RAINBOW") == 0){
        activeBackground = RAINBOW;
      }else if(strcmp(val.val.s, "STATIC") == 0){
        activeBackground = STATIC;
      }else{
        activeBackground = CLOUD;
      }
      param->updateAndReport(val);
    }
  }
}

void setPowerParamOff(){
  esping.updateAndReportParam("Power", false);
  powerState = false;
}

void setPowerParamOn(){
  esping.updateAndReportParam("Power", true);
  powerState = true;
}
