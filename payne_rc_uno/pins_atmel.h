
    c = new AnalogControl();  c->setPin(A3);  c->setCh(chs[1]);  controlManager.add(c); // rh
    c = new AnalogControl();  c->setPin(A2);  c->setCh(chs[2]);  controlManager.add(c); // rv
    c = new AnalogControl();  c->setPin(A1);  c->setCh(chs[3]);  controlManager.add(c);  // lv
    c = new AnalogControl();  c->setPin(A0);  c->setCh(chs[4]);  controlManager.add(c);  // lh
    
    c = new ReverseBtnControl();  c->setPin(11);  c->setCh(chs[1]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(10);  c->setCh(chs[2]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(9);  c->setCh(chs[3]);  controlManager.add(c);
    c = new ReverseBtnControl();  c->setPin(6);  c->setCh(chs[4]);  controlManager.add(c);

    c = new OffsetBtnControl();  c->setPin(8);  c->setCh(chs[1]);  controlManager.add(c); ((OffsetBtnControl *)c)->setDec(true);  
    c = new OffsetBtnControl();  c->setPin(7);  c->setCh(chs[1]);  controlManager.add(c);
    


