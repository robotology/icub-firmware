/******************************************************************************
 * AD7147.c
 *
 *
 * Created on: Oct 29, 2015
 * Board: NUCLEO STML152RET
 * Author: mmarco aggiali
 *
 * Description: This file contains the functions for configure the AD7147 Capacitance to Digital Converter
 *
 *
 * Modified:
 *
 ******************************************************************************/


#include "if2hw_common.h"

#include "AD7147.h"

#include "AD7147RegMap.h"
#include "I2C_Multi_SDA.h"
#include "stdint.h"



//----------------------
//Function declarations
//----------------------     
//External functions
//==================
//unsigned char WriteViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
//unsigned char ReadViaI2C(unsigned char Channel, unsigned char DeviceAddress, const unsigned int RegisterStartAddress, const unsigned char NumberOfRegistersToRead, unsigned int *DataBuffer, const unsigned int OffsetInBuffer);
//Local functions
//===============

// acemor: move them into 

#include "AD7147_Configuration.h"

//void ConfigAD7147(unsigned char Channel,    unsigned int i, unsigned int pw_control_val, unsigned int * convalue); //i is the number of the triangle
//void ConfigAD7147_THREE(unsigned char Channel,unsigned int i,unsigned int pw_control_val, unsigned int *convalue);
//void ConfigAD7147_ALL(unsigned char Channel,unsigned int i, unsigned int pw_control_val, unsigned int * convalue); //i is the number of the triangle
//void ConfigAD7147_onSdaX(unsigned char Channel, unsigned char setNum, unsigned char indexInSet, unsigned int pw_control_val, uint16_t cdcoffset);


//Recal of variables from other C files
//-------------------------------------
// acemor: use proper .h file (in this case: #include "AD7147.h").
//extern const uint8_t AD7147_ADD[4];

// moved in here as they belong to configuration.
const uint8_t AD7147_ADD[4] = {0x2c, 0x2d, 0x2e, 0x2f};

//---------------------
//Function definitions
//---------------------
void ConfigAD7147_T(unsigned char Channel, unsigned int i,unsigned int pw_control_val, if2hw_data_ad7147_t * convalue)
{
	if2hw_data_i2cmultisda_t ConfigBuffer[12] = {0};
	//=============================================
	//= Stage 0 - CIN0 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0002;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	//Stage 0 connection registers - Start address in RAM 0x80
	
	//=============================================
	//= Stage 1 - CIN1 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0008;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE1_CONNECTION, 8, ConfigBuffer, 0);	//Stage 1 connection registers - Start address in RAM 0x88
	
	//=============================================
	//= Stage 2 - CIN2 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0020;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE2_CONNECTION, 8, ConfigBuffer, 0);	//Stage 2 connection registers - Start address in RAM 0x90
	
	//=============================================
	//= Stage 3 - CIN3 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0080;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE3_CONNECTION, 8, ConfigBuffer, 0);	//Stage 3 connection registers - Start address in RAM 0x98
	
	//=============================================
	//= Stage 4 - CIN4 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0200;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE4_CONNECTION, 8, ConfigBuffer, 0);	//Stage 4 connection registers - Start address in RAM 0xA0
	
	//=============================================
	//= Stage 5 - CIN5 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0800;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE5_CONNECTION, 8, ConfigBuffer, 0);	//Stage 5 connection registers - Start address in RAM 0xA8
	
	//=============================================
	//= Stage 6 - CIN6 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x2000;//0xFFFF;
	ConfigBuffer[1]=0x1000;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE6_CONNECTION, 8, ConfigBuffer, 0);	//Stage 6 connection registers - Start address in RAM 0xB0
	
	//=============================================
	//= Stage 7 - CIN7 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0000;//0xFFFF;
	ConfigBuffer[1]=0x1002;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE7_CONNECTION, 8, ConfigBuffer, 0);	//Stage 7 connection registers - Start address in RAM 0xB8
	
	//=============================================
	//= Stage 8 - CIN8 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0000;//0xFFFF;
	ConfigBuffer[1]=0x1008;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE8_CONNECTION, 8, ConfigBuffer, 0);	//Stage 8 connection registers - Start address in RAM 0xC0
	
	//=============================================
	//= Stage 9 - CIN9 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0000;//0xFFFF;
	ConfigBuffer[1]=0x1020;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE9_CONNECTION, 8, ConfigBuffer, 0);	//Stage 9 connection registers - Start address in RAM 0xC8
	
	//=============================================
	//= Stage 10 - CIN10 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0000;//0xFFFF;
	ConfigBuffer[1]=0x1080;//0x3FFF; //
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE10_CONNECTION, 8, ConfigBuffer, 0);//Stage 10 connection registers - Start address in RAM 0xD0
	
	//=============================================
	//= Stage 11 - CIN11 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0x0000;//0xFFFF;
	ConfigBuffer[1]=0x1200;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE11_CONNECTION, 8, ConfigBuffer, 0);//Stage 11 connection registers - Start address in RAM 0xD8
	
	//=============================================
	//= Configure 1st register bank
	//=============================================
	//Initialisation of the first register bank but not the AMBCOMPCTL_REG0
	ConfigBuffer[PWR_CONTROL]=pw_control_val;	// Full power mode enabled at 32ms - 12 sequences - 256 decimation factor
