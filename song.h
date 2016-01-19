class Song
{
public:
    // Pitches recognised
    enum Pitch : uint8_t
    {
        Pitch_C0,
        Pitch_Db0,
        Pitch_D0,
        Pitch_Eb0,
        Pitch_E0,
        Pitch_F0,
        Pitch_Gb0,
        Pitch_G0,
        Pitch_Ab0,
        Pitch_A0,
        Pitch_Bb0,
        Pitch_B0,
        Pitch_C1,
        Pitch_Db1,
        Pitch_D1,
        Pitch_Eb1,
        Pitch_E1,
        Pitch_F1,
        Pitch_Gb1,
        Pitch_G1,
        Pitch_Ab1,
        Pitch_A1,
        Pitch_Bb1,
        Pitch_B1,
        Pitch_C2,
        Pitch_Db2,
        Pitch_D2,
        Pitch_Eb2,
        Pitch_E2,
        Pitch_F2,
        Pitch_Gb2,
        Pitch_G2,
        Pitch_Ab2,
        Pitch_A2,
        Pitch_Bb2,
        Pitch_B2,
        Pitch_C3,
        Pitch_Db3,
        Pitch_D3,
        Pitch_Eb3,
        Pitch_E3,
        Pitch_F3,
        Pitch_Gb3,
        Pitch_G3,
        Pitch_Ab3,
        Pitch_A3,
        Pitch_Bb3,
        Pitch_B3,
        Pitch_C4,
        Pitch_Db4,
        Pitch_D4,
        Pitch_Eb4,
        Pitch_E4,
        Pitch_F4,
        Pitch_Gb4,
        Pitch_G4,
        Pitch_Ab4,
        Pitch_A4,
        Pitch_Bb4,
        Pitch_B4,
        Pitch_C5,
        Pitch_Db5,
        Pitch_D5,
        Pitch_Eb5,
        Pitch_E5,
        Pitch_F5,
        Pitch_Gb5,
        Pitch_G5,
        Pitch_Ab5,
        Pitch_A5,
        Pitch_Bb5,
        Pitch_B5,
        Pitch_C6,
        Pitch_Db6,
        Pitch_D6,
        Pitch_Eb6,
        Pitch_E6,
        Pitch_F6,
        Pitch_Gb6,
        Pitch_G6,
        Pitch_Ab6,
        Pitch_A6,
        Pitch_Bb6,
        Pitch_B6,
        Pitch_C7,
        Pitch_Db7,
        Pitch_D7,
        Pitch_Eb7,
        Pitch_E7,
        Pitch_F7,
        Pitch_Gb7,
        Pitch_G7,
        Pitch_Ab7,
        Pitch_A7,
        Pitch_Bb7,
        Pitch_B7,
        Pitch_C8,
        Pitch_Db8,
        Pitch_D8,
        Pitch_Eb8,
        Pitch_E8,
        Pitch_F8,
        Pitch_Gb8,
        Pitch_G8,
        Pitch_Ab8,
        Pitch_A8,
        Pitch_Bb8,
        Pitch_B8,
        Pitch_Silence
    };
    // Definition of a single note
    struct Note
    {
        Pitch pitch;     // pitch of the note, A4 = 440Hz
        int duration;    // duration in ms
    };

    // Constructor
    Song(const Note* notes, int nr_notes):
        _notes(notes), _nr_notes(nr_notes), _pin(-1), _cur_idx(0), _cur_ms(0) {}

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
        _cur_idx = _nr_notes;
        _cur_ms = 0;
    }
    // Return true if this song has finished playing, false otherwise
    bool finished() const
    {
        return _cur_idx >= _nr_notes;
    }

    // Update the current playing position, and start playing new note if needed
    void update();

private:
    // The notes defining the song
    const Note *_notes;
    // The number of notes in notes
    int _nr_notes;

    // Piezo pin nr on which to play
    int _pin;
    // index of the note currently playing
    int _cur_idx;
    // nr of ms the current note still needs to playing
    int _cur_ms;
};
