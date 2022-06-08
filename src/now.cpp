#include "now.h"

nowMessage incomingMessage;

void nowReceiveCb(const uint8_t* mac, const uint8_t* data, int len){
  memcpy(&incomingMessage, data, sizeof(incomingMessage));
  activeMode = incomingMessage.activeMode;
  activeBackground = incomingMessage.activeBackground;
  colorBG = incomingMessage.colorBG;
  colorFG = incomingMessage.colorFG;
}
