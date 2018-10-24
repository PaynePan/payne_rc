#include "Control.h"
#include "Config.h"

Control::Control() {
}

void Control::setPin(uint8_t p) {
  pin =p; 
}

ControlManager::ControlManager() {
    char ctl_count = 0;
}

void ControlManager::add(Control *ctl){
    ctls[ctl_count++] = ctl;
}

void ControlManager::handle(){
  for( int i=0; i<ctl_count; i++)
    ctls[i]->handle();
}

#ifdef TEST_CHAN
TestControl::TestControl() {
}

void TestControl::handle() {
  static long last = 0;
  uint16_t v;
  long now = millis(); 
  if ( now - last > 10) {
    v = ch->getRawValue() + 20;
    if (v >= PWM_MAX) {
       v = PWM_MIN;
    }
    ch->setRawValue(v);
    last = now;
  }
}
#endif


