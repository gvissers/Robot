#include <Arduino.h>
#include "song.h"

void Song::update()
{
    // distance between half notes in equal tempered scale [ = 2**(1/12) ]
    static const float half_factor = 1.0594630943592953;
    // Hz values for ground tones in 4th octave (C, D, E, F, G, A, B)
    // equal tempered scale, A = 440Hz
    static const float octave4[] PROGMEM = {
        440.0, 493.8833012561241, 261.6255653005986, 293.6647679174076,
        329.6275569128699, 349.2282314330039, 391.99543598174927,
    };

    if (finished() || --_cur_ms > 0)
        return;

    char c = pgm_read_byte(_cur_char);
    if (c == 'z' || c == 'x')
    {
        ++_cur_char;
        _cur_ms = _getLength();
        return;
    }

    float pitch;
    int shift = _octave - 4;
    if (c >= 'A' && c <= 'G')
    {
        pitch = pgm_read_float(octave4 + (c-'A'));
    }
    else if (c >= 'a' && c <= 'g')
    {
        ++shift;
        pitch = pgm_read_float(octave4 + (c-'a'));
    }
    else
    {
        // End or error, stop interpreting this string
        stop();
        return;
    }

    c = pgm_read_byte(++_cur_char);
    if (c == ',')
    {
        --shift;
        c = pgm_read_byte(++_cur_char);
    }
    else if (c == '\'')
    {
        ++shift;
        c = pgm_read_byte(++_cur_char);
    }

    if (c == '^')
    {
        pitch *= half_factor;
        c = pgm_read_byte(++_cur_char);
    }
    else if (c == '_')
    {
        pitch /= half_factor;
        c = pgm_read_byte(++_cur_char);
    }

    pitch = ldexp(pitch, shift);
    _cur_ms = _getLength();
    tone(_pin, pitch, _cur_ms);
}

int Song::_getLength()
{
    char c = pgm_read_byte(_cur_char);
    int lnum = 1, lden = 1;

    if (isDigit(c))
    {
        lnum = (c - '0');
        c = pgm_read_byte(++_cur_char);
    }
    if (c == '/')
    {
        lden = 2;
        c = pgm_read_byte(++_cur_char);
        if (c == '/')
        {
            lden = 4;
            c = pgm_read_byte(++_cur_char);
        }
        else if (isDigit(c))
        {
            lden = (c - '0');
            c = pgm_read_byte(++_cur_char);
        }
    }

    return (_note_length*lnum) / lden;
}