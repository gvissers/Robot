/******************************************************************
 This is the library for the Adafruit Motor Shield V2 for Arduino.
 It supports DC motors & Stepper motors with microstepping as well
 as stacking-support. It is *not* compatible with the V1 library!

 It will only work with https://www.adafruit.com/products/1483

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/


#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>
#include "motorshield.h"

#if defined(ARDUINO_SAM_DUE)
 #define WIRE Wire1
#else
 #define WIRE Wire
#endif

Adafruit_MotorShield::Adafruit_MotorShield(uint8_t addr):
    _addr(addr),
    _dcmotors{
        Adafruit_DCMotor(this,  8, 10,  9),
        Adafruit_DCMotor(this, 13, 11, 12),
        Adafruit_DCMotor(this,  2,  4,  3),
        Adafruit_DCMotor(this,  7,  5,  6)
    },
    _steppers{
        Adafruit_StepperMotor(this, 8, 13, 10, 9, 11, 12),
        Adafruit_StepperMotor(this, 2,  7, 4 , 3,  5,  6)
    },
     _pwm(addr) {}

void Adafruit_MotorShield::begin(uint16_t freq)
{
    // init PWM w/_freq
    WIRE.begin();
    _pwm.begin();
    _freq = freq;
    _pwm.setPWMFreq(_freq);  // This is the maximum PWM frequency
    for (uint8_t i=0; i<16; i++)
        _pwm.setPWM(i, 0, 0);
}

void Adafruit_MotorShield::setPWM(uint8_t pin, uint16_t value)
{
    if (value > 4095)
        _pwm.setPWM(pin, 4096, 0);
    else
        _pwm.setPWM(pin, 0, value);
}

Adafruit_StepperMotor *Adafruit_MotorShield::getStepper(uint16_t steps, uint8_t num)
{
    if (num == 0 || num > 2)
        return nullptr;

    Adafruit_StepperMotor *stepper = _steppers + (num-1);
    stepper->setStepsPerRev(steps);
    return stepper;
}


/******************************************
               MOTORS
******************************************/

void Adafruit_DCMotor::run(MotorDirection cmd)
{
    switch (cmd)
    {
        case FORWARD:
            _MC->setPin(_IN2_pin, LOW);  // take low first to avoid 'break'
            _MC->setPin(_IN1_pin, HIGH);
            break;
        case BACKWARD:
            _MC->setPin(_IN1_pin, LOW);  // take low first to avoid 'break'
            _MC->setPin(_IN2_pin, HIGH);
            break;
        case RELEASE:
            _MC->setPin(_IN1_pin, LOW);
            _MC->setPin(_IN2_pin, LOW);
            break;
        default:
            /* nothing */ ;
    }
}

void Adafruit_DCMotor::setSpeed(uint8_t speed)
{
    _MC->setPWM(_PWM_pin, speed*16);
}

/******************************************
               STEPPERS
******************************************/

void Adafruit_StepperMotor::release()
{
    _MC->setPin(_IN1_A_pin, LOW);
    _MC->setPin(_IN2_A_pin, LOW);
    _MC->setPin(_IN1_B_pin, LOW);
    _MC->setPin(_IN2_B_pin, LOW);
    _MC->setPWM(_PWM_A_pin, 0);
    _MC->setPWM(_PWM_B_pin, 0);
}

void Adafruit_StepperMotor::step(uint16_t steps, MotorDirection dir,
    StepStyle style)
{
    uint32_t uspers = _us_per_step;

    if (style == INTERLEAVE)
    {
        uspers /= 2;
    }
    else if (style == MICROSTEP)
    {
        uspers /= MICROSTEPS;
        steps *= MICROSTEPS;
#ifdef MOTORDEBUG
        Serial.print("steps = "); Serial.println(steps, DEC);
#endif
    }

    while (steps--)
    {
        onestep(dir, style);
        delayMicroseconds(uspers);
    }
}

