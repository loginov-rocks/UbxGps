/**
 * The sketch restores the GPS receiver default configuration and configures it to get NAV-PVT messages with 100 ms
 * frequency and 115200 baudrate. After the auto-configuration, it transmits the data from the GPS receiver to the
 * computer and vice versa.
 *
 * u-blox NEO-7M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#include <Arduino.h>
#include <UbxGpsConfig.h>

#define COMPUTER_BAUDRATE 115200
#define GPS_BAUDRATE 115200

void setup()
{
    Serial.begin(COMPUTER_BAUDRATE);

    UbxGpsConfig<HardwareSerial, HardwareSerial> *ubxGpsConfig = new UbxGpsConfig<HardwareSerial, HardwareSerial>(Serial3, Serial);
    ubxGpsConfig->setBaudrate(GPS_BAUDRATE);
    ubxGpsConfig->setMessage(UbxGpsConfigMessage::NavPvt);
    ubxGpsConfig->setRate(100);
    ubxGpsConfig->configure();
    delete ubxGpsConfig;

    Serial3.begin(GPS_BAUDRATE);
}

void loop()
{
    // If there is data from the GPS receiver, read it and send it to the computer or vice versa.
    if (Serial3.available())
    {
        Serial.write(Serial3.read());
    }

    if (Serial.available())
    {
        Serial3.write(Serial.read());
    }
}
