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
const int LED0 = 12;
const int LED1 = 11;
const int LED2 = 10;
const int LED3 = 9;
const int ERROR_LED = 7;
const int LEDS[4] = {LED0, LED1, LED2, LED3};  // the four LED output pins

/* Note setup */
const int NOTES_FOR_BUTTON[4] = {NOTE_E3, NOTE_CSHARP3, NOTE_A3, NOTE_E2};
const int ERROR_NOTE = NOTE_A2;
const int WON_NOTE = NOTE_A3;

/* Button setup */
const int BUTTON1 = 5;
const int BUTTON2 = 2;
const int BUTTON3 = 6;
const int BUTTON4 = 4;
const int BUTTONS[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4}; 

/* other */
const int SPEAKER = 3;
const int MAX_LEVELS = 50;
const int NEXT_GAME_PAUSE_DURATION = 800; 
const bool INCREASE_SPEED = true;
const int INCREASE_SPEED_LEVELS = 5; // number of levels before speed increase
const int INCREASE_SPEED_AMOUNT = 30; // number of ms to increase the speed
const int MAX_SPEED = 200;           // max game speed
const int INPUT_TIMEOUT = 6000;      // amount of time you have to move
const int INITIAL_NOTE_DURATION = 500;
const int PAUSE_DURATION = 200;

int levelSequence[10] = {0};                // the level sequence
int level = 1;                              // the current level
bool inputMode = false;
int currentStep = -1;
int noteDuration = INITIAL_NOTE_DURATION;
bool buttonDown = false;
unsigned long lastButtonPress = 0;

/**
    Sets up the app
*/
void setup() {
    Serial.begin(9600);
    for(int i = 0; i < 4; i++) {
        Serial.print("Adding button: ");
        Serial.println(BUTTONS[i]);
        ButtonEvent.addButton(BUTTONS[i], onDown, onUp, NULL, 0, NULL, 0);
        pinMode(LEDS[i], OUTPUT);
    }

    pinMode(ERROR_LED, OUTPUT);
    randomSeed(analogRead(0));
}

void onDown(ButtonInformation* sender) {
    if(!inputMode || buttonDown) return;

    for(int i = 0; i < 4; i++) {
        if(BUTTONS[i] == sender->pin) {
            digitalWrite(LEDS[i], HIGH);
            tone(SPEAKER, NOTES_FOR_BUTTON[i]);
            buttonDown = true;
            lastButtonPress = millis();
            return;
        }
    }
}

void onUp(ButtonInformation* sender) {
    if(!inputMode || !buttonDown) return;

    noTone(SPEAKER);
    for(int i = 0; i < 4; i++) {
        if(BUTTONS[i] == sender->pin) {
            digitalWrite(LEDS[i], LOW);
            buttonPressed(i);
            buttonDown = false;
            return;
        }
    }
}

/**
    Sets up the level sequence
*/
void setupLevel() {
    // check to see if we should increase speed
    if(INCREASE_SPEED && level % INCREASE_SPEED_LEVELS) {
        int mult = level % INCREASE_SPEED_LEVELS;
        noteDuration -= INCREASE_SPEED_AMOUNT * mult;
        if(noteDuration < MAX_SPEED) {
            noteDuration = MAX_SPEED;
        }
    }

    Serial.print("Setting up level: ");
    Serial.println(level);
    currentStep = -1;
    Serial.print("Sequence: "); 
    for(int i = 0; i < level; i++) {
        int step = random(0, 4);
        Serial.print(step + 1);
        Serial.print(" ");
        levelSequence[i] = step;
        digitalWrite(LEDS[step], HIGH);
        playTone(NOTES_FOR_BUTTON[step], noteDuration, 0);
        digitalWrite(LEDS[step], LOW);
        delay(PAUSE_DURATION);
    }
    Serial.println("");
    lastButtonPress = millis();
}

/**
    Plays a note
*/
void playTone(int note, long toneDuration, long PAUSE_DURATION) {
    tone(SPEAKER, note, noteDuration);
    delay(noteDuration);
    noTone(SPEAKER);
    if(PAUSE_DURATION > 0) delay(PAUSE_DURATION);
}

/**
    Plays the game over sound and lights the red error LED
*/
void gameOver() {
    resetGame();
    Serial.println("Game friggin over.");
    digitalWrite(ERROR_LED, HIGH);
    playTone(ERROR_NOTE, 1000, 500);
    delay(NEXT_GAME_PAUSE_DURATION);
    digitalWrite(ERROR_LED, LOW);
    delay(NEXT_GAME_PAUSE_DURATION);
}

/**
    Plays the game won sound and flashes all the LEDs
**/
void gameWon() {
    resetGame();
    delay(200);
    Serial.println("You won!");
    for(int i = 0; i < 8; i++) {
        for(int a = 0; a < 4; a++) {
            digitalWrite(LEDS[a], HIGH);
            tone(SPEAKER, WON_NOTE, 100);
            delay(50);
            noTone(SPEAKER);
            digitalWrite(LEDS[a], LOW);
        }
    }

    delay(NEXT_GAME_PAUSE_DURATION);
}

void resetGame() {
    noteDuration = INITIAL_NOTE_DURATION;
    level = 1;
    inputMode = false;
}

void buttonPressed(int index) {
    currentStep += 1;
    if(index != levelSequence[currentStep]) {
        gameOver();
        return;
    }

    if(currentStep + 1 >= level) {
        inputMode = false;
        level += 1;

        if(level > MAX_LEVELS) {
            gameWon();
            return;
        }

        Serial.print("Continuing to level:");
        Serial.println(level); 
        delay(NEXT_GAME_PAUSE_DURATION);
    }
}
        

/** Main loop **/
void loop() {
    if(inputMode == false) {
        setupLevel();
        inputMode = true;
    }
    else {
        // check to see if they've taken too long to press a button
        if(millis() - lastButtonPress > INPUT_TIMEOUT) {
            gameOver();
            return;
        }
    }

    ButtonEvent.loop();
}
