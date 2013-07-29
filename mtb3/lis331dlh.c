/*****************************************************
 *  MTB board - RBCS,  - 2012                     *
 *  Accelerometer LIS331DLH driver   				     *
 * 						     *
 *  adjusted by Marco Maggiali stolen from Merello			     *
 *  <marco.maggiali@iit.it>			     *
 ****************************************************/

#include "lis331dlh.h"
#include "lis331dlh_reg.h"

tLISI2COps LISI2COps;



void LISRegWrite( unsigned char reg, unsigned char val)
{
 
	LISI2COps.i2c_write(LIS_I2C_ADDR, reg, val );	
}


char LISRegRead(char reg, unsigned char *rbuf)
{
	unsigned char ret;
	ret = LISI2COps.i2c_read(LIS_I2C_ADDR, reg, rbuf);

	return ret;
}

void LISRegBurst(char reg,char naxis, unsigned int *data )
{
	LISI2COps.i2c_burst(2, LIS_I2C_ADDR,reg|0x80, naxis, data);
}

unsigned int LISInit(tLISI2COps ops)
{
	unsigned char tmp;
	LISI2COps = ops;
	unsigned char id;

//	if(LISRegRead(LIS_REG_WHOIAM, &id))
//		return -2; /* i2c dead */
    LISRegRead(LIS_REG_WHOIAM, &id);
	if(LIS_REG_WHOIAM_MAGIC != id)
	 	return -1;/* lis not detected :-( */
	

	tmp = LIS_CTRL2_HPM_NORMAL; // filter normal, disabled

	LISRegWrite(LIS_CTRL2,tmp);

	/* open drain for i2c pads*/
	tmp = LIS_CTRL3_PPOD;
	LISRegWrite(LIS_CTRL3,tmp);

	/* 2g full scale */
	tmp = LIS_CTRL4_SCALE_2G | LTS_CTRL4_BLOCKDATAUPDATE;
	LISRegWrite(LIS_CTRL4,tmp);
	
	/* enable 3 axis */
	tmp = LIS_CTRL1_XEN | LIS_CTRL1_YEN | LIS_CTRL1_ZEN 
			| LIS_CTRL1_MODE_ODR | LIS_CTRL1_MODE_DR1000LP780; // enable, 400Khz

	LISRegWrite(LIS_CTRL1,tmp);

	return 0;
}


void LISAxisRead(int *x, int *y, int *z)
{
	unsigned char xl,xh,yl,yh,zl,zh;
	/* TODO: read in burst */
	 LISRegRead(LIS_OUT_X_L, &xl);
	 LISRegRead(LIS_OUT_X_H, &xh);
	 LISRegRead(LIS_OUT_Y_L, &yl);
	 LISRegRead(LIS_OUT_Y_H, &yh);
 	 LISRegRead(LIS_OUT_Z_L, &zl);
	 LISRegRead(LIS_OUT_Z_H, &zh);

	*x = xl | (xh<<8);
	*y = yl | (yh<<8);
	*z = zl	| (zh<<8);

}

void LISAxisBurst(int *x, int *y, int *z)
{
	unsigned int data[3];
	LISRegBurst(LIS_OUT_X_L,3,data);
	*x = (int) data[0];
	*y = (int) data[1];
	*z = (int) data[2];
}
