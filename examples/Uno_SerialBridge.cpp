/**
 * The sketch establishes serial communication between the terminal on the
 * computer and the UART device with Arduino Uno as a mediator. With u-blox
 * NEO-7M in this case.
 *
 * Please be aware because Arduino Uno is used and it only has one UART, we
 * deal with software implementation for serial communication, which is
 * unstable. Use the hardware serial if available!
 *
 * u-blox NEO-7M - Arduino Uno
 * VCC - 5V
 * RX - 3
 * TX - 2
 * GND - GND
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#define COMPUTER_BAUDRATE 9600
#define GPS_BAUDRATE 9600
#define GPS_RX 3
#define GPS_TX 2

SoftwareSerial ss(GPS_TX, GPS_RX);

void setup()
{
    Serial.begin(COMPUTER_BAUDRATE);
    ss.begin(GPS_BAUDRATE);
}

void loop()
{
    // If there is data from the receiver, read it and send it to the computer
    // or vice versa.
    if (ss.available())
    {
        Serial.write(ss.read());
    }

    if (Serial.available())
    {
        ss.write(Serial.read());
    }
}
