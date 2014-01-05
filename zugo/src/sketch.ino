#include <Pushbutton.h>
#include <ZumoMotors.h>
#include <ZumoBuzzer.h>

#define LED_PIN 13
#define BATTERY_PIN A1

ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);

unsigned long getBatteryVoltage() {
    float voltage = (analogRead(A1) * 5000L * (3/2) / 1023);
    return voltage / 1000.0;
}

void signalBatteryVoltage() {
    float voltage = getBatteryVoltage();
    for(int i = 0; i < (int)voltage; i++) {
        buzzer.playNote(NOTE_E(4), 100, 15);
        delay(250);
        buzzer.stopPlaying();
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    if(button.getSingleDebouncedRelease()) {
        signalBatteryVoltage();
    }
}
