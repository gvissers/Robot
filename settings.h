#ifndef SETTINGS_H
#define SETTINGS_H

// Interval of timer1 interrupt in microseconds
const uint16_t timer1_us = 50;
// number of timer ticks for music update
const uint16_t music_ticks = 1000 / timer1_us;
// Trigger distance sensor every trigger_ticks ticks
const uint16_t US_trigger_ticks = 100000ul / timer1_us;
/// Maximum valid ultrasound distance reading, values above this are ignored
const uint16_t US_max_distance = 500;

const uint8_t US_trigger_pin = 4; // Distance sensor trigger pin
const uint8_t US_echo_pin = 2;    // Distance sensor echo pin
const uint8_t piezo_pin = 8;      // Piezo element pin

// Note: some pins reserved:
// A4 and A5: I2C pins
// D2 and D3: interrupt pins

#endif
