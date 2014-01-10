
/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "hl_core.h"
//#include "stm32f10x.h"
//#include "stm32f10x_flash.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "stm32f1_flash.h"     


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t stm32f1_flash_Lock(void)
{
    FLASH_Lock();
    return(0);
}


extern uint32_t stm32f1_flash_Unlock(void)
{
    FLASH_Unlock();
    return(0);
}

extern uint32_t stm32f1_flash_SetLatency(uint32_t lat)
{
    FLASH_SetLatency(lat);
    return(0);
}

extern uint32_t stm32f1_flash_ErasePage(uint32_t addr)
{
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(addr);
    return(0);
}


extern uint32_t stm32f1_flash_WriteWord(uint32_t addr, uint32_t data)
{
    FLASH_ProgramWord(addr, data);
    return(0);
}

extern uint32_t stm32f1_flash_WritePage(uint32_t addr, uint32_t size, uint8_t *buff)
{

    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    uint32_t EndAddr = 0;
    uint32_t *data = (uint32_t*)buff;

    if(0 != size%4)
    {
        for(;;);
    }
    
//    addr >>= 11;
//    addr <<= 11;
    EndAddr = addr + size; 

    while((addr < EndAddr) && (FLASHStatus == FLASH_COMPLETE))
    {
        FLASHStatus = FLASH_ProgramWord(addr, *data);
        addr = addr + 4;
        data++;
    }
    
    return(0);

}


extern uint32_t stm32f1_flash_WriteU16(uint32_t addr, uint16_t hword)
{
    static volatile uint32_t err = 0;
    if(0xFFFF == *(volatile uint16_t*)addr)
    {
        FLASH_ProgramHalfWord(addr, hword);
    }
    else
    {
        err ++;
    }

    return(0);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



