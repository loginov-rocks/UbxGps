#ifndef UBXGPSCONFIG_H_INCLUDED
#define UBXGPSCONFIG_H_INCLUDED

#include <Arduino.h>

// The default baudrate is determined by the GPS receiver manufacturer.
#define GPS_DEFAULT_BAUDRATE 9600

enum class UbxGpsConfigMessage
{
    NavPosecef = 0x01,
    NavPossllh = 0x02,
    NavPvt = 0x07,
    NavSol = 0x06,
};

// An array of possible baudrates that can be used by the GPS receiver, sorted descending to prevent excess Serial
// flush/begin after restoring defaults. You can uncomment values that can be used by your GPS receiver before the
// auto-configuration.
const long gpsPossibleBaudrates[] = {
    // 921600,
    // 460800,
    // 230400,
    115200,
    // 57600,
    // 38400,
    // 19200,
    9600,
    // 4800,
};

template <class GpsSerial = HardwareSerial, class LogSerial = HardwareSerial>
class UbxGpsConfig
{
private:
    GpsSerial &gpsSerial;
    LogSerial &logSerial;

    unsigned long baudrate;
    UbxGpsConfigMessage message;
    unsigned short rate;
    bool customPins = false;
    uint16_t tx;
    uint16_t rx;

    // Print a packet to the log serial port in a hexadecimal form.
    void printPacket(byte *packet, byte len)
    {
        char temp[3];

        for (byte i = 0; i < len; i++)
        {
            sprintf(temp, "%.2X", packet[i]);
            logSerial.print(temp);

            if (i != len - 1)
            {
                logSerial.print(' ');
            }
        }

        logSerial.println();
    }

    // Send a packet to the GPS receiver.
    void sendPacket(byte *packet, byte len)
    {
        for (byte i = 0; i < len; i++)
        {
            gpsSerial.write(packet[i]);
        }

        printPacket(packet, len);
    }

    // Send a packet to the GPS receiver to restore the default configuration.
    void restoreDefaults()
    {
        // CFG-CFG packet.
        byte packet[] = {
            0xB5, // sync char 1
            0x62, // sync char 2
            0x06, // class
            0x09, // id
            0x0D, // length
            0x00, // length
            0xFF, // payload
            0xFF, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0xFF, // payload
            0xFF, // payload
            0x00, // payload
            0x00, // payload
            0x17, // payload
            0x2F, // CK_A
            0xAE, // CK_B
        };

        sendPacket(packet, sizeof(packet));
    }

    // Send a set of packets to the GPS receiver to disable NMEA messages.
    void disableNmea()
    {
        // An array of two bytes for CFG-MSG packets payload.
        byte messages[][2] = {
            {0xF0, 0x0A},
            {0xF0, 0x09},
            {0xF0, 0x00},
            {0xF0, 0x01},
            {0xF0, 0x0D},
            {0xF0, 0x06},
            {0xF0, 0x02},
            {0xF0, 0x07},
            {0xF0, 0x03},
            {0xF0, 0x04},
            {0xF0, 0x0E},
            {0xF0, 0x0F},
            {0xF0, 0x05},
            {0xF0, 0x08},
            {0xF1, 0x00},
            {0xF1, 0x01},
            {0xF1, 0x03},
            {0xF1, 0x04},
            {0xF1, 0x05},
            {0xF1, 0x06},
        };

        // CFG-MSG packet buffer.
        byte packet[] = {
            0xB5, // sync char 1
            0x62, // sync char 2
            0x06, // class
            0x01, // id
            0x03, // length
            0x00, // length
            0x00, // payload (first byte from messages array element)
            0x00, // payload (second byte from messages array element)
            0x00, // payload (not changed in this case)
            0x00, // CK_A
            0x00, // CK_B
        };
        byte packetSize = sizeof(packet);

        // Offset to a place where payload starts.
        byte payloadOffset = 6;

        // Iterate over the messages array.
        for (byte i = 0; i < sizeof(messages) / sizeof(*messages); i++)
        {
            // Copy two bytes of payload to the packet buffer.
            for (byte j = 0; j < sizeof(*messages); j++)
            {
                packet[payloadOffset + j] = messages[i][j];
            }

            // Set checksum bytes to null.
            packet[packetSize - 2] = 0x00;
            packet[packetSize - 1] = 0x00;

            // Calculate checksum over the packet buffer excluding sync (first two) and checksum chars (last two).
            for (byte j = 0; j < packetSize - 4; j++)
            {
                packet[packetSize - 2] += packet[2 + j];
                packet[packetSize - 1] += packet[packetSize - 2];
            }

            sendPacket(packet, packetSize);
        }
    }

