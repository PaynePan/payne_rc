#ifndef AnalogControl_H_
#define AnalogControl_H_
#include "Control.h"

class AnalogControl  : public Control {
private:
    
public:
    virtual void setPin(uint8_t p);  
    virtual void handle();
};
#endif

class AnalogRangeControl  : public Control {
private:
    
public:
    virtual void handle();
}; 
