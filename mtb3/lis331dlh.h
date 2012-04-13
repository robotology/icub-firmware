/*****************************************************
 *  MTB board - RBCS, EDL - 2012                     *
 *  LIS331DLH driver   				     *
 * 						     *
 *  Written by Marco Maggiali			     *
 *  <marco.maggiali@iit.it>			     *
 ****************************************************/

#ifndef __LIS331DLH_H
#define __LIS331DLH_H

typedef struct{
	unsigned char (*i2c_write)(unsigned char DeviceAddress, const unsigned char RegisterStartAddress, unsigned char DataBuffer);
	unsigned char (*i2c_read) (unsigned char DeviceAddress, const unsigned char RegisterAddress, unsigned char *DataBuffer );
	unsigned char (*i2c_burst)(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer);

} tLISI2COps;

extern unsigned int LISInit(tLISI2COps ops);
extern void LISAxisRead(int *x, int *y, int *z);
extern void LISAxisBurst(int *x, int *y, int *z);

#endif