    // Send a packet to the GPS receiver to change the baudrate to 115200.
    void changeBaudrate()
    {
        // CFG-PRT packet.
        byte packet[] = {
            0xB5, // sync char 1
            0x62, // sync char 2
            0x06, // class
            0x00, // id
            0x14, // length
            0x00, // length
            0x01, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0xD0, // payload
            0x08, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0xC2, // payload
            0x01, // payload
            0x00, // payload
            0x07, // payload
            0x00, // payload
            0x03, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0x00, // payload
            0xC0, // CK_A
            0x7E, // CK_B
        };

        sendPacket(packet, sizeof(packet));
    }

    // Send a packet to the GPS receiver to change the frequency to 100 ms.
    void changeFrequency()
    {
        // CFG-RATE packet.
        byte packet[] = {
            0xB5, // sync char 1
            0x62, // sync char 2
            0x06, // class
            0x08, // id
            0x06, // length
            0x00, // length
            0x64, // payload
            0x00, // payload
            0x01, // payload
            0x00, // payload
            0x01, // payload
            0x00, // payload
            0x7A, // CK_A
            0x12, // CK_B
        };

        sendPacket(packet, sizeof(packet));
    }

    // Send a packet to the GPS receiver to disable unnecessary channels.
    void disableUnnecessaryChannels()
    {
        // CFG-GNSS packet.
        byte packet[] = {
            0xB5, // sync char 1
            0x62, // sync char 2
            0x06, // class
            0x3E, // id
            0x24, // length
            0x00, // length

            0x00, 0x00, 0x16, 0x04, 0x00, 0x04, 0xFF, 0x00, // payload
            0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x03, 0x00, // payload
            0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x03, 0x00, // payload
            0x00, 0x00, 0x00, 0x01, 0x06, 0x08, 0xFF, 0x00, // payload
            0x00, 0x00, 0x00, 0x01,                         // payload

            0xA4, // CK_A
            0x25, // CK_B
        };

        sendPacket(packet, sizeof(packet));
    }

    // Send a packet to the GPS receiver to enable NAV-PVT messages.
    void enableNavPvt()
    {
        // CFG-MSG packet.
        byte packet[] = {
            0xB5,          // sync char 1
            0x62,          // sync char 2
            0x06,          // class
            0x01,          // id
            0x03,          // length
            0x00,          // length
            0x01,          // payload
            (byte)message, // payload
            0x01,          // payload
            // TODO: Update checksum to fit message other than 0x07.
            0x13, // CK_A
            0x51, // CK_B
        };

        sendPacket(packet, sizeof(packet));
    }

public:
    UbxGpsConfig(GpsSerial &gpsSerial, LogSerial &logSerial) : gpsSerial(gpsSerial),
                                                               logSerial(logSerial)
    {
        //
    }

    // The target buadrate at the moment can be 9600 (not changed after defaults) or 115200 (changed by the
    // `changeBaudrate()` function with a special message).
    void setBaudrate(unsigned long _baudrate)
    {
        baudrate = _baudrate;
    }

    void setCustomPins(uint16_t _tx, uint16_t _rx)
    {
        customPins = true;
        tx = _tx;
        rx = _rx;
    }

    void setMessage(UbxGpsConfigMessage _message)
    {
        message = _message;
    }

