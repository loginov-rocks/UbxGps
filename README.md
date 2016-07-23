# UBX GPS Library

**This Arduino library was developed for the fastest and simplest communication with u-blox GPS modules**, which supports proprietary UBX protocol that is more compact than common NMEA. Main idea was to achieve real 10 Hz from NEO-7M and it was done.

This library depends on device configuration and can handle only one type of UBX packet, which you can choose during [GPS device configuration](#gps-device-configuration). UBX GPS Library provides easy-to-use interface to all the available data in accordance with the Protocol Specification.

## Quick Start

Download `UbxGps` and place it to the Arduino libraries directory. Refer to [How to install Libraries](https://www.arduino.cc/en/Guide/Libraries) for details.

Next step is configuring your GPS module properly, to find out how to do this see section [GPS device configuration](#gps-device-configuration).

After that you can use included examples or play with following simple sketch:

```cpp
#include "UbxGpsNavPvt.h"

UbxGpsNavPvt gps(Serial3);

void setup() {
    Serial.begin(9600);
    gps.begin(9600);
}

void loop() {
    if (gps.ready()) {
        Serial.print(gps.lon / 10000000.0, 7); Serial.print(',');
        Serial.print(gps.lat / 10000000.0, 7); Serial.print(',');
        Serial.print(gps.height / 1000.0, 3); Serial.print(',');
        Serial.println(gps.gSpeed * 0.0036, 5);
    }
}
```

## GPS device configuration

Coming soon

## Compatible GPS devices

* NEO-7M — tested
* Other u-blox GPS modules, which supports UBX protocol
* _Please, notice me if it works with your GPS module_

## History

### July 23, 2016

First release
