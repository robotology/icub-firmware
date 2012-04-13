 /****************************************************
 *  EMS board - RBCS, EDL - 2010                     *
 *  L3G4200D driver                                  *
 *                                                   *
 *  adjusted by Marco Maggiali stolen from Merello                      *  
 *  <andrea.merello@iit.it>                          *
 ****************************************************/

#ifndef __L3G4200D_H
#define __L3G4200D_H

typedef struct{
	unsigned char (*i2c_write)(unsigned char DeviceAddress, const unsigned char RegisterStartAddress, unsigned char DataBuffer);
	unsigned char (*i2c_read) (unsigned char DeviceAddress, const unsigned char RegisterAddress, unsigned char *DataBuffer );
	unsigned char (*i2c_burst)(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer);
} tL3GI2COps;

extern int L3GInit(tL3GI2COps ops);
extern void L3GAxisRead(int *x, int *y, int *z);
extern void L3GAxisBurst(int *x, int *y, int *z);

#endif
