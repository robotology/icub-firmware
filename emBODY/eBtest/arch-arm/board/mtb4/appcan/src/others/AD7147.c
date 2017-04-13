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
// acemor
//#include "stm32l4xx_hal.h" 
#include "stm32hal.h"
#include "AD7147RegMap.h"
#include "I2C_Multi_SDA.h"

unsigned int PW_CONTROL= 0x0B0; // 0x1B0 for 128 decim

extern void ConfigAD7147(unsigned char Channel, unsigned int i, unsigned int pw_control_val, uint16_t * convalue); //i is the number of the triangle
extern void ConfigAD7147_onSdaX(unsigned char Channel, unsigned char setNum, unsigned char indexInSet, unsigned int pw_control_val, uint16_t cdcoffset);

static triangle_cfg_t triangle_cfg_list[16];
const uint8_t AD7147_ADD[4] = {0x2c, 0x2d, 0x2e, 0x2f};
// acemor has changed from uinsigned int into uint16_t in order to save ram. moreover has removed first indexing [2]
static uint16_t AD7147Registers[16][12];	// il primo campo rappresenta il numero dei canali (non c'e' +)
static uint16_t CapOffset[16][12];  //
static uint16_t ConValue[2]={0x2200, 0x2200};	//offset of the CDC reading
static const unsigned int SHIFT=2;
static error_cap err[16];

#define triangles_max_num   16



extern void ad7147_init(void)
{
    TrianglesInit(0);    
}

extern void ad7147_acquire(void)
{
    ServiceAD7147Isr(0);
}

extern uint16_t * ad7147_get12valuesoftriangle(uint8_t trg)
{
    if(trg >= triangles_max_num)
    {
        return(NULL);
    }
    return(AD7147Registers[trg]);
}    


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

void SetCDCoffsetOnAllTriangles(unsigned char Channel, uint16_t cdcOffset)
{
    uint8_t i;
    uint16_t cdcOffset_aux = cdcOffset;
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
    uint8_t res = 0;
	for (i=0;i<4;i++)
	{
        res = ReadViaI2C(Channel,AD7147_ADD[i],(ADCRESULT_S0), 12, AD7147Registers[i],AD7147Registers[i+4],AD7147Registers[i+8],AD7147Registers[i+12], 0);
	}
    res = res;
    if(0 == res)
    {
        res = res;
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
                CapOffset[i][k]=AD7147Registers[i][k];
                if (AD7147Registers[i][k]==0xFFFF)
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
