
#define ON_STATE 1

void ppm_handler(void) {   // check duration

   static boolean state = true;

  ppm_timer.setCount(0);   
   digitalWrite(PPM_pin, !ON_STATE);  //set the PPM signal pin to the default state (off)
  if (state) { //start pulse
    digitalWrite(PPM_pin, ON_STATE);
	 ppm_timer.setCompare(TIMER_CH1, PPM_PulseLen); 
    state = false;
  }
  else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
    digitalWrite(PPM_pin, !ON_STATE);
    state = true;

    if (cur_chan_numb >= CH_MAX) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;
	  ppm_timer.setCompare(TIMER_CH1, PPM_FrLen - calc_rest);
      calc_rest = 0;
    }
    else {
	  ppm_timer.setCompare(TIMER_CH1,ppm[cur_chan_numb] - PPM_PulseLen);
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }



}