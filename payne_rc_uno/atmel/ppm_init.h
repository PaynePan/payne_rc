pinMode(PPM_pin, OUTPUT);

byte scale = 2;

  // set ppm out timer
  OCR1A = 50 * scale;
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TCCR1A |= _BV(COM1A0);

#if defined(__AVR_ATmega8__)
  // your m8 timer register code
  TIMSK |= (1 << OCIE1A); // enable timer compare interrupt
#elif defined (__AVR_ATmega328P__)||(__AVR_ATmega88__)||(__AVR_ATmega168__)
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  // your m328 timer register code
#else
#error // only m8 & m328 are supported !
#endif
  sei();
