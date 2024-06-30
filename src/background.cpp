#include "background.h"

#include "display.h"
#include <FastLED.h>

void noisebg(float speed, float acceleration, float mutation, int scale, int minHue, int maxHue, int saturation, int intensity){
    static unsigned int offsetX = 0;
    static unsigned int offsetX = 0;
    unsigned long offsetMutation = int(millis() * mutation);
    static unsigned long lastAcceleration = 0;
    float speedX = speed * inoise8(acceleration * millis());
    // Add some random offset for speedY to get different speeds for X and Y
    float speedY = speed * inoise8(acceleration * (millis() + 21479))
    if(lastAcceleration == 0){
        lastAcceleration = millis();
    }
    offsetX += int((millis()-lastAcceleration) * speedX);
    offsetY += int((millis()-lastAcceleration) * speedY);

    for(int col = 0; col<ledColumns; col++){
        for(int row = 0; row<ledRows; row++){
            if(display[row][col] != NULL){
                uint8_t noise = int16_t(inoise8(scale * row + offsetX, scale * col + offsetY, scale * offsetMutation));
                noise = map(noise, 0, 255, minHue, maxHue)
                *display[row][col] = CHSV(noise, saturation, intensity);
            }
        }
    }
}

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

