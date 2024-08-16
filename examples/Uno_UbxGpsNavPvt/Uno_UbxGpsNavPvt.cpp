/**
 * The sketch parses UBX messages from u-blox NEO-7M and outputs ready GPS data to a serial port in a CSV format.
 *
 * Please be aware because Arduino Uno is used and it only has one UART, we deal with software implementation for
 * serial communication, which is unstable. Use the hardware serial if available!
 *
 * u-blox NEO-7M - Arduino Uno
 * VCC - 5V
 * RX - 3
 * TX - 2
 * GND - GND
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <UbxGpsNavPvt.h>

#define COMPUTER_BAUDRATE 115200
#define GPS_BAUDRATE 115200
#define GPS_RX 3
#define GPS_TX 2

#define DATETIME_FORMAT "%04d.%02d.%02d %02d:%02d:%02d"
#define DATETIME_LENGTH 20

SoftwareSerial ss(GPS_TX, GPS_RX);
UbxGpsNavPvt<SoftwareSerial> gps(ss);

char datetime[DATETIME_LENGTH];

void setup()
{
    Serial.begin(COMPUTER_BAUDRATE);
    gps.begin(GPS_BAUDRATE);
}

void loop()
{
    if (gps.ready())
    {
        snprintf(datetime, DATETIME_LENGTH, DATETIME_FORMAT, gps.year, gps.month, gps.day, gps.hour, gps.min, gps.sec);

        Serial.print(datetime);
        Serial.print(',');
        Serial.print(gps.lon / 10000000.0, 7);
        Serial.print(',');
        Serial.print(gps.lat / 10000000.0, 7);
        Serial.print(',');
        Serial.print(gps.height / 1000.0, 3);
        Serial.print(',');
        Serial.print(gps.gSpeed * 0.0036, 5);
        Serial.print(',');
        Serial.print(gps.heading / 100000.0, 5);
        Serial.print(',');
        Serial.print(gps.fixType);
        Serial.print(',');
        Serial.println(gps.numSV);
    }
}
