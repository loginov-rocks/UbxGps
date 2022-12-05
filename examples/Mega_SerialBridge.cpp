/**
 * The sketch establishes serial communication between the terminal on the computer and the UART device with Arduino
 * Mega as a mediator. With u-blox NEO-7M in this case.
 *
 * u-blox NEO-7M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#include <Arduino.h>

#define COMPUTER_BAUDRATE 9600
#define GPS_BAUDRATE 9600

void setup()
{
    Serial.begin(COMPUTER_BAUDRATE);
    Serial3.begin(GPS_BAUDRATE);
}

void loop()
{
    // If there is data from the receiver, read it and send it to the computer or vice versa.
    if (Serial3.available())
    {
        Serial.write(Serial3.read());
    }

    if (Serial.available())
    {
        Serial3.write(Serial.read());
    }
}
