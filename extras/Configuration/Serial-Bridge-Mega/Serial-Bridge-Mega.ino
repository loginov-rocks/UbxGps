/**
 * The sketch is for communication between PC and any other device using Arduino Mega. With u-blox NEO-7M in the case.
 *
 * u-blox NEO-7M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#include <Arduino.h>

#define PC_BAUDRATE 9600
#define GPS_BAUDRATE 9600

void setup()
{
    Serial.begin(PC_BAUDRATE);
    Serial3.begin(GPS_BAUDRATE);
}

// If there is a data from the receiver, read it and send to the PC or vice versa.
void loop()
{
    if (Serial3.available())
    {
        Serial.write(Serial3.read());
    }

    if (Serial.available())
    {
        Serial3.write(Serial.read());
    }
}
