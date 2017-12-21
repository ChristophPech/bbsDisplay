#include "comm.h"
#include "gfx.h"
#include "clock.h"
#include "ctrl.h"

bool bLightOn=false;
int recvTimeOut=0;
int recvLastBytes=0;

void SendPAS()
{
  uint8_t i=0;
  switch(iPas)
  {
    case -1: i=0x06; break;
    case 0: i=0x00; break;
    case 1: i=0x01; break;
    case 2: i=0x0b; break;
    case 3: i=0x0c; break;
    case 4: i=0x0d; break;
    case 5: i=0x02; break;
    case 6: i=0x15; break;
    case 7: i=0x16; break;
    case 8: i=0x17; break;
    case 9: i=0x03; break;
  }
  
  Serial1.write(uint8_t(0x16));
  Serial1.write(uint8_t(0x0b));
  Serial1.write(uint8_t(i));
  Serial1.write(uint8_t(i+0x21));
}

void SendLight()
{
  Serial1.write(uint8_t(0x16));
  Serial1.write(uint8_t(0x1a));
  Serial1.write(uint8_t(bLightOn?0xf0:0xf1));
}
void SendCfg()
{
  uint8_t v0=Config_Road_01;
  uint8_t v1=Config_Road_02;
  if(!modeRoad) {
    v0=Config_OffR_01;
    v1=Config_OffR_02;
  }
  
  Serial1.write(uint8_t(0x16));
  Serial1.write(uint8_t(0x1f));
  Serial1.write(uint8_t(v0));
  Serial1.write(uint8_t(v1));
  Serial1.write(uint8_t(v0+v1+0x35));
}

////////////////////////////////////////////////////////////////
int iCurRequest=0;

void ReqSpeed()
{
  Serial1.write(uint8_t(0x11));
  Serial1.write(uint8_t(0x20));
  iCurRequest=1;
  recvLastBytes=-1;
}
void ReqStatus()
{
  Serial1.write(uint8_t(0x11));
  Serial1.write(uint8_t(0x08));
  iCurRequest=2;
  recvLastBytes=-1;
}
void ReqPower()
{
  Serial1.write(uint8_t(0x11));
  Serial1.write(uint8_t(0x0a));
  iCurRequest=3;
  recvLastBytes=-1;
}
void ReqSomething()
{
  //no, clue. omit.
  Serial1.write(uint8_t(0x11));
  Serial1.write(uint8_t(0x31));
  iCurRequest=4;
  recvLastBytes=-1;
}
void ReqBattery()
{
  //we don't care about the bogus prediction and make our own, see gfx.h
  Serial1.write(uint8_t(0x11));
  Serial1.write(uint8_t(0x11));
  iCurRequest=5;
  recvLastBytes=-1;
}

////////////////////////////////////////////////////////////////////
void RecvSpeed()
{
  int r=Serial1.available();
  if(r<3) return;
  uint8_t b0=Serial1.read();
  uint8_t b1=Serial1.read();
  uint8_t b2=Serial1.read();

  long i=(int(b0)<<8)|b1;
  //b2=iSpeed+20 (checksum)
  
#ifndef METRIC
  i=(i*1000)/621; //murrican conversion
#endif

  iSpeed=(Tire_Circum_mm*i*6)/100;

  iCurRequest=-1;
}

void RecvStatus()
{
  int r=Serial1.available();
  if(r<1) return;
  uint8_t b0=Serial1.read();

  iErrCode=b0;

  iCurRequest=-1;
}

void RecvPower()
{
  int r=Serial1.available();
  if(r<2) return;
  uint8_t b0=Serial1.read();
  uint8_t b1=Serial1.read();

  iPower=b0;
  //b1=b0 (checksum)

  iCurRequest=-1;
}

void RecvSomething()
{
  int r=Serial1.available();
  if(r<2) return;
  uint8_t b0=Serial1.read();
  uint8_t b1=Serial1.read();

  //0x30 0x30

  iCurRequest=-1;
}

void RecvBattery()
{
  int r=Serial1.available();
  if(r<2) return;
  uint8_t b0=Serial1.read();
  uint8_t b1=Serial1.read();

  //b0 is battery % from 0 to 100, don't believe it though

  iCurRequest=-1;
}

////////////////////////////////////////////////////////////////////

unsigned int cmdCycle=-1;
void Comm_Tick()
{
  int w=Serial1.availableForWrite();
  if(w<63) return;
  //Serial.println(w);
  
    /*int r=Serial1.available();
    if(r>0) {
      uint8_t b0=Serial1.read();
      Serial.print(p2dig(b0,HEX));
      Serial.print(" ");
    }*/

  if(iCurRequest)
  {
    if(recvLastBytes==-1) {
      recvLastBytes=0;
      recvTimeOut=30;
      //Serial.print("\nto");
      //Serial.print(iCurRequest);
    } else {
      int r=Serial1.available();
      if(r!=recvLastBytes) {
        recvTimeOut=15;
        recvLastBytes=r;
        //Serial.print("+");
      } else {
        recvTimeOut--;
        //Serial.print("-");
        if(recvTimeOut<=0) {
          //Serial.print("end");
          iCurRequest=0; //timeout
        }
        return;
      }
    }
    
    switch(iCurRequest)
    {
      case 1:RecvSpeed(); return;
      case 2:RecvStatus(); return;
      case 3:RecvPower(); return;
      case 4:RecvSomething(); return;
      case 5:RecvBattery(); return;
      case -1: 
        while(Serial1.available()) {
          char a=Serial1.read();
          Serial.print(a);
        };
        iCurRequest=0; return; //flush read buffer
    }
  }
  
  
  if(w<63) return;
  //Serial.println(w);
  cmdCycle++;

  if(cmdCycle==10) {ReqSpeed();return;};
  if(cmdCycle==20) {SendPAS();return;};

  if(cmdCycle==40) {ReqStatus();return;};
  if(cmdCycle==50) {SendLight();return;};

  if(cmdCycle==70) {ReqPower();return;};
  if(cmdCycle==80) {SendCfg();return;};

  if(cmdCycle>=90) {cmdCycle=-1;return;};
}

void Comm_Init()
{
    //pinMode(0, INPUT_PULLUP);
    Serial1.begin(1200);
}

