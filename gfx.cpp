#include "gfx.h"
#include "clock.h"
#include <U8glib.h>

// Width: 16, Height: 7
const unsigned char imgBatt[] PROGMEM = {
  0xFF, 0x7F, 0x01, 0x40, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x01, 0x40, 
  0xFF, 0x7F, };

U8GLIB_SSD1306_128X64 u8g( 15, 16, 10, 9, 8 );
//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g(U8G2_R0, /* clock=*/ 15, /* data=*/ 16, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

int iSpeed=36;
int iPower=500;
int iPas=0;
long distAll=32365;
long distTrip=38;
long timeTrip=60*5+3600*0+12;
long iBatVolt=40000;

long iBatVoltSmooth=iBatVolt;
int iBatLevel=0;

void gfx_draw(void) {
  char buffer[32];
  int iW,iH,iM,iS;

  ////////////////////////////////////////////////////////////////////
  //u8g.setFont(u8g2_font_blipfest_07_t);
  u8g.setFont(u8g_font_04b_03r);
  sprintf(buffer, "%02d.%02d.%02d", RTC.dd,RTC.mm,RTC.yyyy%100);
  u8g.drawStr( 1, 6, buffer);

  iS=(millis()/1000)%60;
  sprintf(buffer, "%02d:%02d:%02d",RTC.h,RTC.m, RTC.s);
  //iW=u8g.getStrPixelWidth(buffer);
  iW=4*8;
  u8g.drawStr( 128-iW, 6, buffer);

  u8g.drawStr( 106, 40, "km/h");
  u8g.drawStr( 106, 50, "W");

  sprintf(buffer, "%d km", distAll);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 1, 63, buffer);

  sprintf(buffer, "%d km", distTrip);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 1, 53, buffer);

  iH=timeTrip/3600;
  iM=(timeTrip%3600)/60;
  iS=(timeTrip%3600)%60;
  if(iH>0) {
    sprintf(buffer, "%d:%02d:%02d", iH,iM,iS);
  } else {
    sprintf(buffer, "%d:%02d", iM,iS);
  }
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 1, 46, buffer);


  sprintf(buffer, "%d", iPower);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 100-iW, 50, buffer);

  u8g.drawXBMP( 1, 10, 16, 7, imgBatt);
  int v=(iBatVoltSmooth+50)/100;
  sprintf(buffer, "%d.%dV", v/10,v%10);
  u8g.drawStr( 19, 16, buffer);

  if(iBatLevel==0) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+2,10+2,11,3);
    u8g.setColorIndex(1);
  } else if(iBatLevel==1) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+5,10+2,8,3);
    u8g.setColorIndex(1);
  } else if(iBatLevel==2) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+8,10+2,5,3);
    u8g.setColorIndex(1);
  } else if(iBatLevel==3) {
    u8g.setColorIndex(0); 
    u8g.drawBox(1+11,10+2,2,3);
    u8g.setColorIndex(1);
  }

  ////////////////////////////////////////////////////////////////////
  u8g.setFont(u8g_font_8x13Br);
  const char* pas[]={"off","eco","normal","turbo",">",">>",">>>",">>>>",">>>>>",">>>>>>"};
  sprintf(buffer, "%s", pas[iPas]);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 64-iW/2, 10, buffer);


  ////////////////////////////////////////////////////////////////////
  //u8g.setFont(u8g2_font_courB24_tn);
  u8g.setFont(u8g_font_helvB24n);

  sprintf(buffer, "%d", iSpeed%10);
  iW=u8g.getStrWidth(buffer);
  u8g.drawStr( 98-iW, 40, buffer);
  if(iSpeed>10) {
    sprintf(buffer, "%d", iSpeed/10);
    iW=u8g.getStrWidth(buffer);
    u8g.drawStr( 78-iW, 40, buffer);
  }

}

void gfx_init()
{
    u8g.begin();
    u8g.setColorIndex(1); 
    pinMode(8, OUTPUT);
}

void gfx_render()
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
  
  u8g.firstPage();
  do {
    gfx_draw();
  } while ( u8g.nextPage() );
}


