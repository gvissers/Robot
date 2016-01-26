#ifndef ENGINE_H
#define ENGINE_H

#include "motorshield.h"

/**
 * Class for driving a robot.
 *
 * Class for driving a robot controlled by two motors, on on each side (be it a
 * wheel, or a track), using the Adafruit motor shield to control the motors.
 * Currently, no calibration options are provided, so it is assumed that when
 * the motors run at the same speed, the robot will go in a straight line.
 */
class Engine
{
public:
    /**
     * Constructor
     *
     * Create a new Engine object, using the motors connected on ports 1 and 2
     * of the motor shield.
     */
    Engine(Adafruit_MotorShield *shield):
        _motor_left(shield->getMotor(1)), _motor_right(shield->getMotor(2)),
        _directions((RELEASE << 3) | RELEASE) {}

    /// Halt the robot, releasing both motors
    void halt()
    {
        _setDirections(RELEASE, RELEASE);
    }
    /**
     * Move forward
     *
     * Move the robot forward in a straight line at speed \a speed. The speed
     * should be a number between 0 (not moving) and 255 (full speed).
     * \param speed The speed with which to drive forward.
     */
    void moveForward(uint8_t speed)
    {
        _move(FORWARD, speed, FORWARD, speed);
    }
    /**
     * Move backward
     *
     * Move the robot backward in a straight line at speed \a speed. The speed
     * should be a number between 0 (not moving) and 255 (full speed).
     * \param speed The speed with which to drive forward.
     */
    void moveBackward(uint8_t speed)
    {
        _move(BACKWARD, speed, BACKWARD, speed);
    }
    /**
     * Move in a straight line
     *
     * Move the robot in a straight line at velocity \a speed. If \a speed is
     * negative, the robot will move backwards, if it is positive it will move
     * forwards. The magnitude of \a speed determines the speed of the robot,
     * with a value of 255 being full speed.
     * \param speed The speed with which to drive.
     */
    void moveStraight(int speed);

    /**
     * Turn left going forward
     *
     * Turn the robot left going forward. The outer side will move at speed
     * \a speed, the inner track will move slower or backwards with a speed
     * factor determined by \a turn_speed. The higher \a turn_speed, the sharper
     * the turn made. When \a turn_speed is 255, the turn is made in place.
     * \param speed      The speed at which the outer side drives.
     * \param turn_speed The turning speed.
     */
    void turnLeftForward(uint8_t speed, uint8_t turn_speed)
    {
        _turnLeft(speed, turn_speed, FORWARD);
    }
    /**
     * Turn right going forward
     *
     * Turn the robot right going forward. The outer side will move at speed
     * \a speed, the inner track will move slower or backwards with a speed
     * factor determined by \a turn_speed. The higher \a turn_speed, the sharper
     * the turn made. When \a turn_speed is 255, the turn is made in place.
     * \param speed      The speed at which the outer side drives.
     * \param turn_speed The turning speed.
     */
    void turnRightForward(uint8_t speed, uint8_t turn_speed)
    {
        _turnRight(speed, turn_speed, FORWARD);
    }
    /**
     * Turn left going backward
     *
     * Turn the robot left going backward. The outer side will move at speed
     * \a speed, the inner track will move slower or backwards with a speed
     * factor determined by \a turn_speed. The higher \a turn_speed, the sharper
     * the turn made. When \a turn_speed is 255, the turn is made in place.
     * \param speed      The speed at which the outer side drives.
     * \param turn_speed The turning speed.
     */
    void turnLeftBackward(uint8_t speed, uint8_t turn_speed)
    {
        _turnLeft(speed, turn_speed, BACKWARD);
    }
    /**
     * Turn right going backward
     *
     * Turn the robot right going forward. The outer side will move at speed
     * \a speed, the inner track will move slower or backwards with a speed
     * factor determined by \a turn_speed. The higher \a turn_speed, the sharper
     * the turn made. When \a turn_speed is 255, the turn is made in place.
     * \param speed      The speed at which the outer side drives.
     * \param turn_speed The turning speed.
     */
    void turnRightBackward(uint8_t speed, uint8_t turn_speed)
    {
        _turnRight(speed, turn_speed, BACKWARD);
    }
    /**
     * Turn the robot
     *
     * Make the robot turn. The robot will move at velocity \a speed, with
     * negative values making the robot drive backwards. It will turn with
     * speed \a turn_speed, a negative value denoting a left turn and a positive
     * value making the robot turn right.
     * \param speed      The speed direction at which the outer side drives.
     * \param turn_speed The turning speed and direction.
     */
    void turn(int speed, int turn_speed);

private:
    /// Motor driving the left track
    Adafruit_DCMotor *_motor_left;
    /// Motor driving the right track
    Adafruit_DCMotor *_motor_right;
    /**
     * Directions the motors are turning. Lower three bits for the direction of
     * the left motor, bits 4-6 for the right motor.
     */
    uint8_t _directions;

    /// Set and store the running directions of the motors
    void _setDirections(MotorDirection left_dir, MotorDirection right_dir);
    /// Set the turning speeds of the motors
    void _setSpeed(uint8_t left_speed, uint8_t right_speed)
    {
        _motor_left->setSpeed(left_speed);
        _motor_right->setSpeed(right_speed);
    }

    /**
     * Set the direction and speed of both motors.
     * \param left_dir    running direction of the left.
     * \param left_speed  turning speed of the left motor.
     * \param right_dir   running direction of the left.
     * \param right_speed turning speed of the left motor.
     */
    void _move(MotorDirection left_dir, uint8_t left_speed,
        MotorDirection right_dir, uint8_t right_speed)
    {
        _setDirections(left_dir, right_dir);
        _setSpeed(left_speed, right_speed);
    }

    /// Turn the robot left
    void _turnLeft(uint8_t speed, uint8_t turn_speed, MotorDirection dir);
    /// Turn the robot right
    void _turnRight(uint8_t speed, uint8_t turn_speed, MotorDirection dir);
};

#endif // ENGINE_H