//	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 1, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
//	ConfigBuffer[STAGE_CAL_EN]=0x0;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);

	//Calibration configuration
	ConfigBuffer[AMB_COMP_CTRL0]=0x0;;//0x220;//0x220;
	ConfigBuffer[AMB_COMP_CTRL1]=0x0;//0x14C8;
	ConfigBuffer[AMB_COMP_CTRL2]=0x0;//0xBFF;//0x0832;
	//Interrupt configuration
	ConfigBuffer[STAGE_LOW_INT_EN]=0x0000;
	ConfigBuffer[STAGE_HIGH_INT_EN]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_INT_EN]=0x400;//0x400;//0x0001;//0x0001;
	ConfigBuffer[STAGE_LOW_LIMIT_INT]=0x0000;
	ConfigBuffer[STAGE_HIGH_LIMIT_INT]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_LIMIT_INT]=0xfff;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 9, ConfigBuffer, AMB_COMP_CTRL0);
		WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 11, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
	ConfigBuffer[STAGE_CAL_EN]=0x0FFF;//0x0FFF;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
}

void ConfigAD7147(unsigned char Channel, unsigned int i,unsigned int pw_control_val, if2hw_data_ad7147_t * convalue)
{
	if2hw_data_i2cmultisda_t ConfigBuffer[12] = {0};
	//=============================================
	//= Stage 0 - CIN0 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFE;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	//Stage 0 connection registers - Start address in RAM 0x80
	
	//=============================================
	//= Stage 1 - CIN1 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFB;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE1_CONNECTION, 8, ConfigBuffer, 0);	//Stage 1 connection registers - Start address in RAM 0x88
	
	//=============================================
	//= Stage 2 - CIN2 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFEF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE2_CONNECTION, 8, ConfigBuffer, 0);	//Stage 2 connection registers - Start address in RAM 0x90
	
	//=============================================
	//= Stage 3 - CIN3 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFBF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE3_CONNECTION, 8, ConfigBuffer, 0);	//Stage 3 connection registers - Start address in RAM 0x98
	
	//=============================================
	//= Stage 4 - CIN4 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFEFF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE4_CONNECTION, 8, ConfigBuffer, 0);	//Stage 4 connection registers - Start address in RAM 0xA0
	
	//=============================================
	//= Stage 5 - CIN5 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFBFF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE5_CONNECTION, 8, ConfigBuffer, 0);	//Stage 5 connection registers - Start address in RAM 0xA8
	
	//=============================================
	//= Stage 6 - CIN6 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xEFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE6_CONNECTION, 8, ConfigBuffer, 0);	//Stage 6 connection registers - Start address in RAM 0xB0
	
	//=============================================
	//= Stage 7 - CIN7 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FFE;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE7_CONNECTION, 8, ConfigBuffer, 0);	//Stage 7 connection registers - Start address in RAM 0xB8
	
	//=============================================
	//= Stage 8 - CIN8 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FFB;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE8_CONNECTION, 8, ConfigBuffer, 0);	//Stage 8 connection registers - Start address in RAM 0xC0
	
	//=============================================
	//= Stage 9 - CIN9 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FEF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE9_CONNECTION, 8, ConfigBuffer, 0);	//Stage 9 connection registers - Start address in RAM 0xC8
	
	//=============================================
	//= Stage 10 - CIN10 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FBF;//0x3FFF; //
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE10_CONNECTION, 8, ConfigBuffer, 0);//Stage 10 connection registers - Start address in RAM 0xD0
	
	//=============================================
	//= Stage 11 - CIN11 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1EFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE11_CONNECTION, 8, ConfigBuffer, 0);//Stage 11 connection registers - Start address in RAM 0xD8
	
	//=============================================
	//= Configure 1st register bank
	//=============================================
	//Initialisation of the first register bank but not the AMBCOMPCTL_REG0
	ConfigBuffer[PWR_CONTROL]=pw_control_val;	// Full power mode enabled at 32ms - 12 sequences - 256 decimation factor
