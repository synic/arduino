#define F_CPU 8000000UL
#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define NOTE_E3 165
#define NOTE_CS3 139
#define NOTE_A3 220
#define NOTE_E2 82
#define NOTE_C3 131
#define NOTE_A5 880

#define LED1 PA3
#define LED2 PA2
#define LED3 PA1
#define LED4 PA0

#define BUTTON1 PA7
#define BUTTON2 PA6
#define BUTTON3 PA5
#define BUTTON4 PA4

// game mode DIP switch
#define GAMEMODE1 PB1
#define GAMEMODE2 PB0

#define ERROR_TONE NOTE_C3
#define WON_TONE NOTE_A5
#define MAX_LEVELS 50
#define NEXT_GAME_PAUSE_DURATION 800
#define INCREASE_SPEED_LEVELS 3
#define INCREASE_SPEED_AMOUNT 20
#define PAUSE_DURATION 200
#define INITIAL_TONE_DURATION 500
#define MAX_SPEED 200

const int TONES_FOR_BUTTON[5] = {NOTE_E3, NOTE_CS3, NOTE_A3, NOTE_E2,
    NOTE_A3};
const int LEDS[4] = {LED1, LED2, LED3, LED4};
const int BUTTONS[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};

int button_pressed[4];
int level_sequence[10];
int increase_speed;
int level;
int input_mode;
int current_step;
int tone_duration;
int button_down;
unsigned long last_button_press;
unsigned int randint;

void _delay(int delay);
void tone(int frequency, int delay);
void led_on(int led);
void led_off(int led);
void setup_level(void);
void reset_game(void);
void game_over(void);
void game_won(void);

void _delay(int delay) {
    for(int i = 0; i < delay; i++) _delay_ms(1);
}

void no_tone(void) {
    PORTB |= (0 << PB2);
    TCCR0B = 0x00;
}

void tone(int frequency, int delay) {
    OCR0A = ((F_CPU / 1024.0) / (frequency * 2.0)) - 1;
    TCCR0A = _BV(WGM01) | _BV(COM0A0);   // toggle output, ctc mode
    TCCR0B = _BV(CS02) | _BV(CS00);      // set prescaler to 1024 

    if(delay > -1) {
        _delay(delay);
        no_tone();
    }
}

void led_on(int led) {
    PORTA |= (1 << led);
}

void led_off(int led) {
    PORTA |= (0 << led);
}

void setup_level(void) {
    if(increase_speed && level % INCREASE_SPEED_LEVELS == 0) {
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
        tone(TONES_FOR_BUTTON[step], tone_duration);
        led_off(LEDS[step]);
        _delay(PAUSE_DURATION);
    }
}

void reset_game(void) {
    tone_duration = INITIAL_TONE_DURATION;
    level = 1;
    input_mode = 0;
}

void game_over(void) {
    reset_game();
    for(int i = 0; i < 4; i++) {
        led_on(LEDS[i]);
    }
    tone(ERROR_TONE, 1000);
    _delay(NEXT_GAME_PAUSE_DURATION * 2);
    for(int i = 0; i < 4; i++) {
        led_off(LEDS[i]);
    }
}

void game_won(void) {
    reset_game();
    _delay(200);
    for(int i = 0; i < 8; i++) {
        for(int a = 0; a < 4; a++) {
            led_on(LEDS[a]);
            tone(WON_TONE, 100);
            _delay(50);
            led_off(LEDS[a]);
        }
    }

    _delay(NEXT_GAME_PAUSE_DURATION);
}

void button_press(int index) {
    // turn LED on for this button
    PORTA |= _BV(LEDS[index]);

    // play tone for this button
    tone(TONES_FOR_BUTTON[index], -1);
}

void button_release(int index) {
    // turn LED off for this button
    PORTA |= (0 << LEDS[index]);

    // turn tone off for this button
    no_tone();
    
    current_step += 1;
    if(index != level_sequence[current_step]) {
        game_over();
        return;
    }

    if(current_step + 1 >= level) {
        input_mode = 0;
        level ++;

        if(level > MAX_LEVELS) {
            game_won();
            return;
        }

        _delay(NEXT_GAME_PAUSE_DURATION);
    }
}

void check_button_press(int index) {
    if(bit_is_clear(PINA, BUTTONS[index])) {
        _delay(50);
        if(bit_is_clear(PINA, BUTTONS[index])) {
            button_pressed[index] = 1;
            button_press(index);
        }
    }
    
    if(button_pressed[index] && !bit_is_clear(PINA, BUTTONS[index])) {
        _delay(50);
        if(!bit_is_clear(PINA, BUTTONS[index])) {
            button_pressed[index] = 0;
            button_release(index);
        }
    }
}

int main(void) {
    // set PB2 as an output
    DDRB = _BV(PB2); 

    // set LED pins as outputs
    DDRA = _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);
    
    // enable pullup resistors on the button pins
    PORTA |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3) | _BV(BUTTON4);

    // enable pullup resistors on the game mode dip switches
    PORTB |= _BV(GAMEMODE1) | _BV(GAMEMODE2);

    // get a random number to seed with
    srand(eeprom_read_word(&randint));

    // write the random seed number for next time ;-)
    eeprom_write_word(&randint, rand());

    // initialize variables
    level = 1;
    input_mode = 0;
    current_step = -1;
    tone_duration = INITIAL_TONE_DURATION;
    button_down = 0;
    increase_speed = 1;
    last_button_press = 0L;
    for(int i = 0; i < 4; i++) button_pressed[i] = 0;

    // loop forever
    while(1) { 
        if(!input_mode) {
            setup_level();
            input_mode = 1;
        }
        else {
            // check for button presses
            for(int i = 0; i < 4; i++) {
                check_button_press(i);
            }
        }
    }
}
