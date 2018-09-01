/**
 * The sketch is for communication between PC and any other device using Arduino Uno. With u-blox NEO-7M in the case.
 *
 * Because the Arduino Uno is used, which has only 1 USART, we have to deal with not quite stable `SoftwareSerial`
 * library. If there is one another hardware USART on your board, it's better to use it and replace `ss` with a
 * `Serial2`, for example.
 *
 * u-blox NEO-7M - Arduino Uno
 * VCC - 5V
 * RX - 3
 * TX - 2
 * GND - GND
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#define PC_BAUDRATE 9600
#define GPS_BAUDRATE 9600
#define GPS_RX 3
#define GPS_TX 2

SoftwareSerial ss(GPS_TX, GPS_RX);

void setup()
{
    Serial.begin(PC_BAUDRATE);
    ss.begin(GPS_BAUDRATE);
}

// If there is a data from the receiver, read it and send to the PC or vice versa.
void loop()
{
    if (ss.available())
    {
        Serial.write(ss.read());
    }

    if (Serial.available())
    {
        ss.write(Serial.read());
    }
}
