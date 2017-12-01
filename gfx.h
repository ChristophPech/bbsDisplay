#pragma once
#include <Arduino.h>

extern int iSpeed;
extern int iPower;
extern int iPas;
extern long iBatVolt;  //in mV
extern int iErrCode;

//configure battery voltage percentages, 0% 25% 50% 75%
const long BattPrc [4] PROGMEM = {42000,46200,50400,54600}; //14s battery, 100%=58.8V, 75%=54.6V, 50%=40.4V, 0%=42.0V
const int BattPrcHeur = 150; //anti jitter heuristic 150mV

void Gfx_Init();
void Gfx_Render();
