
#define CH_MAX   7

#define USE_WIRELESS_UART

//#define CH1_OFFSET_REVERSE
//#define CH4_OFFSET_REVERSE
//#define NO_MOVE_ALERT

//#define DEBUG
#define TEST_CHAN   // comment this, if you won't need use ch7 as test chan which will move continually

/* ===================== define for stm32 board ==================  */
#ifdef STM32_BOARD

#define Tone_pin   PA8
#define PPM_pin    PA11


/* ===================== define for atmel board ==================  */
#else  // atmel board

#define Tone_pin     3
#define PPM_pin     13

//#define BAT_ALART  //now only for atmel and for 1S Lipo 

#endif


