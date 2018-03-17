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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>

#include "ms5611.h"

// --------------------------------------------------------
// User configuration - modify these to suit your project
//
#include "../i2chw/i2cmaster.h" // i2c Fleury lib

// TODO: there can be two MS5611 sensors on the I2C bus, ids 0x76, 0x77.
// Currently we support only one sensor for the sake of simplicity
#define MS5611_ADDR	0x76	// can be 0x77 or 0x76, 0x76 is CSB=VCC

#define MS5611_I2CINIT		// comment out if called from somewhere else

// provide the debugging function sending NAME=value pairs somewhere if needed
// it is possible to use the uart_print() function from example.c:
//
// void uart_print(char *name, long val);
// #define MS5611_DEBUG(name, val)	uart_print((name), (val))

// End of the user configuration
// --------------------------------------------------------

// empty debugging function, if not provided above
#ifndef MS5611_DEBUG
#define MS5611_DEBUG(name, val)	do {  } while (0)
#endif

#define MS5611_CMD_RESET	0x1E
#define MS5611_RESET_US		2800

#define MS5611_CMD_PROM(reg)	(0xA0 + ((reg) << 1))

// FIXME: for now, we hardcode over-sampling rate of 4096
#define MS5611_CMD_CONV_D1	0x48
#define MS5611_CMD_CONV_D2	0x58

#define MS5611_CONV_DELAY_US	8220

#define MS5611_CMD_READ_ADC	0x00

// Global variables for the sake of simplicity.
// We provide wrapper macros for them in ms5611.h.
int32_t _ms5611_temp;
uint32_t _ms5611_pres;

void ms5611_reset()
{
	i2c_start_wait((MS5611_ADDR << 1) | I2C_WRITE);
	i2c_write(MS5611_CMD_RESET);
	i2c_stop();

	_delay_us(MS5611_RESET_US);
}

static uint16_t ms5611_read_cal_reg(uint8_t reg)
{
	uint16_t rv = 0;

	i2c_start_wait((MS5611_ADDR << 1) | I2C_WRITE);
        i2c_write(MS5611_CMD_PROM(reg));
        i2c_rep_start((MS5611_ADDR << 1) | I2C_READ);

	rv = i2c_readAck() << 8;
	rv += (uint16_t)i2c_readNak();

	i2c_stop();

	return rv;
}

static void ms5611_start_conv(uint8_t conv)
{
	i2c_start_wait((MS5611_ADDR << 1) | I2C_WRITE);
        i2c_write(conv);
	i2c_stop();
}
	
static uint32_t ms5611_read_adc()
{
	uint32_t rv = 0;

	i2c_start_wait((MS5611_ADDR << 1) | I2C_WRITE);
        i2c_write(MS5611_CMD_READ_ADC);
        i2c_rep_start((MS5611_ADDR << 1) | I2C_READ);

	rv = (uint32_t)i2c_readAck() << 16;
	rv += (uint32_t)i2c_readAck() << 8;
	rv += (uint32_t)i2c_readNak();

	i2c_stop();

	return rv;
}

static uint32_t ms5611_conv(uint8_t conv)
{
	uint32_t rv;

	ms5611_start_conv(conv);
	_delay_us(MS5611_CONV_DELAY_US);

	rv = ms5611_read_adc();

	return rv;	
}

// we create a struct for this, so that we can add support for two
// sensors on a I2C bus

struct _ms5611_cal {
	uint16_t sens, off, tcs, tco, tref, tsens;
} ms5611_cal;

/*
 * read calibration registers
 */

static void ms5611_getcalibration(void)
{
	memset(&ms5611_cal, 0, sizeof(ms5611_cal));

	// for now, we ignore vendor-specific register 0
	// and crc register 7
	ms5611_cal.sens  = ms5611_read_cal_reg(1);
	ms5611_cal.off   = ms5611_read_cal_reg(2);
	ms5611_cal.tcs   = ms5611_read_cal_reg(3);
	ms5611_cal.tco   = ms5611_read_cal_reg(4);
	ms5611_cal.tref  = ms5611_read_cal_reg(5);
	ms5611_cal.tsens = ms5611_read_cal_reg(6);

#ifdef DATASHEET_EXAMPLE
	ms5611_cal.sens  = 40127;
	ms5611_cal.off   = 36924;
	ms5611_cal.tcs   = 23317;
	ms5611_cal.tco   = 23282;
	ms5611_cal.tref  = 33464;
	ms5611_cal.tsens = 28312;
#endif

	MS5611_DEBUG("SENS ", (int32_t)ms5611_cal.sens);
	MS5611_DEBUG("OFF  ", (int32_t)ms5611_cal.off);
	MS5611_DEBUG("TCS  ", (int32_t)ms5611_cal.tcs);
	MS5611_DEBUG("TCO  ", (int32_t)ms5611_cal.tco);
	MS5611_DEBUG("TREF ", (int32_t)ms5611_cal.tref);
	MS5611_DEBUG("TSENS", (int32_t)ms5611_cal.tsens);
}

uint8_t ms5611_init(void)
{
	uint8_t buffer[1];
#ifdef MS5611_I2CINIT
        i2c_init();
        _delay_us(10);
#endif

	ms5611_reset();
	ms5611_getcalibration();

	return 1;
}

void ms5611_measure(void)
{
	int32_t temp_raw, pres_raw, dt;
	int64_t sens, off;
	
	temp_raw = ms5611_conv(MS5611_CMD_CONV_D2);
	pres_raw = ms5611_conv(MS5611_CMD_CONV_D1);

#ifdef DATASHEET_EXAMPLE
	temp_raw = 8569150;
	pres_raw = 9085466;
#endif

	MS5611_DEBUG("temp_raw", temp_raw);
	MS5611_DEBUG("pres_raw", pres_raw);

	dt = temp_raw - ((int32_t)ms5611_cal.tref << 8);
	_ms5611_temp = 2000 + ((dt*((int64_t)ms5611_cal.tsens)) >> 23);

	MS5611_DEBUG("dt", dt);
	MS5611_DEBUG("temp*100", _ms5611_temp);

	off = ((int64_t)ms5611_cal.off << 16) + (((int64_t)dt*(int64_t)ms5611_cal.tco) >> 7);
	sens = ((int64_t)ms5611_cal.sens << 15) + ((int64_t)ms5611_cal.tcs*dt >> 8);
	_ms5611_pres = ((((uint64_t)pres_raw*sens) >> 21) - off) >> 15;
	MS5611_DEBUG("pres*100", _ms5611_pres);
}

double ms5611_getaltitude() {
	double alt;
	alt = (1 - pow(_ms5611_pres/(double)101325, 0.1903)) / 0.0000225577;

	MS5611_DEBUG("alt x 100", (long)(alt * 100));

	return alt;
}
