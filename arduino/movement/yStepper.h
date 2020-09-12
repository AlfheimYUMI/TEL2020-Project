#ifndef yStepper_h
#define yStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

// These defs cause trouble on some versions of Arduino
#undef round

/////////////////////////////////////////////////////////////////////
/// \class yStepper yStepper.h <yStepper.h>
/// \brief Support for stepper motors with acceleration etc.
///
/// This defines a single 2 or 4 pin stepper motor, or stepper moter with fdriver chip, with optional
/// acceleration, deceleration, absolute positioning commands etc. Multiple
/// simultaneous steppers are supported, all moving 
/// at different speeds and accelerations. 
///
/// \par Operation
/// This module operates by computing a step time in microseconds. The step
/// time is recomputed after each step and after speed and acceleration
/// parameters are changed by the caller. The time of each step is recorded in
/// microseconds. The run() function steps the motor once if a new step is due.
/// The run() function must be called frequently until the motor is in the
/// desired position, after which time run() will do nothing.
///
/// \par Positioning
/// Positions are specified by a signed long integer. At
/// construction time, the current position of the motor is consider to be 0. Positive
/// positions are clockwise from the initial position; negative positions are
/// anticlockwise. The current position can be altered for instance after
/// initialization positioning.
///
/// \par Caveats
/// This is an open loop controller: If the motor stalls or is oversped,
/// yStepper will not have a correct 
/// idea of where the motor really is (since there is no feedback of the motor's
/// real position. We only know where we _think_ it is, relative to the
/// initial starting point).
///
/// \par Performance
/// The fastest motor speed that can be reliably supported is about 4000 steps per
/// second at a clock frequency of 16 MHz on Arduino such as Uno etc. 
/// Faster processors can support faster stepping speeds. 
/// However, any speed less than that
/// down to very slow speeds (much less than one per second) are also supported,
/// provided the run() function is called frequently enough to step the motor
/// whenever required for the speed set.
/// Calling setAcceleration() is expensive,
/// since it requires a square root to be calculated.
///
/// Gregor Christandl reports that with an Arduino Due and a simple test program, 
/// he measured 43163 steps per second using runSpeed(), 
/// and 16214 steps per second using run();
class yStepper
{
public:
    yStepper(uint8_t motorNum = 1, uint8_t enablePin = 0xff);

    boolean runSpeed();
    void    setMaxSpeed(float speed);
    // void    setAcceleration(float acceleration);
    void    setSpeed(float speed);
    // void stop();
    virtual void    disableOutputs();
    virtual void    enableOutputs();
    virtual void   setOutputPins(uint8_t mask);
    virtual void   step(long step);
    
private:
    struct {
    uint8_t _direction; // 1 == CW
    float          _speed;         // Steps per second

    /// The maximum permitted speed in steps per second. Must be > 0.
    float          _maxSpeed;

    /// The current interval between steps in microseconds.
    /// 0 means the motor is currently stopped with _speed == 0
    unsigned long  _stepInterval;

    /// The last step time in microseconds
    unsigned long  _lastStepTime;

    /// The minimum allowed pulse width in microseconds
    unsigned int   _minPulseWidth;

    /// Is the enable pin inverted?
    bool           _enableInverted;

    /// Enable pin for stepper driver, or 0xFF if unused.
    uint8_t        _enablePin;
    } motor[4];
};
#endif 
