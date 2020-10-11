#include <Timer.h>
#include <Servo.h>
#define BAUDRATE 9600
#define valueBuffer 8
#define startchar '['
#define endchar ']'
#define splitchar ','
#define delayTime 500
#define motorSpeed 10

Timer t;
int comma;
int value[valueBuffer];
int usMotor[] = {1500, 1500, 1500, 1500, 1500, 1500};
int usMotorTmp[] = {1500, 1500, 1500, 1500, 1500, 1500};
char instruction;
bool flag;
Servo servo1, servo2, servo3, servo4, servo5, servo6;
String inString = "";

// NOTE //
//MOTOR1: 2500RIGHT ; 500LEFT
//MOTOR2: 500往上轉 ; 2500往下轉
//MOTOR3: 500往下轉 ; 2500往上轉
//MOTOR4: 500往下轉 ; 2500往上轉
// 手臂一條線: M1=1445, M2=1520, M3=900, M4=1950, M5=1400, M6= 2400(爪子張最開),1350(爪子閉起來)

void sendSignals();
void inStringComplete();
void robotInitPosition()
{
  usMotor[0] = 2500;
  usMotor[1] = 500;
  usMotor[2] = 1900;
  usMotor[3] = 800;
  usMotor[4] = 1500;
  usMotor[5] = 1500;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  Serial.println("start");
  servo6.attach(9, 500, 2500);
  delay(delayTime);
  servo5.attach(8, 500, 2500);
  delay(delayTime);
  servo4.attach(7, 500, 2500);
  servo3.attach(6, 500, 2500);
  servo2.attach(5, 500, 2500);
  delay(delayTime + 200);
  servo1.attach(4, 500, 2500);
  delay(delayTime + 200);
  robotInitPosition();
  t.every(20, sendSignals);
  comma = 0;
  flag = 0;
}

void loop()
{
  // put your main code here, to run repeatedly:
  t.update();
  if (flag)
  {
    inStringComplete();
    Serial.print("cmd: ");
    Serial.println(instruction);
    for (int i = 0; i < valueBuffer; i++)
    {
      Serial.print(value[i]);
      Serial.print("\t");
    }
    Serial.print('\n');
    flag = 0;
  }
}

void serialEvent()
{
  //硬體中斷，收資料
  while (Serial.available())
  {
    char inChar = Serial.read();
    switch (inChar)
    {
    case startchar:
      comma = 0;
      inString = "";
      break;
    case endchar:
      flag = 1;
    case splitchar:
      if (comma == 0)
      {
        instruction = inString[0];
      }
      else
      {
        value[comma - 1] = inString.toInt();
      }
      inString = "";
      comma++;
      break;
    default:
      inString += inChar;
    }
  }
}
void inStringComplete()
{
  switch (instruction)
  {
  case 'f':
    //front
    //cmd_S();
    break;
  case 'b':
    //back
    //cmd_V();
    break;
  case 'l':
    //left
    //cmd_M();
    break;
  case 'r':
    //right
    //cmd_E();
    break;
  case 't':
    //test for all motor
    cmd_t();
    break;
  case 's':
    cmd_s();
    //test for singal motor
    break;
  default:
    break;
  }
}
void cmd_t()
{
  //[t,motor1,motor2,motor3,motor4,motor5,motor6]
  usMotor[0] = value[0];
  usMotor[1] = value[1];
  usMotor[2] = value[2];
  usMotor[3] = value[3];
  usMotor[4] = value[4];
  usMotor[5] = value[5];
}
void cmd_s()
{
  //[s,第幾個馬達,微秒數]
  switch (value[0])
  {
  case 1:
    usMotor[0] = value[1];
    break;
  case 2:
    usMotor[1] = value[1];
    break;
  case 3:
    usMotor[2] = value[1];
    break;
  case 4:
    usMotor[3] = value[1];
    break;
  case 5:
    usMotor[4] = value[1];
    break;
  case 6:
    usMotor[5] = value[1];
    break;
  }
}

void sendSignals()
{
  //調整速度。位置時間圖
  for (int i = 0; i < 6; i++)
  {
    if (abs(usMotor[i] - usMotorTmp[i]) < motorSpeed)
    {
      usMotorTmp[i] += (usMotor[i] - usMotorTmp[i]);
    }
    else
    {
      usMotorTmp[i] += usMotor[i] > usMotorTmp[i] ? motorSpeed : -motorSpeed;
    }
  }
  servo1.writeMicroseconds(usMotorTmp[0]);
  servo2.writeMicroseconds(usMotorTmp[1]);
  servo3.writeMicroseconds(usMotorTmp[2]);
  servo4.writeMicroseconds(usMotorTmp[3]);
  servo5.writeMicroseconds(usMotorTmp[4]);
  servo6.writeMicroseconds(usMotorTmp[5]);
}
