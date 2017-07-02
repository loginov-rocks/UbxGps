/**
 * UBX GPS Library
 * Created by Danila Loginov, July 23, 2016
 * https://github.com/1oginov/UBX-GPS-Library
 */

#ifndef UBXGPSNAVPOSLLH_H_
#define UBXGPSNAVPOSLLH_H_

#include "UbxGps.h"

class UbxGpsNavPosllh : public UbxGps {
public:

    //        Type  Name           Unit   Description (scaling)
    unsigned  long  iTOW;       // ms     GPS time of week of the navigation epoch. See the description of iTOW for details.
    long            lon;        // deg    Longitude (1e-7)
    long            lat;        // deg    Latitude (1e-7)
    long            height;     // mm     Height above ellipsoid
    long            hMSL;       // mm     Height above mean sea level
    unsigned  long  hAcc;       // mm     Horizontal accuracy estimate
    unsigned  long  vAcc;       // mm     Vertical accuracy estimate

    UbxGpsNavPosllh(HardwareSerial& serial) : UbxGps(serial) {
        this->setLength(28);
    }
};

#endif
