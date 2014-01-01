#include "TrinketFakeUsbSerial.h"

#define LED_PIN 3

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    TFUSerial.begin();
}

void loop()
{
    TFUSerial.println("this is a god damn test.");
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW);
    delay(10);
}
