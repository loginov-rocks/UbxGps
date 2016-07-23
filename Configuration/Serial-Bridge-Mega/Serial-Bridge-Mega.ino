/**
 * UBX GPS Library
 * Created by Danila Loginov, July 23, 2016
 * https://github.com/1oginov/UBX-GPS-Library
 *
 * Sketch for communication between PC and any other device through Arduino Mega. With U-blox NEO-7M in this case.
 * Скетч для общения между компьютером и любым другим устройством через Arduino Mega. В данном примере - с U-blox NEO-7M.
 *
 * U-blox NEO-7M - Arduino Mega
 * VCC - 5V
 * RX - TX3
 * TX - RX3
 * GND - GND
 */

#define PC_BAUDRATE   9600
#define GPS_BAUDRATE  9600

void setup() {
    Serial.begin(PC_BAUDRATE);
    Serial3.begin(GPS_BAUDRATE);
}

/**
 * If there is data from the device, read it and send to the PC or vice versa.
 * Если есть данные от устройства, то считать и отправить их компьютеру, и наоборот.
 */
void loop() {

    if (Serial3.available()) {
        Serial.write(Serial3.read());
    }

    if (Serial.available()) {
        Serial3.write(Serial.read());
    }

}
