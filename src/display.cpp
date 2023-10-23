#include "display.h"

#include <FastLED.h>

void setupLED(){
    LEDS.addLeds<WS2811, dataPin, GRB>(leds,ledQuantity);
    // FastLED.setMaxRefreshRate( 10 );


    const int amountOfCompleteColumns = 16;

    // set short columns on the left (4,5,6)
    // left column with 4 leds
    display[3][0] = &leds[1];
    display[4][0] = &leds[0];
    display[5][0] = &leds[8];
    display[6][0] = &leds[9];

    // left column with 5 leds
    display[2][1] = &leds[2];
    display[3][1] = &leds[6];
    display[4][1] = &leds[7];
    display[5][1] = &leds[11];
    display[6][1] = &leds[10];

    // left column with 6 leds
    display[1][2] = &leds[4];
    display[2][2] = &leds[5];
    display[3][2] = &leds[13];
    display[4][2] = &leds[12];
    display[5][2] = &leds[22];
    display[6][2] = &leds[23];


    // all columns with 7 leds
    for(int column = 3; column<amountOfCompleteColumns+3; column++){
        if(column%2 == 1){
            display[0][column] = &leds[3+(7*(column-3))];
            display[1][column] = &leds[15+(7*(column-3))];
            display[2][column] = &leds[14+(7*(column-3))];
            display[3][column] = &leds[20+(7*(column-3))];
            display[4][column] = &leds[21+(7*(column-3))];
            display[5][column] = &leds[25+(7*(column-3))];
            display[6][column] = &leds[24+(7*(column-3))];
        }else{
            display[0][column] = &leds[16+(7*(column-4))];
            display[1][column] = &leds[18+(7*(column-4))];
            display[2][column] = &leds[19+(7*(column-4))];
            display[3][column] = &leds[27+(7*(column-4))];
            display[4][column] = &leds[26+(7*(column-4))];
            display[5][column] = &leds[36+(7*(column-4))];
            display[6][column] = &leds[37+(7*(column-4))];
        }
    }

    // right column with 4 leds
    display[0][amountOfCompleteColumns+5] = &leds[129];
    display[1][amountOfCompleteColumns+5] = &leds[140];
    display[2][amountOfCompleteColumns+5] = &leds[139];
    display[3][amountOfCompleteColumns+5] = &leds[141];

    // right column with 5 leds
    display[0][amountOfCompleteColumns+4] = &leds[128];
    display[1][amountOfCompleteColumns+4] = &leds[130];
    display[2][amountOfCompleteColumns+4] = &leds[131];
    display[3][amountOfCompleteColumns+4] = &leds[138];
    display[4][amountOfCompleteColumns+4] = &leds[137];

    // right column with 6 leds
    display[0][amountOfCompleteColumns+3] = &leds[115];
    display[1][amountOfCompleteColumns+3] = &leds[127];
    display[2][amountOfCompleteColumns+3] = &leds[126];
    display[3][amountOfCompleteColumns+3] = &leds[132];
    display[4][amountOfCompleteColumns+3] = &leds[133];
    display[5][amountOfCompleteColumns+3] = &leds[136];
}

// input row and column specify the ball where the number will appear
void writeDigit(int number, int row, int column, long color){
    // check if number is out of range
    if(number > 9 || number < 0){
        return;
    }

    // check if position is invalid (out of field)
    if(display[row-1][column-1] == NULL || display[row-1][column+2] == NULL || display[row+3][column-1] == NULL || display[row+3][column-2] == NULL){
        return;
    }

    switch (number)
    {
    case 0:
        *display[row][column-1] = color;
        *display[row-1][column] = color;
        *display[row-1][column+1] = color;
        *display[row][column+1] = color;
        *display[row+2][column-2] = color;
        *display[row+3][column-2] = color;
        *display[row+3][column-1] = color;
        *display[row+2][column] = color;
        break;

    case 2:
        *display[row-1][column] = color;
        *display[row-1][column+1] = color;
        *display[row][column+1] = color;
        *display[row+1][column] = color;
        *display[row+1][column-1] = color;
        *display[row+2][column-2] = color;
        *display[row+3][column-2] = color;
        *display[row+3][column-1] = color;
        break;

    case 4:
        *display[row-1][column] = color;
        *display[row][column-1] = color;
        *display[row+1][column-1] = color;
        *display[row+1][column] = color;
        *display[row][column+1] = color;
        *display[row-1][column+2] = color;
        *display[row+2][column] = color;
        *display[row+3][column-1] = color;
        break;

    case 7:
        *display[row-1][column] = color;
        *display[row-1][column+1] = color;
        *display[row][column+1] = color;
        *display[row+1][column] = color;
        *display[row+2][column-1] = color;
        *display[row+3][column-2] = color;
        break;

    case 9:
        *display[row][column-1] = color;

    case 3:
        *display[row-1][column] = color;
        *display[row+1][column-1] = color;
        *display[row+3][column-2] = color;

    case 1:
        *display[row-1][column+1] = color;
        *display[row][column+1] = color;
        *display[row+1][column] = color;
        *display[row+2][column] = color;
        *display[row+3][column-1] = color;
        break;

    case 8:
        *display[row][column+1] = color;

    case 6:
        *display[row+2][column-2] = color;

    case 5:
        *display[row-1][column+1] = color;
        *display[row-1][column] = color;
        *display[row][column-1] = color;
        *display[row+1][column-1] = color;
        *display[row+1][column] = color;
        *display[row+2][column] = color;
        *display[row+3][column-1] = color;
        *display[row+3][column-2] = color;
        break;

    default:
        break;
    }

}

