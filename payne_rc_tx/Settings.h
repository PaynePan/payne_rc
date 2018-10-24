#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "types.h"
class Settings {
  private:
    long updateTime;
    long saveTime;

    Settings_t data;

	public:
    Settings();
		bool load();
    void loadDefaults();
    
		void store();
    Ch_t * getChData(byte index) { return &data.ch[index]; }

    void refreshUpdateTime() {  updateTime = millis(); }
    boolean saveIfUpdate();

};

#endif


