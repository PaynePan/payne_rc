#include "Config.h"
#include "Ch.h"
#include "Control.h"
#include "BtnControl.h"
#include "AnalogControl.h"
#include "Settings.h"
#include "BatAlertControl.h"

#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000μs)
#define PPM_PulseLen 300  //set the pulse length

#include <avr/eeprom.h>


#define MOVE_CHECK_INTERVAL (60000*3);
long lastMoveInterval = MOVE_CHECK_INTERVAL;
long lastMoveCheck = millis();

Ch *chs[CHAN_SIZE+1];
uint16_t lastChRawValue[CHAN_SIZE+1];
uint16_t ppms[CHAN_SIZE];
ControlManager controlManager;
Settings settings;

void noMoveAlert();
void resetSettings();

#include "payne_protocol.h"
//====================== setup ========================
void setup() {

#if defined(DEBUG) && !defined(USE_WIRELESS_UART)
  Serial.begin(57600);//for debug
  Serial.println("start");
#endif

#ifdef USE_WIRELESS_UART
  Serial.begin(57600);
#endif
  // init channel
  for(int i=1;i<=CHAN_SIZE;i++) {
    chs[i] = new Ch();
  }

#ifdef CH1_OFFSET_REVERSE
  chs[1]->enalbeOffsetReverse();
#endif
  
#ifdef CH4_OFFSET_REVERSE
  chs[4]->enalbeOffsetReverse();
#endif
  
  for(int i=0;i<CHAN_SIZE;i++) {
    ppms[i] = 1500;
    if ( i == 2)
    ppms[i] = 1000;
  }

  settings.load();
  for(int i=1;i<=CHAN_SIZE;i++) {
    chs[i]->bind(settings.getChData(i));
  }

  Control *c;
  
  #include "pins_atmel.h"

   //c = new FuncBtnControl();  c->setPin(2); controlManager.add(c);  ((FuncBtnControl *)c)->setFunc(resetSettings); 
 

#ifdef TEST_CHAN
    c = new TestControl();  c->setPin(0);  c->setCh(chs[7]);  controlManager.add(c); // ch test
#endif  

#ifdef BAT_ALART
    c = new BatAlertControl(); controlManager.add(c);
#endif      

  #include "atmel/ppm_init.h"
#ifndef USE_WIRELESS_UART
  tx_init(1); // auto bind
#endif  
}

int STD_LOOP_TIME = 30;
int lastLoopUsefulTime;
unsigned long loopStartTime = 0;

//====================== loop ========================
void loop() {
  controlManager.handle();
  noInterrupts();
  for(int i=0;i<CHAN_SIZE;i++) {
    ppms[i] = chs[i+1]->getValue();
  }
  interrupts();

  if (settings.saveIfUpdate())  {
      lastMoveCheck = millis();
      lastMoveInterval = MOVE_CHECK_INTERVAL;
  }

#ifdef NO_MOVE_ALERT  
  noMoveAlert();
#endif 

#ifdef USE_WIRELESS_UART
  static uint8_t uart_count = 0;
  if (++uart_count > 10) {
    uart_count;
    for(int i=0;i<CHAN_SIZE;i++) {
            Serial.print("#"); 
            Serial.print(i+1);
            Serial.print("P");
            Serial.print( chs[i+1]->getValue());
    }
   Serial.println();
  }
#endif

  
  // *********************** loop timing control **************************
  lastLoopUsefulTime = micros()/100 -loopStartTime;
  if(lastLoopUsefulTime< STD_LOOP_TIME)        delayMicroseconds((STD_LOOP_TIME-lastLoopUsefulTime)*142);
  loopStartTime = micros()/100;

#ifndef USE_WIRELESS_UART
  tx_run();
#endif

}

#ifdef NO_MOVE_ALERT
void noMoveAlert() {
   if (lastChRawValue[1] == 0) { //init value
    for(int i=1;i<=CHAN_SIZE;i++) {
        lastChRawValue[i] = chs[i]->getRawValue();
    }
  } else if ( millis() -  lastMoveCheck > lastMoveInterval) {
    lastMoveCheck = millis();
    boolean moveFlag = false;
    for(int i=1;i<=CHAN_SIZE;i++) {
       if ( abs(lastChRawValue[i] - chs[i]->getRawValue()) > 100) {
          moveFlag = true;
          break;
       }
    }
   if (  moveFlag == false) {
      tone(Tone_pin,1400,500);
      lastMoveInterval = 2000;// reduce alert interval to 2 sec
    } else {
       lastMoveInterval = MOVE_CHECK_INTERVAL;// restore to 60 sec
       for(int i=1;i<=CHAN_SIZE;i++) {
         lastChRawValue[i]  =  chs[i]->getRawValue();    
       }
    }
  }
}
#endif

void resetSettings() {
  settings.loadDefaults();
}

#include "atmel/ppm_exec.h"

