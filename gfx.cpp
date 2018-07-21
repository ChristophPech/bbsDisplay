#include "gfx.h"
#include "clock.h"
#include "storage.h"
#include "ctrl.h"
#include <U8glib.h>

// Width: 16, Height: 7
const unsigned char imgBatt[] PROGMEM = {
  0xFF, 0x7F, 0x01, 0x40, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x01, 0x40, 
  0xFF, 0x7F, };

U8GLIB_SSD1306_128X64 u8g( 15, 16, 10, 9, 8 );
//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g(U8G2_R0, /* clock=*/ 15, /* data=*/ 16, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

long iSpeed=350;
int iPower=0;
int iPas=0;
long iBatVolt=40000;
int iErrCode=0;
int iThrottle=0;

long iBatVoltSmooth=iBatVolt;
int iBatLevel=0;
long iSpeed_copy=0;
long iPower_copy=0;
int iPas_copy=0;

void gfx_draw(void) {
  char buffer[32];
  int iW,iH,iM,iS;

  ////////////////////////////////////////////////////////////////////
  //u8g.setFont(u8g2_font_blipfest_07_t);
  u8g.setFont(u8g_font_04b_03r);
  sprintf(buffer, "%02d.%02d.%02d", RTC.dd,RTC.mm,RTC.yyyy%100);
  u8g.drawStr( 1, 10, buffer);

  iS=(millis()/1000)%60;
  sprintf(buffer, "%02d:%02d:%02d",RTC.h,RTC.m, RTC.s);
  //iW=u8g.getStrPixelWidth(buffer);
  iW=4*8+3;
  u8g.drawStr( 128-iW, 10, buffer);

  u8g.drawStr( 106, 44, Label_Speed);
  if(!modeRoad) u8g.drawStr( 106, 54, "W");

  long dm=curState.distTrip/10;
  sprintf(buffer, "%ld.%03ld %s", dm/1000,dm%1000,Label_Dist);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 1, 49, buffer);

  iH=curState.timeTrip/3600;
  iM=(curState.timeTrip%3600)/60;
  iS=(curState.timeTrip%3600)%60;
  if(iH>0) {
    sprintf(buffer, "%d:%02d:%02d", iH,iM,iS);
  } else {
    sprintf(buffer, "%d:%02d", iM,iS);
  }
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 1, 42, buffer);

  sprintf(buffer, "%d", (iPower_copy*iBatVoltSmooth)/2000);
  iW=u8g.getStrWidth(buffer);
  if(!modeRoad) u8g.drawStr( 100-iW, 54, buffer);

  int8_t iBatY=14;
  u8g.drawXBMP( 1, iBatY, 16, 7, imgBatt);
  int v=(iBatVoltSmooth+50)/100;
  sprintf(buffer, "%d.%dV", v/10,v%10);
  u8g.drawStr( 19, iBatY+6, buffer);

  if(iBatLevel==0) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+2,iBatY+2,11,3);
    u8g.setColorIndex(1);
  } else if(iBatLevel==1) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+5,iBatY+2,8,3);
    u8g.setColorIndex(1);
  } else if(iBatLevel==2) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+8,iBatY+2,5,3);
    u8g.setColorIndex(1);
  } else if(iBatLevel==3) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+11,iBatY+2,2,3);
    u8g.setColorIndex(1);
  }

  const char* errMsg=NULL;
  switch(iErrCode) {
    case 0x04 : errMsg="04 Thrt high"; break;
    case 0x05 : errMsg="05 Thrt err"; break;
    case 0x06 : errMsg="06 Volt low"; break;
    case 0x07 : errMsg="07 Volt high"; break;
    case 0x08 : errMsg="08 Hall sensor"; break;
    case 0x09 : errMsg="09 Phase line"; break;
    case 0x10 : errMsg="10 Ctrl temp"; break;
    case 0x11 : errMsg="11 Motor temp"; break;
    case 0x12 : errMsg="12 Current sensor"; break;
    case 0x13 : errMsg="13 Bat temp sensor"; break;
    case 0x14 : errMsg="14 Mot temp sensor"; break;
    case 0x15 : errMsg="15 Ctrl temp sensor"; break;
    case 0x21 : errMsg="21 Speed sensor"; break;
    case 0x22 : errMsg="22 BMS comm"; break;
    case 0x23 : errMsg="23 Head light"; break;
    case 0x24 : errMsg="24 Head light sensor"; break;
    case 0x25 : errMsg="25 TSensor torque"; break;
    case 0x26 : errMsg="26 TSensor speed"; break;
    case 0x30 : errMsg="30 Communication"; break;
  }
  if(errMsg)
  {
    iW=u8g.getStrWidth(errMsg);
    u8g.drawStr( 128-iW, 59, errMsg);
  }
  else
  {
    sprintf(buffer, "%ld %s", curState.distAll/10000,Label_Dist);
    iW=u8g.getStrWidth(buffer);
    u8g.drawStr( 1, 59, buffer);
  }

  ////////////////////////////////////////////////////////////////////
  u8g.setFont(u8g_font_8x13Br);
  const char* pas[]={"push","off","eco","normal","turbo",">",">>",">>>",">>>>",">>>>>",">>>>>>"};
  sprintf(buffer, "%s", pas[iPas_copy+1]);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 64-iW/2, 12, buffer);


  ////////////////////////////////////////////////////////////////////
  //u8g.setFont(u8g2_font_courB24_tn);
  u8g.setFont(u8g_font_helvB24n);

  iS=iSpeed_copy/1000;
  sprintf(buffer, "%d", iS%10);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 98-iW, 44, buffer);
  if(iSpeed_copy>10) {
    sprintf(buffer, "%d", iS/10);
    iW=u8g.getStrWidth(buffer);
    u8g.drawStr( 78-iW, 44, buffer);
  }

  if(!modeRoad) {
    iW=(iThrottle/500);
    u8g.drawHLine(128-iW,59,iW);
  }
}

