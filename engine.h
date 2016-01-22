#ifndef ENGINE_H
#define ENGINE_H

#include <Adafruit_MotorShield.h>

class Engine
{
public:
    Engine(Adafruit_MotorShield *shield):
        _motor_left(shield->getMotor(1)), _motor_right(shield->getMotor(2)),
        _directions((RELEASE << 3) | RELEASE) {}

    void halt()
    {
        _setDirections(RELEASE, RELEASE);
    }
    void moveForward(uint8_t speed)
    {
        _move(FORWARD, speed, FORWARD, speed);
    }
    void moveBackward(uint8_t speed)
    {
        _move(BACKWARD, speed, BACKWARD, speed);
    }
    void moveStraight(int speed);

    void turnLeftForward(uint8_t speed, uint8_t turn_speed)
    {
        _turnLeft(speed, turn_speed, FORWARD);
    }
    void turnRightForward(uint8_t speed, uint8_t turn_speed)
    {
        _turnRight(speed, turn_speed, FORWARD);
    }
    void turnLeftBackward(uint8_t speed, uint8_t turn_speed)
    {
        _turnLeft(speed, turn_speed, BACKWARD);
    }
    void turnRightBackward(uint8_t speed, uint8_t turn_speed)
    {
        _turnRight(speed, turn_speed, BACKWARD);
    }
    void turn(int speed, int turn_speed);

private:
    Adafruit_DCMotor *_motor_left;
    Adafruit_DCMotor *_motor_right;
    uint8_t _directions;

    void _setDirections(uint8_t left_dir, uint8_t right_dir);
    void _setSpeed(uint8_t left_speed, uint8_t right_speed)
    {
        _motor_left->setSpeed(left_speed);
        _motor_right->setSpeed(right_speed);
    }

    void _move(uint8_t left_dir, uint8_t left_speed,
        uint8_t right_dir, uint8_t right_speed)
    {
        _setDirections(left_dir, right_dir);
        _setSpeed(left_speed, right_speed);
    }

    void _turnLeft(uint8_t speed, uint8_t turn_speed, uint8_t dir);
    void _turnRight(uint8_t speed, uint8_t turn_speed, uint8_t dir);
};

#endif // ENGINE_H