//	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 1, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
//	ConfigBuffer[STAGE_CAL_EN]=0x0;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);

	//Calibration configuration
	ConfigBuffer[AMB_COMP_CTRL0]=0x0;;//0x220;//0x220;
	ConfigBuffer[AMB_COMP_CTRL1]=0x0;//0x14C8;
	ConfigBuffer[AMB_COMP_CTRL2]=0x0;//0xBFF;//0x0832;
	//Interrupt configuration
	ConfigBuffer[STAGE_LOW_INT_EN]=0x0000;
	ConfigBuffer[STAGE_HIGH_INT_EN]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_INT_EN]=0x400;//0x400;//0x0001;//0x0001;
	ConfigBuffer[STAGE_LOW_LIMIT_INT]=0x0000;
	ConfigBuffer[STAGE_HIGH_LIMIT_INT]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_LIMIT_INT]=0xfff;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 9, ConfigBuffer, AMB_COMP_CTRL0);
		WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 11, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
	ConfigBuffer[STAGE_CAL_EN]=0x0FFF;//0x0FFF;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
}





void ConfigAD7147_onSdaX(unsigned char Channel, unsigned char setNum, unsigned char indexInSet, unsigned int pw_control_val, if2hw_data_ad7147_t cdcoffset/*unsigned int * cdcOffset*/)
{
    i2c_sda_num_t sdaNum = (i2c_sda_num_t)setNum;
    
	if2hw_data_i2cmultisda_t ConfigBuffer[12] = {0};
    
	//=============================================
	//= Stage 0 - CIN0 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFE;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	//Stage 0 connection registers - Start address in RAM 0x80
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	
    
	//=============================================
	//= Stage 1 - CIN1 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFB;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE1_CONNECTION, 8, ConfigBuffer, 0);	//Stage 1 connection registers - Start address in RAM 0x88
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE1_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 2 - CIN2 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFEF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE2_CONNECTION, 8, ConfigBuffer, 0);	//Stage 2 connection registers - Start address in RAM 0x90
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE2_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 3 - CIN3 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFBF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE3_CONNECTION, 8, ConfigBuffer, 0);	//Stage 3 connection registers - Start address in RAM 0x98
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE3_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 4 - CIN4 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFEFF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE4_CONNECTION, 8, ConfigBuffer, 0);	//Stage 4 connection registers - Start address in RAM 0xA0
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE4_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 5 - CIN5 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFBFF;//0xAAAA;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE5_CONNECTION, 8, ConfigBuffer, 0);	//Stage 5 connection registers - Start address in RAM 0xA8
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE5_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 6 - CIN6 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xEFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE6_CONNECTION, 8, ConfigBuffer, 0);	//Stage 6 connection registers - Start address in RAM 0xB0
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE6_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 7 - CIN7 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FFE;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE7_CONNECTION, 8, ConfigBuffer, 0);	//Stage 7 connection registers - Start address in RAM 0xB8
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE7_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 8 - CIN8 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FFB;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE8_CONNECTION, 8, ConfigBuffer, 0);	//Stage 8 connection registers - Start address in RAM 0xC0
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE8_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 9 - CIN9 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FEF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE9_CONNECTION, 8, ConfigBuffer, 0);	//Stage 9 connection registers - Start address in RAM 0xC8
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE9_CONNECTION, 8, ConfigBuffer, 0);	
    
	//=============================================
	//= Stage 10 - CIN10 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1FBF;//0x3FFF; //
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE10_CONNECTION, 8, ConfigBuffer, 0);//Stage 10 connection registers - Start address in RAM 0xD0
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE10_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Stage 11 - CIN11 Single-Ended(+) =
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF;
	ConfigBuffer[1]=0x1EFF;//0x3FFF;
	ConfigBuffer[2]=cdcoffset; //cdcOffset[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE11_CONNECTION, 8, ConfigBuffer, 0);//Stage 11 connection registers - Start address in RAM 0xD8
	WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE11_CONNECTION, 8, ConfigBuffer, 0);
    
	//=============================================
	//= Configure 1st register bank
	//=============================================
	//Initialisation of the first register bank but not the AMBCOMPCTL_REG0
	ConfigBuffer[PWR_CONTROL]=pw_control_val;	// Full power mode enabled at 32ms - 12 sequences - 256 decimation factor
