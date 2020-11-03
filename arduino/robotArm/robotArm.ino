#include <Timer.h>
#include <Servo.h>
#define BAUDRATE 115200
#define valueBuffer 8
#define startchar '['
#define endchar ']'
#define splitchar ','
#define delayTime 500
#define motorSpeed 10
//超音波感測器
#define TRIG_PIN    12
#define ECHO_PIN    11
#define destinationGoal 10 //超音波測距10mm以內

Timer t;
int comma;
int value[valueBuffer];
int usMotor[] = {1500, 1500, 1500, 1500, 1500, 1500};
int usMotorTmp[] = {1500, 1500, 1500, 1500, 1500, 1500};
char instruction;
bool flag;
String inString = "";
int coordinate[3];
float angle[6];
long int x;
long int y;
long int z;
Servo servo1, servo2, servo3, servo4, servo5, servo6;
//超音波
long int tmpduration;
bool duration = 0;
float mm;
//倉儲
bool flag2 = 0;
Servo servo7;

                // NOTE //
                //MOTOR1: 2500RIGHT ; 500LEFT
                //MOTOR2: 500往上轉 ; 2500往下轉
                //MOTOR3: 500往下轉 ; 2500往上轉
                //MOTOR4: 500往下轉 ; 2500往上轉
                // 手臂一條線: M1=1445, M2=1520, M3=900, M4=1950, M5=1400, M6= 2400(爪子張最開),1350(爪子閉起來)

void sendSignals();
void inStringComplete();
void covertCoordinateToAngle();
void convertAngleTousMotor();
void ultrasonicSensor();
void storageUnload();
void robotUnloadBox()
{
  usMotor[0] = 1500;
  usMotor[1] = 1600;
  usMotor[2] = 1950;
  usMotor[3] = 500;
  usMotor[4] = 2500;
  usMotor[5] = 2400;
  flag2 = 1;
}
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
  t.every(30, sendSignals);
  comma = 0;
  flag = 0;
  //超音波感測器
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  //倉儲
  servo7.attach(10, 500, 2500);
  servo7.writeMicroseconds(1500);
}

void loop()
{
  // put your main code here, to run repeatedly:
  t.update();
  if (flag) {
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
  while (Serial.available()) {
    char inChar = Serial.read();
    switch (inChar) {
      case startchar:
        comma = 0;
        inString = "";
        break;
      case endchar:
        flag = 1;
      case splitchar:
        if (comma == 0) {
          instruction = inString[0];
        } else {
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
    case 't':
      //test for all motor
      cmd_t();
      break;
    case 's':
      cmd_s();
      //test for singal motor
      break;
    case 'c':
      //robotArmMovement
      coordinate[0] = value[0];
      coordinate[1] = value[1];
      coordinate[2] = value[2];
      covertCoordinateToAngle();
      convertAngleTousMotor();
      break;
    case 'u':
      robotUnloadBox();
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
  for (int i = 0; i < 6; i++) {
    if (abs(usMotor[i] - usMotorTmp[i]) < motorSpeed) {
      usMotorTmp[i] += (usMotor[i] - usMotorTmp[i]);
    } else {
      usMotorTmp[i] += usMotor[i] > usMotorTmp[i] ? motorSpeed : -motorSpeed;
    }
  }
  servo1.writeMicroseconds(usMotorTmp[0]);
  servo2.writeMicroseconds(usMotorTmp[1]);
  servo3.writeMicroseconds(usMotorTmp[2]);
  servo4.writeMicroseconds(usMotorTmp[3]);
  servo5.writeMicroseconds(usMotorTmp[4]);
  servo6.writeMicroseconds(usMotorTmp[5]);
  ultrasonicSensor();
}

void covertCoordinateToAngle()
{
  x = coordinate[0];
  y = coordinate[1];
  z = coordinate[2];
  angle[0] = atan2(y, x) * 57.295;
  float tmp1, tmp2, tmp3, length1, length1Angle;
  tmp1 = sqrt(sq(x) + sq(y));
  length1 = sqrt(sq(tmp1) + sq(z));
  length1Angle = acos((10000 + 22500 - sq(length1)) / (2 * 15000));
  tmp2 = atan2(z, tmp1) * 57.295;
  tmp3 = asin(100 * sin(length1Angle) / length1) * 57.295;
  angle[1] = 90 - tmp2 - tmp3;
  angle[2] = 180 -  length1Angle * 57.295;
  angle[3] = 180 - angle[1] - angle[2];
  //Serial.print("tmp1:");
  //Serial.println(tmp1);
  //Serial.print("tmp2:");
  //Serial.println(tmp2);
  //Serial.print("tmp3:");
  //Serial.println(tmp3);
  //Serial.print("length1:");
  //Serial.println(length1);
  //Serial.print("length1Angle:");
  //Serial.println(length1Angle);
  //Serial.print("angle1:");
  //Serial.println(angle[0]);
  //Serial.print("angle2:");
  //Serial.println(angle[1]);
  //Serial.print("angle3:");
  //Serial.println(angle[2]);
  //Serial.print("angle4:");
  //Serial.println(angle[3]);
}

void convertAngleTousMotor()
{
  // 手臂一條線: M1=1445, M2=1520, M3=900, M4=1950, M5=1400, M6= 2400(爪子張最開),1350(爪子閉起來
  usMotor[0] = map(angle[0], -90, 90, 500, 2500);
  //  Serial.print("M1:");
  //  Serial.println(usMotor[0]);
  usMotor[1] = map(angle[1], -90, 90, 700, 2500);
  //  Serial.print("M2:");
  //  Serial.println(usMotor[1]);
  usMotor[2] = map(angle[2], 90, 0, 1860, 900);
  //  Serial.print("M3:");
  //  Serial.println(usMotor[2]);
  usMotor[3] = map(angle[3], 90, 0, 1000, 1950);
  //  Serial.print("M4:");
  //  Serial.println(usMotor[3]);
  //  usMotor[4] = map(angle[4], 0, 180, 500, 2500); //未調整馬達
  //  Serial.print("M5:");
  //  Serial.println(usMotor[4]);
  //  usMotor[5] = map(angle[6], 0, 180, 500, 2500); //未調整馬達
  //  Serial.print("M6:");
  //  Serial.println(usMotor[5]);
}

void ultrasonicSensor()
{
  digitalWrite(TRIG_PIN, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  tmpduration = pulseIn(ECHO_PIN, HIGH);   // 收到高電位時的時間

  mm = (tmpduration / 2) / 2.91;
  //Serial.print("Distance : ");
  //Serial.println(mm);

  if (mm <= destinationGoal)
  {
    duration = 1;
  }
  else
  {
    duration = 0;
  }
}

void storageUnload()
{
  if (flag2 ==1)
  {
    servo7.writeMicroseconds(2500);
    delay(8000);
    servo7.writeMicroseconds(1500);
    }
    flag2 = 0;
  }