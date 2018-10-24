      pinMode(PPM_pin,OUTPUT);
	  
	  ppm_timer.pause();
	  
	  TIMER3_BASE->PSC = 71;			//72-1;for 72 MHZ
   	  TIMER3_BASE->ARR = 0xFFFF;
	  
      ppm_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
      ppm_timer.setCompare(TIMER_CH1, 2000); 
      ppm_timer.attachCompare1Interrupt(ppm_handler);
      ppm_timer.refresh();
      ppm_timer.resume();
