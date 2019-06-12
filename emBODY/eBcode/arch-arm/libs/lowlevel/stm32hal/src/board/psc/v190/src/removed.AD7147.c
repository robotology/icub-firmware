/******************************************************************************
 * AD7147.c
 *
 *
 * Created on: Oct 29, 2015
 * Board: NUCLEO STML152RET
 * Author: mmarco aggiali
 *
 * Description: This file contains the functions for sending and reading info to the AD7147 Capacitance to Digital Converter
 *
 *
 * Modified: Andrea Mura
 *
 ******************************************************************************/


#include "AD7147.h"
#include "stm32l4xx_hal.h"
#include "AD7147RegMap.h"
#include "I2C_Multi_SDA.h"

unsigned int PW_CONTROL= 0x0B0; // 0x1B0 for 128 decim

extern void ConfigAD7147(unsigned char Channel, unsigned int i, unsigned int pw_control_val, unsigned int * convalue); //i is the number of the triangle
extern void ConfigAD7147_onSdaX(unsigned char Channel, unsigned char setNum, unsigned char indexInSet, unsigned int pw_control_val, uint16_t cdcoffset);

triangle_cfg_t triangle_cfg_list[16];
unsigned char AD7147_ADD[4] = {0x2c, 0x2d, 0x2e, 0x2f};
unsigned int AD7147Registers[2][16][12];	// il primo campo rappresenta il numero dei canali
unsigned int CapOffset[2][16][12];  //
unsigned int ConValue[2]={0x2000, 0x2000};	//offset of the CDC reading
unsigned int SHIFT=2;
error_cap err[16];

#define triangles_max_num   16

void SetCDCoffsetOnSingleTriangle(uint16_t cdcOffset, unsigned char triangleN)
{
//    uint16_t cdcOffset_aux = cdcOffset;
	 //init triangle_cfg_list
	unsigned int j=0;

	for(j=0;j<triangles_max_num; j++)
	{
		triangle_cfg_list[j].shift = SHIFT;
		triangle_cfg_list[j].CDCoffset = ConValue[0];
		triangle_cfg_list[j].setNum = j/4;
		triangle_cfg_list[j].indexInSet =  j%4;
	}

    ConfigAD7147_onSdaX(CH0, triangle_cfg_list[triangleN].setNum, triangle_cfg_list[triangleN].indexInSet,  PW_CONTROL, cdcOffset);

    //ConfigAD7147(CH0,triangle_cfg_list[triangleN].indexInSet,PW_CONTROL, &cdcOffset_aux);
}

void SetCDCoffsetOnAllTriangles(unsigned char Channel, unsigned int cdcOffset)
{
    uint8_t i;
    unsigned int cdcOffset_aux = cdcOffset;
    if(Channel==0){
		for (i=0;i<4;i++)
		{
			ConfigAD7147(CH0,i,PW_CONTROL, &cdcOffset_aux); //0 is the number of the device
		}
    }
    if(Channel==1){
		for (i=0;i<4;i++)
		{
			ConfigAD7147(CH1,i,PW_CONTROL, &cdcOffset_aux); //0 is the number of the device
		}
	}
}

void ServiceAD7147Isr(unsigned char Channel)
{
  unsigned int i=0;
	for (i=0;i<4;i++)
	{
	   ReadViaI2C(Channel,AD7147_ADD[i],(ADCRESULT_S0), 12, AD7147Registers[Channel][i],AD7147Registers[Channel][i+4],AD7147Registers[Channel][i+8],AD7147Registers[Channel][i+12], 0);
	}
}

void TrianglesInit(unsigned char Channel)
{
    int i,k;
    int unconnect=0;

    	SetCDCoffsetOnAllTriangles(Channel, ConValue[0]);
    	//TIMER
    	ServiceAD7147Isr(Channel);
    	//TIMER
    	for (i=0;i<16;i++)
    	{
            err[i].error=error_ok;
            for (k=0;k<12;k++)
            {
                CapOffset[Channel][i][k]=AD7147Registers[Channel][i][k];
                if (AD7147Registers[Channel][i][k]==0xFFFF)
                {
                    unconnect +=1;
                }
            }
            if (unconnect==12) // no answer from the chip at startup
            {
                err[i].error |=error_notconnected;
            }
            unconnect=0;
            triangle_cfg_list[i].shift = SHIFT;
    	}
}
