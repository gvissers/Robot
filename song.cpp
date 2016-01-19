#include <Arduino.h>
#include "song.h"

void Song::update()
{
    // Hz values for 4th octave (A, Db, D, ..., A, Bb, B)
    /*
    // equal tempered scale
    static const float octave4[] = {
        261.6255653005986, 277.1826309768721,  293.6647679174076,
        311.1269837220809, 329.6275569128699,  349.2282314330039,
        369.9944227116344, 391.99543598174927, 415.3046975799451,
        440.0,             466.1637615180899,  493.8833012561241
    };
    */
    // just intonation
    static const float octave4[] = {
        //
        264.0, 281.6, 297.0, 316.8, 330.0  , 352.0,
        369.6, 396.0, 422.4, 440.0, 469.333, 495.0
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
