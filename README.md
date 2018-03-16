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

## Credits

This library uses the the [I2C/TWI master library](http://homepage.hispeed.ch/peterfleury/doxygen/avr-gcc-libraries/group__pfleury__ic2master.html) by Peter Fleury for its I2C configuration.

The `example.c` file uses the UART library by Peter Fleury for debugging output.

Apart from the above, the MS5611 library itself as well as the example file
is written by [Jan "Yenya" Kasprzak](https://www.fi.muni.cz/~kas/).

