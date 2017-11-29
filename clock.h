#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// configure your time zone specifics here, the clock runs UTC
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const int TimeZoneSTD = 1; //UTC+1 CET
const int TimeZoneDST = 2; //UTC+2 CEST

const int DSTYearOfs = 2017; //starting year of the array below
const int DSTBegin [] PROGMEM = { 326, 325, 331, 329, 328, 327, 326}; //month*100+day
const int DSTEnd   [] PROGMEM = {1029,1028,1027,1025,1031,1030,1029};

const int DS18B20_PIN = -1;   // DS18B20 temperature sensor, -1 if not installed
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MD_DS1307.h>

const char *p2dig(uint8_t v, uint8_t mode);
char htoa(uint8_t i);
uint8_t htoi(char c);

void RTC_Init();
void RTC_ReadTime_Local();
void RTC_ReadTime_UTC();
void RTC_WriteTime_UTC(int yyyy,int mm, int dd, int h, int m, int s);


float Temp_Read();
