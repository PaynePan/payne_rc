#ifndef Ch_H_
#define Ch_H_
#include <Arduino.h>

#include "types.h"

#define PWM_MIN  1000
#define PWM_MAX  2000
#define PWM_MID  1500
#define OFFSET_RANGE  100
#define OFFSET_STEP   10

class Ch {
  private:
    char increment;
    uint16_t raw_value;
    boolean offsetReverse  = false;
    Ch_t *data;    

  public:
    Ch();
  
    void changeOffset(bool dec);
    void enalbeOffsetReverse() {  offsetReverse = true; }
    void setReversed(bool b) { data->reversed = b; }
    void setRawValue(uint16_t r) { raw_value = r; }
    uint16_t getRawValue() { return raw_value; }
    uint16_t getValue();     
    
    uint16_t bind(Ch_t *ch_t){ data = ch_t; }

};
#endif

