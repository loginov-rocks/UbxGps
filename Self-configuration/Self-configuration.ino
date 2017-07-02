/**
 * UBX GPS Library
 * Created by Danila Loginov, July 2, 2016
 * https://github.com/1oginov/UBX-GPS-Library
 *
 * U-blox NEO-7M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#define PC_SERIAL   Serial
#define PC_BAUDRATE 115200L
#define GPS_SERIAL  Serial3

// Default baudrate is determined by the manufacturer
#define GPS_DEFAULT_BAUDRATE    9600L

// At the moment can be: 9600L (not changed after defaults), 115200L (changed
// by the changeBaudrate() function with prepared message)
#define GPS_WANTED_BAUDRATE     115200L

// Array of possible baudrates that can be used by receiver, sorted descending
// to prevent excess Serial flush/begin after restoring defaults
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

void setup() {
    PC_SERIAL.begin(PC_BAUDRATE);
    PC_SERIAL.println("Starting auto-configuration...");

    for (byte i = 0; i < sizeof(possibleBaudrates) / sizeof(*possibleBaudrates); i++) {
        PC_SERIAL.print("Trying to restore defaults at ");
        PC_SERIAL.print(possibleBaudrates[i]);
        PC_SERIAL.println(" baudrate...");

        if (i != 0) {
            delay(100); // Little delay before flushing
            GPS_SERIAL.flush();
        }

        GPS_SERIAL.begin(possibleBaudrates[i]);
        restoreDefaults();
    }

    if (possibleBaudrates[sizeof(possibleBaudrates) / sizeof(*possibleBaudrates) - 1] != GPS_DEFAULT_BAUDRATE) {
        PC_SERIAL.print("Switching to the default baudrate which is ");
        PC_SERIAL.print(GPS_DEFAULT_BAUDRATE);
        PC_SERIAL.println("...");

        delay(100); // Little delay before flushing
        GPS_SERIAL.flush();
        GPS_SERIAL.begin(GPS_DEFAULT_BAUDRATE);
    }

    PC_SERIAL.println("Disabling NMEA messages...");
    disableNmea();

    if (GPS_WANTED_BAUDRATE != GPS_DEFAULT_BAUDRATE) {
        PC_SERIAL.print("Switching receiver to the wanted baudrate which is ");
        PC_SERIAL.print(GPS_WANTED_BAUDRATE);
        PC_SERIAL.println("...");

        changeBaudrate();

        delay(100); // Little delay before flushing
        GPS_SERIAL.flush();
        GPS_SERIAL.begin(GPS_WANTED_BAUDRATE);
    }

    PC_SERIAL.println("Changing receiving frequency to 100 ms...");
    changeFrequency();

    PC_SERIAL.println("Disabling unnecessary channels...");
    disableUnnecessaryChannels();

    PC_SERIAL.println("Enabling NAV-PVT messages...");
    enableNavPvt();

    PC_SERIAL.println("Auto-configuration is done...");
}

void restoreDefaults() {
    // Packet buffer
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

    // Send packet to the receiver
    sendPacket(packet, sizeof(packet));
}

void disableNmea() {
    // Array of CFG-MSG payload
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

    // Packet buffer
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x01, // id
        0x03, // length
        0x00, // length
        0x00, // payload
        0x00, // payload
        0x00, // payload (not changed in the case)
        0x00, // CK_A
        0x00, // CK_B
    };

    // Offset to the place where payload starts
    byte payloadOffset = 6;

    for (byte i = 0; i < sizeof(messages) / sizeof(*messages); i++) {
        // Copy payload to the packet buffer
        for (byte j = 0; j < sizeof(*messages); j++) {
            packet[payloadOffset + j] = messages[i][j];
        }

        // Set checksum element to the null
        packet[sizeof(packet) - 2] = 0x00;
        packet[sizeof(packet) - 1] = 0x00;

        // Calculate checksum over the packet buffer excluding sync chars
        for (byte j = 0; j < sizeof(packet) - 4; j++) {
            packet[sizeof(packet) - 2] += packet[2 + j];
            packet[sizeof(packet) - 1] += packet[sizeof(packet) - 2];
        }

        // Send packet to the receiver
        sendPacket(packet, sizeof(packet));
    }
}

void changeBaudrate() {
    // Packet buffer
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

    // Send packet to the receiver
    sendPacket(packet, sizeof(packet));
}

void changeFrequency() {
    // Packet buffer
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

    // Send packet to the receiver
    sendPacket(packet, sizeof(packet));
}

void disableUnnecessaryChannels() {
    // Packet buffer
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
        0x00, 0x00, 0x00, 0x01, // payload

        0xA4, // CK_A
        0x25, // CK_B
    };

    // Send packet to the receiver
    sendPacket(packet, sizeof(packet));
}

void enableNavPvt() {
    // Packet buffer
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

    // Send packet to the receiver
    sendPacket(packet, sizeof(packet));
}

void sendPacket(byte *packet, byte len) {
    for (byte i = 0; i < len; i++) {
        GPS_SERIAL.write(packet[i]);
    }

    printPacket(packet, len);
}

// Print packet to the Serial like that:
// B5 62 06 01 03 00 F0 0A 00 04 23
void printPacket(byte *packet, byte len) {
    char temp[3];

    for (byte i = 0; i < len; i++) {
        sprintf(temp, "%.2X", packet[i]);
        PC_SERIAL.print(temp);
        if (i != len - 1) {
            PC_SERIAL.print(' ');
        }
    }

    PC_SERIAL.println();
}

void loop() {

    if (GPS_SERIAL.available()) {
        PC_SERIAL.write(GPS_SERIAL.read());
    }

    if (PC_SERIAL.available()) {
        GPS_SERIAL.write(PC_SERIAL.read());
    }

}

