#ifndef EYES_H
#define EYES_H

#include "Arduino.h"

/**
 * Class for distance sensors
 *
 * Class Eyes handles the distance sensors on the robot. It provides functions
 * and handlers to perform distance measurements on a regular basis.
 */
class Eyes
{
public:
    /// Constructor
    Eyes(): _current_tick(1), _US_pulse_on(false), _US_last_distance(0) {}

    /**
     * Handle timer tick
     *
     * Handle a tick from the timer. This function is called from the timer
     * interrupt handler. It will do a distance measurement using the ultrasound
     * sensor, every \c US_trigger_ticks time it is called.
     */
    void ultrasoundTick();
    /**
     * Handle ultrasound response
     *
     * Handle a response to the ultrasound trigger. This function is called
     * from an interrupt handler which is triggered by a change in status on
     * the echo pin of the ultrasound sensor. It calculates the time since the
     * last change, which is then converted to a distance which is stored.
     */
    void handleUltrasoundEcho();

    /// Return the value of the last successful measurement
    volatile uint16_t distance() const { return _US_last_distance; }

private:
    /// Current time tick. When it reaches zero, a new pulse is sent
    uint16_t _current_tick;
    /// Whether the ultrasound trigger pulse is currently on
    bool _US_pulse_on;
    /// Start time (microseconds) of the last echo pulse
    uint32_t _US_echo_start;
    /// Last successful distance reading from the ultrasound sensor
    volatile uint16_t _US_last_distance;
};

#endif // EYES_H