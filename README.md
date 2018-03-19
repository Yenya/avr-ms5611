# MS5611 pressure sensor library for AVR (Arduino) in plain C

This library can be used to add MS5611 pressure and temperature
sensor from Measurement Specialities to your AVR/Arduino projects.
The GY-63 breakout board is especially suitable for it, because
it contains its own 3V voltage regulator, and therefore can be independent
of Arduino voltage circuitry.

## How to use the sample code

* Get an AVR/Arduino-based board (I used Arduino Nano)
* Get a break-out board with MS5611 (I used GY-63)
* Connect your AVR/Arduino board to the MS5611 board using I2C:
	- GND at Arduino to GND at MS5611
	- VCC to VCC
	- SDA to SDA
	- SCL to SCL
	- VCC to PS and CSB
* Connect the Arduno to your computer
* Run "`make`" and "`make program_flash`"
* Read the Arduino serial port ("`cat /dev/ttyUSB0`" when using Linux)

## How to use the library

* Copy the `ms5611/` and `i2chw/` subdirectories to your project.
* Adjust `ms5611/ms5611.c` (see the comments at the beginning)
* Edit your `Makefile` to compile `i2chw/i2cmaster.o` and `ms5611/ms5611.o`,
	and add these object files to your linker command.
* Profit! :-)

## TODO/FIXME

* Support two MS-5611 sensors on the same I2C bus.
* Compute the second-order temperature correction, as described in the datasheet, page 8/20
* Support the user-settable oversampling rate (OSR)
* Provide the low-level interface, which would allow reuse the temperature
reading for several pressure readings, and which would allow the time
between the conversion start and the reading to be spent by something other
than busy-wait loop.
* Implement the CRC-4 code from Application Note AN520 to verify the calibration data are being read correctly.
* It seems that my own MS-5611 GY-63 boards provide incorrect absolute
pressure - both of them report the pressure corresponding to about
430 m above the sea level, while I am at about 290 m. I have verified that
also other MS-5611 libraries (see below) return the same incorrect pressure
from both of my boards.
So maybe my MS-5611 parts are faulty or miscalibrated. I have added the sample
values from the datasheet (see `#ifdef DATASHEET_EXAMPLE' in the code),
and with these values my code returns the same output as described in
the datasheet. From the above I think the code itself is correct
and the problem is in my GY-63 boards. I am interested in user tests - do your MS-5611 return the correct pressure/altitude with this code? Mail me your results. Thanks!

## Credits

This library uses the the [I2C/TWI master
library](http://homepage.hispeed.ch/peterfleury/doxygen/avr-gcc-libraries/group__pfleury__ic2master.html)
by Peter Fleury for its I2C configuration.

The `example.c` file uses the UART library by Peter Fleury for debugging
output.

Apart from the above, the MS5611 library itself as well as the example
file is written by [Jan "Yenya" Kasprzak](https://www.fi.muni.cz/~kas/).

When testing the output from my own MS-5611 boards,
I have cross-verified the results with the [MS-5611 C++
library](https://github.com/gronat/MS5611) for Arduino IDE, written
by gronat.

