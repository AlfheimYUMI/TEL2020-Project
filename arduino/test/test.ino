#include <avr/interrupt.h>
#include <avr/io.h>

#define BAUDRATE 9600

char cmdLine[20];
int value[4] = {0};
int RXpoint;
bool RXcomplete;

ISR(USART_RX_vect){
    uint8_t r = UDR0;
    if (r == 2){
        RXpoint = 0;
    }else{
        cmdLine[RXpoint++] = r;
    }
    if (r == 3 && RXpoint == cmdLength){
        RXcomplete = true;
    }
}

void setup(){
    Rxpoint = 0;
    RXcomplete = false;

    initUART();
}

void loop(){
    //loop
}

void initUART(){
    UBRR0 = 103;
    UCSR0C |= (1<<UCSZ00)|(1<<USBS0);
    UCSR0B |= (1<<RXENO)|(1<<TXENO)|(1<<RXCIEO);
}

void reciveComplete(){
    //set something
    RXcomplete = false;
}