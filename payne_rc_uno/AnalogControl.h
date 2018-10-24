#ifndef AnalogControl_H_
#define AnalogControl_H_
#include "Control.h"

class AnalogControl  : public Control {
private:
    
public:
    virtual void setPin(uint8_t p);  
    virtual void handle();
};

#ifdef ANALOG_OFFSET
class AnalogRangeControl  : public Control {
private:
    
public:
    virtual void handle();
};
#endif 
#endif 

