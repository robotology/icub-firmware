/******************************************************************************
 * I2C_Multi_SDA.c
 *
 *
 * Created on: Jan 18, 2016
 * Board: SSI
 * Author: andrea mura
 *
 * Description: This file contains the functions for sending data from a non standard I2C
 * 		with 2 SCK lines and 8 SDA lines
 *
 * Modified:
 *
 ******************************************************************************/

//---------------------------------
//Function prototypes
//---------------------------------
//#include "main.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_gpio.h"
#include "I2C_Multi_SDA.h"
//#include "stm32l4xx.h"
//#include "stm32l443xx.h"
//---------------------------------
//Function prototypes
//---------------------------------
#define I2C_WR		0x00
#define I2C_RD		0x01
#define	ACK			0
#define	NACK		1
unsigned int I2Cbit=1; //the duration of a bit 10 is about 28Khz, 1 is about 500KHz


#define	MCO_0_on		HAL_GPIO_WritePin(SCK0_GPIO_Port, SCK0_Pin, GPIO_PIN_SET);
#define	MCO_0_off   HAL_GPIO_WritePin(SCK0_GPIO_Port, SCK0_Pin, GPIO_PIN_RESET);

#define DO_0on      HAL_GPIO_WritePin(SDA0_GPIO_Port, SDA0_Pin, GPIO_PIN_SET);\
                    HAL_GPIO_WritePin(SDA1_GPIO_Port, SDA1_Pin, GPIO_PIN_SET);\
                    HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_SET);\
                    HAL_GPIO_WritePin(SDA3_GPIO_Port, SDA3_Pin, GPIO_PIN_SET);

#define DO_0off     HAL_GPIO_WritePin(SDA0_GPIO_Port, SDA0_Pin, GPIO_PIN_RESET);\
                    HAL_GPIO_WritePin(SDA1_GPIO_Port, SDA1_Pin, GPIO_PIN_RESET);\
                    HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_RESET);\
                    HAL_GPIO_WritePin(SDA3_GPIO_Port, SDA3_Pin, GPIO_PIN_RESET);

#define DE_0input   LL_GPIO_SetPinMode(SDA0_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_INPUT);\
                    LL_GPIO_SetPinMode(SDA1_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_INPUT);\
                    LL_GPIO_SetPinMode(SDA2_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_INPUT);\
                    LL_GPIO_SetPinMode(SDA3_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_INPUT);\
                    LL_GPIO_SetPinPull(SDA0_GPIO_Port, SDA0_Pin,LL_GPIO_PULL_NO);\
                    LL_GPIO_SetPinPull(SDA1_GPIO_Port, SDA0_Pin,LL_GPIO_PULL_NO);\
                    LL_GPIO_SetPinPull(SDA2_GPIO_Port, SDA0_Pin,LL_GPIO_PULL_NO);\
                    LL_GPIO_SetPinPull(SDA3_GPIO_Port, SDA0_Pin,LL_GPIO_PULL_NO);

#define DE_0output  LL_GPIO_SetPinMode(SDA0_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_OUTPUT);\
                    LL_GPIO_SetPinMode(SDA1_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_OUTPUT);\
                    LL_GPIO_SetPinMode(SDA2_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_OUTPUT);\
                    LL_GPIO_SetPinMode(SDA3_GPIO_Port, SDA0_Pin, LL_GPIO_MODE_OUTPUT);\
                    LL_GPIO_SetPinOutputType(SDA0_GPIO_Port, SDA0_Pin, LL_GPIO_OUTPUT_PUSHPULL);\
                    LL_GPIO_SetPinOutputType(SDA1_GPIO_Port, SDA0_Pin, LL_GPIO_OUTPUT_PUSHPULL);\
                    LL_GPIO_SetPinOutputType(SDA2_GPIO_Port, SDA0_Pin, LL_GPIO_OUTPUT_PUSHPULL);\
                    LL_GPIO_SetPinOutputType(SDA3_GPIO_Port, SDA0_Pin, LL_GPIO_OUTPUT_PUSHPULL);

#define MCE_0input  LL_GPIO_SetPinMode(SCK0_GPIO_Port, SCK0_Pin, LL_GPIO_MODE_INPUT);\
                    LL_GPIO_SetPinOutputType(SCK0_GPIO_Port, SCK0_Pin, LL_GPIO_PULL_NO);\

#define MCE_0output LL_GPIO_SetPinMode(SCK0_GPIO_Port, SCK0_Pin, LL_GPIO_MODE_OUTPUT);\
                    LL_GPIO_SetPinOutputType(SCK0_GPIO_Port, SCK0_Pin, LL_GPIO_OUTPUT_PUSHPULL);\


