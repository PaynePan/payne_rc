#include "Settings.h"
#include "payne_rc.h"

#include <arduino.h>
#ifdef STM32_BOARD
  #include <EEPROM.h>
#endif

Settings::Settings() {
  updateTime = saveTime = millis();
}

static uint8_t calculate_sum(uint8_t *cb , uint8_t siz) {
    uint8_t sum=0x55;  // checksum init
    while(--siz) sum += *cb++;  // calculate checksum (without checksum byte)
    return sum;
}

void Settings::loadDefaults() {
    for(int i=1;i<=CH_MAX;i++) {
      data.ch[i].offset = 0;
      data.ch[i].reversed = false;
  }
}
#ifdef STM32_BOARD

static void eeprom_read_block (void *__dst, uint16_t __src, size_t __n) {
   uint8_t * dst = (uint8_t *)__dst;
    for ( int i =0; i<__n; i++) {
      uint16_t data;
      data = EEPROM.read(__src+i);
      dst[i] = data;
    }
}

void eeprom_write_block (const void *__src, uint16_t __dst, size_t __n) {
    uint8_t * src = (uint8_t *)__src;
    for ( int i =0; i<__n; i++) {
      uint16_t data;
      data = src[i] & 0x00FF;
      EEPROM.write(__dst+i,data);
    }
}

#endif

bool Settings::load() {
  eeprom_read_block((void*)&data, 0, sizeof(data));
  if(calculate_sum((uint8_t*)&data, sizeof(data)) != data.checksum) {
      loadDefaults();                 // force load defaults
      return false;
  }
 return true;
}

void Settings::store() {
  data.checksum = calculate_sum((uint8_t*)&data, sizeof(data));
  eeprom_write_block((void*)&data, 0, sizeof(data));
}

boolean Settings::saveIfUpdate() {
  if ( updateTime != saveTime  &&  (millis() - updateTime) > 2000) {
      store();
      saveTime = updateTime; 
      return true;
  }
  return false;
}

