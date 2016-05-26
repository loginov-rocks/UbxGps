/**
 *
 * Arduino GPS 10 Hz to GMaps
 * https://github.com/1oginov/Arduino-GPS-10-Hz-to-GMaps
 *
 * Sketch, that parses UBX messages from U-blox NEO-7M and outputs
 * ready GPS fata to the serial port in the CSV format.
 *
 * Скетч, обрабатывающий UBX сообщения от U-box NEO-7M и выводящий
 * выводящий готовые GPS данные в последовательный порт в CSV формате.
 *
 * U-blox NEO-7M - Arduino Uno
 * VCC - 5V
 * RX - 3
 * TX - 2
 * GND - GND
 *
 */

#define PC_BAUDRATE     9600
#define GPS_BAUDRATE    9600
#define GPS_RX          3
#define GPS_TX          2
#define DATETIME_FORMAT "%04d.%02d.%02d %02d:%02d:%02d"
#define DATETIME_LENGTH 20

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

/**
 * The header of UBX packet, not changing for different messages.
 * Заголовок UBX пакета, не меняется для различных сообщений.
 */
const unsigned char GPS_UBX_HEADER[] = {0xB5, 0x62};

/**
 * Structure, representing payload of specific UBX message, NAV-PVT in this case.
 * All fields taken from Protocol Specification V14.
 * 
 * Структура, представляющая тело конкретного UBX сообщения, NAV-PVT в данном
 * примере. Все поля взяты из Protocol Specification V14.
 */
struct GPS_UBX_NAV_PVT {
  unsigned  char  _class;
  unsigned  char  _id;
  unsigned  short _length;

  /**
   *        Type  Name           Unit   Descrption (scaling)
   */
  unsigned  long  iTOW;       // ms     GPS time of week of the navigation epoch. See the description of iTOW for details.
  unsigned  short year;       // y      Year UTC
  unsigned  char  month;      // month  Month, range 1..12 UTC
  unsigned  char  day;        // d      Day of month, range 1..31 UTC
  unsigned  char  hour;       // h      Hour of day, range 0..23 UTC
  unsigned  char  min;        // min    Minute of hour, range 0..59 UTC
  unsigned  char  sec;        // s      Seconds of minute, range 0..60 UTC
            char  valid;      // -      Validity Flags (see graphic below)
  unsigned  long  tAcc;       // ns     Time accuracy estimate UTC
            long  nano;       // ns     Fraction of second, range -1e9..1e9 UTC
  unsigned  char  fixType;    // -      GNSSfix Type, range 0..5
            char  flags;      // -      Fix Status Flags (see graphic below)
  unsigned  char  reserved1;  // -      Reserved
  unsigned  char  numSV;      // -      Number of satellites used in Nav Solution
            long  lon;        // deg    Longitude (1e-7)
            long  lat;        // deg    Latitude (1e-7)
            long  height;     // mm     Height above Ellipsoid
            long  hMSL;       // mm     Height above mean sea level
  unsigned  long  hAcc;       // mm     Horizontal Accuracy Estimate
  unsigned  long  vAcc;       // mm     Vertical Accuracy Estimate
            long  velN;       // mm/s   NED north velocity
            long  velE;       // mm/s   NED east velocity
            long  velD;       // mm/s   NED down velocity
            long  gSpeed;     // mm/s   Ground Speed (2-D)
            long  heading;    // deg    Heading of motion 2-D (1e-5)
  unsigned  long  sAcc;       // mm/s   Speed Accuracy Estimate
  unsigned  long  headingAcc; // deg    Heading Accuracy Estimate (1e-5)
  unsigned  short pDOP;       // -      Position DOP (0.01)
            short reserved2;  // -      Reserved
  unsigned  long  reserved3;  // -      Reserved
};

GPS_UBX_NAV_PVT gps;
char datetime[DATETIME_LENGTH];

void setup() {
  Serial.begin(PC_BAUDRATE);
  ss.begin(GPS_BAUDRATE);
}

void loop() {
  if (getGps()) {
    snprintf(datetime, DATETIME_LENGTH, DATETIME_FORMAT, gps.year, gps.month, gps.day, gps.hour, gps.min, gps.sec);
    Serial.print(datetime); Serial.print(',');
    Serial.print(gps.lon / 10000000.0, 7); Serial.print(',');
    Serial.print(gps.lat / 10000000.0, 7); Serial.print(',');
    Serial.print(gps.height / 1000.0, 3); Serial.print(',');
    Serial.print(gps.gSpeed * 0.0036, 5); Serial.print(',');
    Serial.print(gps.heading  / 100000.0, 5); Serial.print(',');
    Serial.print(gps.fixType); Serial.print(',');
    Serial.println(gps.numSV);
  }
}

/**
 * Calculate checksum, algorithm is the same for all messages.
 * Калькулирование контрольной суммы, алгоритм одинаковый для всех сообщений.
 */
void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int) sizeof(GPS_UBX_NAV_PVT); i++) {
    CK[0] += ((unsigned char*) (&gps))[i];
    CK[1] += CK[0];
  }
}

/**
 * Parse input data, algorithm is the same for all messages. Returns true
 * if data is ready, false otherwise.
 * 
 * Обработка поступающих данных, алгоритм одинаковый для всех сообщений.
 * Возвращает true в случае готовности данных, false в противном случае.
 */
bool getGps() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(GPS_UBX_NAV_PVT);

  while (ss.available()) {
    byte c = ss.read();
    if (fpos < 2) {
      if (c == GPS_UBX_HEADER[fpos]) {
        fpos++;
      }
      else {
        fpos = 0;
      }
    }
    else {
      if ((fpos - 2) < payloadSize) {
        ((unsigned char*) (&gps))[fpos - 2] = c;
      }

      fpos++;

      if (fpos == (payloadSize + 2)) {
        calcChecksum(checksum);
      }
      else if (fpos == (payloadSize + 3)) {
        if (c != checksum[0]) {
          fpos = 0;
        }
      }
      else if (fpos == (payloadSize + 4)) {
        fpos = 0;
        if (c == checksum[1]) {
          return true;
        }
      }
      else if (fpos > (payloadSize + 4)) {
        fpos = 0;
      }
    }
  }
  return false;
}