//======================================= I2C interface initialization ========================

//TRISFbits.TRISF6=0; // CLK in I2C mode

#define I2CCLKPW		1             //This value define the CLK frequency
#define REGISTER_LENGTH	16

//External functions
//==================



static unsigned char ReceivedByte[4] = { 0, 0, 0, 0 };

void Wait(unsigned int cycles) {

	while (cycles>0)
	{
		cycles--;
	//HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
	//	GPIOA->ODR ^= GPIO_PIN_5; //da aggiungere se si ottimizza
	}

}

void I2C_Multi_SDA_deInit(unsigned char Channel) {
  MCE_0output
  DE_0output
  MCO_0_off
  DO_0off
}

void I2C_Multi_SDA_Init(unsigned char Channel) {
  MCE_0output
  DE_0output
}

void I2C_Multi_SDA_test(unsigned char Channel) {
  DE_0output
  MCO_0_on
  Wait(I2Cbit);           // Wait(I2Cbit);
  MCO_0_off
  DO_0on
  Wait(I2Cbit);           //Wait(I2Cbit);
  MCO_0_on
  DO_0on
  Wait(I2Cbit);           //Wait(I2Cbit);
  MCO_0_off
  DO_0off
  Wait(I2Cbit);           //Wait(I2Cbit);
}

//---------------------------------
//WriteAD7147ViaI2C()
//---------------------------------
//Function that writes one byte via the I2C port . It sends first the device
//address including the write bit, then the register address and finally the
//data. The function returns "1" if successfull otherwise "0".
//--------------------------------------------------------------------------------
unsigned char WriteByteViaI2C(unsigned char Channel,
		unsigned char DeviceAddress, const unsigned char RegisterStartAddress,
		unsigned char DataBuffer) {

	unsigned char ByteAddress;
	unsigned char AcknError;
	unsigned char DeviceAddressHeader;
	AcknError = 1; //No error on initialisation

	//Add the write bit to the device address
	DeviceAddressHeader = DeviceAddress << 1 | I2C_WR;
	//Split the address in two bytes
	ByteAddress = RegisterStartAddress & 0xFF;

	//Start the I2C transfer
	InitialiseI2CMaster(Channel);
	StartI2CMaster(Channel);
	//Send device address
	if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
		//Send register address if the acknowledgement is there
		if (!SendByteI2CMaster(Channel, ByteAddress)) {
			SendByteI2CMaster(Channel, DataBuffer);

			StopI2CMaster(Channel);
			AcknError = 0;
		} else //No acknowledgement was found therefore send the stop condition
		{
			StopI2CMaster(Channel);
			AcknError = 0;
		}
	} else //No acknowledgement was found therefore send the stop condition
	{
		StopI2CMaster(Channel);
		AcknError = 0;
	}
	return (AcknError);
}

//---------------------------------
//WriteAD7147ViaI2C()
//---------------------------------
//Function that writes  via the I2C port pair of 16 bits data. It sends first the device
//address including the write bit, then the register address and finally the
//data. The function returns "1" if successfull otherwise "0".
//--------------------------------------------------------------------------------
unsigned char WriteViaI2C(unsigned char Channel, unsigned char DeviceAddress,
		const unsigned int RegisterStartAddress,
		const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer,
		const unsigned int OffsetInBuffer) {
	unsigned int DataToWrite;
	unsigned char LowByteAddress, HighByteAddress;
	unsigned char LowByteData, HighByteData;
	unsigned char r, AcknError;
	unsigned char DeviceAddressHeader;

	AcknError = 1; //No error on initialisation

	//Add the write bit to the device address
	DeviceAddressHeader = DeviceAddress << 1 | I2C_WR;
	//Split the address in two bytes
	HighByteAddress = (RegisterStartAddress & 0xFF00) >> 8;
	LowByteAddress = RegisterStartAddress & 0x00FF;

	//Start the I2C transfer
	InitialiseI2CMaster(Channel);
	StartI2CMaster(Channel);
	//Send device address
	if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
		//Send register address if the acknowledgement is there
		if (!SendByteI2CMaster(Channel, HighByteAddress)) {
			if (!SendByteI2CMaster(Channel, LowByteAddress)) {
				//Perform block write
				for (r = 0; r < NumberOfRegistersToWrite; r++) {
					DataToWrite = DataBuffer[OffsetInBuffer + r];
					LowByteData = DataToWrite & 0x00FF;
					HighByteData = (DataToWrite & 0xFF00) >> 8;
					if (!SendByteI2CMaster(Channel, HighByteData)) {
						SendByteI2CMaster(Channel, LowByteData);
					} else //No acknowledgement was found therefore send the stop condition
					{
						StopI2CMaster(Channel);
						AcknError = 0;
					}
				}
				//Stop transfer
				StopI2CMaster(Channel);
			} else //No acknowledgement was found therefore send the stop condition
			{
				StopI2CMaster(Channel);
				AcknError = 0;
			}
		} else //No acknowledgement was found therefore send the stop condition
		{
			StopI2CMaster(Channel);
			AcknError = 0;
		}
	} else //No acknowledgement was found therefore send the stop condition
	{
		StopI2CMaster(Channel);
		AcknError = 0;
	}
	return (AcknError);
}

