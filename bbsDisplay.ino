#include "gfx.h"
#include "clock.h"
#include "storage.h"
#include "comm.h"

const int RXLED = 17;
unsigned int loopCnt=0;

void setup() {
  Serial.begin(9600);
  Comm_Init();
  pinMode(RXLED, OUTPUT);

  Gfx_Init();
  RTC_Init();
  Storage_Init();

  //RTC_ReadTime_UTC();
  //RTC_WriteTime_UTC(2017,11,29,15,0,0);
  //RTC_WriteTime_UTC(2017,RTC.mm,RTC.dd,RTC.h,RTC.m,RTC.s);

  //Timer0 1ms interrupt
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect)
{
  Comm_Tick();
}


void DoBlink() {
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
}

void Test()
{
 //ClearRAM();
 //DumpRAM();
 //DumpROM();
 //Storage_Init();

 int w=Serial1.availableForWrite();
 Serial.println(w);
}

void loop()
{
 //Serial1.println("Hello!");  // Print "Hello!" over hardware UART

 RTC_ReadTime_Local();
 Storage_Tick();
 Gfx_Render();
 delay(100);

 if(loopCnt==5)   iBatVolt=54800;
 if(loopCnt==100)   iBatVolt=46100;
 if(loopCnt==20) Test();
 loopCnt++;
}
