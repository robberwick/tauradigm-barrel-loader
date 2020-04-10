# Tauradigm Barrel Loader Firmware

This project provides the firmware for the barrel loader attachment for the Team Tauradigm Pi Wars 2020 robot.

## Hardware

This firmware is designed to work with an Arduino Nano, but any microcontroller which provides a hardware I2C bus, and can also drive another I2C bus in software will probably work fine. YMMV.

Oh, you'll also need 3 pins to drive servos, and another digital pin to control the LED on a colour sensor.

```
                      +-----+
         +------------| USB |------------+
         |            +-----+            |
    B5   | [*]D13/SCK        MISO/D12[*] |   B4
         | [ ]3.3V           MOSI/D11[*]~|   B3
         | [ ]V.ref     ___    SS/D10[*]~|   B2
    C0   | [*]A0       / N \       D9[ ]~|   B1
    C1   | [*]A1      /  A  \      D8[ ] |   B0
    C2   | [ ]A2      \  N  /      D7[ ] |   D7
    C3   | [ ]A3       \_0_/       D6[ ]~|   D6
    C4   | [*]A4/SDA               D5[ ]~|   D5
    C5   | [*]A5/SCL               D4[ ] |   D4
         | [ ]A6              INT1/D3[ ]~|   D3
         | [ ]A7              INT0/D2[ ] |   D2
         | [ ]5V                  GND[ ] |     
    C6   | [ ]RST                 RST[ ] |   C6
         | [ ]GND   5V MOSI GND   TX1[ ] |   D0
         | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
         |          [ ] [ ] [ ]          |
         |          MISO SCK RST         |
         | NANO-V3                       |
         +-------------------------------+
         
         http://busyducks.com/ascii-art-arduinos

         Pins:
         -----

         A0 - Internal I2C Bus (Software) SDA - connect to colour sensor as master
         A1 - Internal I2C Bus (Software) SCL - connect to colour sensor as master
         A4 - External I2C Bus (Hardware) SDA - connect to robot I2C bus as slave
         A5 - External I2C Bus (Hardware) SCL - connect to robot I2C bus as slave
         D10 - Pivot Servo signal
         D11 - Lifter Servo signal
         D12 - Jaws Servo signal
         D13 - Enable pin for colour sensor LED
```


## Installation

Clone this repository, open with [Platform IO](https://platformio.org), build and upload to the target microcontroller

## Configuration

In order to tune or customise the operation of this firmware for your particular hardware it may be necessary to tweak the pin assignment values for the various IO. These can be found in `main.cpp`. If it is necessary to adjust the servo start and end angles, these can be found in the `jaws|Pivot|Lifter` header files.

By default, this firware will present on the external I2C bus at address `\x04`. This can also be changed by amending the value in `main.cpp`.

## I2C API Usage

This firmware exposes a simple API which allows it to be in either a stopped or a running state (note that if it receives a `STOP` command whilst running, it will finish resetting before moving ito the stopped state).

To set the state, write a byte of the corect value via the external I2C bus. allowed values are:

```
STOP: 0
RUN: 1
```

Additionally, it is possible to request the current state of the module by reading a single byte on the external I2C bus. Possible values for this state byte are:

```
WAIT/STOPPED = 0,   // Stopped or inactive
LOOKING = 1,        // Looking for a barrel
GRABBING = 2,       // Jaws closing
GRABBED = 3,        // Jaws closed, checking colour
LIFTING = 4,        // Lifting barrel
RELEASING = 5,      // Opening jaws to release barrel
RESETTING = 6       // Resetting servos and moving to LOOKING or STOPPED
```


## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[MIT](https://choosealicense.com/licenses/mit/)