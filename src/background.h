#include <FastLED.h>
#include "display.h"

extern CRGB leds[ledQuantity];
extern CRGB* display[ledRows][ledColumns];

void cloudbg(float speed, int scale);
void rainbowbg(float speed);
void staticbg(int hue, int saturation, int intensity);