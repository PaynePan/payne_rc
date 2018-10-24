
    // adjust channel
    c = new AnalogControl();  c->setPin(PA1);  c->setCh(chs[1]);  controlManager.add(c); // rh
    c = new AnalogControl();  c->setPin(PA2);  c->setCh(chs[2]);  controlManager.add(c); // rv
    c = new AnalogControl();  c->setPin(PA3);  c->setCh(chs[3]);  controlManager.add(c);  // lv
    c = new AnalogControl();  c->setPin(PA4);  c->setCh(chs[4]);  controlManager.add(c);  // lh
    c = new AnalogControl();  c->setPin(PA5);  c->setCh(chs[5]);  controlManager.add(c); // ch 5
    c = new AnalogControl();  c->setPin(PA6);  c->setCh(chs[6]);  controlManager.add(c); // ch 6

    c = new ReverseBtnControl();  c->setPin(PB12);  c->setCh(chs[1]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(PB13);  c->setCh(chs[2]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(PB14);  c->setCh(chs[3]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(PB15);  c->setCh(chs[4]);  controlManager.add(c);

    
    c = new OffsetBtnControl();  c->setPin(PB6);   c->setCh(chs[1]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);  
    c = new OffsetBtnControl();  c->setPin(PB7);  c->setCh(chs[1]);  controlManager.add(c);
    c = new OffsetBtnControl();  c->setPin(PB8);  c->setCh(chs[2]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);  
    c = new OffsetBtnControl();  c->setPin(PB9);  c->setCh(chs[2]);  controlManager.add(c);


