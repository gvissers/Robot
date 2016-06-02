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
    if (--_current_US_tick == 0)
    {
        digitalWrite(US_trigger_pin, HIGH);
        _US_pulse_on = true;
        _current_US_tick = US_trigger_ticks;
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

void Eyes::infraredTick()
{
    if (--_current_IR_tick == 0)
    {
        infraredMeasure(_current_IR_number);
        _current_IR_tick = IR_trigger_ticks;
        _current_IR_number = static_cast<IRSensorNumber>(_current_IR_number+1);
        if (_current_IR_number == IR_COUNT)
            _current_IR_number = IR_CENTER;
    }
}

void Eyes::infraredMeasure(IRSensorNumber which)
{
    int v = analogRead(IR_pins[which]);
    _IR_last_distance[which] = infraredVoltToCm(v);
}

unsigned int Eyes::infraredVoltToCm(unsigned int v)
{
    if (v < 60)
        // too far
        return 200;
    if (v > 600)
        // too close
        return 0;
    uint32_t vl = v;
    return (175*(16135808ul - 12393*vl)) / (884*(619*vl - 800));
}
