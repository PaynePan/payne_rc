#define ON_STATE 1
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;
  static boolean state = true;
  byte scale = 2;
  digitalWrite(PPM_pin, !ON_STATE);  //set the PPM signal pin to the default state (off)

  if (state) { //start pulse
    digitalWrite(PPM_pin, ON_STATE);
    OCR1A = PPM_PulseLen * scale;
    state = false;
  }
  else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
    digitalWrite(PPM_pin, !ON_STATE);
    state = true;

    if (cur_chan_numb >= CHAN_SIZE) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;//
      OCR1A = (PPM_FrLen - calc_rest) * scale;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppms[cur_chan_numb] - PPM_PulseLen) * scale;
      calc_rest = calc_rest + ppms[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}