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
 * Modified: Andrea Mura, marco.accame
 *
 ******************************************************************************/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "if2hw_common.h"

#include "AD7147_Configuration.h"

// marco.accame: removed
//#include "stm32l4xx_hal.h" 
#include "AD7147RegMap.h"
#include "I2C_Multi_SDA.h"

// marco.accame: moved into AD7147_Configuration.h to avoid multiple definitions.
//extern void ConfigAD7147(unsigned char Channel, unsigned int i, unsigned int pw_control_val, if2hw_data_ad7147_t * convalue); //i is the number of the triangle
//extern void ConfigAD7147_onSdaX(unsigned char Channel, unsigned char setNum, unsigned char indexInSet, unsigned int pw_control_val, if2hw_data_ad7147_t cdcoffset);

// marco.accame: moved into AD7147_Configuration.h (also, made it const)
//unsigned char AD7147_ADD[4] = {0x2c, 0x2d, 0x2e, 0x2f};

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


#include "AD7147.h"

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#if !defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)

if2hw_data_ad7147_t AD7147Registers[16][12];	// il primo campo rappresenta il numero dei canali (non c'e' +)
if2hw_data_ad7147_t CapOffset[16][12];


#endif

#if defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)
#else

triangle_cfg_t triangle_cfg_list[16];
error_cap err[16];
5th_triangle_cfg_t triangle_cfg_list[4];
5th_error_cap err[4];

#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const if2hw_data_ad7147_t PW_CONTROL= 0x0B0; // 0x1B0 for 128 decim
static const if2hw_data_ad7147_t s_ConValue[2]={0x2200, 0x2200};	//offset of the CDC reading


#if defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)
#else
static const unsigned int SHIFT = 2;
#endif

#if defined(if2hw_common_AD7147_USE_EXTERNALDATA)

static if2hw_data_ad7147_t (*s_AD7147Registers)[12] = NULL;
static if2hw_data_ad7147_t (*s_CapOffset)[12] = NULL;

static if2hw_data_ad7147_t (*s_AD7147Registers_5th)[12] = NULL;
static if2hw_data_ad7147_t (*s_CapOffset_5th)[12] = NULL;

#else


static if2hw_data_ad7147_t (*s_AD7147Registers)[12] = AD7147Registers;
static if2hw_data_ad7147_t (*s_CapOffset)[12] = CapOffset;

#endif

uint16_t SKV = 0;

static uint32_t triangleconnectionmask = 0xffff;




void SetCDCoffsetOnSingleTriangle(uint16_t cdcOffset, unsigned char triangleN)
{
#if defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)
    
    ConfigAD7147_onSdaX(CH0, triangleN/4, triangleN%4,  PW_CONTROL, cdcOffset);
    
#else
    
    
//    uint16_t cdcOffset_aux = cdcOffset;
	 //init triangle_cfg_list
	unsigned int j=0;

	for(j=0;j<triangles_max_num; j++)
	{
		triangle_cfg_list[j].shift = SHIFT;
		triangle_cfg_list[j].CDCoffset = s_ConValue[0];
		triangle_cfg_list[j].setNum = j/4;
		triangle_cfg_list[j].indexInSet =  j%4;
	}

    ConfigAD7147_onSdaX(CH0, triangle_cfg_list[triangleN].setNum, triangle_cfg_list[triangleN].indexInSet,  PW_CONTROL, cdcOffset);

    //ConfigAD7147(CH0,triangle_cfg_list[triangleN].indexInSet,PW_CONTROL, &cdcOffset_aux);

#endif
}

void SetCDCoffsetOnAllTriangles(unsigned char Channel, if2hw_data_ad7147_t cdcOffset)
{
    uint8_t i;
    if2hw_data_ad7147_t cdcOffset_aux = cdcOffset;
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
    // code added SD + Marco Accame
    constexpr size_t NumberOfRegistertoRead  {12};
	for (i=0;i<4;i++)// this 4 is number of addresses on each I2C line
	{
	   ReadViaI2C(Channel,AD7147_ADD[i],(ADCRESULT_S0), NumberOfRegistertoRead,
        s_AD7147Registers[i],
        s_AD7147Registers[i+4],
        s_AD7147Registers[i+8],
        s_AD7147Registers[i+12],
        s_AD7147Registers_5th[i],
        0);
	}
    static int x = 0;
    x++;
    if(x > 11) x =0;
#ifdef DEBUG_FIFTH_I2C
    SKV = s_AD7147Registers_5th[0][x];
#endif

    
}

