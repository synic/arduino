/*
    Simon Game Clone

    A game of memory and skill!

    circuit:
    
    * Blue LED connected to digital pin 0
    * Yellow LED connected to digital pin 1
    * Red LEDs connected to digital pins 2 and 7
    * Green LED connected to digital pin 3
    * 8 OHM speaker connected to digital pin 6
    * buttons connected to digital pins 8, 9, 10, and 11

    created 10 Dec 2013
    by Adam Olsen <arolsen@gmail.com>
*/

#include "pitches.h"
#include "ButtonEvent.h"

/* LED setup */
const int LED0 = 0;
const int LED1 = 1;
const int LED2 = 2;
const int LED3 = 3;
const int ERROR_LED = 7;
const int LEDS[4] = {LED0, LED1, LED2, LED3};  // the four LED output pins

/* Note setup */
const int NOTES_FOR_BUTTON[4] = {NOTE_E3, NOTE_CSHARP3, NOTE_A3, NOTE_E2};
const int ERROR_NOTE = NOTE_A2;
const int WON_NOTE = NOTE_A3;

/* Button setup */
const int BUTTON1 = 8;
const int BUTTON2 = 9;
const int BUTTON3 = 10;
const int BUTTON4 = 11;
const int BUTTONS[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4}; 

/* other */
const int SPEAKER = 6;
const int MAX_LEVELS = 10;


int levelSequence[10] = {0};                // the level sequence
int level = 1;                              // the current level
bool inputMode = false;
int currentStep = -1;
int noteDuration = 700;
int pauseDuration = 500;
bool buttonDown = false;

/**
    Sets up the app
*/
void setup() {
    for(int i = 0; i < 4; i++) {
        ButtonEvent.addButton(LEDS[i], onDown, onUp, NULL, 0, NULL, 0);
        pinMode(LEDS[i], OUTPUT);
    }

    pinMode(ERROR_LED, OUTPUT);
    randomSeed(analogRead(0));
}

void onDown(ButtonInformation* sender) {
    if(!inputMode || buttonDown) return;
    for(int i = 0; i < 4; i++) {
        if(BUTTONS[i] == sender->pin) {
            tone(SPEAKER, NOTES_FOR_BUTTON[i]);
            return;
        }
    }
}

void onUp(ButtonInformation* sender) {
    if(!inputMode || !buttonDown) return;
    noTone(SPEAKER);
    for(int i = 0; i < 4; i++) {
        if(BUTTONS[i] == sender->pin) {
            buttonPressed(i);
            return;
        }
    }
}

/**
    Sets up the level sequence
*/
void setupLevel() {
    currentStep = -1;
    for(int i = 0; i < level; i++) {
        int step = random(0, 4);
        levelSequence[i] = step;
        digitalWrite(LEDS[i], HIGH);
        playTone(NOTES_FOR_BUTTON[step], noteDuration, 0);
        digitalWrite(LEDS[i], LOW);
        delay(pauseDuration);
    }
}

/**
    Plays a note
*/
void playTone(int note, long toneDuration, long pauseDuration) {
    tone(SPEAKER, note, noteDuration);
    noTone(SPEAKER);
    if(pauseDuration > 0) delay(pauseDuration);
}

/**
    Plays the game over sound and lights the red error LED
*/
void playGameOverSound() {
    digitalWrite(ERROR_LED, HIGH);
    playTone(ERROR_NOTE, 1000, 500);
    digitalWrite(ERROR_LED, LOW);
}

/**
    Plays the game won sound and flashes all the LEDs
**/
void playGameWonSound() {
    for(int i = 0; i < 4; i++) {
        digitalWrite(LEDS[i], HIGH);
        playTone(WON_NOTE, 300, 200);
        digitalWrite(LEDS[i], LOW);
    }
}

void buttonPressed(int index) {
    currentStep += 1;
    if(index != levelSequence[currentStep]) {
        playGameOverSound();
        inputMode = false;
        level = 1;
    }

    if(currentStep >= level) {
        inputMode = false;
        level += 1;

        if(level > MAX_LEVELS) {
            playGameWonSound();
            level = 1;
            inputMode = false;
        }
    }
}
        

/** Main loop **/
void loop() {
    if(inputMode == false) {
        setupLevel();
        inputMode = true;
    }

    ButtonEvent.loop();
}
