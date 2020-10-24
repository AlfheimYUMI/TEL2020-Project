#include <Servo.h>
#include <Timer.h>
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
char instruction;
bool flag;
String inString = "";

void event();
void inStringComplete();

class Step{
    public:
        bool inverse;
        Step(int dir, int step, bool inverseDIR);
        void run();
        void setSpeed(int speed);
        void stop();

    private:
        int _dir;
        int _step;
        int _target;
        unsigned int _duration;
        unsigned long _last;
}
Step::Step(int dir, int step, bool inverseDIR){
    _duration = 0;
    _target = 0;
    _dir = dir;
    _step = step;
    _last = micros();
    pinMode(_dir, OUTPUT);
    pinMode(_step, OUTPUT);
    digitalWrite(_dir, LOW);
    digitalWrite(_step, LOW);
}
Step::run(){
    if (_duration){
        if (micros()-_last > _duration){
            digitalWrite(_step, HIGH);
            digitalWrite(_step, LOW);
            _last = micros();
        }
    }
}
Step::setSpeed(int speed){
    if (speed>1000){
        speed = 1000;
    }
    if (speed<50){
        speed = 50;
    }
    _duration = 1000000 / speed;
}
Step::stop(){
    _duration = 0;
}

Step RT(3, 2, 1);
Step RB(5, 4, 1);
Step LT(A2, A3, 0);
Step LB(A4, A5, 0);

void setup()
{
    Serial.begin(BAUDRATE);
    Serial.println("start");
    t.every(20, event);
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
    default:
        break;
    }
}

void cmd_T(){

}

void event()
{

}
