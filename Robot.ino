#include "engine.h"
#include "song.h"

const bool debug = false;

const int trig_pin = 2;  // Ultra-sound sensor trigger pin
const int echo_pin = 4;  // Ultra-sound sensor echo pin
const int piezo_pin = 8; // Piezo element pin

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Engine engine(&AFMS);

Song r2d2_song("AGECDBFcAGECDBFc", 100, 7);
Song popcorn_song("aGaECEA,zaGaECEA,zabc'bc'ababgagafa");

Song* current_song = nullptr;

void setupTimer()
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
}

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
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
    pinMode(piezo_pin, OUTPUT);

    setupTimer();

    if (debug)
        Serial.begin(9600);

    AFMS.begin();  // create with the default frequency 1.6KHz
}

void loop()
{
    if (!current_song)
    {
        popcorn_song.start(piezo_pin);
        current_song = &popcorn_song;
    }

    for (int i = 0; i < 128; ++i)
    {
        engine.moveForward(i);
        delay(10);
    }
    engine.turnLeftForward(128, 64);
    delay(1000);
    engine.halt();
    delay(500);
    engine.turnRightForward(128, 200);
    delay(1000);
    for (int i = 128; i != 0; --i)
    {
        engine.moveForward(i);
        delay(10);
    }

    delay(1000);
}