void Gfx_Init()
{
    u8g.begin();
    u8g.setColorIndex(1); 
    pinMode(8, OUTPUT);
}

void Gfx_Render()
{
  iBatVoltSmooth=(iBatVoltSmooth*80)/100;
  iBatVoltSmooth+=(iBatVolt/100)*20;

  int iBatLevelUp=4;
  if(iBatVoltSmooth<BattPrc[0]+BattPrcHeur) {
    iBatLevelUp=0;
  } else if(iBatVoltSmooth<BattPrc[1]+BattPrcHeur) {
    iBatLevelUp=1;
  } else if(iBatVoltSmooth<BattPrc[2]+BattPrcHeur) {
    iBatLevelUp=2;
  } else if(iBatVoltSmooth<BattPrc[3]+BattPrcHeur) {
    iBatLevelUp=3;
  }

  int iBatLevelDn=4;
  if(iBatVoltSmooth<BattPrc[0]) {
    iBatLevelDn=0;
  } else if(iBatVoltSmooth<BattPrc[1]) {
    iBatLevelDn=1;
  } else if(iBatVoltSmooth<BattPrc[2]) {
    iBatLevelDn=2;
  } else if(iBatVoltSmooth<BattPrc[3]) {
    iBatLevelDn=3;
  }

  if(iBatLevelUp>iBatLevel)
  {
    iBatLevel=iBatLevelUp;
  } 
  else if(iBatLevelDn<iBatLevel)
  {
    iBatLevel=iBatLevelDn;
  }

  //copy values so they don't change in between rendering slices
  iSpeed_copy=iSpeed;
  iPower_copy=iPower;
  iPas_copy=iPas;
  
  u8g.firstPage();
  do {
    gfx_draw();
  } while ( u8g.nextPage() );
}


