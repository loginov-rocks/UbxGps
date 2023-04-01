#ifndef UBXGPS_H_INCLUDED
#define UBXGPS_H_INCLUDED

#include <Arduino.h>

const unsigned char UBXGPS_HEADER[] = {0xB5, 0x62};

template <class T = HardwareSerial>
class UbxGps
{
private:
  // Class properties.
  T &serial;
  unsigned char offsetClassProperties = 8;
  unsigned char offsetHeaders = 4;
  unsigned char size;
  unsigned char carriagePosition;
  unsigned char checksum[2];

  // Headers (common).
  unsigned char headerClass;
  unsigned char headerId;
  unsigned short headerLength;

  int available()
  {
    return serial.available();
  }

  byte read()
  {
    return serial.read();
  }

  void calculateChecksum()
  {
    memset(checksum, 0, 2);

    for (int i = 0; i < size; i++)
    {
      checksum[0] += ((unsigned char *)(this))[i + offsetClassProperties];
      checksum[1] += checksum[0];
    }
  }

protected:
  UbxGps(T &serial) : serial(serial)
  {
    carriagePosition = 0;
  }

  void setLength(unsigned char length)
  {
    size = length + offsetHeaders;
  }

public:
  void begin(unsigned long baudrate)
  {
    serial.begin(baudrate);
  }

  void begin(unsigned long baudrate, long speed, uint16_t tx, uint16_t rx)
  {
    serial.begin(baudrate, speed, tx, rx);
  }

  boolean ready()
  {
    unsigned char p = carriagePosition;

    while (available())
    {
      byte c = read();

      // Carriage is at the first or the second sync byte, should be equals.
      if (p < 2)
      {
        if (c == UBXGPS_HEADER[p])
        {
          p++;
        }
        // Reset if not.
        else
        {
          p = 0;
        }
      }

      // Sync with header after success.
      else
      {
        // Put the byte read to a particular address of this object which depends on the carriage position.
        if (p < (size + 2))
        {
          ((unsigned char *)(this))[p - 2 + offsetClassProperties] = c;
        }

        // Move the carriage forward.
        p++;

        // Carriage is at the first checksum byte, we can calculate our checksum, but not compare, because this byte is
        // not read.
        if (p == (size + 2))
        {
          calculateChecksum();
        }
        // Carriage is at the second checksum byte, but only the first byte of checksum read, check if it equals to
        // ours.
        else if (p == (size + 3))
        { 
          // Reset if not.
          if (c != checksum[0])
          {
            p = 0;
          }
        }
        // Carriage is after the second checksum byte, which has been read, check if it equals to ours.
        else if (p == (size + 4))
        {
          // Reset the carriage.
          p = 0;

          // The readings are correct and filled the object, return true.
          if (c == checksum[1])
          {
            carriagePosition = p;

            return true;
          }
        }
        // Reset the carriage if it is out of a packet.
        else if (p > (size + 4))
        {
          p = 0;
        }
      }
    }

    carriagePosition = p;

    return false;
  }
};

#endif