//	WriteViaI2C(Channel,AD7147_ADD[indexInSet],PWR_CONTROL, 1, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
//	ConfigBuffer[STAGE_CAL_EN]=0x0;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[indexInSet],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);

	//Calibration configuration
	ConfigBuffer[AMB_COMP_CTRL0]=0x0;;//0x220;//0x220;
	ConfigBuffer[AMB_COMP_CTRL1]=0x0;//0x14C8;
	ConfigBuffer[AMB_COMP_CTRL2]=0x0;//0xBFF;//0x0832;
	//Interrupt configuration
	ConfigBuffer[STAGE_LOW_INT_EN]=0x0000;
	ConfigBuffer[STAGE_HIGH_INT_EN]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_INT_EN]=0x400;//0x400;//0x0001;//0x0001;
	ConfigBuffer[STAGE_LOW_LIMIT_INT]=0x0000;
	ConfigBuffer[STAGE_HIGH_LIMIT_INT]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_LIMIT_INT]=0xfff;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[indexInSet],AMB_COMP_CTRL0, 9, ConfigBuffer, AMB_COMP_CTRL0);
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],PWR_CONTROL, 11, ConfigBuffer, PWR_CONTROL);
    WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],PWR_CONTROL, 11, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
	ConfigBuffer[STAGE_CAL_EN]=0x0FFF;//0x0FFF;
	//WriteViaI2C_onSdaX2(Channel,sdaNum,AD7147_ADD[indexInSet],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
    WriteViaI2C_onSdaX2(sdaNum,AD7147_ADD[indexInSet],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
}


/* void ConfigAD7147(unsigned char Channel, unsigned int i,unsigned int pw_control_val, unsigned int * convalue)
{
    unsigned char j;

    for(j=sda1; j<i2c_sda_num_max; j++)
    {
        ConfigAD7147_onSdaX(Channel, i, sdaNum, pw_control_val, convalue);
    }
} */


