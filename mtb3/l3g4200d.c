 /****************************************************
 *  MTB board - RBCS,  - 2012                        *
 *  Gyroscope L3G4200D                               *
 *                                                   *
 *  Written by Marco Maggiali stolen by Merello                        *
 *  <marco.maggiali@iit.it>                          *
 ****************************************************/

#include "l3g4200d.h"
#include "l3g4200d_reg.h"

#define FIFO_INT_THRESH 8

#define L3G_I2C_ADDR 0x68

#define L3G_I2C_CHANNEL 0x0 //for the palms
tL3GI2COps L3GI2COps;



void L3GRegWrite(  unsigned char RegisterAddress, unsigned char DataBuffer)
{
	L3GI2COps.i2c_write(L3G_I2C_CHANNEL,L3G_I2C_ADDR, RegisterAddress, DataBuffer);
}

char L3GRegRead(char reg)
{
	unsigned char rbuf;
	L3GI2COps.i2c_read(L3G_I2C_CHANNEL,L3G_I2C_ADDR,reg, &rbuf );

	return rbuf;
}

void L3GRegBurst(char reg,char naxis, unsigned int *data )
{
	L3GI2COps.i2c_burst(L3G_I2C_CHANNEL, L3G_I2C_ADDR,reg|0x80, naxis, data);
}


int L3GInit(tL3GI2COps ops)
{
	unsigned char tmp;
	L3GI2COps = ops;

   
	if(L3G_REG_WHOIAM_MAGIC != (unsigned char) L3GRegRead(L3G_REG_WHOIAM) )
	 	return -1;/* l3g not detected :-( */


	
	tmp = L3G_CTRL2_HPM_NORMAL2 | L3G_CTRL2_HPCFG(0b1001); //0x29
	L3GRegWrite(L3G_CTRL2,tmp);
	
	/* enable fifo WTM int*/
	tmp =  0;// L3G_CTRL3_I2WTM | L3G_CTRL3_I1INT; 
	L3GRegWrite(L3G_CTRL3,tmp);

	/* medium sensitivity, medium full scale */
	tmp = L3G_CTRL4_SCALE_250 | L3G_CTRL4_BLOCKUPD; //0x80
	L3GRegWrite(L3G_CTRL4,tmp);

	/* set stream mode & fifo int threshold */
	tmp = L3G_FIFO_CTRL_STREAM | L3G_FIFO_WTM_THRESH(FIFO_INT_THRESH);
	L3GRegWrite(L3G_FIFO_CTRL,tmp);

	/* enable fifo, no filter en */
	tmp = L3G_CTRL5_FIFOEN | L3G_CTRL5_HPFEN | 0b11; //0x53
	L3GRegWrite(L3G_CTRL5,tmp);
	/* enable 3 axis */
	 tmp = L3G_CTRL1_XEN | L3G_CTRL1_YEN | L3G_CTRL1_ZEN | L3G_CTRL1_PON | 
		L3G_CTRL1_ODR_800HZ | L3G_CTRL1_BW_XW | L3G_CTRL5_HPFEN ; //0xff
	L3GRegWrite(L3G_CTRL1,tmp);
	return 0;
}

void L3GAxisRead(int *x, int *y, int *z)
{
	char xl,xh,yl,yh,zl,zh;

	yh = (unsigned char) L3GRegRead(L3G_OUT_Y_H);
	zh = (unsigned char) L3GRegRead(L3G_OUT_Z_H);
	xl = (unsigned char) L3GRegRead(L3G_OUT_X_L);
    xh = (unsigned char) L3GRegRead(L3G_OUT_X_H);
    yl = (unsigned char) L3GRegRead(L3G_OUT_Y_L);
	zl = (unsigned char) L3GRegRead(L3G_OUT_Z_L);
		*x = xl | (xh<<8);
	

		*y = yl | (yh<<8);
 

	*z = zl	| (zh<<8);
}

void L3GAxisBurst(int *x, int *y, int *z)
{
	unsigned int data[3];
	L3GRegBurst(L3G_OUT_X_L,3,data);
	*x = (int) data[0];
	*y = (int) data[1];
	*z = (int) data[2];
}
	