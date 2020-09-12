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

void yStepper::moveTo(long absolute)
{
    if (_targetPos != absolute)
    {
	_targetPos = absolute;
	computeNewSpeed();
	// compute new n?
    }
}

void yStepper::move(long relative)
{
    moveTo(_currentPos + relative);
}

// Implements steps according to the current step interval
// You must call this at least once per step
// returns true if a step occurred
boolean yStepper::runSpeed()
{
    // Dont do anything unless we actually have a step interval
    if (!_stepInterval)
	return false;

    unsigned long time = micros();   
    if (time - _lastStepTime >= _stepInterval)
    {
	if (_direction == DIRECTION_CW)
	{
	    // Clockwise
	    _currentPos += 1;
	}
	else
	{
	    // Anticlockwise  
	    _currentPos -= 1;
	}
	step(_currentPos);

	_lastStepTime = time; // Caution: does not account for costs in step()

	return true;
    }
    else
    {
	return false;
    }
}

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

// Run the motor to implement speed and acceleration in order to proceed to the target position
// You must call this at least once per step, preferably in your main loop
// If the motor is in the desired position, the cost is very small
// returns true if the motor is still running to the target position.
boolean yStepper::run()
{
    if (runSpeed())
	computeNewSpeed();
    return _speed != 0.0 || distanceToGo() != 0;
}

yStepper::yStepper(uint8_t motorNum, uint8_t enablePin)
{
    _interface = interface;
    _currentPos = 0;
    _targetPos = 0;
    _speed = 0.0;
    _maxSpeed = 1.0;
    _acceleration = 0.0;
    _sqrt_twoa = 1.0;
    _stepInterval = 0;
    _minPulseWidth = 1;
    _enablePin = enablepin;
    _lastStepTime = 0;
    _pin[0] = pin1;
    _pin[1] = pin2;
    _enableInverted = false;
    
    // NEW
    _n = 0;
    _c0 = 0.0;
    _cn = 0.0;
    _cmin = 1.0;
    _direction = DIRECTION_CCW;

    int i;
    for (i = 0; i < 4; i++)
	_pinInverted[i] = 0;
    if (enable)
	enableOutputs();
    // Some reasonable default
    setAcceleration(1);
}

void yStepper::setMaxSpeed(float speed)
{
    if (speed < 0.0)
       speed = -speed;
    if (_maxSpeed != speed)
    {
	_maxSpeed = speed;
	_cmin = 1000000.0 / speed;
	// Recompute _n from current speed and adjust speed if accelerating or cruising
	if (_n > 0)
	{
	    _n = (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16
	    computeNewSpeed();
	}
    }
}

float   yStepper::maxSpeed()
{
    return _maxSpeed;
}

// void yStepper::setAcceleration(float acceleration)
// {
//     if (acceleration == 0.0)
// 	return;
//     if (acceleration < 0.0)
//       acceleration = -acceleration;
//     if (_acceleration != acceleration)
//     {
// 	// Recompute _n per Equation 17
// 	_n = _n * (_acceleration / acceleration);
// 	// New c0 per Equation 7, with correction per Equation 15
// 	_c0 = 0.676 * sqrt(2.0 / acceleration) * 1000000.0; // Equation 15
// 	_acceleration = acceleration;
// 	computeNewSpeed();
//     }
// }

void yStepper::setSpeed(float speed)
{
    if (speed == _speed)
        return;
    speed = constrain(speed, -_maxSpeed, _maxSpeed);
    if (speed == 0.0)
	_stepInterval = 0;
    else
    {
	_stepInterval = fabs(1000000.0 / speed);
	_direction = (speed > 0.0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    _speed = speed;
}

float yStepper::speed()
{
    return _speed;
}

// Subclasses can override
void yStepper::step(long step)
{
    switch (_interface)
    {
        case FUNCTION:
            step0(step);
            break;

	case DRIVER:
	    step1(step);
	    break;
    
	case FULL2WIRE:
	    step2(step);
	    break;
    
	case FULL3WIRE:
	    step3(step);
	    break;  

	case FULL4WIRE:
	    step4(step);
	    break;  

	case HALF3WIRE:
	    step6(step);
	    break;  
		
	case HALF4WIRE:
	    step8(step);
	    break;  
    }
}

// You might want to override this to implement eg serial output
// bit 0 of the mask corresponds to _pin[0]
// bit 1 of the mask corresponds to _pin[1]
// ....
void yStepper::setOutputPins(uint8_t mask)
{
    uint8_t numpins = 2;
    if (_interface == FULL4WIRE || _interface == HALF4WIRE)
	numpins = 4;
    else if (_interface == FULL3WIRE || _interface == HALF3WIRE)
	numpins = 3;
    uint8_t i;
    for (i = 0; i < numpins; i++)
	digitalWrite(_pin[i], (mask & (1 << i)) ? (HIGH ^ _pinInverted[i]) : (LOW ^ _pinInverted[i]));
}
// 1 pin step function (ie for stepper drivers)
// This is passed the current step number (0 to 7)
// Subclasses can override
void yStepper::step1(long step)
{
    (void)(step); // Unused

    // _pin[0] is step, _pin[1] is direction
    setOutputPins(_direction ? 0b10 : 0b00); // Set direction first else get rogue pulses
    setOutputPins(_direction ? 0b11 : 0b01); // step HIGH
    // Caution 200ns setup time 
    // Delay the minimum allowed pulse width
    delayMicroseconds(_minPulseWidth);
    setOutputPins(_direction ? 0b10 : 0b00); // step LOW
}
    
// Prevents power consumption on the outputs
void    yStepper::disableOutputs()
{   
    if (_enablePin != 0xff)
    {
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, LOW ^ _enableInverted);
    }
}

void    yStepper::enableOutputs()
{
    pinMode(_pin[0], OUTPUT);
    pinMode(_pin[1], OUTPUT);
    pinMode(_enablePin, OUTPUT);
    digitalWrite(_enablePin, HIGH ^ _enableInverted);
}

void yStepper::setMinPulseWidth(unsigned int minWidth)
{
    _minPulseWidth = minWidth;
}

void yStepper::setEnablePin(uint8_t enablePin)
{
    _enablePin = enablePin;

    // This happens after construction, so init pin now.
    if (_enablePin != 0xff)
    {
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, HIGH ^ _enableInverted);
    }
}

void yStepper::setPinsInverted(bool directionInvert, bool stepInvert, bool enableInvert)
{
    _pinInverted[0] = stepInvert;
    _pinInverted[1] = directionInvert;
    _enableInverted = enableInvert;
}

void yStepper::setPinsInverted(bool pin1Invert, bool pin2Invert, bool pin3Invert, bool pin4Invert, bool enableInvert)
{    
    _pinInverted[0] = pin1Invert;
    _pinInverted[1] = pin2Invert;
    _pinInverted[2] = pin3Invert;
    _pinInverted[3] = pin4Invert;
    _enableInverted = enableInvert;
}

// Blocks until the target position is reached and stopped
void yStepper::runToPosition()
{
    while (run())
	;
}

boolean yStepper::runSpeedToPosition()
{
    if (_targetPos == _currentPos)
	return false;
    if (_targetPos >_currentPos)
	_direction = DIRECTION_CW;
    else
	_direction = DIRECTION_CCW;
    return runSpeed();
}

// Blocks until the new target position is reached
void yStepper::runToNewPosition(long position)
{
    moveTo(position);
    runToPosition();
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

bool yStepper::isRunning()
{
    return !(_speed == 0.0 && _targetPos == _currentPos);
}