//---------------------------------
//WriteAD7147ViaI2C()
//---------------------------------
//Function that writes  via the I2C port pair of 16 bits data. It sends first the device
//address including the write bit, then the register address and finally the
//data. The function returns "1" if successfull otherwise "0".
//--------------------------------------------------------------------------------
unsigned char WriteViaI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum,
		unsigned char DeviceAddress, const unsigned int RegisterStartAddress,
		const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer,
		const unsigned int OffsetInBuffer) {
	unsigned int DataToWrite;
	unsigned char LowByteAddress, HighByteAddress;
	unsigned char LowByteData, HighByteData;
	unsigned char r, AcknError;
	unsigned char DeviceAddressHeader;

	AcknError = 1; //No error on initialisation

	//Add the write bit to the device address
	DeviceAddressHeader = DeviceAddress << 1 | I2C_WR;
	//Split the address in two bytes
	HighByteAddress = (RegisterStartAddress & 0xFF00) >> 8;
	LowByteAddress = RegisterStartAddress & 0x00FF;

	//Start the I2C transfer
	InitialiseI2CMaster(Channel);
	StartI2CMaster_onSdaX(Channel, sdaNum);
	//Send device address
	if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, DeviceAddressHeader)) {
		//Send register address if the acknowledgement is there
		if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, HighByteAddress)) {
			if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, LowByteAddress)) {
				//Perform block write
				for (r = 0; r < NumberOfRegistersToWrite; r++) {
					DataToWrite = DataBuffer[OffsetInBuffer + r];
					LowByteData = DataToWrite & 0x00FF;
					HighByteData = (DataToWrite & 0xFF00) >> 8;
					if (!SendByteI2CMaster_onSdaX(Channel, sdaNum,
							HighByteData)) {
						SendByteI2CMaster_onSdaX(Channel, sdaNum, LowByteData);
					} else //No acknowledgement was found therefore send the stop condition
					{
						StopI2CMaster_onSdaX(Channel, sdaNum);
						AcknError = 0;
					}
				}
				//Stop transfer
				StopI2CMaster_onSdaX(Channel, sdaNum);
			} else //No acknowledgement was found therefore send the stop condition
			{
				StopI2CMaster_onSdaX(Channel, sdaNum);
				AcknError = 0;
			}
		} else //No acknowledgement was found therefore send the stop condition
		{
			StopI2CMaster_onSdaX(Channel, sdaNum);
			AcknError = 0;
		}
	} else //No acknowledgement was found therefore send the stop condition
	{
		StopI2CMaster_onSdaX(Channel, sdaNum);
		AcknError = 0;
	}
	return (AcknError);
}

unsigned char WriteByteI2C_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum,
        unsigned char DeviceAddress, const unsigned int RegisterStartAddress,
        const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer,
        const unsigned int OffsetInBuffer) {
    unsigned int DataToWrite;
    unsigned char LowByteAddress, HighByteAddress;
    unsigned char LowByteData, HighByteData;
    unsigned char r, AcknError;
    unsigned char DeviceAddressHeader;

    AcknError = 1; //No error on initialisation

    //Add the write bit to the device address
    DeviceAddressHeader = DeviceAddress << 1 | I2C_WR;
    //Start the I2C transfer
    InitialiseI2CMaster(Channel);
    StartI2CMaster_onSdaX(Channel, sdaNum);
    //Send device address
    if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, DeviceAddressHeader)) {
        //Send register address if the acknowledgement is there
            if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, RegisterStartAddress)) {
                //Perform block write
                for (r = 0; r < NumberOfRegistersToWrite; r++) {
                    DataToWrite = DataBuffer[OffsetInBuffer + r];
                    LowByteData = DataToWrite & 0x00FF;
                    if (!SendByteI2CMaster_onSdaX(Channel, sdaNum, LowByteData)){
                    } else //No acknowledgement was found therefore send the stop condition
                    {
                        StopI2CMaster_onSdaX(Channel, sdaNum);
                        AcknError = 0;
                    }
                }
                //Stop transfer
                StopI2CMaster_onSdaX(Channel, sdaNum);
            } else //No acknowledgement was found therefore send the stop condition
            {
                StopI2CMaster_onSdaX(Channel, sdaNum);
                AcknError = 0;
            }
    } else //No acknowledgement was found therefore send the stop condition
    {
        StopI2CMaster_onSdaX(Channel, sdaNum);
        AcknError = 0;
    }
    return (AcknError);
}





