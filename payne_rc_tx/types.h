#ifndef TYPES_H_
#define TYPES_H_

#include "Config.h"
#include <arduino.h>

typedef struct {
    int16_t offset;
    boolean reversed;
} Ch_t;

typedef struct {
   Ch_t ch[CHAN_SIZE+1];	
   uint8_t checksum;   
} Settings_t;

#endif
