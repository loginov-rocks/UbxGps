/**
 * UBX GPS Library
 * Created by Danila Loginov, July 23, 2016
 * https://github.com/1oginov/UBX-GPS-Library
 *
 * Sketch for communication between PC and any other device through Arduino Uno. With U-blox NEO-7M in this case.
 * Скетч для общения между компьютером и любым другим устройством через Arduino Uno. В данном примере - с U-blox NEO-7M.
 *
 * U-blox NEO-7M - Arduino Uno
 * VCC - 5V
 * RX - 3
 * TX - 2
 * GND - GND
 */

#define PC_BAUDRATE   9600
#define GPS_BAUDRATE  9600
#define GPS_RX        3
#define GPS_TX        2

/**
 * Because the Arduino Uno is used, which has only 1 USART, we have to deal
 * with not quite stable SoftwareSerial library. If there is one another
 * hardware USART on your board, it's better to use it and replace ss with
 * Serial2, for example.
 *
 * Так как используется Arduino Uno, у которого только 1 USART, приходится
 * задействовать не совсем стабильную библиотеку SoftwareSerial. Если
 * на плате есть еще один аппаратный USART, то лучше использовать его
 * и в коде заменить ss на Serial2, например.
 */
#include <SoftwareSerial.h>
SoftwareSerial ss(GPS_TX, GPS_RX);

void setup() {
    Serial.begin(PC_BAUDRATE);
    ss.begin(GPS_BAUDRATE);
}

/**
 * If there is data from the device, read it and send to the PC or vice versa.
 * Если есть данные от устройства, то считать и отправить их компьютеру, и наоборот.
 */
void loop() {

    if (ss.available()) {
        Serial.write(ss.read());
    }

    if (Serial.available()) {
        ss.write(Serial.read());
    }

}
