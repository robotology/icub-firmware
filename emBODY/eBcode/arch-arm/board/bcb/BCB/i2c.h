 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  I2C configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#ifndef __I2C_H
#define __I2C_H

//------------------------------------------------------------------------------
// I/O Port Defines
//------------------------------------------------------------------------------
#define  	SDATA			0x0080			// Serial data
#define  	SCLK			0x0040			// Serial clock


// Functions defined in module i2c.c
void I2C_Init           (void);
void i2c_start          (void);						//	Sends I2C Start Trasfer
void i2c_stop           (void);						//	Sends I2C Stop Trasfer
void i2c_write          (unsigned char input_data);	//	Writes data over the I2C bus
unsigned char i2c_read  (void);                     //	Reads data from the I2C bus

#endif // __I2C_H


