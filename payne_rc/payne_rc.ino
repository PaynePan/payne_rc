#include "Config.h"
#include "Ch.h"
#include "Control.h"
#include "BtnControl.h"
#include "AnalogControl.h"
#include "Settings.h"
#include "BatAlertControl.h"

#ifdef STM32_BOARD   
#include "stm32/my_tone.h"
#endif

#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000渭s)
#define PPM_PulseLen 300  //set the pulse length

#ifdef STM32_BOARD
	#include <arduino.h>
	#include <libmaple/usart.h>
	#include <libmaple/timer.h>
	#include <SPI.h>	
	#include <EEPROM.h>
  #include <HardwareTimer.h> 
  HardwareTimer ppm_timer(3);
  void ppm_handler(void);
#else
  #include <avr/eeprom.h>
#endif

#define MOVE_CHECK_INTERVAL (60000*3);
long lastMoveInterval = MOVE_CHECK_INTERVAL;
long lastMoveCheck = millis();

Ch *chs[CH_MAX+1];
uint16_t lastChRawValue[CH_MAX+1];
uint16_t ppm[CH_MAX];
ControlManager controlManager;
Settings settings;

void noMoveAlert();
void resetSettings();
void setup() {

#if defined(DEBUG) && !defined(USE_WIRELESS_UART)
  Serial.begin(57600);//for debug
  Serial.println("start");
#endif

#ifdef USE_WIRELESS_UART
  Serial.begin(57600);
#endif
  // init channel
  for(int i=1;i<=CH_MAX;i++) {
    chs[i] = new Ch();
  }

#ifdef CH1_OFFSET_REVERSE
  chs[1]->enalbeOffsetReverse();
#endif
  
#ifdef CH4_OFFSET_REVERSE
  chs[4]->enalbeOffsetReverse();
#endif
  
  for(int i=0;i<CH_MAX;i++) {
    ppm[i] = 1500;
    if ( i == 2)
    ppm[i] = 1000;
  }

  settings.load();
  for(int i=1;i<=CH_MAX;i++) {
    chs[i]->bind(settings.getChData(i));
  }

  Control *c;

#ifdef STM32_BOARD
  #include "pins_stm32.h"
#else
  #include "pins_atmel.h"
#endif

#ifndef STM32_BOARD
   c = new FuncBtnControl();  c->setPin(2); controlManager.add(c);  ((FuncBtnControl *)c)->setFunc(resetSettings); 
#endif   

#ifdef TEST_CHAN
    c = new TestControl();  c->setPin(0);  c->setCh(chs[7]);  controlManager.add(c); // ch test
#endif  

#ifdef BAT_ALART
    c = new BatAlertControl(); controlManager.add(c);
#endif      

#ifdef STM32_BOARD
  #include "stm32/ppm_init.h"
#else
  #include "atmel/ppm_init.h"
#endif
}

int STD_LOOP_TIME = 10;

int lastLoopUsefulTime;
unsigned long loopStartTime = 0;

void loop() {
  controlManager.handle();
  noInterrupts();
  for(int i=0;i<CH_MAX;i++) {
    ppm[i] = chs[i+1]->getValue();
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
  static uint8_t xuart_count = 0;
  if ( ++xuart_count == 4) {  //since xuart need more that 30ms to resend message
    xuart_count = 0;
    for(int i=0;i<CH_MAX;i++) {
            Serial.print("#"); 
            Serial.print(i+1);
            Serial.print("P");
            Serial.print( chs[i+1]->getValue());
    }
    Serial.println();
  }
#endif
  
  // *********************** loop timing control **************************
  lastLoopUsefulTime = millis()-loopStartTime;
  if(lastLoopUsefulTime<=STD_LOOP_TIME)        delay(STD_LOOP_TIME-lastLoopUsefulTime);
  loopStartTime = millis(); 
}

#ifdef NO_MOVE_ALERT
void noMoveAlert() {
   if (lastChRawValue[1] == 0) { //init value
    for(int i=1;i<=CH_MAX;i++) {
        lastChRawValue[i] = chs[i]->getRawValue();
    }
  } else if ( millis() -  lastMoveCheck > lastMoveInterval) {
    lastMoveCheck = millis();
    boolean moveFlag = false;
    for(int i=1;i<=CH_MAX;i++) {
       if ( abs(lastChRawValue[i] - chs[i]->getRawValue()) > 100) {
          moveFlag = true;
          break;
       }
    }
   if (  moveFlag == false) {
#ifdef STM32_BOARD  
      my_tone(Tone_pin,1400,500);
#else
      tone(Tone_pin,1400,500);
#endif
      lastMoveInterval = 2000;// reduce alert interval to 2 sec
    } else {
       lastMoveInterval = MOVE_CHECK_INTERVAL;// restore to 60 sec
       for(int i=1;i<=CH_MAX;i++) {
         lastChRawValue[i]  =  chs[i]->getRawValue();    
       }
    }
  }
}
#endif

void resetSettings() {
  settings.loadDefaults();
}

#ifdef STM32_BOARD
  #include "stm32/ppm_exec.h"
#else
  #include "atmel/ppm_exec.h"
#endif




