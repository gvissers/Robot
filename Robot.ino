#include "song.h"

const bool debug = false;

const int piezo_pin = 8;
const Song::Note r2d2_notes[] = {
    {Song::Pitch_A7, 100},
    {Song::Pitch_G7, 100},
    {Song::Pitch_E7, 100},
    {Song::Pitch_C7, 100},
    {Song::Pitch_D7, 100},
    {Song::Pitch_B7, 100},
    {Song::Pitch_F7, 100},
    {Song::Pitch_C8, 100},
    {Song::Pitch_A7, 100},
    {Song::Pitch_G7, 100},
    {Song::Pitch_E7, 100},
    {Song::Pitch_C7, 100},
    {Song::Pitch_D7, 100},
    {Song::Pitch_B7, 100},
    {Song::Pitch_F7, 100},
    {Song::Pitch_C8, 100}
};
const int r2d2_nr_notes = sizeof(r2d2_notes) / sizeof(*r2d2_notes);
const Song::Note popcorn_notes[] = {
    {Song::Pitch_A5, 150},
    {Song::Pitch_G4, 150},
    {Song::Pitch_A5, 150},
    {Song::Pitch_E4, 150},
    {Song::Pitch_C4, 150},
    {Song::Pitch_E4, 150},
    {Song::Pitch_A3, 300},

    {Song::Pitch_A5, 150},
    {Song::Pitch_G4, 150},
    {Song::Pitch_A5, 150},
    {Song::Pitch_E4, 150},
    {Song::Pitch_C4, 150},
    {Song::Pitch_E4, 150},
    {Song::Pitch_A3, 300},

    {Song::Pitch_A5, 150},
    {Song::Pitch_B5, 150},
    {Song::Pitch_C6, 150},
    {Song::Pitch_B5, 150},
    {Song::Pitch_C6, 150},
    {Song::Pitch_A5, 150},
    {Song::Pitch_B5, 150},
    {Song::Pitch_A5, 150},
    {Song::Pitch_B5, 150},
    {Song::Pitch_G5, 150},
    {Song::Pitch_A5, 150},
    {Song::Pitch_G5, 150},
    {Song::Pitch_A5, 150},
    {Song::Pitch_F5, 150},
    {Song::Pitch_A5, 300},
};
const int popcorn_nr_notes = sizeof(popcorn_notes) / sizeof(*popcorn_notes);

Song r2d2_song(r2d2_notes, r2d2_nr_notes);
Song popcorn_song(popcorn_notes, popcorn_nr_notes);

Song* current_song = nullptr;

ISR(TIMER1_COMPA_vect)
{
    // timer1 interrupt 1kHz
    if (current_song)
    {
        current_song->update();
        if (current_song->finished())
            current_song = nullptr;
    }
}

void setup()
{
    noInterrupts();

    // timer0 used for millis() and delay(), timer2 for tone. That leaves us
    // timer1

    // set timer1 interrupt at 1kHz
    TCCR1A = 0;  // clear TCCR1A register
    TCCR1B = 0;  // same for TCCR1B
    TCNT1  = 0;  //initialize counter value to 0
    // set compare match register for 1kHz increments
    OCR1A = 249; // = (16*10^6) / (1000*64) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS01 and CS00 bits for 64 prescaler
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    interrupts();

    if (debug)
        Serial.begin(9600);
}

void loop()
{
    if (!current_song)
    {
        popcorn_song.start(piezo_pin);
        current_song = &popcorn_song;
    }

    delay(3000);
}
