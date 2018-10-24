
#ifdef STM32_BOARD
#include "Config.h"

#include "Arduino.h"
#include <HardwareTimer.h>

#ifndef TONE_TIMER
#define TONE_TIMER 2
#endif

HardwareTimer my_tone_timer(TONE_TIMER);

bool my_tone_state = true;             // last pin state for toggling
short my_tone_pin = -1;                // pin for outputting sound
short my_tone_freq = 444;              // tone frequency (0=pause)
unsigned my_tone_micros = 500000/444;  // tone have wave time in usec
int my_tone_counts = 0;                // tone duration in units of half waves

// timer hander for tone with no duration specified, 
// will keep going until noTone() is called
void my_tone_handler_1(void) {
    my_tone_state = !my_tone_state;
    digitalWrite(my_tone_pin,my_tone_state);
}

// timer hander for tone with a specified duration,
// will stop automatically when duration time is up.
void my_tone_handler_2(void) {   // check duration
    if(my_tone_freq>0){
       my_tone_state = !my_tone_state;
       digitalWrite(my_tone_pin,my_tone_state);
    }
    if(!--my_tone_counts){
       my_tone_timer.pause();
       pinMode(my_tone_pin, INPUT);
    }
}

//  play a tone on given pin with given frequency and optional duration in msec
void my_tone(uint8_t pin, unsigned short freq, unsigned duration = 0) {
   my_tone_pin = pin;
   my_tone_freq = freq;
   my_tone_micros = 500000/(freq>0?freq:1000);
   my_tone_counts = 0;

   my_tone_timer.pause();

   if(freq >= 0){
      if(duration > 0)my_tone_counts = ((long)duration)*1000/my_tone_micros;
      pinMode(my_tone_pin, OUTPUT);

      // set timer to half period in microseconds
      my_tone_timer.setPeriod(my_tone_micros);

      // Set up an interrupt on channel 1
      my_tone_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
      my_tone_timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
      my_tone_timer.attachCompare1Interrupt(my_tone_counts?my_tone_handler_2:my_tone_handler_1);

      // Refresh the tone timer
      my_tone_timer.refresh();

      // Start the timer counting
      my_tone_timer.resume();
   } else
      pinMode(my_tone_pin, INPUT);
}

// disable tone on specified pin, if any
void my_noTone(uint8_t pin){
    my_tone(pin,-1);
}

#endif

