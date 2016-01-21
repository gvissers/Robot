#include <Arduino.h>
#include "song.h"

void Song::update()
{
    // Hz values for 4th octave (C, Db, D, ..., A, Bb, B)
    // equal tempered scale
    static const float octave4[] = {
        261.6255653005986, 277.1826309768721,  293.6647679174076,
        311.1269837220809, 329.6275569128699,  349.2282314330039,
        369.9944227116344, 391.99543598174927, 415.3046975799451,
        440.0,             466.1637615180899,  493.8833012561241
    };

    if (finished() || --_cur_ms > 0)
        return;

    int pitch_idx;
    int shift = _octave - 4;
    char c = _desc[_cur_idx];
    switch (c)
    {
        case 'A':
        case 'B':
            pitch_idx = 9 + 2 * (c - 'A');
            break;
        case 'C':
        case 'D':
        case 'E':
            pitch_idx = 2 * (c - 'C');
            break;
        case 'F':
        case 'G':
            pitch_idx = 5 + 2 * (c - 'F');
            break;
        case 'a':
        case 'b':
            ++shift;
            pitch_idx = 9 + 2 * (c - 'a');
            break;
        case 'c':
        case 'd':
        case 'e':
            ++shift;
            pitch_idx = 2 * (c - 'c');
            break;
        case 'f':
        case 'g':
            ++shift;
            pitch_idx = 5 + 2 * (c - 'f');
            break;
        default:
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
        ++pitch_idx;
        c = _desc[++_cur_idx];
    }
    else if (c == '_')
    {
        --pitch_idx;
        c = _desc[++_cur_idx];
    }

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

    float frequency = ldexp(octave4[pitch_idx], shift);
    _cur_ms = (_note_length*lnum) / lden;
    tone(_pin, frequency, _cur_ms);
}
