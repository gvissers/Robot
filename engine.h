#ifndef ENGINE_H
#define ENGINE_H

#include <Adafruit_MotorShield.h>

class Engine
{
public:
    Engine(Adafruit_MotorShield *shield):
        _motor_left(shield->getMotor(1)), _motor_right(shield->getMotor(2)),
        _directions((RELEASE << 3) | RELEASE) {}

    void runStraight(int speed);

private:
    Adafruit_DCMotor *_motor_left;
    Adafruit_DCMotor *_motor_right;
    uint8_t _directions;

    void _setDirections(uint8_t left_dir, uint8_t right_dir);
};

#endif // ENGINE_H