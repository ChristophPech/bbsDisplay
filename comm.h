#pragma once
#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////////////////
/// configure comm
const long Tire_Circum_mm=2333; //26" Fatbike / 29"

//display settings 0x16 0x1f, refer to bafang_proto.txt
const uint8_t Config_Road_01 = 0x00;
const uint8_t Config_Road_02 = 0xac;  //29in, 24kmh

const uint8_t Config_OffR_01 = 0x01;
const uint8_t Config_OffR_02 = 0xb0;  //29in, 60kmh
////////////////////////////////////////////////////////////////////////////////////////////

void Comm_Init();
void Comm_Tick();

void SendPAS();

