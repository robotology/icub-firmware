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
	int (*i2c_write)(char adr,char *wbuf, int wlen);
	int (*i2c_read)(char adr,char *wbuf, int wlen, char *rbuf, int rlen);
} tLISI2COps;

extern int LISInit(tLISI2COps ops);
extern void LISAxisRead(int *x, int *y, int *z);

#endif
