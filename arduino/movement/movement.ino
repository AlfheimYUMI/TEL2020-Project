#include "config.h"
#include "math.h"
#include "AccelStepper.h"
#include <avr/interrupt.h>
#include <avr/io.h>
//ADD STOP BUTTUN
/*
計時器逾時
起始標記
結束標記
指令:
  Svvaa       設定
  V11223344   速度
  Mvvrrww     移動
  Evvddaa     PID控制
  Dddgg       平移
  Add         選轉
  P           停止
*/

union {
  char cmdLine[17];
  struct
  {
    char instruction;
    int16_t value[8];
  };
} cmd;

int RXpoint;
bool RXcomplete;
int TXpoint;
bool TXcomplete;

AccelStepper motor_TR(1, STEP_TR, DIR_TR);
AccelStepper motor_TL(1, STEP_TL, DIR_TL);
AccelStepper motor_BR(1, STEP_BR, DIR_BR);
AccelStepper motor_BL(1, STEP_BL, DIR_BL);

ISR(USART_RX_vect)
{
  uint8_t r = UDR0;
  if (r == 2)
  {
    RXpoint = 0;
  }
  else if (r == 3)
  {
    RXcomplete = true;
  }
  else if (RXpoint < commandLength)
  {
    cmd.cmdLine[RXpoint++] = r;
  }
}

void setup()
{
  RXpoint = 0;
  RXcomplete = false;
  initUART();
}

void loop()
{
  if (RXcomplete)
  {
    reciveComplete();
  }
  motor_TR.runSpeed();
  motor_TL.runSpeed();
  motor_BR.runSpeed();
  motor_BL.runSpeed();
}

void initUART()
{
  UBRR0 = 103;
  UCSR0C |= (1 << UCSZ00) | (1 << USBS0);
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void reciveComplete()
{
  switch (cmd.instruction)
  {
  case 'S':
    cmd_S();
    break;
  case 'V':
    cmd_V();
    break;
  case 'M':
    cmd_M();
    break;
  case 'E':
    cmd_E();
    break;
  case 'D':
    cmd_D();
    break;
  case 'A':
    cmd_A();
    break;
  case 'p':
    cmd_P();
    break;
  default:
    break;
  }
  UDR0 = cmd.instruction; //test
  cmd.instruction = 0;
  RXcomplete = false;
}

void cmd_S()
{
  motor_TR.setMaxSpeed(cmd.value[0] * mm2step);
  motor_TL.setMaxSpeed(cmd.value[0] * mm2step);
  motor_BR.setMaxSpeed(cmd.value[0] * mm2step);
  motor_BL.setMaxSpeed(cmd.value[0] * mm2step);
  motor_TR.setAcceleration(cmd.value[1] * mm2step * mm2step);
  motor_TL.setAcceleration(cmd.value[1] * mm2step * mm2step);
  motor_BR.setAcceleration(cmd.value[1] * mm2step * mm2step);
  motor_BL.setAcceleration(cmd.value[1] * mm2step * mm2step);
}

void cmd_V()
{
  motor_TR.setSpeed(cmd.value[0] * mm2step);
  motor_TL.setSpeed(cmd.value[1] * mm2step);
  motor_BR.setSpeed(cmd.value[2] * mm2step);
  motor_BL.setSpeed(cmd.value[3] * mm2step);
}

void cmd_M()
{
  motor_TR.setSpeed(cmd.value[0] * (cos(cmd.value[1]) - sin(cmd.value[1])) + cmd.value[1] * distance); //Vc-Vs+W(w+h)
  motor_TL.setSpeed(cmd.value[0] * (cos(cmd.value[1]) + sin(cmd.value[1])) - cmd.value[1] * distance); //Vc+Vs-W(w+h)
  motor_BR.setSpeed(cmd.value[0] * (cos(cmd.value[1]) + sin(cmd.value[1])) + cmd.value[1] * distance); //Vc+Vs+W(w+h)
  motor_BL.setSpeed(cmd.value[0] * (cos(cmd.value[1]) - sin(cmd.value[1])) - cmd.value[1] * distance); //Vc-Vs-W(w+h)
}
void cmd_E() {}
void cmd_D() {}
void cmd_A() {}
void cmd_P()
{
  motor_TR.setSpeed(0);
  motor_TL.setSpeed(0);
  motor_BR.setSpeed(0);
  motor_BL.setSpeed(0);
}
