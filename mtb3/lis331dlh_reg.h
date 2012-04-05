/*****************************************************
 *  MTB board - RBCS,  - 2012                     *
 *  Accelerometer LIS331DLH driver    				     *
 * 						     *
 *  Written by Marco Maggiali			     *
 *  <marco.maggiali@iit.it>			     *
 ****************************************************/

#ifndef __LIS331DLH
#define __LIS331DLH

#define LIS_I2C_ADDR 0x18

#define LIS_REG_WHOIAM		0xf
#define LIS_REG_WHOIAM_MAGIC 0x32

#define LIS_CTRL1			0x20

#define LIS_CTRL1_MODE_OFF	(0<<5)
#define LIS_CTRL1_MODE_ODR	(1<<5)
#define LIS_CTRL1_MODE_05	(2<<5)
#define LIS_CTRL1_MODE_1	(3<<5)
#define LIS_CTRL1_MODE_2	(4<<5)
#define LIS_CTRL1_MODE_5	(5<<5)
#define LIS_CTRL1_MODE_10	(6<<5)
#define LIS_CTRL1_MODE_DR50LP37 (0<<3)
#define LIS_CTRL1_MODE_DR100LP74 (1<<3)
#define LIS_CTRL1_MODE_DR400LP292 (2<<3)
#define LIS_CTRL1_MODE_DR1000LP780 (3<<3)
#define LIS_CTRL1_ZEN 		(1<<2)
#define LIS_CTRL1_YEN 		(1<<1)
#define LIS_CTRL1_XEN 		(1<<0)

#define LIS_CTRL2			0x21
#define LIS_CTRL2_REBOOT	(1<<7)
#define LIS_CTRL2_HPM_NORMAL (0<<5)
#define LIS_CTRL2_HPM_REF 	(1<<5)
#define LIS_CTRL2_FDS		(1<<4)
#define LIS_CTRL2_HPEN2		(1<<3)
#define LIS_CTRL2_HPEN1		(1<<2)
#define LIS_CTRL2_HPCFG(x)	(x<<0)

#define LIS_CTRL3			0x22
#define LIS_CTRL3_I1_SOURCE	(0<<0)
#define LIS_CTRL3_I1ORI2_SOURCE (1<<0)
#define LIS_CTRL3_I1_DATA_READY (2<<0)
#define LIS_CTRL3_I1_BOOT 	(2<<0)
#define LIS_CTRL3_LIR1 		(1<<2)
#define	LIS_CTRL3_I2_SOURCE (0<<3)
#define LIS_CTRL3_I2ORI1_SOURCE (1<<3)
#define LIS_CTRL3_I2_DATA_READY	(2<<3)
#define LIS_CTRL3_I2_BOOT	(3<<3)
#define LIS_CTRL3_LIR2		(1<<5)
#define LIS_CTRL3_PPOD		(1<<6)
#define LIS_CTRL3_IHL		(1<<7)

#define LIS_CTRL4			0x23
#define LIS_CTRL4_SIM		(1<<0)
#define LIS_CTRL4_SELFT		(0<<1)
#define LIS_CTRL4_SELFTSIGN	(1<<2)
#define LIS_CTRL4_SCALE_2G	(0<<4)
#define LIS_CTRL4_SCALE_4G	(1<<4)
#define LIS_CTRL4_SCALE_8G	(3<<4)
#define LIS_CTRL4_BIGENDIAN	(5<<6)
#define LTS_CTRL4_BLOCKDATAUPDATE (1<<7)

#define LIS_CTRL5			0x24
#define LIS_CTRL5_TURNON	(3<<0)

#define LIS_HP_FILTER_RESET	0x25

#define LIS_REFERENCE		0x26

#define LIS_STATUS			0x27
#define LIS_STATUS_XDA		(1<<0)
#define LIS_STATUS_YDA		(1<<1)
#define LIS_STATUS_ZDA		(1<<2)
#define LIS_STATUS_ZYXDA	(1<<3)
#define LIS_STATUS_XOR		(1<<4)
#define LIS_STATUS_YOR		(1<<5)
#define LIS_STAUTS_ZOR		(1<<6)
#define LIS_STATUS_ZYXOR	(1<<7)

#define LIS_OUT_X_L			0x28
#define LIS_OUT_X_H			0x29
#define LIS_OUT_Y_L			0x2a
#define LIS_OUT_Y_H			0x2b
#define LIS_OUT_Z_L			0x2c
#define LIS_OUT_Z_H			0x2d

#define LIS_INT1_CFG		0x30
#define LIS_INT1_AOI		(1<<7)
#define LIS_INT1_6DIR		(1<<6)
#define LIS_INT1_ZHIE		(1<<5)
#define LIS_INT1_ZlIE		(1<<4)
#define LIS_INT1_YHIE		(1<<3)
#define LIS_INT1_YLIE		(1<<2)
#define LIS_INT1_XHIE		(1<<1)
#define LIS_INT1_XLIE		(1<<0)

#define LIS_INT1_SRC		0x31
#define LIS_INT1_SRC_IA	  	(1<<6)
#define LIS_INT1_SRC_ZH		(1<<5)
#define LIS_INT1_SRC_ZL		(1<<4)
#define LIS_INT1_SRC_YH		(1<<3)
#define LIS_INT1_SRC_YL		(1<<2)
#define LIS_INT1_SRC_XH		(1<<1)
#define LIS_INT1_SRC_XL		(1<<0)

#define LIS_INT1_THS_XH		0x32

#define LIS_INT1_DURATION	0x33

#define LIS_INT2_CFG		0x34
#define LIS_INT2_AOI		(1<<7)
#define LIS_INT2_6DIR		(1<<6)
#define LIS_INT2_ZHIE		(1<<5)
#define LIS_INT2_ZlIE		(1<<4)
#define LIS_INT2_YHIE		(1<<3)
#define LIS_INT2_YLIE		(1<<2)
#define LIS_INT2_XHIE		(1<<1)
#define LIS_INT2_XLIE		(1<<0)

#define LIS_INT2_SRC		0x35
#define LIS_INT2_SRC_IA	  	(1<<6)
#define LIS_INT2_SRC_ZH		(1<<5)
#define LIS_INT2_SRC_ZL		(1<<4)
#define LIS_INT2_SRC_YH		(1<<3)
#define LIS_INT2_SRC_YL		(1<<2)
#define LIS_INT2_SRC_XH		(1<<1)
#define LIS_INT2_SRC_XL		(1<<0)

#define LIS_INT2_THS		0x36

#define LIS_INT2_DURATION	0x37

#endif
