#pragma once
#include <Arduino.h>

/////////////////////////////////////////////////////////////////////////////////////////////
/// The storage module will save the persistent state every second in RAM and
/// every minute in EEPROM, this way EEPROM lifetime will be about 9 years continous
/// (so no need to use the EEPROM on the RTC module)
/// additionally this means swapping out either the RTC or the Arduino without data loss
/////////////////////////////////////////////////////////////////////////////////////////////

const int RAM_Offs=8; //DS1307 ram has 64B but 7 are used by the clock
const int RAM_Size=64-RAM_Offs;

const int ROM_Offs=0; //reserved for settings
const int ROM_Size=(1+E2END)-ROM_Offs; //EEPROM size

struct PersState {
  long distAll;     //in cm
  long timeAll;     //in s
  long distTrip;    //in cm
  long timeTrip;    //in s
  long crc;

  long CalcCrc();
  bool Validate() {return crc==CalcCrc();};
  void SaveCrc()  {crc=CalcCrc();};
};

extern PersState curState;

void Storage_Init();
void Storage_Tick();  //should be called at least once a second

/////////////////////////////////////////////////////////////////////////////////////////////
void ClearRAM();
void ClearROM();
void DumpRAM();
void DumpROM();
unsigned long calc_crc(byte* data,int len);

