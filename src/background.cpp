#include "background.h"

#include "display.h"
#include <FastLED.h>
#include "OTA.h"

#define DEBUGGING 1

void cloudbg(float speed, int scale){
    unsigned int offset = int(millis()*speed);

    for(int col = 0; col<ledColumns; col++){
        for(int row = 0; row<ledRows; row++){
            if(display[row][col] != NULL){
                uint8_t noise = inoise8(scale * row, scale * col + offset);
                noise = qsub8(noise, 10);
                noise = qadd8(noise,scale8(noise,100));

                *display[row][col] = CHSV(160, noise, 255);
            }
        }
    }
}

void rainbowbg(float speed){
    unsigned int baseHue = int(millis()*speed)%256;

    for(int col = 0; col<ledColumns; col++){
        unsigned int hue = (baseHue+col*10)%256;
        for(int row = 0; row<ledRows; row++){
            if(display[row][col] != NULL){
                *display[row][col] = CHSV(hue, 255, 255);
            }
        }
    }
}

void staticbg(int hue, int saturation, int intensity){
    for(int col = 0; col<ledColumns; col++){
        for(int row = 0; row<ledRows; row++){
            if(display[row][col] != NULL){
                *display[row][col] = CHSV(hue, saturation, intensity);
            }
        }
    }
}

