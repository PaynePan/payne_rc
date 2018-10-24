#ifndef Control_H_
#define Control_H_
#include <Arduino.h>
#include "Ch.h"

class Control {
  protected:
    uint8_t pin;
    Ch * ch;

  public:
    Control();
    virtual void setPin(uint8_t p);
    void setCh(Ch *c) { ch = c;} 
    virtual void handle() = 0;
};

#ifdef TEST_CHAN
class TestControl  : public Control {
public:
    TestControl();
    void handle();
};
#endif


class ValueRangeOffsetControl {
  protected:
    Ch * ch;

  public:
    ValueRangeControl();
    void setCh(Ch *c) { ch = c;} 
    virtual void handle(uint16_t v);
};

#define CTL_MAX   20
class ControlManager {
  public:
    Control *ctls[CTL_MAX];
    char ctl_count;

    void add(Control *ctl);
    ControlManager();
    void handle();
};

#endif
