/**
 * The sketch restores the receiver default configuration and configure it to get NAV-PVT messages with 100 ms
 * frequency and 115200 baudrate. After the auto-configuration, it transmits the data from the receiver to the PC and
 * vice versa.
 *
 * u-blox NEO-7M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#include <Arduino.h>

#define PC_SERIAL Serial
#define PC_BAUDRATE 115200L
#define GPS_SERIAL Serial3

// Default baudrate is determined by the receiver manufacturer.
#define GPS_DEFAULT_BAUDRATE 9600L

// Wanted buadrate at the moment can be 9600L (not changed after defaults) or 115200L (changed by the
// `changeBaudrate()` function with a prepared message).
#define GPS_WANTED_BAUDRATE 115200L

// Array of possible baudrates that can be used by the receiver, sorted descending to prevent excess Serial flush/begin
// after restoring defaults. You can uncomment values that can be used by your receiver before the auto-configuration.
const long possibleBaudrates[] = {
    //921600L,
    //460800L,
    //230400L,
    115200L,
    //57600L,
    //38400L,
    //19200L,
    9600L,
    //4800L,
};

void setup()
{
    PC_SERIAL.begin(PC_BAUDRATE);
    PC_SERIAL.println("Starting auto-configuration...");

    // Restore the receiver default configuration.
    for (byte i = 0; i < sizeof(possibleBaudrates) / sizeof(*possibleBaudrates); i++)
    {
        PC_SERIAL.print("Trying to restore defaults at ");
        PC_SERIAL.print(possibleBaudrates[i]);
        PC_SERIAL.println(" baudrate...");

        if (i != 0)
        {
            delay(100); // Little delay before flushing.
            GPS_SERIAL.flush();
        }

        GPS_SERIAL.begin(possibleBaudrates[i]);
        restoreDefaults();
    }

    // Switch the receiver serial to the default baudrate.
    if (possibleBaudrates[sizeof(possibleBaudrates) / sizeof(*possibleBaudrates) - 1] != GPS_DEFAULT_BAUDRATE)
    {
        PC_SERIAL.print("Switching to the default baudrate which is ");
        PC_SERIAL.print(GPS_DEFAULT_BAUDRATE);
        PC_SERIAL.println("...");

        delay(100); // Little delay before flushing.
        GPS_SERIAL.flush();
        GPS_SERIAL.begin(GPS_DEFAULT_BAUDRATE);
    }

    // Disable NMEA messages by sending appropriate packets.
    PC_SERIAL.println("Disabling NMEA messages...");
    disableNmea();

    // Switch the receiver serial to the wanted baudrate.
    if (GPS_WANTED_BAUDRATE != GPS_DEFAULT_BAUDRATE)
    {
        PC_SERIAL.print("Switching receiver to the wanted baudrate which is ");
        PC_SERIAL.print(GPS_WANTED_BAUDRATE);
        PC_SERIAL.println("...");

        changeBaudrate();

        delay(100); // Little delay before flushing.
        GPS_SERIAL.flush();
        GPS_SERIAL.begin(GPS_WANTED_BAUDRATE);
    }

    // Increase frequency to 100 ms.
    PC_SERIAL.println("Changing receiving frequency to 100 ms...");
    changeFrequency();

    // Disable unnecessary channels like SBAS or QZSS.
    PC_SERIAL.println("Disabling unnecessary channels...");
    disableUnnecessaryChannels();

    // Enable NAV-PVT messages.
    PC_SERIAL.println("Enabling NAV-PVT messages...");
    enableNavPvt();

    PC_SERIAL.println("Auto-configuration is complete!");

    delay(100); // Little delay before flushing.
    GPS_SERIAL.flush();
}

// Send a packet to the receiver to restore default configuration.
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

// Send a set of packets to the receiver to disable NMEA messages.
void disableNmea()
{
    // Array of two bytes for CFG-MSG packets payload.
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
        0x00, // payload (not changed in the case)
        0x00, // CK_A
        0x00, // CK_B
    };
    byte packetSize = sizeof(packet);

    // Offset to the place where payload starts.
    byte payloadOffset = 6;

    // Iterate over the messages array.
    for (byte i = 0; i < sizeof(messages) / sizeof(*messages); i++)
    {
        // Copy two bytes of payload to the packet buffer.
        for (byte j = 0; j < sizeof(*messages); j++)
        {
            packet[payloadOffset + j] = messages[i][j];
        }

        // Set checksum bytes to the null.
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

// Send a packet to the receiver to change baudrate to 115200.
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

// Send a packet to the receiver to change frequency to 100 ms.
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

// Send a packet to the receiver to disable unnecessary channels.
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

// Send a packet to the receiver to enable NAV-PVT messages.
void enableNavPvt()
{
    // CFG-MSG packet.
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x01, // id
        0x03, // length
        0x00, // length
        0x01, // payload
        0x07, // payload
        0x01, // payload
        0x13, // CK_A
        0x51, // CK_B
    };

    sendPacket(packet, sizeof(packet));
}

// Send the packet specified to the receiver.
void sendPacket(byte *packet, byte len)
{
    for (byte i = 0; i < len; i++)
    {
        GPS_SERIAL.write(packet[i]);
    }

    printPacket(packet, len);
}

// Print the packet specified to the PC serial in a hexadecimal form.
void printPacket(byte *packet, byte len)
{
    char temp[3];

    for (byte i = 0; i < len; i++)
    {
        sprintf(temp, "%.2X", packet[i]);
        PC_SERIAL.print(temp);

        if (i != len - 1)
        {
            PC_SERIAL.print(' ');
        }
    }

    PC_SERIAL.println();
}

// If there is a data from the receiver, read it and send to the PC or vice versa.
void loop()
{
    if (GPS_SERIAL.available())
    {
        PC_SERIAL.write(GPS_SERIAL.read());
    }

    if (PC_SERIAL.available())
    {
        GPS_SERIAL.write(PC_SERIAL.read());
    }
}
