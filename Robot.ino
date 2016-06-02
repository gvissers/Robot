#include "engine.h"
#include "eyes.h"
#include "settings.h"
#include "song.h"

const bool debug = false;

// Minimum distance from walls for full speed cruising
const int min_cruise_dist = 100;
// Minimum distance to wall, soft and hard limits
const int min_dist_soft = 20;
const int min_dist_hard = 15;

// Create the motor shield object with the default I2C address 0x60
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
            popcorn_song.start();
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

//    eyes.ultrasoundTick();
    eyes.infraredTick();
}

void setup()
{
    if (debug)
        Serial.begin(9600);

//    pinMode(US_trigger_pin, OUTPUT);
//    pinMode(US_echo_pin, INPUT);
    pinMode(piezo_pin, OUTPUT);

    setupTimer(timer1_us);

//    attachInterrupt(digitalPinToInterrupt(US_echo_pin),
//        []() { eyes.handleUltrasoundEcho(); }, CHANGE);

    AFMS.begin();  // create with the default frequency 1.6KHz

    r2d2_song.start();
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
        return;

    uint16_t dist = eyes.distance();
    if (debug)
    {
        Serial.print("dist = ");
        Serial.println(dist);
    }
    sleep_until = now + 10;
    if (dist > min_cruise_dist)
    {
        if (speed < 255)
            engine.moveForward(++speed);
    }
    else if (dist > min_dist_hard || (state == CRUISING && dist > min_dist_soft))
    {
        uint8_t new_speed = 20 + (255 - 20) * (dist - min_dist_hard) / (min_cruise_dist - min_dist_hard);
        if (new_speed != speed)
        {
            speed = new_speed;
            engine.moveForward(speed);
        }
        state = CRUISING;
    }
    else
    {
        if (speed > 0)
        {
            speed = 0;
            engine.halt();
        }
        if (state != TURNING)
        {
            engine.turnLeftForward(128, 255);
            state = TURNING;
        }
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
