/******************************************************************************
 * I2C_Multi_SDA.h
 *
 *
 * Created on: Oct 29, 2015
 * Board: NUCLEO STML152RET
 * Author: marco aggiali
 *
 * Description: This file contains the functions for sending data from a non standard I2C
 * 		with 1 SCK line and 4 SDA lines
 *
 * Modified:
 *
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __i2c_SSI_H
#define __i2c_SSI_H


typedef enum
{
    sda0 = 0,
    sda1 = 1,
    sda2 = 2,
    sda3 = 3,
	sda4 = 4,
	sda5 = 5,
	sda6 = 6,
	sda7 = 7,
}i2c_sda_num_t;


#define i2c_sda_num_max 8
#define CH0     0
#define CH1     1


void I2C_Multi_SDA_Init(unsigned char Channel);
void I2C_Multi_SDA_test(unsigned char Channel);
void I2C_Multi_SDA_deInit(unsigned char Channel);
//Local functions
//===============
//High level functions
unsigned char WriteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
extern unsigned char WriteByteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned char RegisterStartAddress, unsigned char DataBuffer);
unsigned char WriteByteI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
unsigned char WriteViaI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
unsigned char ReadBurstViaI2C(unsigned char Channel, unsigned char SDAnum, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer);
extern unsigned char ReadViaI2C(unsigned char Channel,unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer1, unsigned int *DataBuffer2, unsigned int *DataBuffer3, unsigned int *DataBuffer4, const unsigned int OffsetInBuffer);
unsigned char ReadByteViaI2C(unsigned char Channel, unsigned char SDAnum, unsigned char DeviceAddress, const unsigned char RegisterAddress, unsigned char *DataBuffer );
unsigned char ReadByteViaI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char DeviceAddress, const unsigned char RegisterAddress, unsigned char *DataBuffer );

//Low level functions
void Wait(unsigned int cycles);
void InitialiseI2CMaster(unsigned char Channel);
void StartI2CMaster(unsigned char Channel);
void StopI2CMaster(unsigned char Channel);
unsigned char SendByteI2CMaster(unsigned char Channel,unsigned char ByteToSend);
void ReceiveByteI2CMaster(unsigned char Channel,unsigned char ackn);  //modificated bit with unsigned char

void SetValReg(i2c_sda_num_t sdaNum, unsigned char val);
void SetSdaReg(i2c_sda_num_t sdaNum, unsigned char input);
void StartI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum);
void StopI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum);
unsigned char SendByteI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum, unsigned char ByteToSend);


#endif
