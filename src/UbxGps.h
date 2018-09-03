#ifndef UBXGPS_H_
#define UBXGPS_H_

#include <Arduino.h>

const unsigned char UBXGPS_HEADER[] = {0xB5, 0x62};

template <class T = HardwareSerial>
class UbxGps
{
public:
  void begin(long speed)
  {
    return this->serial.begin(speed);
  };

  boolean ready()
  {
    unsigned char p = this->carriagePosition;

    while (this->available())
    {
      byte c = this->read();

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
        if (p < (this->size + 2))
        {
          ((unsigned char *)(this))[p - 2 + this->offsetClassProperties] = c;
        }

        // Move the carriage forward.
        p++;

        // Carriage is at the first checksum byte, we can calculate our checksum, but not compare, because this byte is
        // not read.
        if (p == (this->size + 2))
        {
          this->calculateChecksum();
        }
        // Carriage is at the second checksum byte, but only the first byte of checksum read, check if it equals to
        // ours.
        else if (p == (this->size + 3))
        {
          // Reset if not.
          if (c != this->checksum[0])
          {
            p = 0;
          }
        }
        // Carriage is after the second checksum byte, which has been read, check if it equals to ours.
        else if (p == (this->size + 4))
        {
          // Reset the carriage.
          p = 0;

          // The readings are correct and filled the object, return true.
          if (c == this->checksum[1])
          {
            this->carriagePosition = p;
            return true;
          }
        }
        // Reset the carriage if it is out of a packet.
        else if (p > (this->size + 4))
        {
          p = 0;
        }
      }
    }

    this->carriagePosition = p;

    return false;
  };

protected:
  UbxGps(T &serial) : serial(serial)
  {
    this->carriagePosition = 0;
  };

  void setLength(unsigned char length)
  {
    this->size = length + this->offsetHeaders;
  };

private:
  int available()
  {
    return this->serial.available();
  };

  byte read()
  {
    return this->serial.read();
  };

  void calculateChecksum()
  {
    memset(this->checksum, 0, 2);

    for (int i = 0; i < this->size; i++)
    {
      this->checksum[0] += ((unsigned char *)(this))[i + this->offsetClassProperties];
      this->checksum[1] += this->checksum[0];
    }
  };

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
};

#endif
