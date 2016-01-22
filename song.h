#ifndef SONG_H
#define SONG_H

/**
 * Class for simple songs
 *
 * Class Song is used to play simple tunes on a piezo element. The songs are
 * described as simple ASCII text strings in a simplified abc notation (only
 * note names, sharps and flats, and note lengths are recognised. No keys,
 * repeats, or whitespace. Output on the piezo is done using the tone()
 * function. To keep the song playing, its update() function should be called
 * every millisecond (i.e. with a frequency of 1 kHz), most likely from a timer
 * ISR.
 */
class Song
{
public:
    /**
     * Constructor
     *
     * Create a new Song object for the tune described in \a desc. The duration
     * in milliseconds of a single note, without length denotation, is given by
     * \a note_length. The \a octave argument specifies the base octave of the
     * tune; when octave = 4 (the default), an A will be 440Hz.
     * \param desc        abc description of the tune
     * \param note_length length of a single note in ms
     * \param octave      base octave of the tune
     */
    Song(const char* desc, int note_length=250, int octave=4):
        _desc(desc), _note_length(note_length), _octave(octave) {}


    /**
     * Start playing the tune
     *
     * Start playing this tune, on piezo pin \a pin.
     * \param pin The pin number to which the piezo element is attached.
     */
    void start(int pin)
    {
        _pin = pin;
        _cur_idx = 0;
        _cur_ms = 0;
    }
    /// Stop playing this tune
    void stop()
    {
        while (_desc[_cur_idx])
            ++_cur_idx;
        _cur_ms = 0;
    }
    /// Return \c true if this tune has finished playing, \c false otherwise
    bool finished() const
    {
        return !_desc[_cur_idx];
    }

    /**
     * Update the current playing position
     *
     * Update the timestamp of the tune. If the current not has finished,
     * parse a new note and start playing it. This function should be called
     * every millisecond.
     */
    void update();

private:
    /// Description of the tune to play, in simplified abc notation
    const char *_desc;
    /// Length of a single note, in ms
    int _note_length;
    /// Octave offset of the tune
    int _octave;

    /// Piezo pin number on which to play
    int _pin;
    /// Index in the description string of the next note to play
    int _cur_idx;
    /// Number of ms the current note still needs to play
    int _cur_ms;
};

#endif // SONG_H