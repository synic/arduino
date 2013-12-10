#include "pitches.h"

const int NOTES[4] = {NOTE_E3, NOTE_CSHARP3, NOTE_A3, NOTE_E2};
const int ERROR_NOTE = NOTE_A2;

const int LEDS[4] = {0, 1, 2, 3};           // the four LED output pins
const int ERROR_LED = 7;
const int NOTE_LEDS = {LED[0], LED[1], LED[2], LED[3], ERROR_LED};

const int BUTTONS[4] = {8, 9, 10, 11};      // the four button input pins
const int SPEAKER = 6;

const int MAX_LEVELS = 10;
int levelSequence[10] = {0};                // the level sequence
int level = 1;                              // the current level
bool inputMode = false;
int currentStep = -1;
int noteDuration = 500;
int pauseDuration = 500;

void setup() {
    for(int i = 0; i < 4; i++) {
        pinMode(LEDS[i], OUTPUT);
        pinMode(BUTTONS[i], INPUT);
    }

    pinMode(ERROR_LED, OUTPUT);
    randomSeed(analogRead(0));
}

void setupLevel() {
    currentStep = -1;
    for(int i = 0; i < level; i++) {
        int step = random(0, 4);
        levelSequence[i] = step;
        playNote(NOTES[step]);
    }
}

void playNote(int note) {
    digitalWrite(NOTE_LEDS[note], HIGH);
    tone(SPEAKER, note, noteDuration);
    delay(pauseDuration);
    noTone(SPEAKER);
    digitalWrite(NOTE_LEDS[note], LOW);
}

int buttonPressed() {
    for(int i = 0; i < 4; i++) {
        if(digitalRead(BUTTONS[i] == HIGH) {
            return i;
        }
    }

    return -1;
}

void loop() {
    if(inputMode == false) {
        setupLevel(level);
        inputMode = true;
    }

    int pressed = buttonPressed();
    if(pressed > -1) {
        currentStep += 1;
        if(pressed != levelSequence[currentStep]) {
            gameOver(); // play game over sound, start over
            inputMode = false;
            level = 1;
            return;
        }

        if(currentStep >= level) {
            inputMode = false;
            level += 1;

            if(level > MAX_LEVELS) {
                gameWon();
                level = 1;
                inputMode = false;
            }

            return; // go to the next level!
        }

        playNote(NOTES[pressed]);
    }
}
