#include "U8glib.h"


// Width: 32, Height: 13
const unsigned char imgBatt[52] PROGMEM = {
  0xff, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xfc, 
  0xc0, 0x00, 0x00, 0x0c, 
  0xc0, 0x00, 0x00, 0x0c, 
  0xc0, 0x00, 0x00, 0x0f, 
  0xc0, 0x00, 0x00, 0x0f, 
  0xc0, 0x00, 0x00, 0x0f, 
  0xc0, 0x00, 0x00, 0x0f, 
  0xc0, 0x00, 0x00, 0x0f, 
  0xc0, 0x00, 0x00, 0x0c, 
  0xc0, 0x00, 0x00, 0x0c, 
  0xff, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xfc, 
};

U8GLIB_SSD1306_128X64 u8g( 13, 11, 10, 9, 8 );

void setup() {
  // put your setup code here, to run once:
  //pinMode(LED_BUILTIN, OUTPUT);
  u8g.setColorIndex(1); 
  pinMode(8, OUTPUT);
}

int iSpeed=0;
int iPower=500;
int iPas=0;
long distAll=32365;
long distTrip=36;
long timeTrip=60*5+3600*0+12;

void draw(void) {
  int iW,iH,iM,iS;

  ////////////////////////////////////////////////////////////////////
  u8g.setFont(u8g_font_04b_03r);
  u8g.drawStr( 0, 6, "23.11.17");

  char buffer[32];
  iS=(millis()/1000)%60;
  sprintf(buffer, "16:12:%02d", iS);
  
  //iW=u8g.getStrPixelWidth(buffer);
  iW=4*8;
  u8g.drawStr( 128-iW, 6, buffer);

  u8g.drawStr( 106, 40, "km/h");
  u8g.drawStr( 106, 50, "W");

  sprintf(buffer, "%d km", distAll);
  iW=u8g.getStrPixelWidth(buffer);
  u8g.drawStr( 1, 63, buffer);

  sprintf(buffer, "%d km", distTrip);
  iW=u8g.getStrPixelWidth(buffer);
  u8g.drawStr( 1, 53, buffer);

  iH=timeTrip/3600;
  iM=(timeTrip%3600)/60;
  iS=(timeTrip%3600)%60;
  if(iH>0) {
    sprintf(buffer, "%d:%02d:%02d", iH,iM,iS);
  } else {
    sprintf(buffer, "%d:%02d", iM,iS);
  }
  iW=u8g.getStrPixelWidth(buffer);
  u8g.drawStr( 1, 46, buffer);


  sprintf(buffer, "%d", iPower);
  iW=u8g.getStrPixelWidth(buffer);
  u8g.drawStr( 100-iW, 50, buffer);

  ////////////////////////////////////////////////////////////////////
  u8g.setFont(u8g_font_8x13Br);
  const char* pas[]={"off","eco","normal","turbo",">",">>",">>>",">>>>",">>>>>",">>>>>>"};
  sprintf(buffer, "%s", pas[iPas]);
  iW=u8g.getStrPixelWidth(buffer);
  u8g.drawStr( 64-iW/2, 10, buffer);


  ////////////////////////////////////////////////////////////////////
  u8g.setFont(u8g_font_courB24);

  sprintf(buffer, "%d", iSpeed%10);
  iW=u8g.getStrPixelWidth(buffer);
  u8g.drawStr( 98-iW, 40, buffer);
  if(iSpeed>10) {
    sprintf(buffer, "%d", iSpeed/10);
    iW=u8g.getStrPixelWidth(buffer);
    u8g.drawStr( 78-iW, 40, buffer);
  }


  u8g.drawBitmapP( 1, 20, 4, 13, imgBatt);

}
void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);                       // wait for a second

  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );


  iSpeed++;
  if(iSpeed>64) iSpeed=0;
  if((iSpeed%10)==0) iPas++;
  if(iPas>9) iPas=0;
  
  delay(100);

}
