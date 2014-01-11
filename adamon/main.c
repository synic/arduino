#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "pitches.h"

const int TONES_FOR_BUTTON[5] = {NOTE_E3, NOTE_CS3, NOTE_A3, NOTE_E2,
    NOTE_A3};

const int LED1 = PA3;
const int LED2 = PA2;
const int LED3 = PA1;
const int LED4 = PA0;

const int LEDS[4] = {LED1, LED2, LED3, LED4};

const int BUTTON1 = PA7;
const int BUTTON2 = PA6;
const int BUTTON3 = PA5;
const int BUTTON4 = PA4;

const int BUTTONS[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};
const int ERROR_TONE = NOTE_C3;

const int MAX_LEVELS = 50;
const int NEXT_GAME_PAUSE_DURATION = 800;
const bool INCREASE_SPEED = true;

void tone(int frequency, int delay) {
    OCR0A = ((F_CPU / 1024.0) / (frequency * 2.0)) - 1;
    TCCR0A = _BV(WGM01) | _BV(COM0A0);   // toggle output, ctc mode
    TCCR0B = _BV(CS02) | _BV(CS00);      // set prescaler to 1024 
    for(int i = 0; i < delay; i++) _delay_ms(1);
    PORTB |= (0 << PB2);
    TCCR0B = 0x00;
}

int main(void) {
    int count = 0;
    DDRB = _BV(PB2);



    while(1) {
        if(count < 4) {
            tone(TONES_FOR_BUTTON[count], 1500);
            count++;
        }
    }
}
