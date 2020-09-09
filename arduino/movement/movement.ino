#include <avr/interrupt.h>
#include <avr/io.h>
#include "config.h"
#include <AccelStepper.h>
#include "math.h"
//ADD STOP BUTTUN

union {
    char cmdLine[17];
    struct {
        char instruction;
        int16_t value[8];
    };
}cmd;

int RXpoint;
bool RXcomplete;
int TXpoint;
bool TXcomplete;

AccelStepper motor_TR(1, STEP_TR, DIR_TR);
AccelStepper motor_TL(1, STEP_TL, DIR_TL);
AccelStepper motor_BR(1, STEP_BR, DIR_BR);
AccelStepper motor_BL(1, STEP_BL, DIR_BL);

ISR(USART_RX_vect){
    uint8_t r = UDR0;
    if (r == 2){
        RXpoint = 0;
    }else if (r == 3){
        RXcomplete = true;
    }else if (RXpoint < commandLength){
        cmd.cmdLine[RXpoint++] = r;
    }
}

void setup(){
    Rxpoint = 0;
    RXcomplete = false;
    initUART();
}

void loop(){
    if (RXcomplete){
        reciveComplete();
    }
    //print text
}

void initUART(){
    UBRR0 = 103;
    UCSR0C |= (1<<UCSZ00)|(1<<USBS0);
    UCSR0B |= (1<<RXENO)|(1<<TXENO)|(1<<RXCIEO);
}

void reciveComplete(){
    switch cmd.instruction{
        'S':cmd_S();break;
        'V':cmd_V();break;
        'M':cmd_M();break;
        'E':cmd_E();break;
        'D':cmd_D();break;
        'A':cmd_A();break;
        'p':cmd_p();break;
    }
    UDR0 = cmd.instruction;//test
    cmd.instruction = 0;
    RXcomplete = false;

}

void cmd_S(){
    motor_TR.setMaxSpeed(cmd.value[0]*mm2step)
    motor_TL.setMaxSpeed(cmd.value[0]*mm2step)
    motor_BR.setMaxSpeed(cmd.value[0]*mm2step)
    motor_BL.setMaxSpeed(cmd.value[0]*mm2step)
    motor_TR.setAcceleration(cmd.value[1]*mm2step*mm2step)
    motor_TL.setAcceleration(cmd.value[1]*mm2step*mm2step)
    motor_BR.setAcceleration(cmd.value[1]*mm2step*mm2step)
    motor_BL.setAcceleration(cmd.value[1]*mm2step*mm2step)
}

void cmd_V(){
    motor_TR.setMaxSpeed(cmd.value[0]*mm2step)
    motor_TL.setMaxSpeed(cmd.value[1]*mm2step)
    motor_BR.setMaxSpeed(cmd.value[2]*mm2step)
    motor_BL.setMaxSpeed(cmd.value[3]*mm2step)
}

void cmd_M(){
    motor_TR.setMaxSpeed()//Vc-Vs+W(w+h)
    motor_TL.setMaxSpeed()//Vc+Vs-W(w+h)
    motor_BR.setMaxSpeed()//Vc+Vs+W(w+h)
    motor_BL.setMaxSpeed()//Vc-Vs-W(w+h)
}
void cmd_E(){}
void cmd_D(){}
void cmd_S(){}
void cmd_P(){}

// motor_TR
// motor_TL
// motor_BR
// motor_BL