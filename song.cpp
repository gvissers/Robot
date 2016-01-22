#include <Arduino.h>
#include "song.h"

void Song::update()
{
    // distance between half notes in equal tempered scale [ = 2**(1/12) ]
    static const float half_factor = 1.0594630943592953;
    // Hz values for ground tones in 4th octave (C, D, E, F, G, A, B)
    // equal tempered scale, A = 440Hz
    static const float octave4[] = {
        261.6255653005986, 293.6647679174076,  329.6275569128699,
        349.2282314330039, 391.99543598174927, 440.0, 493.8833012561241
    };

    if (finished() || --_cur_ms > 0)
        return;

    float pitch;
    int shift = _octave - 4;
    char c = _desc[_cur_idx];

    if (c >= 'A' && c <= 'G')
    {
        pitch = octave4[c - 'A'];
    }
    else if (c >= 'a' && c <= 'g')
    {
        ++shift;
        pitch = octave4[c - 'a'];
    }
    else
    {
        // Error, stop interpreting this string
        stop();
        return;
    }

    c = _desc[++_cur_idx];
    if (c == ',')
    {
        --shift;
        c = _desc[++_cur_idx];
    }
    else if (c == '\'')
    {
        ++shift;
        c = _desc[++_cur_idx];
    }

    if (c == '^')
    {
        pitch *= half_factor;
        c = _desc[++_cur_idx];
    }
    else if (c == '_')
    {
        pitch /= half_factor;
        c = _desc[++_cur_idx];
    }

    pitch = ldexp(pitch, shift);

    int lnum = 1, lden = 1;
    if (isDigit(c))
    {
        lnum = (c - '0');
        c = _desc[++_cur_idx];
    }
    if (c == '/')
    {
        lden = 2;
        c = _desc[++_cur_idx];
        if (c == '/')
        {
            lden = 4;
            c = _desc[++_cur_idx];
        }
        else if (isDigit(c))
        {
            lden = (c - '0');
            c = _desc[++_cur_idx];
        }
    }

    _cur_ms = (_note_length*lnum) / lden;
    tone(_pin, pitch, _cur_ms);
}
