#include "now.h"
#include "main.h"

nowMessage incomingMessage;

void nowReceiveCb(const uint8_t* mac, const uint8_t* data, int len){
  memcpy(&incomingMessage, data, sizeof(incomingMessage));
  activeMode = incomingMessage.activeMode;
  activeBackground = incomingMessage.activeBackground;
  CHSV colorBG = CHSV(rgb2hsv_approximate(incomingMessage.colorBG));
  CHSV colorFG = CHSV(rgb2hsv_approximate(incomingMessage.colorFG));
  hue = colorBG.hue;
  saturation = colorBG.saturation;
  intensity = colorBG.value;

  if(colorFG==CHSV(0,0,0)){
    setPowerParamOff();
  }else{
    setPowerParamOn();
  }
}
