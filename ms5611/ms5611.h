#ifndef MS5611_H_
#define MS5611_H_
/*
 * Library for Bosch MS5611 pressure sensor for AVR MCUs in plain old C.
 *
 * This library uses the I2C layer by Peter Fleury, and is loosely modelled
 * after the bmp085 library by Davide Gironi.
 * 
 * The library is distributable under the terms of the GNU General
 * Public License, version 2 only.
 *
 * Written by Jan "Yenya" Kasprzak, https://www.fi.muni.cz/~kas/
 *
 * Configuration can be done by editing the top of the ms5611.c file.
 */

uint8_t ms5611_init(void);		// call this first
void ms5611_measure(void);		// do a measurement

// the following functions return the result of the last measurement
#define ms5611_getpressure()	(_ms5611_pres)
#define ms5611_gettemperature()	(_ms5611_temp)
double ms5611_getaltitude(void);

// do not use directly, call the macros above
extern int32_t _ms5611_temp;
extern uint32_t _ms5611_pres;

#endif /* MS5611_H_ */