void writeTime(int hours, int minutes, long colorFG, long colorBG){
    
    // animate blinking dots in the middle
    uint8_t dotsSaturation = (millis()%1000)/10;
    if(millis()%2000>1000){
        dotsSaturation = 100-dotsSaturation;
    }
    uint8_t dotsRed = (colorFG>>16)*dotsSaturation/100 + (colorBG>>16)*(100-dotsSaturation)/100;
    uint8_t dotsGreen = ((colorFG>>8)%(1<<8))*dotsSaturation/100 + ((colorBG>>8)%(1<<8))*(100-dotsSaturation)/100;
    uint8_t dotsBlue = (colorFG%(1<<8))*dotsSaturation/100 + (colorBG%(1<<8))*(100-dotsSaturation)/100;
    long dotsColor = (dotsRed<<16) + (dotsGreen<<8) + (dotsBlue);
    *display[4][10] = dotsColor;
    *display[2][11] = dotsColor;
    
    
    // check if time has changed since last call
    // if(lastTimeShown == hours*60+minutes) {
    //     FastLED.show();
    //     return;
    // }
    // else lastTimeShown = hours*60+minutes;

    //write hours
    if(hours>9){
        writeDigit((int)(hours/10), 2, 4, colorFG);
    }
    writeDigit(hours%10, 2, 8, colorFG);

    //write minutes
    writeDigit((int)(minutes/10), 2, 14, colorFG);
    writeDigit(minutes%10, 2, 18, colorFG);

}

void writeNumber(unsigned int number, long color){
    if(number > 99999) return;
    writeDigit(number%10, 2, 19, color);
    if(number>=10){
        writeDigit(int((number%100)/10), 2, 15, color);
    }

    if(number>=100){
        writeDigit(int((number%1000)/100), 2, 11, color);
    }

    if(number>=1000){
        writeDigit(int((number%10000)/1000), 2, 7, color);
    }

    if(number>=10000){
        writeDigit(int((number%100000)/10000), 2, 3, color);
    }
}

void writeTemp(int temperature, long color){
    bool tempNegative = false;
    // absolute value of temperature
    int absTemp;
    absTemp = abs(temperature);
    tempNegative = temperature<0;

    //check if temperature is out of range
    if(absTemp>100) return;

    // print ° sign
    *display[1][15] = color;
    *display[2][15] = color;
    *display[1][16] = color;

    writeDigit(absTemp%10, 2, 12, color);
    if(absTemp > 10){
        writeDigit(absTemp/10, 2, 8, color);
        if(tempNegative){
            *display[3][5] = color;
            *display[3][4] = color;
            return;
        }
    }

    if(tempNegative){
        *display[3][9] = color;
        *display[3][8] = color;
    }
}

void animateWifiError(int row, int column, long color){
    int stageOfAnimation = (millis()%3000)/1000;
    switch(stageOfAnimation){
        case 2:
                *display[row+1][column+0] = color;
                *display[row+0][column+1] = color;
                *display[row+0][column+2] = color;
                *display[row+0][column+3] = color;
                *display[row+0][column+4] = color;
                *display[row+0][column+5] = color;
                *display[row+1][column+5] = color;
        case 1: 
                *display[row+3][column] = color;
                *display[row+2][column+1] = color;
                *display[row+2][column+2] = color;
                *display[row+2][column+3] = color;
                *display[row+3][column+3] = color;
        case 0:
                *display[row+4][column+1] = color;
                break;
        default:
                break;
    }
}

void testAllLeds(){
    for(int i = 0; i<ledQuantity; i++){
        leds[i] = 0x0000ff;
        delay(40);
    }
    for(int i = 0; i<ledQuantity; i++){
        leds[i] = 0x000000;
        delay(40);
    }
}