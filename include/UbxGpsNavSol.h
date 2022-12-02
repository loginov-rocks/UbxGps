#ifndef UBXGPSNAVSOL_H_
#define UBXGPSNAVSOL_H_

#include <UbxGps.h>

template <class T>
class UbxGpsNavSol : public UbxGps<T>
{
  public:
    // Type         Name           Unit     Description (scaling)
    unsigned long   iTOW;       // ms       GPS time of week of the navigation epoch. See the description of iTOW for
                                //          details
    long            fTOW;       // ns       Fractional part of iTOW (range: +/-500000). The precise GPS time of week in
                                //          seconds is: (iTOW * 1e-3) + (fTOW * 1e-9)
    short           week;       // weeks    GPS week number of the navigation epoch
    unsigned char   gpsFix;     // -        GPSfix Type, range 0..5
    char            flags;      // -        Fix Status Flags (see graphic below)
    long            ecefX;      // cm       ECEF X coordinate
    long            ecefY;      // cm       ECEF Y coordinate
    long            ecefZ;      // cm       ECEF Z coordinate
    unsigned long   pAcc;       // cm       3D Position Accuracy Estimate
    long            ecefVX;     // cm/s     ECEF X velocity
    long            ecefVY;     // cm/s     ECEF Y velocity
    long            ecefVZ;     // cm/s     ECEF Z velocity
    unsigned long   sAcc;       // cm/s     Speed Accuracy Estimate
    unsigned short  pDOP;       // -        Position DOP (0.01)
    unsigned char   reserved1;  // -        Reserved
    unsigned char   numSV;      // -        Number of satellites used in Nav Solution
    unsigned long   reserved2;  // -        Reserved

    UbxGpsNavSol(T &serial) : UbxGps<T>(serial)
    {
        this->setLength(52);
    }
};

#endif