//---------------------------------
//ReadByteViaI2C()
//---------------------------------
//Function that reads a Byte from  via the I2C port. It sends first the device
//address including the write bit, then the register address and finally reads data
//back. The function returns "1" if successfull otherwise "0". If an error occurs,
//Then the stop condition is sent.
//--------------------------------------------------------------------------------
unsigned char ReadByteViaI2C(unsigned char Channel, unsigned char SDAnum,
		unsigned char DeviceAddress, const unsigned char RegisterAddress,
		unsigned char *DataBuffer) {

	unsigned char AcknError;
	unsigned char SDA_number = SDAnum; //for the palm
	unsigned char DeviceAddressHeader;

	AcknError = 1; //No error on initialisation

	//Add the write bit to the device address
	DeviceAddressHeader = DeviceAddress << 1 | I2C_WR; //qui ci vuole I2C_WR o RD secondo me RD

	//Start the I2C transfer
	InitialiseI2CMaster(Channel);
	StartI2CMaster(Channel);

	//Send device address
	if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
		//Send register address
		if (!SendByteI2CMaster(Channel, RegisterAddress)) {
			//Send the repeated start
			StartI2CMaster(Channel);
			//Send device address again changing the Rd/Wr bit
			DeviceAddressHeader = DeviceAddress << 1 | I2C_RD;
			if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
				//Perform block read, but first,we need to know if we must send an ACKN or a NACK

				ReceiveByteI2CMaster(Channel, NACK);
				*DataBuffer = ReceivedByte[SDA_number];
				//Stop transfer
				StopI2CMaster(Channel);
			} else //No acknowledgement was found therefore send the stop condition
			{
				StopI2CMaster(Channel);
				AcknError = 0;
			}
		} else //No acknowledgement was found therefore send the stop condition
		{
			StopI2CMaster(Channel);
			AcknError = 0;
		}
	} else //No acknowledgement was found therefore send the stop condition
	{
		StopI2CMaster(Channel);
		AcknError = 0;
	}
	return (AcknError);
}
//---------------------------------
//ReadViaI2C()
//---------------------------------
//Function that reads from  via the I2C port. It sends first the device
//address including the write bit, then the register address and finally reads data
//back. The function returns "1" if successfull otherwise "0". If an error occurs,
//Then the stop condition is sent.
//--------------------------------------------------------------------------------
unsigned char ReadViaI2C(unsigned char Channel, unsigned char DeviceAddress,
		const unsigned int RegisterStartAddress,
		const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer1,
		unsigned int *DataBuffer2, unsigned int *DataBuffer3,
		unsigned int *DataBuffer4, const unsigned int OffsetInBuffer) {
	unsigned char LowByteAddress, HighByteAddress;
	unsigned char LowByteData[4], HighByteData[4];
	unsigned char r, AcknError;
	unsigned char DeviceAddressHeader;

	AcknError = 1; //No error on initialisation

	//Add the write bit to the device address
	DeviceAddressHeader = DeviceAddress << 1 | I2C_WR; //qui ci vuole I2C_WR o RD secondo me RD
	//Split the address in two bytes
	HighByteAddress = (RegisterStartAddress & 0xFF00) >> 8;
	LowByteAddress = RegisterStartAddress & 0x00FF;

	//Start the I2C transfer
	InitialiseI2CMaster(Channel);
	StartI2CMaster(Channel);

	//Send device address
	if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
		//Send register address
		if (!SendByteI2CMaster(Channel, HighByteAddress)) {
			if (!SendByteI2CMaster(Channel, LowByteAddress)) {
				//Send the repeated start
				StartI2CMaster(Channel);
				//Send device address again changing the Rd/Wr bit
				DeviceAddressHeader = DeviceAddress << 1 | I2C_RD;
				if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
					//Perform block read, but first,we need to know if we must send an ACKN or a NACK
					if (NumberOfRegistersToRead == 1) {
						ReceiveByteI2CMaster(Channel, ACK);
						HighByteData[0] = ReceivedByte[0];
						HighByteData[1] = ReceivedByte[1];
						HighByteData[2] = ReceivedByte[2];
						HighByteData[3] = ReceivedByte[3];

						ReceiveByteI2CMaster(Channel, NACK);
						LowByteData[0] = ReceivedByte[0];
						LowByteData[1] = ReceivedByte[1];
						LowByteData[2] = ReceivedByte[2];
						LowByteData[3] = ReceivedByte[3];

						DataBuffer1[OffsetInBuffer] = ((HighByteData[0] & 0xFF)
								<< 8) | LowByteData[0];

						DataBuffer2[OffsetInBuffer] = ((HighByteData[1] & 0xFF)
								<< 8) | LowByteData[1];

						DataBuffer3[OffsetInBuffer] = ((HighByteData[2] & 0xFF)
								<< 8) | LowByteData[2];

						DataBuffer4[OffsetInBuffer] = ((HighByteData[3] & 0xFF)
								<< 8) | LowByteData[3];

					} else {
						for (r = 0; r < (NumberOfRegistersToRead - 1); r++) {
							ReceiveByteI2CMaster(Channel, ACK);
							HighByteData[0] = ReceivedByte[0];
							HighByteData[1] = ReceivedByte[1];
							HighByteData[2] = ReceivedByte[2];
							HighByteData[3] = ReceivedByte[3];

							ReceiveByteI2CMaster(Channel, ACK);
							LowByteData[0] = ReceivedByte[0];
							LowByteData[1] = ReceivedByte[1];
							LowByteData[2] = ReceivedByte[2];
							LowByteData[3] = ReceivedByte[3];

							DataBuffer1[OffsetInBuffer + r] = ((HighByteData[0]
									& 0xFF) << 8) | LowByteData[0];

							DataBuffer2[OffsetInBuffer + r] = ((HighByteData[1]
									& 0xFF) << 8) | LowByteData[1];

							DataBuffer3[OffsetInBuffer + r] = ((HighByteData[2]
									& 0xFF) << 8) | LowByteData[2];

							DataBuffer4[OffsetInBuffer + r] = ((HighByteData[3]
									& 0xFF) << 8) | LowByteData[3];

						}
						//Do the last read sending the NACK
						ReceiveByteI2CMaster(Channel, ACK);
						HighByteData[0] = ReceivedByte[0];
						HighByteData[1] = ReceivedByte[1];
						HighByteData[2] = ReceivedByte[2];
						HighByteData[3] = ReceivedByte[3];

						ReceiveByteI2CMaster(Channel, NACK);
						LowByteData[0] = ReceivedByte[0];
						LowByteData[1] = ReceivedByte[1];
						LowByteData[2] = ReceivedByte[2];
						LowByteData[3] = ReceivedByte[3];

						DataBuffer1[OffsetInBuffer + NumberOfRegistersToRead - 1] =
								((HighByteData[0] & 0xFF) << 8)
										| LowByteData[0];

						DataBuffer2[OffsetInBuffer + NumberOfRegistersToRead - 1] =
								((HighByteData[1] & 0xFF) << 8)
										| LowByteData[1];

						DataBuffer3[OffsetInBuffer + NumberOfRegistersToRead - 1] =
								((HighByteData[2] & 0xFF) << 8)
										| LowByteData[2];

						DataBuffer4[OffsetInBuffer + NumberOfRegistersToRead - 1] =
								((HighByteData[3] & 0xFF) << 8)
										| LowByteData[3];

					}
					//Stop transfer
					StopI2CMaster(Channel);
				} else //No acknowledgement was found therefore send the stop condition
				{
					StopI2CMaster(Channel);
					AcknError = 0;
				}
			} else //No acknowledgement was found therefore send the stop condition
			{
				StopI2CMaster(Channel);
				AcknError = 0;
			}
		} else //No acknowledgement was found therefore send the stop condition
		{
			StopI2CMaster(Channel);
			AcknError = 0;
		}
	} else //No acknowledgement was found therefore send the stop condition
	{
		StopI2CMaster(Channel);
		AcknError = 0;
	}
	return (AcknError);
}