void ConfigAD7147_ALL(unsigned char Channel,unsigned int i,unsigned int pw_control_val, if2hw_data_ad7147_t *convalue)
{
	if2hw_data_i2cmultisda_t ConfigBuffer[12] = {0};
	
	//=============================================
	//= Stage 0 - Connected to Vbias
	//=============================================
	ConfigBuffer[0]=0xEAAA;//0xFFFF;//0xFFFF; 9_10_11_12 (CIN8,CIN9,CIN10,CIN11)
	ConfigBuffer[1]=0x1EAA;//0x1EAB;//0x3EAB;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	//Stage 9 connection registers - Start address in RAM 0xC8
	
	//=============================================
	//= Configure 1st register bank
	//=============================================
	//Initialisation of the first register bank but not the AMBCOMPCTL_REG0
	ConfigBuffer[PWR_CONTROL]=pw_control_val;	// Full power mode enabled at 32ms - 4 sequences - 256 decimation factor
//	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 1, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences

//	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
	ConfigBuffer[STAGE_CAL_EN]=0x0;//0x0FFF;	
	//Calibration configuration
	ConfigBuffer[AMB_COMP_CTRL0]=0x3230;//0x220;
	ConfigBuffer[AMB_COMP_CTRL1]=0x14C8;//0x14c8
	ConfigBuffer[AMB_COMP_CTRL2]=0x0832;//0xBFF;//0x0832;
	//Interrupt configuration
	ConfigBuffer[STAGE_LOW_INT_EN]=0x0000;
	ConfigBuffer[STAGE_HIGH_INT_EN]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_INT_EN]=0x800;//0x0001;
//	ConfigBuffer[STAGE_LOW_LIMIT_INT]=0x0000;
//	ConfigBuffer[STAGE_HIGH_LIMIT_INT]=0x0000;
//	ConfigBuffer[STAGE_COMPLETE_LIMIT_INT]=0xFFF;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 9, ConfigBuffer, AMB_COMP_CTRL0);
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 6, ConfigBuffer, AMB_COMP_CTRL0);

	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 8, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
	ConfigBuffer[STAGE_CAL_EN]=0x0fff;//0x0FFF;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
}

void ConfigAD7147_THREE(unsigned char Channel,unsigned int i,unsigned int pw_control_val, if2hw_data_ad7147_t *convalue)
{
	if2hw_data_i2cmultisda_t ConfigBuffer[12] = {0};
	
	//=============================================
	//= Stage 0 - Connected to Vbias
	//=============================================
	ConfigBuffer[0]=0xFFAA;//0xFFFF; //1_2_3_4 (CIN0,CIN1,CIN2,CIN3)
	ConfigBuffer[1]=0x1FFF;//0x1FFF;//0x3FFF;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	//Stage 9 connection registers - Start address in RAM 0xC8
	
	//=============================================
	//= Stage 1 - Connected to Vbias
	//=============================================
	ConfigBuffer[0]=0x2AFF;//0xEAFF; 5_6_7_8 (CIN4,CIN5,CIN6,CIN7)
	ConfigBuffer[1]=0x1FFe;//0x3FF2;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE1_CONNECTION, 8, ConfigBuffer, 0);//Stage 10 connection registers - Start address in RAM 0xD0
	
	//=============================================
	//= Stage 2 - Connected to Vbias
	//=============================================
	ConfigBuffer[0]=0xFFFF;//0xFFFF; 9_10_11_12 (CIN8,CIN9,CIN10,CIN11)
	ConfigBuffer[1]=0x1EAB;//0x3EAB;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE2_CONNECTION, 8, ConfigBuffer, 0);//Stage 11 connection registers - Start address in RAM 0xD8
	
	//=============================================
	//= Configure 1st register bank
	//=============================================
	//Initialisation of the first register bank but not the AMBCOMPCTL_REG0
	ConfigBuffer[PWR_CONTROL]=pw_control_val;	// Full power mode enabled at 32ms - 4 sequences - 256 decimation factor
//	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 1, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences

//	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
	ConfigBuffer[STAGE_CAL_EN]=0x0;//0x0FFF;	
	//Calibration configuration
	ConfigBuffer[AMB_COMP_CTRL0]=0x3230;//0x220;
	ConfigBuffer[AMB_COMP_CTRL1]=0x14C8;//0x14c8
	ConfigBuffer[AMB_COMP_CTRL2]=0x0832;//0xBFF;//0x0832;
	//Interrupt configuration
	ConfigBuffer[STAGE_LOW_INT_EN]=0x0000;
	ConfigBuffer[STAGE_HIGH_INT_EN]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_INT_EN]=0x800;//0x0001;
