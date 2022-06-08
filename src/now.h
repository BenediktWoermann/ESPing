// Include guard
#ifndef NOW_H
#define NOW_H

#include <Arduino.h>
#include "esp_now.h"
#include "webserver2.h"

typedef struct nowMessage{
    long colorBG;
    long colorFG;
    int activeMode;
    int activeBackground;
}nowMessage;

extern long colorBG;
extern long colorFG;
extern int activeMode;
extern int activeBackground;

void nowReceiveCb(const uint8_t* mac, const uint8_t* data, int len);

#endif //NOW_H