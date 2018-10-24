#include "Config.h"
#include "BatAlertControl.h"

#define BAT_FACTOR  2
#define BAT_BASE  3.3
#define BAT_MIN  3.6

void BatAlertControl::handle() {
#ifdef BAT_ALART  
  static long last = 0;
  long now = millis(); 
  if ( now - last > 1000*10) {  //every 10 seconds check
       float v = (float)(analogRead(A5) * BAT_FACTOR)/1023 * BAT_BASE;
       if (v < BAT_MIN)  { // 1.1 factor only for 1S
        tone(Tone_pin,1600,800);
      }
      last = now;      
  }
#endif  
}