void TrianglesInit(unsigned char Channel, uint8_t applycdcoffset)
{
    int i,k;
    int unconnect=0;
    
    triangleconnectionmask = 0xffff;

    if(1 == applycdcoffset)
    {
    	SetCDCoffsetOnAllTriangles(Channel, s_ConValue[0]);
    }
    	//TIMER
    	ServiceAD7147Isr(Channel);
    	//TIMER
    	for (i=0;i<16;i++)
    	{
#if !defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)
            err[i].error=error_ok;
#endif            
            for (k=0;k<12;k++)
            {
                s_CapOffset[i][k]=s_AD7147Registers[i][k];
                if (s_AD7147Registers[i][k]==0xFFFF)
                {
                    unconnect +=1;
                }
            }
            if (unconnect==12) // no answer from the chip at startup
            {
#if !defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)                
                err[i].error |=error_notconnected;
#endif                
                // clear bit i-th
                triangleconnectionmask &= (~(0x0001<<i));

            }
            unconnect=0;
#if !defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)            
            triangle_cfg_list[i].shift = SHIFT;
#endif
    	}
   	for (i=0;i<4;i++)
    	{
            for (k=0;k<12;k++)
            {
                s_CapOffset_5th[i][k]=s_AD7147Registers_5th[i][k];              
                if (s_AD7147Registers_5th[i][k]==0xFFFF)
                {
                    unconnect +=1;
                }
            }
            if (unconnect==12) // no answer from the chip at startup
            {
#if !defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)                
                err[i].error |=error_notconnected;
#endif                
                // clear bit i-th
                triangleconnectionmask &= (~(0x0001<<i));

            }
            unconnect=0;
#if !defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)            
            5th_triangle_cfg_list[i].shift = SHIFT;
#endif
    	}
}



// - new api


extern void ad7147_init(if2hw_data_ad7147_t ext_rawvalues[][12], if2hw_data_ad7147_t ext_capoffsets[][12],
    if2hw_data_ad7147_t ext_rawvalues5th[][12], if2hw_data_ad7147_t ext_capoffsets5th[][12])
{
    
#if defined(if2hw_common_AD7147_USE_EXTERNALDATA)    
    s_AD7147Registers = ext_rawvalues;
    s_CapOffset = ext_capoffsets;
    s_AD7147Registers_5th = ext_rawvalues5th;
    s_CapOffset_5th = ext_capoffsets5th;
    
#endif
    
    triangleconnectionmask = 0;
    
    SetCDCoffsetOnAllTriangles(0, s_ConValue[0]);
    
    ad7147_calibrate();  
}


extern void ad7147_set_cdcoffset(uint8_t trg, uint16_t cdcoffset)
{
    SetCDCoffsetOnSingleTriangle(cdcoffset, trg);
}


extern uint16_t ad7147_gettrianglesconnectedmask(void)
{
    return triangleconnectionmask;
}


extern void ad7147_calibrate(void)
{
    TrianglesInit(0, 0);
}


extern void ad7147_acquire(void)
{
    ServiceAD7147Isr(0);
}


extern if2hw_data_ad7147_t * ad7147_get12rawvaluesoftriangle(uint16_t trg)
{
    if(trg >= triangles_max_num + triangles_add_num )
    {
        return(NULL);
    }
    else if ( trg >= triangles_max_num)
    {
        return(s_AD7147Registers_5th[0]);//trg - triangles_max_num]); 
    }
    else
        return(s_AD7147Registers[trg]);
}  


extern if2hw_data_ad7147_t * ad7147_get12capoffsetsoftriangle(uint16_t trg)
{
    if(trg >= triangles_max_num + triangles_add_num )
    {
        return(NULL);
    }
    else if ( trg >= triangles_max_num)
    {
        return(s_CapOffset_5th[trg - triangles_max_num]); 
    }
    else
        return(s_CapOffset[trg]);
}  


extern uint8_t ad7147_istriangleconnected(uint8_t trg)
{
    if(trg >= triangles_max_num)
    {
        return(0);
    }
    
    if(triangleconnectionmask & (0x0001<<trg))
    {
        return(1);
    }
    
    return(0);    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




