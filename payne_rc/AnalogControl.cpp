#include "AnalogControl.h"
#include "Config.h"
#include "payne_rc.h"

void AnalogControl::setPin(uint8_t p) {
  pin =p; 
#ifdef STM32_BOARD  
  pinMode(pin,INPUT_ANALOG);
#endif
} 

void AnalogControl::handle() {

  uint16_t v = analogRead(pin);
  
#ifdef STM32_BOARD  
  v = map(v, 0, 4095, 1000, 2000);
#else
  v = map(v, 0, 1023, 1000, 2000);
#endif
  ch->setRawValue(v);

}

#ifdef ANALOG_OFFSET 
void AnalogRangeControl::handle() {
  static long last = 0;
  long now = millis(); 
  if ( now - last < 150)
    return;

  last = now;
  int v = analogRead(pin);
  if (abs(v-182) < 30) {
    chs[1]->changeOffset(true); //dec
  } else if (abs(v-325) < 30) {
    chs[1]->changeOffset(false);
  } else if (abs(v-511) < 30) {
    chs[2]->changeOffset(true);//dec
  } else if (abs(v-682) < 30) {
    chs[2]->changeOffset(false);
  }  
}
#endif


