#ifndef PAYNE_RC_H_
#define PAYNE_RC_H_
#include "types.h"
#include "Ch.h" 

#include "Settings.h"

extern Settings settings;

extern long lastMoveTime;

extern Ch *chs[];


#define Red_LED_ON      digitalWrite(Red_LED_pin,HIGH);
#define Red_LED_OFF     digitalWrite(Red_LED_pin,LOW);

#endif
