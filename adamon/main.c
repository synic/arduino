#define F_CPU 8000000UL
#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "pitches.h"

#define LED1 PA3
#define LED2 PA2
#define LED3 PA1
#define LED4 PA0

#define BUTTON1 PA7
#define BUTTON2 PA6
#define BUTTON3 PA5
#define BUTTON4 PA4

#define ERROR_TONE NOTE_C3
#define MAX_LEVELS 50
#define NEXT_GAME_PAUSE_DURATION 800
#define INCREASE_SPEED_LEVELS 3
#define INCREASE_SPEED_AMOUNT 20
#define PAUSE_DURATION 200
#define INITIAL_TONE_DURATION 500

const int TONES_FOR_BUTTON[5] = {NOTE_E3, NOTE_CS3, NOTE_A3, NOTE_E2,
    NOTE_A3};
const int LEDS[4] = {LED1, LED2, LED3, LED4};
const int BUTTONS[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};

int level_sequence[10];
int level;
int input_mode;
int current_step;
int tone_duration;
int button_down;
unsigned long last_button_press;
unsigned int randint;

void _delay(int delay) {
    for(int i = 0; i < delay; i++) _delay_ms(1);
}

void tone(int frequency, int delay) {
    OCR0A = ((F_CPU / 1024.0) / (frequency * 2.0)) - 1;
    TCCR0A = _BV(WGM01) | _BV(COM0A0);   // toggle output, ctc mode
    TCCR0B = _BV(CS02) | _BV(CS00);      // set prescaler to 1024 
    _delay(delay);
    PORTB |= (0 << PB2);                 // turn port PB2 off
    TCCR0B = 0x00;                       // turn the timer CTC mode off
}

void setup_level() {
    if(INCREASE_SPEED && level % INCREASE_SPEED_LEVELS == 0) {
        int mult = level / INCREASE_SPEED_LEVELS;
        tone_duration -= INCREASE_SPEED_AMOUNT * mult;
        if(tone_duration < MAX_SPEED) {
            tone_duration = MAX_SPEED;
        }
    }

    current_step = -1;

    for(int i = 0; i < level; i++) {
        int step = rand() % 4;
        level_sequence[i] = step;
        led_on(LEDS[step]);
        tone(TONES_FOR_BUTTON[step], tone_duration, 0);
        led_off(LEDS[step]);
        _delay(PAUSE_DURATION);
    }

    last_button_press = 
}

int main(void) {
    // get a random number to seed with
    srand(eeprom_read_word(&randint));
    eeprom_write_word(&randint, rand());

    // initialize variables
    level = 1;
    input_mode = 0;
    current_step = -1;
    tone_duration = INITIAL_TONE_DURATION;
    button_down = 0;
    last_button_press = 0L;
    DDRB = _BV(PB2); // set PB2 as an output

    



    for(;;);
}
