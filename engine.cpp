#include "engine.h"

namespace
{

inline uint8_t reverseDirection(uint8_t dir)
{
    switch (dir)
    {
        case FORWARD:  return BACKWARD;
        case BACKWARD: return FORWARD;
        default:       return dir;
    }
}

} // namespace

void Engine::moveStraight(int speed)
{
    if (speed == 0)
        halt();
    else if (speed > 0)
        moveForward(speed&0xff);
    else
        moveBackward(-speed&0xff);
}

void Engine::turn(int speed, int turn_speed)
{
    if (turn_speed < 0)
    {
        if (speed < 0)
            _turnLeft(-speed, -turn_speed, BACKWARD);
        else
            _turnLeft(speed, -turn_speed, FORWARD);
    }
    else
    {
        if (speed < 0)
            _turnRight(-speed, turn_speed, BACKWARD);
        else
            _turnRight(speed, turn_speed, FORWARD);
    }
}

void Engine::_turnLeft(uint8_t speed, uint8_t turn_speed, uint8_t dir)
{
    if (turn_speed <= 0x80)
        _move(dir, speed * (0x80-turn_speed) / 0x80, dir, speed);
    else
        _move(reverseDirection(dir), speed * (turn_speed-0x80) / 0x7f, dir, speed);
}

void Engine::_turnRight(uint8_t speed, uint8_t turn_speed, uint8_t dir)
{
    if (turn_speed <= 0x80)
        _move(dir, speed, dir, speed * (0x80-turn_speed) / 0x80);
    else
        _move(dir, speed, reverseDirection(dir), speed * (turn_speed-0x80) / 0x7f);
}

void Engine::_setDirections(uint8_t left_dir, uint8_t right_dir)
{
    if ((_directions >> 3) != left_dir)
        _motor_left->run(left_dir);
    if ((_directions & 7) != right_dir)
        _motor_right->run(right_dir);
    _directions = (left_dir << 3) | right_dir;
}