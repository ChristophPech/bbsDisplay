#include <Arduino.h>
#include "clock.h"
#include <Wire.h>

#include <OneWire.h> 
#include <DallasTemperature.h>
OneWire* oneWire=NULL;
DallasTemperature* tempSensor=NULL;

const char *p2dig(uint8_t v, uint8_t mode)
// print 2 digits leading zero
{
  uint8_t n = 0;
  static char c[3] = { "00" };

  switch(mode)
  {
    case HEX:
    {
      c[0] = htoa((v >> 4) & 0xf);
      c[1] = htoa(v & 0xf);
    }
    break;
  
    case DEC:
    {
      c[0] = ((v / 10) % 10) + '0';
      c[1] = (v % 10) + '0';
    }
    break;
  }

  return(c);
}

char htoa(uint8_t i)
{
  if (i >= 0 && i <= 9)
    return(i + '0');
  if (i >= 10 && i <= 15)
    return(i - 10 + 'a');

  return('?');
}

uint8_t htoi(char c)
{
  c = toupper(c);
  
  if (c >= '0' && c <= '9')
      return(c - '0');
  else if (c >= 'A' && c <= 'F')
      return(c - 'A' + 10);
  else
      return(0);
}

void RTC_Init()
{
  RTC.control(DS1307_CLOCK_HALT, DS1307_OFF);
  RTC.control(DS1307_SQW_RUN, DS1307_OFF);
  RTC.control(DS1307_12H, DS1307_OFF);

  if(DS18B20_PIN>=0) {
    oneWire=new OneWire(DS18B20_PIN);        
    tempSensor=new DallasTemperature(oneWire);
    tempSensor->begin();
    tempSensor->setWaitForConversion(false);
    tempSensor->requestTemperatures();
  }

}

float Temp_Read()
{
  static float lastTemp=0;

  //needs to be async because it takes 94ms for default 9bit resolution
  if(tempSensor!=NULL&&tempSensor->isConversionComplete())
  {
    lastTemp=tempSensor->getTempCByIndex(0);
    tempSensor->requestTemperatures();
  }
  
  return lastTemp;
}

void RTC_WriteTime_UTC(int yyyy,int mm, int dd, int h, int m, int s)
{
  RTC.yyyy = yyyy;
  RTC.mm = mm;
  RTC.dd = dd;
  
  RTC.h = h;
  RTC.m = m;
  RTC.s = s;
  
  RTC.dow = RTC.calcDoW(yyyy,mm,dd);
  
  RTC.writeTime(); 
}

void RTC_ReadTime_UTC()
{
  RTC.readTime();
}

void RTC_ReadTime_Local()
{
  RTC.readTime();

  // ---- Convert UTC to Local Time ------------------------------------
  int theHour = RTC.h;
  int theDay = RTC.dd;
  int theMonth = RTC.mm;
  int theYear = RTC.yyyy;

  byte daysPerMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  // -------------------- check for leap year --------------------------
  if ((theYear % 4) == 0) {
    daysPerMonth[1] = 29; // leap year
  }

  // ------------------- dst time zone --------
  if (theMonth * 100 + theDay >= DSTBegin[theYear - DSTYearOfs] && theMonth * 100 + theDay < DSTEnd[theYear - DSTYearOfs]) {
    theHour += TimeZoneDST;
  } else {
    // --------------------normal time zone --------
    theHour += TimeZoneSTD;
  }

  // ----------------------- correct day, mo & yr for roll backward ----
  if (theHour < 0) {
    theHour += 24;
    theDay -= 1;
    if (theDay < 1) {
      if (theMonth == 1) { // Jan 1
        theMonth = 12;
        theYear -= 1;
      } else {
        theMonth -= 1;
      }
      theDay = daysPerMonth[theMonth - 1];
    }
  }

  // ----------------------- roll forward if east of prime meridian ----
  if (theHour >= 24) {
    theHour -= 24;
    theDay += 1;
    if (theDay > daysPerMonth[theMonth - 1]) {
      theDay = 1;
      theMonth += 1;
      if (theMonth > 12) { // Jan 1
        theYear += 1;
        theMonth = 1;
      }
    }
  }

  //write back
  RTC.h=theHour;
  RTC.dd=theDay;
  RTC.mm=theMonth;
  RTC.yyyy=theYear;
}




