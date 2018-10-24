#ifndef BtnControl_H_
#define BtnControl_H_
#include "Control.h"

class BtnControl  : public Control {
protected:
    virtual bool isTriggered();
public:

    virtual void setPin(uint8_t p);    
    virtual void handle() = 0;
};

class FuncBtnControl  : public BtnControl {
private:
   void (*func)();
public:
    virtual void handle();
    void setFunc(void (*f)())  { func = f; }
};

class OffsetBtnControl  : public BtnControl {
private:
   bool dec;
   void (*longPressFunc)();
public:
    OffsetBtnControl();
    virtual void handle();

    void setDec(bool b) { dec = b; }
    virtual bool isTriggered();
 };

class ReverseBtnControl  : public BtnControl {
public:
    virtual void handle();
 };
 
#endif 
