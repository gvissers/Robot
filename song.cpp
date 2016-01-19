#include <Arduino.h>
#include "song.h"

void Song::update()
{
    static const float octave4[] = {
        261.6255653005986, 277.1826309768721,  293.6647679174076,
        311.1269837220809, 329.6275569128699,  349.2282314330039,
        369.9944227116344, 391.99543598174927, 415.3046975799451,
        440.0,             466.1637615180899,  493.8833012561241
    };

    if (finished())
        return;

    if (_cur_ms == 0 && _notes[_cur_idx].pitch != Pitch_Silence)
    {
        int idx = _notes[_cur_idx].pitch;
        float frequency = ldexp(octave4[idx%12], idx/12 - 4);
        tone(_pin, frequency, _notes[_cur_idx].duration);
    }
    if (++_cur_ms >= _notes[_cur_idx].duration)
    {
        ++_cur_idx;
        _cur_ms = 0;
    }
}