/********************************************************************************************************/
/*** Low level functions, do not change anything below this line, however check the valid clock level ***/
/********************************************************************************************************/
//ReadViaI2C()
//---------------------------------
//Function that reads from  via the I2C port. It sends first the device
//address including the write bit, then the register address and finally reads data
//back. The function returns "1" if successfull otherwise "0". If an error occurs,
//Then the stop condition is sent.
//--------------------------------------------------------------------------------
unsigned char ReadBurstViaI2C(unsigned char Channel, unsigned char SDAnum,
		unsigned char DeviceAddress, const unsigned int RegisterStartAddress,
		const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer) {
	unsigned char ByteAddress;
	unsigned char LowByteData, HighByteData;
	unsigned char r, AcknError;
	unsigned char SDA_number = SDAnum;
	unsigned char DeviceAddressHeader;

	AcknError = 1; //No error on initialisation

	//Add the write bit to the device address
	DeviceAddressHeader = DeviceAddress << 1 | I2C_WR; //qui ci vuole I2C_WR o RD secondo me RD

	ByteAddress = RegisterStartAddress & 0x00FF;

	//Start the I2C transfer
	InitialiseI2CMaster(Channel);
	StartI2CMaster(Channel);

	//Send device address
	if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
		//Send register address

		if (!SendByteI2CMaster(Channel, ByteAddress)) {
			//Send the repeated start
			StartI2CMaster(Channel);
			//Send device address again changing the Rd/Wr bit
			DeviceAddressHeader = DeviceAddress << 1 | I2C_RD;
			if (!SendByteI2CMaster(Channel, DeviceAddressHeader)) {
				//Perform block read, but first,we need to know if we must send an ACKN or a NACK
				if (NumberOfRegistersToRead == 1) {
					ReceiveByteI2CMaster(Channel, ACK);
					HighByteData = ReceivedByte[SDA_number];
					;

					ReceiveByteI2CMaster(Channel, NACK);
					LowByteData = ReceivedByte[SDA_number];
					;

					DataBuffer[0] = ((HighByteData & 0xFF) << 8) | LowByteData;

				} else {
					for (r = 0; r < (NumberOfRegistersToRead - 1); r++) {
						ReceiveByteI2CMaster(Channel, ACK);
						HighByteData = ReceivedByte[SDA_number];
						;

						ReceiveByteI2CMaster(Channel, ACK);
						LowByteData = ReceivedByte[SDA_number];
						;

						DataBuffer[r] = ((HighByteData & 0xFF) << 8)
								| LowByteData;

					}
					//Do the last read sending the NACK
					ReceiveByteI2CMaster(Channel, ACK);
					HighByteData = ReceivedByte[SDA_number];
					;

					ReceiveByteI2CMaster(Channel, NACK);
					LowByteData = ReceivedByte[SDA_number];
					;

					DataBuffer[NumberOfRegistersToRead - 1] = ((HighByteData
							& 0xFF) << 8) | LowByteData;

				}
				//Stop transfer
				StopI2CMaster(Channel);
			} else //No acknowledgement was found therefore send the stop condition
			{
				StopI2CMaster(Channel);
				AcknError = 0;
			}
		} else //No acknowledgement was found therefore send the stop condition
		{
			StopI2CMaster(Channel);
			AcknError = 0;
		}

	} else //No acknowledgement was found therefore send the stop condition
	{
		StopI2CMaster(Channel);
		AcknError = 0;
	}
	return (AcknError);
}

