#include "ctrl.h"
#include "gfx.h"
#include "clock.h"
#include "storage.h"

uint16_t Pressed_Plus=0;
uint16_t Pressed_Minus=0;
uint16_t Pressed_Mid=0;
uint16_t Released_Plus=0;
uint16_t Released_Minus=0;
uint16_t Released_Mid=0;

bool modeRoad=true;
uint8_t curSeq[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //32+3

long throttleScaled=0;
long batterySum=0;
long batteryCount=0;

long moveSum=0;
long moveMSCount=0;

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

void OnClearTrip() {
  curState.distTrip=0;
  curState.timeTrip=0;
}

void OnBtnClick(int btn,int mode)
{
  if(btn==Btn_Plus&&mode==Seq_Norm) OnPAS(1);
  if(btn==Btn_Minus&&mode==Seq_Norm) OnPAS(-1);
  if(btn==Btn_Mid&&mode==Seq_Long) OnClearTrip();

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
      /*
      Serial.print("seq:");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(v);
      Serial.print(",");
      Serial.print(curSeq[i]);
      Serial.print("\n");/**/
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
  if(Pin_ThrottleIn) {pinMode(Pin_ThrottleIn,INPUT);}

  //pin5 PWM set to 32khz
  TCCR3B = _BV(CS00);
  pinMode(Pin_ThrottleOut, OUTPUT);
  analogWrite(5,255);
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

  if(Pin_BatVolt) {
    static int bat_smaple_tick=0;
    bat_smaple_tick++;
    if(bat_smaple_tick>=1)
    {
      bat_smaple_tick=0;
      int v=analogRead(Pin_BatVolt);
      batterySum+=v;
      batteryCount++;
      if(batteryCount>=1000) {
        iBatVolt=(batterySum*Battery_mV_per_count)/(batteryCount*1);
        //throttleScaled=iBatVolt;
        batterySum=0;
        batteryCount=0;
      }
    };
  }
  if(Pin_ThrottleIn) {
    int v=analogRead(Pin_ThrottleIn);
    //throttleScaled=v;
    v-=Throttle_Scale_Min;
    if(v<0) v=0;
    v=long(v)*10000/(Throttle_Scale_Max-Throttle_Scale_Min);
    if(v>10000) v=10000;
    //throttleScaled=v;

    if(v<iThrottle||Throttle_RampUpMaxSpeed<=0) {
      iThrottle=v;
    }
    else if(v>iThrottle)
    {
      //iSpeed=80*1000;
      long s=iSpeed/(Throttle_RampUpMaxSpeed);
      if(s>1000) s=1000;

      s=(Throttle_RampUpTime0*(1000-s))+(Throttle_RampUpTime1*s);
      s/=1000;
      if(s<1) {
        iThrottle=v;
      } else {
        s=long(10000)/s;
        iThrottle+=s;
        if(iThrottle>v) iThrottle=v;
      }
    }
    
    //iThrottle=throttleScaled;
  }
  if(Pin_ThrottleOut)
  {
    int i=25+(iThrottle/55);
    if(i>255) i=255;
    if(modeRoad) i=0;
    analogWrite(Pin_ThrottleOut,i);
  }
  if(modeRoad)
  {
    if(iThrottle>=5000) {
      iPas=-1;
    } else {
      if(iPas==-1) iPas=0;
    }
  }

static int iP_l=0;
static int iM_l=0;
static int iI_l=0;

  if(iP>0&&iM>0&&!modeRoad) OnModeRoad(true);
  if(iP<-10&&iM<-10) curSeq[0]=0; //delete sequence

  if(iP==0&&iP_l>0) OnBtnClick(Btn_Plus,iP_l<5?Seq_Norm:Seq_Long);
  if(iM==0&&iM_l>0) OnBtnClick(Btn_Minus,iM_l<5?Seq_Norm:Seq_Long);
  if(iI==0&&iI_l>0) OnBtnClick(Btn_Mid,iI_l<5?Seq_Norm:Seq_Long);

  /*   
  static uint16_t ctrlTickCnt=0;
  ctrlTickCnt++;
  if(ctrlTickCnt%100==0) {
  //if(iP_l!=iP||iM_l!=iM) {
    Serial.print(throttleScaled);
  
    //Serial.print(iP);
    //Serial.print(",");
    //Serial.print(iM);
    Serial.print("\n");
  }/**/

  iP_l=iP;
  iM_l=iM;
  iI_l=iI;

  static unsigned long lastTime=micros();
  unsigned long curTime=micros();
  unsigned long iDiff=curTime-lastTime;
  if(iDiff>=1000) {
    unsigned long iMS=iDiff/1000;
    moveSum+=iSpeed*iMS;
    lastTime+=1000*iMS;
    moveMSCount++;
  }
  if(moveMSCount>=1000) {
    unsigned long iMetersMoved=(moveSum/360)/moveMSCount;
    moveMSCount=0;
    moveSum=0;
    curState.distTrip+=iMetersMoved;
    curState.distAll+=iMetersMoved;
  }
  
}

