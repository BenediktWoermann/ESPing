#include <FastLED.h>

#define ledQuantity 142
#define ledRows 7
#define ledColumns 22
#define dataPin 26

extern CRGB leds[ledQuantity];
extern CRGB* display[ledRows][ledColumns];
extern int lastTimeShown;

void testLed();
void setupLED();
void writeDigit(int number, int row, int column, long color);
void writeNumber(unsigned int number, long color);
void writeTime(int hours, int minutes, long colorFG, long colorBG);
void writeTemp(int temperature, long color);
void animateWifiError(int row, int column, long color);
void testAllLeds();