    void setRate(unsigned short _rate)
    {
        rate = _rate;
    }

    long findCurrentBaudrate(int recpin)
    {
        long baud, rate = 10000, x;
        for (int i = 0; i < 10; i++)
        {
            x = pulseIn(recpin, LOW); // measure the next zero bit width
            rate = x < rate ? x : rate;
        }

        if (rate < 12)
            baud = 115200;
        else if (rate < 20)
            baud = 57600;
        else if (rate < 29)
            baud = 38400;
        else if (rate < 40)
            baud = 28800;
        else if (rate < 60)
            baud = 19200;
        else if (rate < 80)
            baud = 14400;
        else if (rate < 150)
            baud = 9600;
        else if (rate < 300)
            baud = 4800;
        else if (rate < 600)
            baud = 2400;
        else if (rate < 1200)
            baud = 1200;
        else
            baud = 0;
        return baud;
    }

    void configure()
    {
        logSerial.println("Starting auto-configuration...");

        // Restore the default GPS receiver configuration.
        for (byte i = 0; i < sizeof(gpsPossibleBaudrates) / sizeof(*gpsPossibleBaudrates); i++)
        {
            logSerial.print("Trying to restore defaults at ");
            logSerial.print(gpsPossibleBaudrates[i]);
            logSerial.println(" baudrate...");

            if (i != 0)
            {
                delay(100); // Little delay before the flush.
                gpsSerial.flush();
            }

            if (customPins)
            {
                gpsSerial.begin(gpsPossibleBaudrates[i], SERIAL_8N1, tx, rx);
            }
            else
            {
                gpsSerial.begin(gpsPossibleBaudrates[i]);
            }

            restoreDefaults();
        }

        logSerial.print("Current Baudrate determined at ");
        logSerial.print(findCurrentBaudrate(rx));
        logSerial.println(" bps...");
        
        // Switch the GPS receiver serial configuration to the default baudrate.
        if (gpsPossibleBaudrates[sizeof(gpsPossibleBaudrates) / sizeof(*gpsPossibleBaudrates) - 1] != GPS_DEFAULT_BAUDRATE)
        {
            logSerial.print("Switching to the default baudrate which is ");
            logSerial.print(GPS_DEFAULT_BAUDRATE);
            logSerial.println("...");

            delay(100); // Little delay before the flush.
            gpsSerial.flush();

            if (customPins)
            {
                gpsSerial.begin(GPS_DEFAULT_BAUDRATE, SERIAL_8N1, tx, rx);
            }
            else
            {
                gpsSerial.begin(GPS_DEFAULT_BAUDRATE);
            }

        }

        // Disable NMEA messages by sending appropriate packets.
        logSerial.println("Disabling NMEA messages...");
        disableNmea();

        // Switch the GPS receiver serial configuration to the target baudrate.
        if (baudrate != GPS_DEFAULT_BAUDRATE)
        {
            logSerial.print("Switching to the target baudrate which is ");
            logSerial.print(baudrate);
            logSerial.println("...");

            changeBaudrate();

            delay(100); // Little delay before the flush.
            gpsSerial.flush();
            if (customPins)
            {
                gpsSerial.begin(baudrate, SERIAL_8N1, tx, rx);
            }
            else
            {
                gpsSerial.begin(baudrate);
            }
        }

        if (rate == 100)
        {
            // Change receiving frequency to 100 ms.
            logSerial.println("Changing receiving frequency to 100 ms...");
            changeFrequency();
        }

        // Disable unnecessary channels like SBAS or QZSS.
        logSerial.println("Disabling unnecessary channels...");
        disableUnnecessaryChannels();

        // Enable NAV-PVT messages.
        logSerial.println("Enabling NAV-PVT messages...");
        enableNavPvt();

        logSerial.println("Auto-configuration is complete!");

        delay(100); // Little delay before the flush.
        gpsSerial.flush();
    }

    ~UbxGpsConfig()
    {
        gpsSerial.end();
    }
};

#endif
