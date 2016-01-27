#ifndef EYES_H
#define EYES_H

#include "Arduino.h"

class Eyes
{
public:
    Eyes(): _current_tick(1), _US_pulse_on(false), _US_last_distance(0) {}

    void ultrasoundTick();
    void handleUltrasoundEcho();

    volatile uint16_t distance() const { return _US_last_distance; }

private:
    uint16_t _current_tick;
    bool _US_pulse_on;
    uint32_t _US_echo_start;
    volatile uint16_t _US_last_distance;

};

#endif // EYES_H