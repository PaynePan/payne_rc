#include "Ch.h"
#include "Config.h"
#include "payne_rc.h"

#ifdef STM32_BOARD   
#include "stm32/my_tone.h"
#endif

Ch::Ch() {
    increment = OFFSET_STEP;
    raw_value = 1500;
}

void Ch::changeOffset(bool dec) {
  if (offsetReverse)
    dec = !dec;
  
   if (dec) {
      if (data->offset - increment < -OFFSET_RANGE) {
          return;
      } else {
        data->offset -=increment;
      }
   } else {
      if ( data->offset + increment > OFFSET_RANGE) {
          return;
      } else {
        data->offset  += increment;
      }
   }
   
   if ( data->offset == 0 ) {
      tone(Tone_pin,1800,80);
   } else {
      tone(Tone_pin,1000,40);
   }
   settings.refreshUpdateTime();
}

uint16_t Ch::getValue() {
    uint16_t v = constrain(raw_value + data->offset, PWM_MIN, PWM_MAX);
    if (data->reversed)
      v = PWM_MIN  + (PWM_MAX - v);
    return v;
} 
