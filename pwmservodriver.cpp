/* GV 2016-01-24: Based on code in Adafruit motorshield V2 library.
 */

/***************************************************
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include "pwmservodriver.h"

#if defined(ARDUINO_SAM_DUE)
    #define WIRE Wire1
#else
    #define WIRE Wire
#endif
#if ARDUINO >= 100
    #define WIRE_WRITE WIRE.write
    #define WIRE_READ WIRE.read
#else
    #define WIRE_WRITE WIRE.send
    #define WIRE_READ WIRE.receive
#endif

void Adafruit_PWMServoDriver::begin()
{
    WIRE.begin();
    reset();
}

void Adafruit_PWMServoDriver::setPWMFreq(float freq)
{
    freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).

    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    uint8_t prescale = floor(prescaleval + 0.5);

    uint8_t oldmode = read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
    write8(PCA9685_MODE1, newmode); // go to sleep
    write8(PCA9685_PRESCALE, prescale); // set the prescaler
    write8(PCA9685_MODE1, oldmode);
    delay(5);
    write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
                                            // This is why the beginTransmission below was not working.
}

void Adafruit_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off)
{
    WIRE.beginTransmission(_i2c_addr);
    WIRE_WRITE(LED0_ON_L+4*num);
    WIRE_WRITE((uint8_t)on);
    WIRE_WRITE((uint8_t)(on>>8));
    WIRE_WRITE((uint8_t)off);
    WIRE_WRITE((uint8_t)(off>>8));
    WIRE.endTransmission();
}

uint8_t Adafruit_PWMServoDriver::read8(uint8_t addr)
{
    WIRE.beginTransmission(_i2c_addr);
    WIRE_WRITE(addr);
    WIRE.endTransmission();

    WIRE.requestFrom((uint8_t)_i2c_addr, (uint8_t)1);
    return WIRE_READ();
}

void Adafruit_PWMServoDriver::write8(uint8_t addr, uint8_t d) {
    WIRE.beginTransmission(_i2c_addr);
    WIRE_WRITE(addr);
    WIRE_WRITE(d);
    WIRE.endTransmission();
}
