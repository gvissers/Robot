#include "engine.h"

void Engine::runStraight(int speed)
{
    if (speed == 0)
    {
        _setDirections(RELEASE, RELEASE);
    }
    else if (speed > 0)
    {
        _setDirections(FORWARD, FORWARD);
        _motor_left->setSpeed(speed & 0xff);
        _motor_right->setSpeed(speed & 0xff);
    }
    else
    {
        _setDirections(BACKWARD, BACKWARD);
        _motor_left->setSpeed(-speed & 0xff);
        _motor_right->setSpeed(-speed & 0xff);
    }
}

void Engine::_setDirections(uint8_t left_dir, uint8_t right_dir)
{
    if ((_directions >> 3) != left_dir)
        _motor_left->run(left_dir);
    if ((_directions & 7) != right_dir)
        _motor_right->run(right_dir);
    _directions = (left_dir << 3) | right_dir;
}