//	ConfigBuffer[STAGE_LOW_LIMIT_INT]=0x0000;
//	ConfigBuffer[STAGE_HIGH_LIMIT_INT]=0x0000;
//	ConfigBuffer[STAGE_COMPLETE_LIMIT_INT]=0xFFF;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 9, ConfigBuffer, AMB_COMP_CTRL0);
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 6, ConfigBuffer, AMB_COMP_CTRL0);

	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 8, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
	ConfigBuffer[STAGE_CAL_EN]=0x0FFF;//0x0FFF;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
}
void ConfigAD7147_FINGERTIP(unsigned char Channel,unsigned int i,unsigned int pw_control_val, if2hw_data_ad7147_t *convalue)
{
	if2hw_data_i2cmultisda_t ConfigBuffer[12] = {0};
	
	//=============================================
	//= Stage 0 - Connected to Vbias
	//=============================================
	ConfigBuffer[0]=0xFF2A;//0xFFFF; // (CIN0,CIN1,CIN2)
	ConfigBuffer[1]=0x1FFF;
//	ConfigBuffer[1]=0x1EAB;//0x1FFF;//0x3FFF;  (CIN8,CIN9,CIN10,CIN11)
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE0_CONNECTION, 8, ConfigBuffer, 0);	//Stage 9 connection registers - Start address in RAM 0xC8
	
	//=============================================
	//= Stage 1 - Connected to Vbias
	//=============================================
	ConfigBuffer[0]=0x2ABF;//0xEAFF; 5_6_7_8 (CIN3,CIN4,CIN5,CIN6,CIN7)
	ConfigBuffer[1]=0x1FFe;//0x3FF2;
	ConfigBuffer[2]=convalue[0];
	ConfigBuffer[3]=0x2626;
	ConfigBuffer[4]=50;
	ConfigBuffer[5]=50;
	ConfigBuffer[6]=100;
	ConfigBuffer[7]=100;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE1_CONNECTION, 8, ConfigBuffer, 0);//Stage 10 connection registers - Start address in RAM 0xD0
	
	
	//=============================================
	//= Configure 1st register bank
	//=============================================
	//Initialisation of the first register bank but not the AMBCOMPCTL_REG0
	ConfigBuffer[PWR_CONTROL]=pw_control_val;	// Full power mode enabled at 32ms - 4 sequences - 256 decimation factor
//	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 1, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences

//	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
	ConfigBuffer[STAGE_CAL_EN]=0x0;//0x0FFF;	
	//Calibration configuration
	ConfigBuffer[AMB_COMP_CTRL0]=0x3230;//0x220;
	ConfigBuffer[AMB_COMP_CTRL1]=0x14C8;//0x14c8
	ConfigBuffer[AMB_COMP_CTRL2]=0x0832;//0xBFF;//0x0832;
	//Interrupt configuration
	ConfigBuffer[STAGE_LOW_INT_EN]=0x0000;
	ConfigBuffer[STAGE_HIGH_INT_EN]=0x0000;
	ConfigBuffer[STAGE_COMPLETE_INT_EN]=0x800;//0x0001;
//	ConfigBuffer[STAGE_LOW_LIMIT_INT]=0x0000;
//	ConfigBuffer[STAGE_HIGH_LIMIT_INT]=0x0000;
//	ConfigBuffer[STAGE_COMPLETE_LIMIT_INT]=0xFFF;//0x0FFF;
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 9, ConfigBuffer, AMB_COMP_CTRL0);
//	WriteViaI2C(Channel,AD7147_ADD[i],AMB_COMP_CTRL0, 6, ConfigBuffer, AMB_COMP_CTRL0);

	WriteViaI2C(Channel,AD7147_ADD[i],PWR_CONTROL, 8, ConfigBuffer, PWR_CONTROL);
	// Run data path for all sequences
	ConfigBuffer[STAGE_CAL_EN]=0x0FFF;//0x0FFF;
	WriteViaI2C(Channel,AD7147_ADD[i],STAGE_CAL_EN, 1, ConfigBuffer, STAGE_CAL_EN);
}


