
    // adjust channel
    c = new AnalogControl();  c->setPin(A0);  c->setCh(chs[1]);  controlManager.add(c); // rh
    c = new AnalogControl();  c->setPin(A1);  c->setCh(chs[2]);  controlManager.add(c); // rv
    c = new AnalogControl();  c->setPin(A7);  c->setCh(chs[3]);  controlManager.add(c);  // lv
    c = new AnalogControl();  c->setPin(A6);  c->setCh(chs[4]);  controlManager.add(c);  // lh
#ifdef SWITCH_CH5_CH6    
    c = new AnalogControl();  c->setPin(A3);  c->setCh(chs[5]);  controlManager.add(c); // ch 5
    c = new AnalogControl();  c->setPin(A2);  c->setCh(chs[6]);  controlManager.add(c); // ch 6
#else
    c = new AnalogControl();  c->setPin(A2);  c->setCh(chs[5]);  controlManager.add(c); // ch 5
    c = new AnalogControl();  c->setPin(A3);  c->setCh(chs[6]);  controlManager.add(c); // ch 6
#endif

    c = new AnalogRangeControl();  c->setPin(A4);  c->setCh(0);  controlManager.add(c); 

#ifdef NANO
    c = new ReverseBtnControl();  c->setPin(4);  c->setCh(chs[1]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(5);  c->setCh(chs[2]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(6);  c->setCh(chs[3]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(7);  c->setCh(chs[4]);  controlManager.add(c);
#else
    c = new ReverseBtnControl();  c->setPin(5);  c->setCh(chs[1]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(6);  c->setCh(chs[2]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(7);  c->setCh(chs[3]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(8);  c->setCh(chs[4]);  controlManager.add(c);
#endif

