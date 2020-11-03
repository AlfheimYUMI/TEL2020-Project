#include <Servo.h>
#include <Timer.h>
#define BAUDRATE 115200
#define valueBuffer 8
#define startchar '['
#define endchar ']'
#define splitchar ','
#define delayTime 500
#define motorSpeed 10

Timer t;
int comma;
int value[valueBuffer];
char instruction;
bool flag;
String inString = "";

void event();
void inStringComplete();

class Step{
    public:
        bool inverse;
        Step(int dir1, int step1, int dir2, int step2, bool inverseDIR);
        void run();
        void setSpeed(int speed);
        void stop();

    private:
        int _dir1;
        int _step1;
        int _dir2;
        int _step2;
        int _target;
        unsigned int _duration;
        unsigned long _last;
};
Step::Step(int dir1, int step1, int dir2, int step2, bool inverseDIR){
    _duration = 0;
    inverse = inverseDIR;
    _target = 0;
    _dir1 = dir1;
    _step1 = step1;
    _dir2 = dir2;
    _step2 = step2;
    _last = micros();
    pinMode(_dir1, OUTPUT);
    pinMode(_step1, OUTPUT);
    pinMode(_dir2, OUTPUT);
    pinMode(_step2, OUTPUT);
    digitalWrite(_step1, LOW);
    digitalWrite(_step2, LOW);
}
void Step::run(){
    if (_duration){
        if (micros()-_last > _duration){
            digitalWrite(_step1, HIGH);
            digitalWrite(_step2, HIGH);
            _last = micros();
            digitalWrite(_step1, LOW);
            digitalWrite(_step2, LOW);
        }
    }
}
void Step::setSpeed(int speed){
    if (speed>0){
      digitalWrite(_dir1, 1^inverse);
      digitalWrite(_dir2, 1^inverse);
    }else{
      speed = -speed;
      digitalWrite(_dir1, 0^inverse);
      digitalWrite(_dir2, 0^inverse);
    }
    if (speed){
        if (speed>1000){
            speed = 1000;
        }
        if (speed<50){
            speed = 50;
        }
        _duration = 1000000 / speed; 
        Serial.println(_duration);
    }else{
        _duration = 0;
    }
}
void Step::stop(){
    _duration = 0;
}

Step MR(2, 3, 4, 5, 1);
Step ML(A3, A2, A5, A4, 0);

void setup()
{
    Serial.begin(BAUDRATE);
    Serial.println("start");
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
//    MR.setSpeed(1000);
//    ML.setSpeed(1000);
    comma = 0;
    flag = 0;
}

void loop()
{
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
    MR.run();
    ML.run();
}

void serialEvent()
{
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
    case 'T':
        cmd_T();
        break;
    case 'V':
        cmd_V();
        break;
    default:
        break;
    }
}

void cmd_T(){
    MR.stop();
    ML.stop();
}
void cmd_V(){
    MR.setSpeed(value[1]);
    ML.setSpeed(value[0]);
}

void event()
{
    MR.run();
    ML.run();
}
