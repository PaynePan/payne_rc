#include "BtnControl.h"

void BtnControl::setPin(uint8_t p) {
  pin =p; 
  pinMode(pin,INPUT_PULLUP);
} 

boolean BtnControl::isTriggered() {
  
  if ( digitalRead(pin) == LOW) {
    delayMicroseconds(1);
    return 1;
  }
  return  0;
}

void FuncBtnControl::handle() {
  if (isTriggered()) {
    func();
  }
}

OffsetBtnControl::OffsetBtnControl() {
  dec = false;
}

boolean OffsetBtnControl::isTriggered() {
  static long last = 0;
  long now = millis(); 

  if ( now - last > 150 && digitalRead(pin) == LOW) {
    delayMicroseconds(1);
    last = now;
    return 1;
  }
  return  0;
}

void OffsetBtnControl::handle() {
  if (isTriggered()) {
    ch->changeOffset(dec);
  }
}

void ReverseBtnControl::handle() {
  if (isTriggered()) {
    ch->setReversed(true);
  }  else {
    ch->setReversed(false);
  }
}

