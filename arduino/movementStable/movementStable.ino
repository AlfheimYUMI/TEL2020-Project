#include "AccelStepper.h"
#include "config.h"
#include "math.h"
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

TODO
    function control with level.
    use timer interrupt.
    Timer1:16bits output low for stepper
    Timer2:8bits output low for stepper
*/

String inString = "";
int comma;
char instruction;
int value[valueBuffer];
bool RXcomplete;
bool timeout;
unsigned long lasttime;

//TODO: function level
AccelStepper motor_TR(A4988, STEP_TR, DIR_TR);
AccelStepper motor_TL(A4988, STEP_TL, DIR_TL);
AccelStepper motor_BR(A4988, STEP_BR, DIR_BR);
AccelStepper motor_BL(A4988, STEP_BL, DIR_BL);

void serialEvent()
{
    while (Serial.available())
    {
        char inChar = Serial.read();
        switch (inChar)
        {
        case startchar:
            inString = "";
            comma = 0;
            break;
        case endchar:
            RXcomplete = true;
        case splitchar:
            if (comma)
            {
                value[comma - 1] = inString.toInt();
            }
            else
            {
                instruction = inString[0];
            }
            inString = "";
            comma++;
            break;
        default:
            inString += inChar;
        }
    }
}

void setup()
{
    RXcomplete = false;
    lasttime = millis();
    Serial.begin(BAUDRATE);
    pinMode(EN, OUTPUT);
    digitalWrite(EN, LOW);
    motor_TR.setMaxSpeed(1000);
    motor_TL.setMaxSpeed(1000);
    motor_BR.setMaxSpeed(1000);
    motor_BL.setMaxSpeed(1000);
    motor_TR.setAcceleration(1000);
    motor_TL.setAcceleration(1000);
    motor_BR.setAcceleration(1000);
    motor_BL.setAcceleration(1000);
    motor_TR.setPinsInverted(1, 0, 0);
    motor_BR.setPinsInverted(1, 0, 0);
    Serial.println("ready");
}

void loop()
{
    if (millis() - lasttime > TIMEOUT)
    {
        timeout = true;
    }
    if (RXcomplete)
    {
        lasttime = millis();
        reciveComplete();
    }
    if (timeout)
    {
    }
    motor_TR.runSpeed();
    motor_TL.runSpeed();
    motor_BR.runSpeed();
    motor_BL.runSpeed();
}

void reciveComplete()
{
    switch (instruction)
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
    case 'P':
        cmd_P();
        break;
    case 'T':
        cmd_T();
        break;
    default:
        break;
    }
#ifdef DEBUG
    Serial.print("cmd: ");
    Serial.println(instruction);
    for (int i = 0; i < valueBuffer; i++)
    {
        Serial.print(value[i]);
        Serial.print("\t");
    }
    Serial.print('\n');
    RXcomplete = false;
#endif
}

void cmd_S()
{
    motor_TR.setMaxSpeed(value[0] * mm2step);
    motor_TL.setMaxSpeed(value[0] * mm2step);
    motor_BR.setMaxSpeed(value[0] * mm2step);
    motor_BL.setMaxSpeed(value[0] * mm2step);
    motor_TR.setAcceleration(value[1] * mm2step * mm2step);
    motor_TL.setAcceleration(value[1] * mm2step * mm2step);
    motor_BR.setAcceleration(value[1] * mm2step * mm2step);
    motor_BL.setAcceleration(value[1] * mm2step * mm2step);
}

void cmd_T()
{
    motor_TR.setSpeed(1000);
    motor_TL.setSpeed(1000);
    motor_BR.setSpeed(1000);
    motor_BL.setSpeed(1000);
}

void cmd_V()
{
//    motor_TR.setSpeed(value[0] * mm2step);
//    motor_TL.setSpeed(value[1] * mm2step);
//    motor_BR.setSpeed(value[0] * mm2step);
//    motor_BL.setSpeed(value[1] * mm2step);
    motor_TR.setSpeed(value[0] * mm2step);
    motor_TL.setSpeed(value[1] * mm2step);
    motor_BR.setSpeed(value[0] * mm2step);
    motor_BL.setSpeed(value[1] * mm2step);
}

void cmd_M()
{
    motor_TR.setSpeed(value[0] * (cos(value[1]) - sin(value[1])) + value[1] * distance); //Vc-Vs+W(w+h)
    motor_TL.setSpeed(value[0] * (cos(value[1]) + sin(value[1])) - value[1] * distance); //Vc+Vs-W(w+h)
    motor_BR.setSpeed(value[0] * (cos(value[1]) + sin(value[1])) + value[1] * distance); //Vc+Vs+W(w+h)
    motor_BL.setSpeed(value[0] * (cos(value[1]) - sin(value[1])) - value[1] * distance); //Vc-Vs-W(w+h)
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

void initTimer()
{
    cli(); //stop interrupts

    // //set timer0 interrupt at 2kHz
    // TCCR0A = 0; // set entire TCCR0A register to 0
    // TCCR0B = 0; // same for TCCR0B
    // TCNT0 = 0;  //initialize counter value to 0
    // // set compare match register for 2khz increments
    // OCR0A = 124; // = (16*10^6) / (2000*64) - 1 (must be <256)
    // // turn on CTC mode
    // TCCR0A |= (1 << WGM01);
    // // Set CS01 and CS00 bits for 64 prescaler
    // TCCR0B |= (1 << CS01) | (1 << CS00);
    // // enable timer compare interrupt
    // TIMSK0 |= (1 << OCIE0A);

    // //set timer1 interrupt at 1Hz
    // TCCR1A = 0; // set entire TCCR1A register to 0
    // TCCR1B = 0; // same for TCCR1B
    // TCNT1 = 0;  //initialize counter value to 0
    // // set compare match register for 1hz increments
    // OCR1A = 15624; // = (16*10^6) / (1*1024) - 1 (must be <65536)
    // // turn on CTC mode
    // TCCR1B |= (1 << WGM12);
    // // Set CS10 and CS12 bits for 1024 prescaler
    // TCCR1B |= (1 << CS12) | (1 << CS10);
    // // enable timer compare interrupt
    // TIMSK1 |= (1 << OCIE1A);

    // //set timer2 interrupt at 8kHz
    // TCCR2A = 0; // set entire TCCR2A register to 0
    // TCCR2B = 0; // same for TCCR2B
    // TCNT2 = 0;  //initialize counter value to 0
    // // set compare match register for 8khz increments
    // OCR2A = 249; // = (16*10^6) / (8000*8) - 1 (must be <256)
    // // turn on CTC mode
    // TCCR2A |= (1 << WGM21);
    // // Set CS21 bit for 8 prescaler
    // TCCR2B |= (1 << CS22)|(1 << CS21)|(1 << CS20);
    // // enable timer compare interrupt
    // TIMSK2 |= (1 << OCIE2A);

    sei(); //allow interrupts
}
