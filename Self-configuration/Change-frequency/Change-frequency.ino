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

#define PC_BAUDRATE     9600
#define GPS_BAUDRATE    115200

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

void setup() {
    Serial.begin(PC_BAUDRATE);
    Serial3.begin(GPS_BAUDRATE);

    // Print ready packet just for debug
    printPacket(packet, sizeof(packet));

    // Send packet to the receiver
    for (byte i = 0; i < sizeof(packet); i++) {
        Serial3.write(packet[i]);
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

