/* GV 2016-01-24: Based in Adafruit motorshield V2 library. Cleaned up
 * a bit to more modern C++, got rid of motor number to conserve some
 * space
 */

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

#ifndef MOTORSHIELD_H
#define MOTORSHIELD_H

#include "pwmservodriver.h"

//#define MOTORDEBUG

#define MICROSTEPS 16         // 8 or 16

enum MotorDirection : uint8_t
{
    FORWARD = 1,
    BACKWARD,
    BREAK,
    RELEASE
};

enum StepStyle: uint8_t
{
    SINGLE = 1,
    DOUBLE,
    INTERLEAVE,
    MICROSTEP
};

class Adafruit_MotorShield;

class Adafruit_DCMotor
{
public:
    Adafruit_DCMotor(Adafruit_MotorShield *shield, uint8_t PWM_pin,
        uint8_t IN1_pin, uint8_t IN2_pin):
        _MC(shield), _PWM_pin(PWM_pin), _IN1_pin(IN1_pin), _IN2_pin(IN2_pin) {}

    void run(MotorDirection cmd);
    void setSpeed(uint8_t speed);

private:
    Adafruit_MotorShield *_MC;
    uint8_t _PWM_pin, _IN1_pin, _IN2_pin;
};

class Adafruit_StepperMotor
{
public:
    Adafruit_StepperMotor(Adafruit_MotorShield * shield,
        uint8_t PWM_A_pin, uint8_t IN1_A_pin, uint8_t IN2_A_pin,
        uint8_t PWM_B_pin, uint8_t IN1_B_pin, uint8_t IN2_B_pin):
        _MC(shield),
        _PWM_A_pin(PWM_A_pin), _IN1_A_pin(IN1_A_pin), _IN2_A_pin(IN2_A_pin),
        _PWM_B_pin(PWM_B_pin), _IN1_B_pin(IN1_B_pin), _IN2_B_pin(IN2_B_pin),
        _currentstep(0) {}

    void setStepsPerRev(uint16_t steps) { _steps_per_rev = steps; }
    void setSpeed(uint16_t rpm)
    {
        _us_per_step = 60000000 / ((uint32_t)_steps_per_rev * (uint32_t)rpm);
    }

    void step(uint16_t steps, MotorDirection dir, StepStyle style=SINGLE);
    uint8_t onestep(MotorDirection dir, StepStyle style);
    void release();

private:
    Adafruit_MotorShield *_MC;
    uint8_t _PWM_A_pin, _IN1_A_pin, _IN2_A_pin;
    uint8_t _PWM_B_pin, _IN1_B_pin, _IN2_B_pin;

    uint32_t _us_per_step;
    uint16_t _steps_per_rev; // # steps per revolution
    uint8_t _currentstep;
};

class Adafruit_MotorShield
{
public:
    Adafruit_MotorShield(uint8_t addr = 0x60);

    void begin(uint16_t freq = 1600);

    void setPWM(uint8_t pin, uint16_t val);
    void setPin(uint8_t pin, uint8_t val)
    {
        _pwm.setPWM(pin, (val == LOW ? 0 : 4096), 0);
    }
    Adafruit_DCMotor *getMotor(uint8_t num)
    {
        return (num == 0 || num > 4) ? nullptr : (_dcmotors + (num-1));
    }
    Adafruit_StepperMotor *getStepper(uint16_t steps, uint8_t num);

private:
    uint8_t _addr;
    uint16_t _freq;
    Adafruit_DCMotor _dcmotors[4];
    Adafruit_StepperMotor _steppers[2];
    Adafruit_PWMServoDriver _pwm;
};

#endif // MOTORSHIELD_H