//---------------------------------
//InitialiseI2CMaster();
//---------------------------------
//Function that configures the I2C port of the ADuC841 in master mode.
//--------------------------------------------------------------------------------

void InitialiseI2CMaster(unsigned char Channel) {
	switch (Channel) {
	case CH0: {
	//	MCE_0output
	}
		break;
	case CH1: {
	//	MCE_1output;
	}
		break;
	}
	/*modificated
	 TRISFbits.TRISF6=0; //Write a "0" to CLK so that it becomes an output.
	 */
	//I2CCON = 0xA8;		//Master mode
}

//---------------------------------
//StartI2CMaster();
//---------------------------------
//Function that implements the start condition of the I2C protocol. The start
//condition consists in a falling edge on SDA when SCL is high.
//--------------------------------------------------------------------------------
void StartI2CMaster(unsigned char Channel) {
  DE_0input;
  Wait(I2Cbit);
  MCO_0_on
  //SCL high
  Wait(I2Cbit);
  Wait(I2Cbit);
  DE_0output
  //SDA as output
  DO_0off
  Wait(I2Cbit);
  MCO_0_off
  //SCL low
  Wait(I2Cbit);
}

void SetSdaReg(i2c_sda_num_t sdaNum, unsigned char input) {
	switch (sdaNum) {
	case sda0: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<9)); //SDA0 as input
	}
		break;
	case sda1: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<8)); //SDA1 as input
	}
		break;
	case sda2: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<7)); //SDA2 as input
	}
		break;
	case sda3: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<6)); //SDA3 as input
	}
		break;
	case sda4: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<4)); //SDA4 as input
	}
		break;
	case sda5: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<3)); //SDA5 as input
	}
		break;
	case sda6: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<2)); //SDA6 as input
	}
		break;
	case sda7: {
		CLEAR_BIT(GPIOA->OTYPER, (GPIO_MODE_INPUT<<1)); //SDA7 as input
	}
		break;
	};

}

