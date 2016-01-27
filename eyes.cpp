#include "eyes.h"
#include "settings.h"

namespace
{

inline uint16_t echoToCentimeter(uint32_t us)
{
    // Sound of speed under normal conditions is 340 m/s
    // Divide by two since the echo pulse has to travel back and forth
    return (17*us) / 1000;
}

} // namespace

void Eyes::ultrasoundTick()
{
    if (--_current_tick == 0)
    {
        digitalWrite(US_trigger_pin, HIGH);
        _US_pulse_on = true;
        _current_tick = US_trigger_ticks;
    }
    else if (_US_pulse_on)
    {
        digitalWrite(US_trigger_pin, LOW);
        _US_pulse_on = false;
    }
}

void Eyes::handleUltrasoundEcho()
{
    uint32_t now = micros();
    if (digitalRead(US_echo_pin) == HIGH)
    {
        // Start of echo pulse
        _US_echo_start = now;
    }
    else
    {
        // End of echo pulse
        uint16_t dist = echoToCentimeter(now - _US_echo_start);
        if (dist <= US_max_distance)
            _US_last_distance = dist;
    }
}
