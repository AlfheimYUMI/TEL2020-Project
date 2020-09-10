//@uart
#define BAUDRATE 9600
#define valueNum 8
#define commandLength 2*valueNum +1

//@motor
#define STEP_TR 1
#define DIR_TR 5

#define STEP_TL 2
#define DIR_TL 6

#define STEP_BR 3
#define DIR_BR 7

#define STEP_BL 4
#define DIR_BL 8

//@unit
#define mm2step 1.0

//@mechanical
#define width 200*mm2step
#define heigth 200*mm2step
#define distance width+heigth
