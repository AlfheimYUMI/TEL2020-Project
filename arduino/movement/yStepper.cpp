#include "yStepper.h"

#if 0
// Some debugging assistance
void dump(uint8_t* p, int l)
{
    int i;

    for (i = 0; i < l; i++)
    {
	Serial.print(p[i], HEX);
	Serial.print(" ");
    }
    Serial.println("");
}
#endif

#if 0
    Serial.println(_speed);
    Serial.println(_acceleration);
    Serial.println(_cn);
    Serial.println(_c0);
    Serial.println(_n);
    Serial.println(_stepInterval);
    Serial.println(distanceTo);
    Serial.println(stepsToStop);
    Serial.println("-----");
#endif
}

yStepper::yStepper(uint8_t motorNum, uint8_t enablePin)
{
    _motorNum = motorNum;
    _enablePin = enablePin;
    _enableInverted = 0;
    for (int i = 0; i < _motorNum; i++)
    {
        motor[i]._direction = 1;
        motor[i]._directionInverted = 0;
        motor[i]._speed = 0.0;
        motor[i]._maxSpeed = 0.0;
        motor[i]._stepInterval = 0;
        motor[i]._lastStepTime = 0;
    }
}

void yStepper::connectPin(uint8_t num, uint8_t dir, uint8_t step)
{
    if (num < _motorNum)
    {
        pinMode(dir, OUTPUT);
        pinMode(step, OUTPUT);
        motor[num]._dir = dir;
        motor[num]._step = step;
        digitalWrite(dir, LOW);
        digitalWrite(step, LOW);
    }
}

void yStepper::setMaxSpeed(uint8_t num, float speed)
{
    if (speed < 0.0)
        speed = -speed;
    if (motor[num]._maxSpeed != speed)
    {
        motor[num]._maxSpeed = speed;
        motor[num]._cmin = 1000000.0 / speed;
    }
}

void yStepper::setSpeed(uint8_t num, float speed)
{
    for (int num = 0; num < _motorNum; num++)
    {
        if (speed == motor[num]._speed)
            return;
        speed = constrain(speed, -motor[num]._maxSpeed, motor[num]._maxSpeed);
        if (speed == 0.0)
            motor[num]._stepInterval = 0;
        else
        {
            motor[num]._speed = speed;
            motor[num]._level = (int)(speed/1000)
            speed /= motor[num]._level
            motor[num]._stepInterval = fabs(1000000.0 / speed);
            motor[num]._direction = (speed > 0.0) ? 1 : 0;
        }
    }
}

boolean runSpeed()
{
    unsigned long time = micros();
    for (int num = 0; num < _motorNum; num++)
    {
        if (time - motor[num]._lastStepTime > motor[num]._stepInterval)
        {
            motor[num].remain = (1 << motor[num]._level);
            motor[num]._lastStepTime = time;
        }
        if (motor[num].remain)
        {
            digitalWrite(motor[num]._dir, motor[num]._direction^motor[num]._directionInverted);
            digitalWrite(motor[num]._step, HIGH);
            motor[num].remain--;
        }
    }
    for (int num = 0; num < _motorNum; num++)
    {
        digitalWrite(motor[num]._step, LOW);
    }
}

// Prevents power consumption on the outputs
void yStepper::disableOutputs()
{
    digitalWrite(_enablePin, HIGH ^ _enableInverted);
}

void yStepper::enableOutputs()
{
    digitalWrite(_enablePin, LOW ^ _enableInverted);
}
// void yStepper::stop()
// {
//     if (_speed != 0.0)
//     {
// 	long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)) + 1; // Equation 16 (+integer rounding)
// 	if (_speed > 0)
// 	    move(stepsToStop);
// 	else
// 	    move(-stepsToStop);
//     }
// }