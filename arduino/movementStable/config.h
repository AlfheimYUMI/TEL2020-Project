#define DEBUG

//@uart
#define BAUDRATE 57600
#define valueBuffer 8
#define commandLength 2*valueNum+2
#define TIMEOUT 500
#define startchar '['
#define splitchar ','
#define endchar ']'
#define endcount 3

//@motor
#define EN 6

#define STEP_TR A4
#define DIR_TR A5

#define STEP_TL 5
#define DIR_TL 4

#define STEP_BR A2
#define DIR_BR A3

#define STEP_BL 3
#define DIR_BL 2

//@unit
#define mm2step 1.0

//@mechanical
#define A4988 1
#define width 200*mm2step
#define heigth 200*mm2step
#define distance width+heigth
