#ifndef UBXGPSNAVPOSECEF_H_INCLUDED
#define UBXGPSNAVPOSECEF_H_INCLUDED

#include <UbxGps.h>

template <class T>
class UbxGpsNavPosecef : public UbxGps<T>
{
public:
  // Type       Name        Unit  Description (Scaling)
  unsigned long iTOW;   //  ms    GPS time of week of the navigation epoch. See the description of iTOW for details.
  long          ecefX;  //  cm    ECEF X coordinate
  long          ecefY;  //  cm    ECEF Y coordinate
  long          ecefZ;  //  cm    ECEF Z coordinate
  unsigned long pAcc;   //  cm    Position Accuracy Estimate

  UbxGpsNavPosecef(T &serial) : UbxGps<T>(serial)
  {
    this->setLength(20);
  }
};

#endif
