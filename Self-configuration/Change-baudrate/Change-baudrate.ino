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

#define PC_BAUDRATE         9600
#define GPS_BAUDRATE_BEFORE 9600
#define GPS_BAUDRATE_AFTER  115200

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

void setup() {
    Serial.begin(PC_BAUDRATE);
    Serial3.begin(GPS_BAUDRATE_BEFORE);

    // Print ready packet just for debug
    printPacket(packet, sizeof(packet));

    // Send packet to the receiver
    for (byte i = 0; i < sizeof(packet); i++) {
        Serial3.write(packet[i]);
    }

    Serial3.flush();
    Serial3.begin(GPS_BAUDRATE_AFTER);
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

