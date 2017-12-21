#include "storage.h"
#include "clock.h"
#include <EEPROM.h>

const unsigned long crc_table[16] PROGMEM = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

unsigned long calc_crc(byte* data,int len)
{
  unsigned long crc = ~0L;

  for (int index = 0 ; index < len  ; ++index) {
    crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

long PersState::CalcCrc()
{
  return calc_crc((byte*)this,sizeof(*this)-sizeof(long));
}


void ClearRAM()
{
    for(int8_t i=0;i<RAM_Size;i++) {
      RTC.writeRAM(RAM_Offs+i,(uint8_t*)&i,1);
    }
}
void ClearROM()
{
    for(int8_t i=0;i<ROM_Size;i++) {
      EEPROM.write(ROM_Offs+i,0);
    }
}

void DumpRAM()
{
  Serial.print("DumpRAM:\n");
  uint8_t buf[16]={0};
  for(int i=0;i<4;i++)
  {
    RTC.readRAM(i*16,buf,16);
    for(int j=0;j<16;j++) {
      Serial.print(p2dig(buf[j],HEX));
      Serial.print(" ");
    }
    Serial.print("\n");
  }
}
void DumpROM()
{
  Serial.print("DumpROM:\n");
  uint8_t buf[16]={0};
  for(int i=0;i<64;i++)
  {
    EEPROM.get(i*16,buf);
    for(int j=0;j<16;j++) {
      Serial.print(p2dig(buf[j],HEX));
      Serial.print(" ");
    }
    Serial.print("\n");
  }
}

const int RAM_Chunks=RAM_Size/sizeof(PersState);
const int ROM_Chunks=ROM_Size/sizeof(PersState);

int curRAMIdx=-1;
int curROMIdx=-1;
PersState curState;
int lastSecond=0;
int saveROMCounter=0;

/*
void SaveRAM()
{
  curRAMIdx++;
  if(curRAMIdx>=RAM_Chunks) curRAMIdx=0;

  curState.SaveCrc();
  RTC.writeRAM(RAM_Offs+curRAMIdx*sizeof(curState),(uint8_t*)&curState,sizeof(curState));
}*/

void SaveROM() 
{
  curROMIdx++;
  if(curROMIdx>=ROM_Chunks) curROMIdx=0;

  //Serial.print("SaveROM:");
  //Serial.println(curROMIdx);

  curState.SaveCrc();
  EEPROM.put(ROM_Offs+curROMIdx*sizeof(curState),curState);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Storage_Tick()
{
  if(lastSecond==RTC.s) return;
  lastSecond=RTC.s;
  
  curState.timeAll++;
  curState.timeTrip++;
  
  saveROMCounter++;
  if(saveROMCounter>=10)
  {
    saveROMCounter=0;
    SaveROM();
    return;
  }
  //SaveRAM();
}

void Storage_Init()
{
  //curState.timeAll=0;
  /*for(int i=0;i<RAM_Chunks;i++) 
  {
    PersState s;
    RTC.readRAM(RAM_Offs+i*sizeof(s),(uint8_t*)&s,sizeof(s));
    if(s.Validate()&&s.timeAll>=curState.timeAll)
    {
      curState=s;
      curRAMIdx=i;

      //Serial.print("LoadRAM:");
      //Serial.println(curRAMIdx);
    }
  }*/

  //curState.timeAll=0;
  for(int i=0;i<ROM_Chunks;i++) 
  {
    PersState s;
    EEPROM.get(ROM_Offs+i*sizeof(s),s);
    if(s.Validate()&&s.timeAll>=curState.timeAll)
    {
      curState=s;
      curROMIdx=i;

      //Serial.print("LoadROM:");
      //Serial.println(curROMIdx);
    
    }
  }

  lastSecond=RTC.s;
}

