#include "gfx.h"
#include "clock.h"

int RXLED = 17;

void setup() {
  Serial.begin(9600);
  Serial1.begin(1200);

  pinMode(RXLED, OUTPUT);

  //gfx_init();

  RTC_Init();
  //RTC_WriteTime_UTC(2017,11,29,15,0,0);
}


void loop_gfx() {
  gfx_render();

  iSpeed++;
  if(iSpeed>64) iSpeed=0;
  if((iSpeed%10)==0) iPas++;
  if(iPas>9) iPas=0;
  
  delay(100);
}

void loop()
{
 Serial1.println("Hello!");  // Print "Hello!" over hardware UART

 //RTC.readTime();
 RTC_ReadTime_Local();

 Serial.print(RTC.yyyy);
 Serial.print("-");
 Serial.print(RTC.mm);
 Serial.print("-");
 Serial.print(RTC.dd);
 Serial.print(" ");
 
 Serial.print(p2dig(RTC.h,DEC));
 Serial.print(":");
 Serial.print(p2dig(RTC.m,DEC));
 Serial.print(":");
 Serial.print(p2dig(RTC.s,DEC));
 Serial.print("\n");

 

 digitalWrite(RXLED, LOW);   // set the LED on
 TXLED0; //TX LED is not tied to a normally controlled pin
 delay(1000);              // wait for a second
 digitalWrite(RXLED, HIGH);    // set the LED off
 TXLED1;
 delay(1000);              // wait for a second
}
