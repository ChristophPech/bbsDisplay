#pragma once
#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////////////////
/// configure ctrl
/// inputs are configured as INPUT_PULLUP, 0=not used
const int Pin_Plus=6;
const int Pin_Minus=7;
const int Pin_Mid=0;
////////////////////////////////////////////////////////////////////////////////////////////

const int Seq_End=0;
const int Seq_Norm=1; //normal press < 500ms
const int Seq_Long=2; //long press >= 500ms

const int Btn_Plus=1;
const int Btn_Minus=2;
const int Btn_Mid=3;

extern bool modeRoad;

const unsigned char ROADMODE_SEQ[] PROGMEM = {
  (Btn_Plus<<4)|Seq_Norm,
  (Btn_Plus<<4)|Seq_Norm,
  (Btn_Minus<<4)|Seq_Norm,
  (Btn_Plus<<4)|Seq_Norm,
  (Btn_Plus<<4)|Seq_Norm,
  (Btn_Minus<<4)|Seq_Norm,
  (Btn_Minus<<4)|Seq_Norm,
  (Btn_Plus<<4)|Seq_Long,
  (Btn_Minus<<4)|Seq_Long,
  (Btn_Plus<<4)|Seq_Norm,
  (Btn_Plus<<4)|Seq_Norm,
  (Btn_Plus<<4)|Seq_Norm,
  0
};

void Ctrl_Init();
void Ctrl_Tick();
