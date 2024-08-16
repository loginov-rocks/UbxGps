/**
 * The sketch parses UBX messages from u-blox NEO-8M and outputs ready GPS data to a serial port in a CSV format.
 *
 * u-blox NEO-8M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#include <Arduino.h>
#include <UbxGpsNavPvt8.h>

#define COMPUTER_BAUDRATE 115200
#define GPS_BAUDRATE 115200

#define DATETIME_FORMAT "%04d.%02d.%02d %02d:%02d:%02d"
#define DATETIME_LENGTH 20

UbxGpsNavPvt8<HardwareSerial> gps(Serial3);

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
        Serial.print(gps.headMot / 100000.0, 5);
        Serial.print(',');
        Serial.print(gps.fixType);
        Serial.print(',');
        Serial.println(gps.numSV);
    }
}
