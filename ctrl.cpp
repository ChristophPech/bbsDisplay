#include "ctrl.h"
#include "gfx.h"
#include "clock.h"

uint16_t Pressed_Plus=0;
uint16_t Pressed_Minus=0;
uint16_t Pressed_Mid=0;
uint16_t Released_Plus=0;
uint16_t Released_Minus=0;
uint16_t Released_Mid=0;

bool modeRoad=true;
uint8_t curSeq[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //32+3

void OnPAS(int val)
{
  iPas+=val;
  if(modeRoad)
  {
    if(iPas<0) iPas=0;
    if(iPas>3) iPas=3;
    return;
  }
  if(iPas<0) iPas=0;
  if(iPas>9) iPas=9;
  if(iPas>=1&&iPas<=3) {
    if(val>0) iPas=4; else iPas=0;
  }
  return;
}

void OnBtnClick(int btn,int mode)
{
  if(btn==Btn_Plus&&mode==Seq_Norm) OnPAS(1);
  if(btn==Btn_Minus&&mode==Seq_Norm) OnPAS(-1);

  //record sequence
  int iSIdx=-1;
  const int iSCnt=sizeof(curSeq)-3;
  for(int i=0;i<iSCnt;i++) {
    if(curSeq[i]==0) break;
    iSIdx=i;
  }
  if(iSIdx>=iSCnt) return;
  iSIdx++;
  curSeq[iSIdx]=(btn<<4)|mode;
  curSeq[iSIdx+1]=0;

  for(int i=0;i<sizeof(ROADMODE_SEQ);i++) {
    uint8_t v=pgm_read_byte_near(ROADMODE_SEQ+i);
    if(v!=curSeq[i]) {
      /*Serial.print("seq:");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(v);
      Serial.print(",");
      Serial.print(curSeq[i]);
      Serial.print("\n");*/
      return;
    }
  }
  //Serial.print("seq_all");
  
  modeRoad=false;
  OnPAS(0);
}

void OnModeRoad(bool val) {
  modeRoad=val;
  OnPAS(0);
  Serial.println("modeRoad");
}

void Ctrl_Init()
{
  if(Pin_Plus) pinMode(Pin_Plus, INPUT_PULLUP);
  if(Pin_Minus) pinMode(Pin_Minus, INPUT_PULLUP);
  if(Pin_Mid) pinMode(Pin_Mid, INPUT_PULLUP);
}

//key debouncing 5ms
// returns positive key pressed duration in 100ms for max 10s (100)
// returns negative key released duration in 100ms for max 10s (-100)
inline int HandleBtn(int pin,uint16_t& pressed,uint16_t& released)
{
  if(!pin) return 0;
  if(!digitalRead(pin)) 
  {
    if(pressed<9999) pressed++;
    released=0;
    if(pressed>5) return (pressed/100)+1;
    return 0;
  }
  else
  {
    if(released<9999) released++;
    if(released>5)
    {
      pressed=0;
      return -((released/100)+1);
    }
  }
  return 0;
}

void Ctrl_Tick()
{
  int iP=HandleBtn(Pin_Plus,Pressed_Plus,Released_Plus);
  int iM=HandleBtn(Pin_Minus,Pressed_Minus,Released_Minus);
  int iI=HandleBtn(Pin_Mid,Pressed_Mid,Released_Mid);

static int iP_l=0;
static int iM_l=0;
static int iI_l=0;

  if(iP>0&&iM>0&&!modeRoad) OnModeRoad(true);
  if(iP<-10&&iM<-10) curSeq[0]=0; //delete sequence

  if(iP==0&&iP_l>0) OnBtnClick(Btn_Plus,iP_l<5?Seq_Norm:Seq_Long);
  if(iM==0&&iM_l>0) OnBtnClick(Btn_Minus,iM_l<5?Seq_Norm:Seq_Long);
  if(iI==0&&iI_l>0) OnBtnClick(Btn_Mid,iI_l<5?Seq_Norm:Seq_Long);

  /*static uint16_t ctrlTickCnt=0;
  ctrlTickCnt++;
  //if(ctrlTickCnt%100==0) {
  if(iP_l!=iP||iM_l!=iM) {
    Serial.print(iP);
    Serial.print(",");
    Serial.print(iM);
    Serial.print("\n");
  }*/

  iP_l=iP;
  iM_l=iM;
  iI_l=iI;
}