void SetValReg(i2c_sda_num_t sdaNum, unsigned char val) {
	switch (sdaNum) {
	case sda0: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 9)); //SDA0
		else
			SET_BIT(GPIOA->ODR, (1 << 9)); //SDA0
	}
		break;
	case sda1: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 8)); //SDA1
		else
			SET_BIT(GPIOA->ODR, (1 << 8)); //SDA1
	}
		break;
	case sda2: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 7)); //SDA2
		else
			SET_BIT(GPIOA->ODR, (1 << 7)); //SDA2
	}
		break;
	case sda3: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 6)); //SDA3
		else
			SET_BIT(GPIOA->ODR, (1 << 6)); //SDA3
	}
		break;
	case sda4: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 4)); //SDA4
		else
			SET_BIT(GPIOA->ODR, (1 << 4)); //SDA4
	}
		break;
	case sda5: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 3)); //SDA5
		else
			SET_BIT(GPIOA->ODR, (1 << 3)); //SDA5
	}
		break;
	case sda6: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 2)); //SDA6
		else
			SET_BIT(GPIOA->ODR, (1 << 2)); //SDA6
	}
		break;
	case sda7: {
		if (val == 0)
			CLEAR_BIT(GPIOA->ODR, (1 << 1)); //SDA7
		else
			SET_BIT(GPIOA->ODR, (1 << 1)); //SDA7
	}
		break;
	};

}

void StartI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum) {
	StartI2CMaster(Channel);
	/*
	switch (Channel) {
	case CH0: {
		//DE_0input;
		SetSdaReg(sdaNum, 1);

		Wait(I2Cbit);
		MCO_0_on
		//SCL high
		Wait(I2Cbit);

		Wait(I2Cbit);
		//DE_0output    //SDA as output
		SetSdaReg(sdaNum, 0);

		//DO_0off
		SetValReg(sdaNum, 0);
		Wait(I2Cbit);
		MCO_0_off
		//SCL low
		Wait(I2Cbit);
	}
		break;
	case CH1: {
		//DE_0input;
		SetSdaReg(sdaNum, 1);

		Wait(I2Cbit);
		MCO_1_on
		//SCL high
		Wait(I2Cbit);

		Wait(I2Cbit);
		//DE_0output    //SDA as output
		SetSdaReg(sdaNum, 0);

		//DO_0off
		SetValReg(sdaNum, 0);
		Wait(I2Cbit);
		MCO_1_off
		//SCL low
		Wait(I2Cbit);
	}
		break;
	}
*/
}

//---------------------------------
//StopI2CMaster();
//---------------------------------
//Function that implements the stop condition of the I2C protocol. The stop
//condition consists in a rising edge on SDA when SCL is high.
//--------------------------------------------------------------------------------
void StopI2CMaster(unsigned char Channel) {
		DE_0output
		//SDA as output
		DO_0off
		//SDA low
		Wait(I2Cbit);
		MCO_0_on
		//SCL high
		Wait(I2Cbit);    //Wait(I2Cbit);
		DE_0input
		//SDA as output
		//DO_0on   //SDA goes from low to high when SCL is already high,
		Wait(I2Cbit);//Wait(I2Cbit);
}

void StopI2CMaster_onSdaX(unsigned char Channel, i2c_sda_num_t sdaNum) {
    StopI2CMaster(Channel);
  
		//DE_0output  //SDA as output
		SetSdaReg(sdaNum, 0);
		//DO_0off  //SDA low
		SetValReg(sdaNum, 0);
		Wait(I2Cbit);
		MCO_0_on
		//SCL high
		Wait(I2Cbit);   //Wait(I2Cbit);
		//DE_0input  //SDA as output
		SetSdaReg(sdaNum, 1);
}

