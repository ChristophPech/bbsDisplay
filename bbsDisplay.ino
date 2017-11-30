#include "gfx.h"
#include "clock.h"

const int RXLED = 17;
unsigned int loopCnt=0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(1200);

  pinMode(RXLED, OUTPUT);

  gfx_init();
  RTC_Init();

  //RTC_ReadTime_UTC();
  //RTC_WriteTime_UTC(2017,11,29,15,0,0);
  //RTC_WriteTime_UTC(2017,RTC.mm,RTC.dd,RTC.h,RTC.m,RTC.s);
}


/*void loop_gfx() {
  gfx_render();

  iSpeed++;
  if(iSpeed>64) iSpeed=0;
  if((iSpeed%10)==0) iPas++;
  if(iPas>9) iPas=0;
  
  delay(100);
}*/

void loop()
{
 //Serial1.println("Hello!");  // Print "Hello!" over hardware UART

 //RTC.readTime();
 RTC_ReadTime_Local();

 /*Serial.print(RTC.yyyy);
 Serial.print("-");
 Serial.print(RTC.mm);
 Serial.print("-");
 Serial.print(RTC.dd);
 Serial.print(" ");
 
 Serial.print(p2dig(RTC.h,DEC));
 Serial.print(":");
 Serial.print(p2dig(RTC.m,DEC));
 Serial.print(":");
 Serial.print(p2dig(RTC.s,DEC));*/

 //Serial.print(" ");
 //Serial.print(Temp_Read());
 //Serial.print("°C");
 //Serial.print("\n");

 gfx_render();
 

 if(loopCnt%2)
 {
  digitalWrite(RXLED, LOW);   // set the LED on
  TXLED1;
 }
 else
 {
  digitalWrite(RXLED, HIGH);    // set the LED off
  TXLED0; //TX LED is not tied to a normally controlled pin
 }

 loopCnt++;
 delay(100);

 if(loopCnt==5)   iBatVolt=54800;
 if(loopCnt==100)   iBatVolt=46100;

}
