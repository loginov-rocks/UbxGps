#ifndef UBXGPSNAVPVT8_H_INCLUDED
#define UBXGPSNAVPVT8_H_INCLUDED

#include <UbxGps.h>

template <class T>
class UbxGpsNavPvt8 : public UbxGps<T>
{
public:
  // Type         Name            Unit  Description (Scaling)
  unsigned long   iTOW;       //  ms    GPS time of week of the navigation epoch. See the description of iTOW for
                              //        details.
  unsigned short  year;       //  y     Year (UTC)
  unsigned char   month;      //  month Month, range 1..12 (UTC)
  unsigned char   day;        //  d     Day of month, range 1..31 (UTC)
  unsigned char   hour;       //  h     Hour of day, range 0..23 (UTC)
  unsigned char   min;        //  min   Minute of hour, range 0..59 (UTC)
  unsigned char   sec;        //  s     Seconds of minute, range 0..60 (UTC)
  char            valid;      //        Validity flags (see graphic below)
  unsigned long   tAcc;       //  ns    Time accuracy estimate (UTC)
  long            nano;       //  ns    Fraction of second, range -1e9 .. 1e9 (UTC)
  unsigned char   fixType;    //        GNSSfix Type:
                              //        0: no fix
                              //        1: dead reckoning only
                              //        2: 2D-fix
                              //        3: 3D-fix
                              //        4: GNSS + dead reckoning combined
                              //        5: time only fix
  char            flags;      //        Fix status flags (see graphic below)
  char            flags2;     //        Additional flags (see graphic below)
  unsigned char   numSV;      //        Number of satellites used in Nav Solution
  long            lon;        //  deg   Longitude (1e-7)
  long            lat;        //  deg   Latitude (1e-7)
  long            height;     //  mm    Height above ellipsoid
  long            hMSL;       //  mm    Height above mean sea level
  unsigned long   hAcc;       //  mm    Horizontal accuracy estimate
  unsigned long   vAcc;       //  mm    Vertical accuracy estimate
  long            velN;       //  mm/s  NED north velocity
  long            velE;       //  mm/s  NED east velocity
  long            velD;       //  mm/s  NED down velocity
  long            gSpeed;     //  mm/s  Ground Speed (2-D)
  long            headMot;    //  deg   Heading of motion (2-D) (1e-5)
  unsigned long   sAcc;       //  mm/s  Speed Accuracy Estimate
  unsigned long   headAcc;    //  deg   Heading accuracy estimate (both motion and vehicle) (1e-5)
  unsigned short  pDOP;       //        Position DOP (0.01)
  short           flags3;     //        Additional flags (see graphic below)
  unsigned char   reserved1;  //        Reserved
  long            headVeh;    //  deg   Heading of vehicle (2-D), this is only valid when headVehValid is set,
                              //        otherwise the output is set to the heading of motion (1e-5)
  short           magDec;     //  deg   Magnetic declination. Only supported in ADR 4.10 and later. (1e-2)
  unsigned short  magAcc;     //  deg   Magnetic declination accuracy. Only supported in ADR 4.10 and later. (1e-2)

  UbxGpsNavPvt8(T &serial) : UbxGps<T>(serial)
  {
    this->setLength(92);
  }
};

#endif