//---------------------------------
//SendByteI2CMaster();
//---------------------------------
//Function that sends a byte to the I2C port and then read the acknowledgement
//bit. If the acknowledgement is found, then the function returns "1" otherwise,
//it returns "0".
//--------------------------------------------------------------------------------
unsigned char SendByteI2CMaster(unsigned char Channel, unsigned char ByteToSend) {

	unsigned char i;
	unsigned char noack = 0;

		DE_0output
		//SDAs as output
		for (i = 8; i > 0; i--) {
			MCO_0_off
			//Reset SCL
			if (ByteToSend >> 7) {
				DO_0on;
			} else {
				DO_0off;
			}
			Wait(I2Cbit);
			MCO_0_on                     //Set SCL
			Wait(I2Cbit);
			Wait(I2Cbit);
			Wait(I2Cbit);
			MCO_0_off
			if (i == 1) {
				DE_0input
				;
			} else {                //Reset SCL
				Wait(I2Cbit);                //Wait(I2Cbit);
				ByteToSend <<= 1;         //Rotate data
			}
		}
		DO_0off
		Wait(I2Cbit);
		DE_0input //SDA becomes an input
		MCO_0_off
		//Reset SCL
		Wait(I2Cbit);                //Wait(I2Cbit);
		MCO_0_on                //Set SCL
		noack = 0;
		Wait(I2Cbit);                //Wait(I2Cbit);
		MCO_0_off
		DE_0output     //SDA becomes an output

	return (noack);
}

unsigned char SendByteI2CMaster_onSdaX(unsigned char Channel,
		i2c_sda_num_t sdaNum, unsigned char ByteToSend) {

	unsigned char i;
	unsigned char noack = 0;

		//DE_0output  //SDAs as output
		SetSdaReg(sdaNum, 0);

		for (i = 8; i > 0; i--) {
			MCO_0_off
			//Reset SCL
			/*@@@@@@@@@@@@
			 DO = ByteToSend >> 7;	//Send data to SDA pin
			 */
			if (ByteToSend >> 7) {
				//DO_0on;
				SetValReg(sdaNum, 1);
			} else {
				//DO_0off;
				SetValReg(sdaNum, 0);
			}
			Wait(I2Cbit); //Wait(0);

			//   Wait(I2Cbit);//Wait(I2Cbit);
			MCO_0_on
			//Set SCL
			Wait(I2Cbit);               //Wait(I2Cbit);
			Wait(I2Cbit);
			Wait(I2Cbit);
			MCO_0_off
			if (i == 1) {
				//DE_0input;
				SetSdaReg(sdaNum, 1);
			} else {                //Reset SCL
				Wait(I2Cbit);                //Wait(I2Cbit);
				ByteToSend <<= 1;         //Rotate data
			}
		}
		//DO_0off
		SetValReg(sdaNum, 0);

		//DE_0input                //SDA becomes an input
		SetSdaReg(sdaNum, 1);

		MCO_0_off
		//Reset SCL

		Wait(I2Cbit);              //Wait(I2Cbit);
		MCO_0_on              //Set SCL

		noack = 0;

		Wait(I2Cbit);              //Wait(I2Cbit);
		MCO_0_off
		//DE_0output   //SDA becomes an output
		SetSdaReg(sdaNum, 0);

	return (noack);
}

//---------------------------------
//ReceiveByteI2CMaster();
//---------------------------------
//Function that reads one byte from the I2C port. If we do continuous read,
//then the acknowledgement must be "0" excepted for the last read sequence which
//it must be "1".
//--------------------------------------------------------------------------------
void ReceiveByteI2CMaster(unsigned char Channel, unsigned char ackn) // changed bit with unsigned char
{
	unsigned char i;
		DE_0input
		//SDA becomes an input
		MCO_0_off
		//Reset SCL
		for (i = 8; i > 0; i--) {
			Wait(I2Cbit);    //Wait(I2Cbit);
			ReceivedByte[0] <<= 1;      //Rotate data
			ReceivedByte[1] <<= 1;      //Rotate data
			ReceivedByte[2] <<= 1;      //Rotate data
			ReceivedByte[3] <<= 1;      //Rotate data
			MCO_0_on                //Set SCL
			ReceivedByte[0] |= (uint16_t) ((GPIOA->IDR & (1<<9))>>9);         //Read SDA0 -> data
			ReceivedByte[1] |= ((GPIOA->IDR &(1<<8))>>8);       //Read SDA1 -> data
			ReceivedByte[2] |= ((GPIOA->IDR &(1<<7))>>7);       //Read SDA2 -> data
			ReceivedByte[3] |= ((GPIOA->IDR &(1<<6))>>6);       //Read SDA3 -> data
			Wait(I2Cbit);    //Wait(I2Cbit);
			MCO_0_off
			//Reset SCL
		}
		DE_0output
		//SDA becomes an output
		if (ackn == 1) {
			DO_0on
		} else
			DO_0off
		Wait(I2Cbit);               //Wait(I2Cbit);
		MCO_0_on
		//Set SCL
		Wait(I2Cbit);    //Wait(I2Cbit);
		MCO_0_off
		//Reset SCL

}




