#include "engine.h"
#include "song.h"

const bool debug = true;

// Interval of timer1 interrupt in microseconds
const uint16_t timer1_us = 50;
// number of timer ticks for music update
const uint16_t music_ticks = 1000 / timer1_us;
// Trigger distance sensor every trigger_ticks ticks
const uint16_t trigger_ticks = 100000ul / timer1_us;
// Maximum valid distance reading, values above this are ignored
const uint16_t max_dist = 500;

const int trigger_pin = 3; // Distance sensor trigger pin
const int echo_pin = 2;    // Distance sensor echo pin
const int piezo_pin = 8;   // Piezo element pin

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Engine engine(&AFMS);

Song r2d2_song("AGECDBFcAGECDBFc", 100, 7);
Song popcorn_song("aGaECEA,zaGaECEA,zabc'bc'ababgagafa");
Song* current_song = nullptr;

volatile uint16_t cur_music_tick = 1;
volatile uint16_t cur_trigger_tick = 1;

volatile uint32_t echo_start = 0;
volatile uint16_t last_distance = 0;

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
            current_song = nullptr;
    }
}

void triggerDistancePulse()
{
    static volatile uint8_t pulse_state = 0;

    if (--cur_trigger_tick == 0)
    {
        cur_trigger_tick = trigger_ticks;
        pulse_state = 1;
    }

    if (pulse_state == 1)
    {
        digitalWrite(trigger_pin, HIGH);
        pulse_state = 2;
    }
    else if (pulse_state == 2)
    {
        digitalWrite(trigger_pin, LOW);
        pulse_state = 0;
    }
}

ISR(TIMER1_COMPA_vect)
{
    // Update music
    if (--cur_music_tick == 0)
    {
        cur_music_tick = music_ticks;
        updateMusic();
    }

    triggerDistancePulse();
}

inline uint16_t echoToCentimeter(uint32_t us)
{
    // Sound of speed under normal conditions is 340 m/s
    // Divide by two since the echo pulse has to travel back and forth
    return (17*us) / 1000;
}

void handleDistanceEcho()
{
    if (digitalRead(echo_pin) == HIGH)
    {
        // Start of echo pulse
        echo_start = micros();
    }
    else
    {
        // End of echo pulse
        uint16_t dist = echoToCentimeter(micros() - echo_start);
        if (dist <= max_dist)
            last_distance = dist;
    }
}

void setup()
{
    if (debug)
        Serial.begin(9600);

    pinMode(trigger_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
    pinMode(piezo_pin, OUTPUT);

    setupTimer(timer1_us);

    attachInterrupt(digitalPinToInterrupt(echo_pin), handleDistanceEcho, CHANGE);

    AFMS.begin();  // create with the default frequency 1.6KHz
}

void loop()
{
    static DriveState state = HALT;
    static uint8_t speed = 0;

    if (!current_song)
    {
        //popcorn_song.start(piezo_pin);
        //current_song = &popcorn_song;
    }

    uint16_t dist = last_distance;
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
            {
                speed = max(speed+1, 128);
                engine.moveForward(speed);
            }
            break;
        case SLOW_1:
            if (speed < 127)
            {
                speed = max(speed+1, 64);
                engine.moveForward(speed);
            }
            else if (speed > 127)
            {
                speed = min(speed-1, 192);
                engine.moveForward(speed);
            }
            break;
        case SLOW_2:
            if (speed < 63)
            {
                speed = max(speed+1, 32);
                engine.moveForward(speed);
            }
            else if (speed > 63)
            {
                speed = min(speed-1, 98);
                engine.moveForward(speed);
            }
            break;
        case TURNING:
            if (speed > 0)
            {
                speed = 0;
                engine.halt();
            }
            engine.turnLeftForward(128, 255);
            break;
    }

    if (debug)
    {
        Serial.print("dist = ");
        Serial.print(dist);
        Serial.print(", state = ");
        Serial.print(state);
        Serial.print(", speed = ");
        Serial.println(speed);
    }

    delay(20);
}
