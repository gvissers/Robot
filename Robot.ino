#include "engine.h"
#include "eyes.h"
#include "settings.h"
#include "song.h"

const bool debug = false;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Engine engine(&AFMS);

const char r2d2_desc[] PROGMEM = "AGECDBFcAGECDBFc";
const char popcorn_desc[] PROGMEM = "aGaECEA,zaGaECEA,zabc'bc'ababgagafaz";
Song r2d2_song(r2d2_desc, 100, 7);
Song popcorn_song(popcorn_desc);
Song* current_song = nullptr;

Eyes eyes;

volatile bool update_music_now = false;

enum DriveState
{
    CRUISING,
    SLOW_1,
    SLOW_2,
    TURNING,
    HALT
};

void setupTimer(long us)
{
    // timer0 used for millis() and delay(), timer2 for tone. That leaves us
    // timer1
    static const uint16_t max_cycles = 0xffff; // counter for timer1 is 16bit

    uint32_t cycles = uint32_t(us) * (F_CPU / 1000000);
    uint8_t bits = (1 << WGM12);          // turn on CTC mode
    if (cycles <= max_cycles)
        bits |= (1 << CS10);              // prescaler = 1;
    else if ((cycles >>= 3) <= max_cycles)
        bits |= (1 << CS11);              // prescaler = 8;
    else if ((cycles >>= 3) <= max_cycles)
        bits |= (1 << CS11) | (1 << CS10);// prescaler = 64;
    else if ((cycles >>= 2) <= max_cycles)
        bits |= (1 << CS12);              // prescaler = 256;
    else if ((cycles >>= 2) <= max_cycles)
        bits |= (1 << CS12) | (1 << CS10);// prescaler = 1024;
    // else timeout to large, disable timer

    if (debug)
    {
        Serial.print("interval = ");
        Serial.print(us);
        Serial.print(", bits = ");
        Serial.print((bits ^ (1 << WGM12)), 2);
        Serial.print(", cycles = ");
        Serial.println(cycles);
    }

    noInterrupts();

    TCCR1A = 0;              // clear TCCR1A register
    TCNT1  = 0;              // initialize counter value to 0
    OCR1A = int(cycles);
    TCCR1B = bits;
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

    interrupts();
}

void updateMusic()
{
    if (current_song)
    {
        current_song->update();
        if (current_song->finished())
        {
            popcorn_song.start(piezo_pin);
            current_song = &popcorn_song;
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    static uint16_t cur_music_tick = 1;

    // Update music
    if (--cur_music_tick == 0)
    {
        cur_music_tick = music_ticks;
        update_music_now = true;
    }

    eyes.ultrasoundTick();
}

void setup()
{
    if (debug)
        Serial.begin(9600);

    pinMode(US_trigger_pin, OUTPUT);
    pinMode(US_echo_pin, INPUT);
    pinMode(piezo_pin, OUTPUT);

    setupTimer(timer1_us);

    attachInterrupt(digitalPinToInterrupt(US_echo_pin),
        []() { eyes.handleUltrasoundEcho(); }, CHANGE);

    AFMS.begin();  // create with the default frequency 1.6KHz

    r2d2_song.start(piezo_pin);
    current_song = &r2d2_song;
}

void loop()
{
    static DriveState state = HALT;
    static uint8_t speed = 0;
    static uint32_t sleep_until = 0;

    uint32_t now = millis();

    if (update_music_now)
    {
        update_music_now = false;
        updateMusic();
    }

    if (now < sleep_until)
    {
        delay(1);
        return;
    }

    uint16_t dist = eyes.distance();
    if (dist > 100)
        state = CRUISING;
    else if (dist > 60)
        state = SLOW_1;
    else if (dist > 20)
        state = SLOW_2;
    else
        state = TURNING;

    switch (state)
    {
        case CRUISING:
            if (speed < 255)
                engine.moveForward(++speed);
            sleep_until = now + 10;
            break;
        case SLOW_1:
            if (speed < 127)
            {
                engine.moveForward(++speed);
            }
            else if (speed > 127)
            {
                speed = min(speed-1, 192);
                engine.moveForward(speed);
            }
            sleep_until = now + 10;
            break;
        case SLOW_2:
            if (speed < 63)
            {
                engine.moveForward(++speed);
            }
            else if (speed > 63)
            {
                speed = min(speed-1, 98);
                engine.moveForward(speed);
            }
            sleep_until = now + 10;
            break;
        case TURNING:
            if (speed > 0)
            {
                speed = 0;
                engine.halt();
            }
            engine.turnLeftForward(128, 255);
            sleep_until = now + 640;
            break;
    }

//     if (debug)
//     {
//         Serial.print("dist = ");
//         Serial.print(dist);
//         Serial.print(", state = ");
//         Serial.print(state);
//         Serial.print(", speed = ");
//         Serial.println(speed);
//     }
}
