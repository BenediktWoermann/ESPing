// Include guard
#ifndef NOW_H
#define NOW_H

#include <Arduino.h>
#include <FastLED.h>
#include "esp_now.h"
#include "webserver2.h"

typedef struct nowMessage{
    long colorBG;
    long colorFG;
    int activeMode;
    int activeBackground;
}nowMessage;

extern uint8_t hue;
extern uint8_t saturation;
extern uint8_t intensity;
extern int activeMode;
extern int activeBackground;

void nowReceiveCb(const uint8_t* mac, const uint8_t* data, int len);

#endif //NOW_H