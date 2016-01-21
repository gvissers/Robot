class Song
{
public:
    // Constructor
    Song(const char* desc, int note_length=250, int octave=4):
        _desc(desc), _note_length(note_length), _octave(octave) {}

    // Start or restart this song, playing on pin pin
    void start(int pin)
    {
        _pin = pin;
        _cur_idx = 0;
        _cur_ms = 0;
    }
    // stop playing this song
    void stop()
    {
        while (_desc[_cur_idx])
            ++_cur_idx;
        _cur_ms = 0;
    }
    // Return true if this song has finished playing, false otherwise
    bool finished() const
    {
        return !_desc[_cur_idx];
    }

    // Update the current playing position, and start playing new note if needed
    void update();

private:
    const char *_desc;
    int _note_length;
    int _octave;

    // Piezo pin nr on which to play
    int _pin;
    // index of the note currently playing
    int _cur_idx;
    // nr of ms the current note still needs to playing
    int _cur_ms;
};