uint8_t Adafruit_StepperMotor::onestep(MotorDirection dir, StepStyle style)
{
#if (MICROSTEPS == 8)
    static const uint8_t microstepcurve[] = {0, 50, 98, 142, 180, 212, 236, 250, 255};
#elif (MICROSTEPS == 16)
    static const uint8_t microstepcurve[] = {0, 25, 50, 74, 98, 120, 141, 162, 180, 197, 212, 225, 236, 244, 250, 253, 255};
#endif

    uint8_t ocrb = 255, ocra = 255;

    // next determine what sort of stepping procedure we're up to
    if (style == SINGLE)
    {
        if ((_currentstep/(MICROSTEPS/2)) % 2)
        { // we're at an odd step, weird
            if (dir == FORWARD)
                _currentstep += MICROSTEPS/2;
            else
                _currentstep -= MICROSTEPS/2;
        }
        else
        {
            // go to the next even step
            if (dir == FORWARD)
                _currentstep += MICROSTEPS;
            else
                _currentstep -= MICROSTEPS;
        }
    }
    else if (style == DOUBLE)
    {
        if (! (_currentstep/(MICROSTEPS/2) % 2))
        {
            // we're at an even step, weird
            if (dir == FORWARD)
                _currentstep += MICROSTEPS/2;
            else
                _currentstep -= MICROSTEPS/2;
        }
        else
        {
            // go to the next odd step
            if (dir == FORWARD)
                _currentstep += MICROSTEPS;
            else
                _currentstep -= MICROSTEPS;
        }
    }
    else if (style == INTERLEAVE)
    {
        if (dir == FORWARD)
            _currentstep += MICROSTEPS/2;
        else
            _currentstep -= MICROSTEPS/2;
    }
    else if (style == MICROSTEP)
    {
        if (dir == FORWARD)
            ++_currentstep;
        else
            --_currentstep;

        _currentstep += MICROSTEPS*4;
        _currentstep %= MICROSTEPS*4;

        ocra = ocrb = 0;
        if (_currentstep >= 0 && _currentstep < MICROSTEPS)
        {
            ocra = microstepcurve[MICROSTEPS - _currentstep];
            ocrb = microstepcurve[_currentstep];
        }
        else if (_currentstep >= MICROSTEPS && _currentstep < MICROSTEPS*2)
        {
            ocra = microstepcurve[_currentstep - MICROSTEPS];
            ocrb = microstepcurve[MICROSTEPS*2 - _currentstep];
        }
        else if (_currentstep >= MICROSTEPS*2 && _currentstep < MICROSTEPS*3)
        {
            ocra = microstepcurve[MICROSTEPS*3 - _currentstep];
            ocrb = microstepcurve[_currentstep - MICROSTEPS*2];
        }
        else if (_currentstep >= MICROSTEPS*3 && _currentstep < MICROSTEPS*4)
        {
            ocra = microstepcurve[_currentstep - MICROSTEPS*3];
            ocrb = microstepcurve[MICROSTEPS*4 - _currentstep];
        }
    }

    _currentstep += MICROSTEPS*4;
    _currentstep %= MICROSTEPS*4;

    _MC->setPWM(_PWM_A_pin, ocra*16);
    _MC->setPWM(_PWM_B_pin, ocrb*16);

    // release all
    uint8_t latch_state = 0; // all motor pins to 0

    if (style == MICROSTEP)
    {
        if (_currentstep >= 0 && _currentstep < MICROSTEPS)
            latch_state |= 0x03;
        else if (_currentstep >= MICROSTEPS && _currentstep < MICROSTEPS*2)
            latch_state |= 0x06;
        else if (_currentstep >= MICROSTEPS*2 && _currentstep < MICROSTEPS*3)
            latch_state |= 0x0C;
        else if (_currentstep >= MICROSTEPS*3 && _currentstep < MICROSTEPS*4)
            latch_state |= 0x09;
    }
    else
    {
        switch (_currentstep/(MICROSTEPS/2))
        {
            case 0:
                latch_state |= 0x1; // energize coil 1 only
                break;
            case 1:
                latch_state |= 0x3; // energize coil 1+2
                break;
            case 2:
                latch_state |= 0x2; // energize coil 2 only
                break;
            case 3:
                latch_state |= 0x6; // energize coil 2+3
                break;
            case 4:
                latch_state |= 0x4; // energize coil 3 only
                break;
            case 5:
                latch_state |= 0xC; // energize coil 3+4
                break;
            case 6:
                latch_state |= 0x8; // energize coil 4 only
                break;
            case 7:
                latch_state |= 0x9; // energize coil 1+4
                break;
        }
    }
#ifdef MOTORDEBUG
  Serial.print("Latch: 0x"); Serial.println(latch_state, HEX);
#endif

    _MC->setPin(_IN2_A_pin, (latch_state & 0x1) ? HIGH : LOW);
    _MC->setPin(_IN1_B_pin, (latch_state & 0x2) ? HIGH : LOW);
    _MC->setPin(_IN1_A_pin, (latch_state & 0x4) ? HIGH : LOW);
    _MC->setPin(_IN2_B_pin, (latch_state & 0x8) ? HIGH : LOW);

    return _currentstep;
}

