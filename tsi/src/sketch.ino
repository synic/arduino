#include "HIDSerial.h"

HIDSerial serial;
const int led = 3;


void setup()
{
	serial.begin();
}

void loop()
{
	serial.println("farts");
}
