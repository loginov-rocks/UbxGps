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

#define PC_BAUDRATE   9600
#define GPS_BAUDRATE  9600

// Array of CFG-MSG payload
const byte messages[][2] = {
    { 0xF0, 0x0A },
    { 0xF0, 0x09 },
    { 0xF0, 0x00 },
    { 0xF0, 0x01 },
    { 0xF0, 0x0D },
    { 0xF0, 0x06 },
    { 0xF0, 0x02 },
    { 0xF0, 0x07 },
    { 0xF0, 0x03 },
    { 0xF0, 0x04 },
    { 0xF0, 0x0E },
    { 0xF0, 0x0F },
    { 0xF0, 0x05 },
    { 0xF0, 0x08 },
    { 0xF1, 0x00 },
    { 0xF1, 0x01 },
    { 0xF1, 0x03 },
    { 0xF1, 0x04 },
    { 0xF1, 0x05 },
    { 0xF1, 0x06 },
};

// Offset to the place where payload starts
const byte payloadOffset = 6;

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

void setup() {
    Serial.begin(PC_BAUDRATE);
    Serial3.begin(GPS_BAUDRATE);

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

        // Print ready packet just for debug
        printPacket(packet, sizeof(packet));

        // Send packet to the receiver
        for (byte j = 0; j < sizeof(packet); j++) {
            Serial3.write(packet[j]);
        }

    }
}

void loop() {

    if (Serial3.available()) {
        Serial.write(Serial3.read());
    }

    if (Serial.available()) {
        Serial3.write(Serial.read());
    }

}

// Print packet to the Serial like that:
// B5 62 06 01 03 00 F0 0A 00 04 23
void printPacket(byte *packet, byte len) {
    char temp[2];

    for (byte i = 0; i < len; i++) {
        sprintf(temp, "%.2X", packet[i]);
        Serial.print(temp);
        if (i != len - 1) {
            Serial.print(' ');
        }
    }

    Serial.println();
}
