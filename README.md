# UBX GPS Library

**This Arduino library was developed for the fastest and simplest communication with u-blox GPS modules**, which supports proprietary UBX protocol that is more compact than common NMEA. Main idea was to achieve real 10 Hz from NEO-7M and it was done. Huge thanks to **iforce2d** for [tutorial video](https://www.youtube.com/watch?v=TwhCX0c8Xe0) whose code is laid in the basics of this library.

This library depends on GPS module configuration and can handle only one type of UBX packet, which you can choose during [GPS module configuration](#gps-module-configuration). UBX GPS Library provides easy-to-use interface to all the available data in accordance with the Protocol Specification that you can find in the `Docs` directory. Also, full description of fields is accessible in the source codes.

#### UbxGpsNavPosecef.h

_NAV-POSECEF (Position Solution in ECEF):_ iTOW, ecefX, ecefY, ecefZ, pAcc

#### UbxGpsNavPosllh.h

_NAV-POSLLH (Geodetic Position Solution):_ iTOW, lon, lat, height, hMSL, hAcc, vAcc

#### UbxGpsNavPvt.h

_NAV-PVT (Navigation Position Velocity Time Solution):_ iTOW, year, month, day, hour, min, sec, valid, tAcc, nano, fixType, flags, reserved1, numSV, lon, lat, height, hMSL, hAcc, vAcc, velN, velE, velD, gSpeed, heading, sAcc, headingAcc, pDOP, reserved2, reserved3

#### UbxGpsNavSol.h

_NAV-SOL (Navigation Solution Information):_ iTOW, fTOW, week, gpsFix, flags, ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDOP, reserved1, numSV, reserved2

### Note for Uno users

This library is designed to work with GPS module through the hardware serial ports, but Uno has only one pair. It means that you can configure GPS module, but the library will occupy serial port and you need to do something more to get the data from it. For example: add an SD card to store the data, or transmit it through the Bluetooth or Ethernet. `SoftwareSerial` is not supported because it isn't quite stable for working on high frequency.

## Quick Start

Download `UbxGps` and place it to the Arduino libraries directory. Refer to [How to install Libraries](https://www.arduino.cc/en/Guide/Libraries) for details.

Next step is configuring your GPS module properly, to find out how to do this see section [GPS module configuration](#gps-module-configuration).

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

## GPS module configuration

### Step 1. Wiring

So we have an Arduino board and a GPS module. Wiring is pretty simple: `GND` to `GND`, `VCC` to `5V`, `TX` to `RX` and `RX` to `TX`.
Because of Uno has only one TX/RX pair we should connect GPS module's `TX` to the `2` pin and `RX` to the `3` pin and use `SoftwareSerial` library for communicate with GPS. If you have something with more than one TX/RX pair on the board you can use it, for example for Mega we can connect GPS's `RX` to the `TX3` and `TX` to the `RX3`.

![Wiring](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%201.%20Wiring.jpg)

### Step 2. Serial bridge

Let's make a bridge between GPS module and computer: upload `Serial-Bridge-Uno.ino` or `Serial-Bridge-Mega.ino` sketch on the board, it allows us to communicate with GPS module directly from computer. Open _Serial Monitor_, and if your GPS module is new or have default settings you will see something like on the picture below. If everything is OK, GPS will send some data.

![Serial bridge](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%202.%20Serial%20bridge.jpg)

### Step 3. Meet u-center

For u-blox GPS module configuration we will use **u-center** program that you can find [here](https://www.u-blox.com/en/product/u-center-windows). It parses data from GPS module and provides useful tools to work with it. Launch program, choose appropriate COM port and set baudrate, 9600 for default. It will start getting some data.

![Meet u-center](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%203.%20Meet%20u-center.jpg)

### Step 4. Change baudrate (if you want)

If you have something with more than one TX/RX pair it will be useful to raise the baudrate of GPS module. It can helps if you gonna work with high frequency like 5 or 10 Hz. Open _View — Messages View_ window and find _UBX — CGF — PRT_ item. Set the baudrate to 115200 for example and click _Send_ button at the bottom left corner.

Changing baudrate using `SoftwareSerial` library can cause errors!

GPS module will stops getting data, because our sketch works with old baudrate. Disconnect from COM port in u-center, update `GPS_BAUDRATE` and `PC_BAUDRATE` if you want and uploads it to the board. Reconnect u-center and it should works!

![Change baudrate](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%204.%20Change%20baudrate.jpg)

### Step 5. Change frequency (if you want)

In _Messages View_ in the _UBX — CFG — RATE_ tab you can change _Measurement Period_ to raise frequency of getting data. I want to achieve 10 Hz, so I change _Measurement Period_ to the 100 milliseconds and clicks _Send_ button.

![Change frequency](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%205.%20Change%20frequency.jpg)

### Step 6. Disable unnecessary channels

To make GPS module’s life easier, we can disable unnecessary channels in the _UBX — CFG — GNSS_ tab. We only need GPS, so uncheck other channels at _enable_ column. Again, click _Send_ to save changes.

![Serial bridge](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%206.%20Disable%20unnecessary%20channels.jpg)

### Step 7. Choose packet

UBX GPS library works with only one type of packet which is UBX, so we need to fully disable NMEA packets and enable one of the UBX group. Open context menu on _Messages View — NMEA_ and click _Disable Child Messages_. u-center will send appropriate command to the GPS module and getting data will stops. If you’re using `SoftwareSerial` it can takes a time to get things right, try to click _Disable Child Messages_ again if it not works.

Then, choose UBX packet you want to work with, for example _UBX — NAV — PVT_, open context menu on it and click _Enable Message_, GPS module will start getting data again. Open _View — Packet Console_ to see if everything is OK, it should get one type of UBX packet with chosen frequency.

![Choose packet](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%207.%20Choose%20packet.gif)

### Step 8. Save configuration

Go to _UBX — CFG — CFG_ and click _Send_ to save current configuration.

**Important!** GPS module will remember what we’ve done, but sometimes it is not enough: GPS module can return to factory settings after a long time without power, so check your battery on the GPS module.

![Save configuration](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%208.%20Save%20configuration.jpg)

### Step 9. Checks

Close u-center and open an example `NavPvt` sketch, check baudrate and upload it to the board. If everything is OK you’ll get desired data. **That’s all Folks!**

![Checks](https://raw.githubusercontent.com/1oginov/UBX-GPS-Library/master/Configuration/Step%209.%20Checks.jpg)

More details about u-blox GPS module configuration you can find in _**Receiver Description** — Receiver Configuration_ in the `Docs` directory.

## Compatible GPS modules

* NEO-7M — tested
* Other u-blox GPS modules, which supports UBX protocol
* _Please, notice me if it works with your GPS module_

## Contribution

Feel free to add something useful to this library :relaxed: For example new classes for UBX packets!

## History

### July 2, 2017

UbxGpsNavPosecef.h added

### July 23, 2016

First release

## Links

* [u-blox Website](https://www.u-blox.com)
* [u-center Download page](https://www.u-blox.com/en/product/u-center-windows)
