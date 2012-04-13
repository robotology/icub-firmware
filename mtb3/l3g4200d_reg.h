 /****************************************************
 *  MTB board - RBCS,  - 2012                        *
 *  Gyroscope L3G4200D                               *
 *                                                   *
 *  Written by Merello                        *
 *  <marco.maggiali@iit.it>                          *
 ****************************************************/

#ifndef __L3G4200D_REG
#define __L3G4200D_REG

#define L3G_REG_WHOIAM	 	0xf
#define L3G_REG_WHOIAM_MAGIC 0xd3

#define L3G_CTRL1			0x20
#define L3G_CTRL1_ODR_100HZ (0<<6)
#define L3G_CTRL1_ODR_200HZ (1<<6)
#define L3G_CTRL1_ODR_400HZ (2<<6)
#define L3G_CTRL1_ODR_800HZ (3<<6)
#define L3G_CTRL1_BW_XN		(0<<4)
#define L3G_CTRL1_BW_N 		(1<<4)
#define L3G_CTRL1_BW_W 		(2<<4)
#define L3G_CTRL1_BW_XW 	(3<<4)
#define L3G_CTRL1_PON		(1<<3)
#define L3G_CTRL1_ZEN 		(1<<2)
#define L3G_CTRL1_YEN 		(1<<0)
#define L3G_CTRL1_XEN 		(1<<1)

#define L3G_CTRL2			0x21
#define L3G_CTRL2_HPM_NORMAL 		(0<<4)
#define L3G_CTRL2_HPM_NORMAL2		(2<<4)
#define L3G_CTRL2_HPM_REF 		(1<<4)
#define L3G_CTRL2_HPM_AUTO 		(3<<4)
#define L3G_CTRL2_HPCFG(x)		(x<<0)


#define L3G_CTRL3			0x22
#define L3G_CTRL3_I1INT		(1<<7)
#define L3G_CTRL3_I1BOOT	(1<<6)
#define L3G_CTRL3_HLACTIVE	(1<<5)
#define L3G_CTRL3_PPOD		(1<<4)
#define L3G_CTRL3_I2DRDY	(1<<3)
#define L3G_CTRL3_I2WTM		(1<<2)
#define L3G_CTRL3_I2ORUN	(1<<1)
#define L3G_CTRL3_I2EMPTY	(1<<0)


#define L3G_CTRL4			0x23
#define L3G_CTRL4_SIM		(1<<0)
#define L3G_CTRL4_SELFTOFF	(0<<1)
#define L3G_CTRL4_SELFT0 	(1<<1)
#define L3G_CTRL4_SELFT1	(2<<1)
#define L3G_CTRL4_SCALE_250 	(0<<4)
#define L3G_CTRL4_SCALE_500 	(1<<4)
#define L3G_CTRL4_SCALE_2000 	(2<<4)
#define L3G_CTRL4_BIGENDIAN	(1<<6)
#define L3G_CTRL4_BLOCKUPD	(1<<7)

#define L3G_CTRL5			0x24
#define L3G_CTRL5_OUT_LPF1	(0<<0)
#define L3G_CTRL5_OUT_HPF	(1<<0)
#define L3G_CTRL5_OUT_LPF2	(2<<0)
#define L3G_CTRL5_INT_LPF1	(0<<2)
#define L3G_CTRL5_INT_HPF	(1<<2)
#define L3G_CTRL5_INT_LPF2	(2<<2)
#define L3G_CTRL5_HPFEN		(1<<4)
#define L3G_CTRL5_FIFOEN	(1<<6)
#define L3G_CTRL5_REBOOT	(1<<7)

#define L3G_INT_REFERENCE	0x25

#define L3G_OUT_TEMP		0x26

#define L3G_STATUS			0x27
#define L3G_STATUS_XYZ_OVER	(1<<7)
#define L3G_STATUS_Z_OVER	(1<<6)
#define L3G_STATUS_Y_OVER	(1<<5)
#define L3G_STATUS_X_OVER	(1<<4)
#define L3G_STAUTS_XYZ_DAV	(1<<3)
#define L3G_STATUS_ZDA		(1<<2)
#define L3G_STATUS_YDA		(1<<1)
#define L3G_STATUS_XDA		(1<<0)


#define L3G_OUT_X_L			0x28
#define L3G_OUT_X_H			0x29
#define L3G_OUT_Y_L			0x2a
#define L3G_OUT_Y_H			0x2b
#define L3G_OUT_Z_L			0x2c
#define L3G_OUT_Z_H			0x2d

#define L3G_FIFO_CTRL		0x2e
#define L3G_FIFO_CTRL_BYPASS (1<<5)
#define L3G_FIFO_CTRL_FIFO	(2<<5)
#define L3G_FIFO_CTRL_STREAM (3<<5)
#define L3G_FIFO_CTRL_STRFIFO (4<<5)
#define L3G_FIFO_CTRL_BYSTRM (5<<5)
#define L3G_FIFO_WTM_THRESH(x) (x<<0)

#define L3G_FIFO_SRC		0x2f
#define L3G_FIFO_SRC_STORED (0x1F<<0)
#define L3G_FIFO_SRC_EMPTY	(1<<5)
#define L3G_FIFO_SRC_FILLED	(1<<6)
#define L3G_FIFO_SRC_WTM	(1<<7)

#define L3G_INT1_CFG		0x30
#define L3G_INT1_ANDEVENT	(1<<7)
#define L3G_INT1_LIR		(1<<6)
#define L3G_INT1_ZHIE		(1<<5)
#define L3G_INT1_ZLIE		(1<<4)
#define L3G_INT1_YHIE		(1<<3)
#define L3G_INT1_YLIE		(1<<2)
#define L3G_INT1_XHIE		(1<<1)
#define L3G_INT1_XLIE		(1<<0)

#define L3G_INT1_SRC		0x31
#define L3G_INT1_SRC_IA	  	(1<<6)
#define L3G_INT1_SRC_ZH		(1<<5)
#define L3G_INT1_SRC_ZL		(1<<4)
#define L3G_INT1_SRC_YH		(1<<3)
#define L3G_INT1_SRC_YL		(1<<2)
#define L3G_INT1_SRC_XH		(1<<1)
#define L3G_INT1_SRC_XL		(1<<0)

#define L3G_INT1_THS_XH		0x32

#define L3G_INT1_THS_XL		0x33

#define L3G_INT1_THS_YH		0x34

#define L3G_INT1_THS_YL 	0x35

#define L3F_INT1_TSH_ZH		0x36

#define L3G_INT1_TSH_ZL		0x37

#define L3G_INT1_DURATION 	0x38
#define L3G_INT1_DURATION_WAIT (1<<7)
#define L3G_INT1_DURATION_VAL(x) (x<<0)

#endif
