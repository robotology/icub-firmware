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


/* @file       hal_chip_xx_eeprom.c
	@brief      This file implements management of some i2c EEPROMs using stm32 apis.
	@author     marco.accame@iit.it
    @date       09/14/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CHIP_XX_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_mpu_stm32xx_include.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_chip_xx_eeprom.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hal_chip_xx_eeprom_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define         HAL_CHIP_XX_EEPROM_maxEEPROMsize                   (64*1024)



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_chip_xx_eeprom_cfg_t hal_chip_xx_eeprom_cfg_default = 
{ 
    .chipcfg                 =
    {
        .chip                   = hal_chip_xx_eeprom_chip_st_m24lr64, 
        .i2cport                = hal_i2c_port1,  
        .hwaddra2a1a0           = (0 << 2) | (0 << 1) | (0 << 0)       
    },

    .wrprcfg                =
    {
        .wrpr_init              = NULL,
        .wrpr_deinit            = NULL,
        .wrpr_enable            = NULL,
        .wrpr_disable           = NULL
    }
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    uint8_t                     initted;
    uint8_t                     hwaddress;                 
    uint16_t                    pagesize;
    uint32_t                    totalsize;
    hal_chip_xx_eeprom_cfg_t    cfg;
    volatile uint16_t           numberofbyte2writeinpage;
    volatile uint32_t           timeout;
} hal_chip_xx_eeprom_generic_container_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_init(void);
static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_deinit(void);
static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_enable(void);
static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_disable(void);



static hal_result_t s_hal_chip_xx_eeprom_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer);

static hal_result_t s_hal_chip_xx_eeprom_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite);

static hal_result_t s_hal_chip_xx_eeprom_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
static hal_result_t s_hal_chip_xx_eeprom_chip_timeoutexpired(void);
static hal_result_t s_hal_chip_xx_eeprom_waiteepromstandbystate(void);

static hal_result_t s_hal_chip_xx_eeprom_wait4operation2complete(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#define hal_chip_xx_eeprom_BASICTIMEOUT                ((uint32_t)0x00111000)
#define hal_chip_xx_eeprom_MAXTRIALS                   (300)

const uint32_t          hal_chip_xx_eeprom_hid_timeout_flag            = hal_chip_xx_eeprom_BASICTIMEOUT;
const uint32_t          hal_chip_xx_eeprom_hid_timeout_long            = 10 * hal_chip_xx_eeprom_BASICTIMEOUT;
const uint32_t          hal_chip_xx_eeprom_hid_ackaddress_maxtrials    = hal_chip_xx_eeprom_MAXTRIALS;



static hal_chip_xx_eeprom_generic_container_t s_hal_chip_xx_eeprom_generics = 
{
    .initted                    = 0,
    .hwaddress                  = 0xA0,
    .pagesize                   = 4,
    .totalsize                  = 0,
    .cfg                        =
    {
        .chipcfg                 =
        {
            .chip                   = hal_chip_xx_eeprom_chip_none, 
            .i2cport                = hal_i2c_port1, 
            .hwaddra2a1a0           = (0 << 2) | (0 << 1) | (0 << 0)        
        },
        .wrprcfg                          =
        {
            .wrpr_init                 = NULL,
            .wrpr_deinit               = NULL,
            .wrpr_enable               = NULL,
            .wrpr_disable              = NULL
        },    
    },
    .numberofbyte2writeinpage   = 0,
    .timeout                    = 0
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_chip_xx_eeprom_init(const hal_chip_xx_eeprom_cfg_t *cfg)
{
//    hal_result_t res = hal_res_NOK_generic;
    
    if(NULL == cfg)
    {
        cfg = &hal_chip_xx_eeprom_cfg_default;
    }
    
    if((hal_chip_xx_eeprom_chip_none == cfg->chipcfg.chip) || (1 == s_hal_chip_xx_eeprom_generics.initted))
    {
        return(hal_res_NOK_generic);
    }
    
     
    // do other controls over cfg ....
    
    
    // ok, cfg is good. i copy it into ...
    
    memcpy(&s_hal_chip_xx_eeprom_generics.cfg, cfg, sizeof(hal_chip_xx_eeprom_cfg_t));

 
    // init the generics according to the device
    
    switch(s_hal_chip_xx_eeprom_generics.cfg.chipcfg.chip)
    {
        case hal_chip_xx_eeprom_chip_st_m24lr64:
        {
            s_hal_chip_xx_eeprom_generics.hwaddress     = 0xA0;    
            s_hal_chip_xx_eeprom_generics.pagesize      = 4;   
            s_hal_chip_xx_eeprom_generics.totalsize     = 8*1024;
        } break;
        
        case hal_chip_xx_eeprom_chip_st_m24c64:
        {
            s_hal_chip_xx_eeprom_generics.hwaddress     = 0xA0;   
            s_hal_chip_xx_eeprom_generics.pagesize      = 32; 
            s_hal_chip_xx_eeprom_generics.totalsize     = 8*1024;
        } break;
        
        case hal_chip_xx_eeprom_chip_st_m24512:
        {
            s_hal_chip_xx_eeprom_generics.hwaddress     = 0xA0;   
            s_hal_chip_xx_eeprom_generics.pagesize      = 128;
            s_hal_chip_xx_eeprom_generics.totalsize     = 64*1024;            
        } break;
        
        case hal_chip_xx_eeprom_chip_atmel_at24c512b:
        {
            s_hal_chip_xx_eeprom_generics.hwaddress     = 0xA0;    
            s_hal_chip_xx_eeprom_generics.pagesize      = 128;  
            s_hal_chip_xx_eeprom_generics.totalsize     = 64*1024;
        } break;
        
        case hal_chip_xx_eeprom_chip_atmel_at24c1024b:
        {
            s_hal_chip_xx_eeprom_generics.hwaddress     = 0xA0;    
            s_hal_chip_xx_eeprom_generics.pagesize      = 256;
            s_hal_chip_xx_eeprom_generics.totalsize     = 128*1024;            
        } break;
        
        default:
        {
            s_hal_chip_xx_eeprom_generics.hwaddress     = 0xA0;    
            s_hal_chip_xx_eeprom_generics.pagesize      = 4;  
            s_hal_chip_xx_eeprom_generics.totalsize     = 8*1024;            
        } break;
    }
    
    // apply extra bits
    s_hal_chip_xx_eeprom_generics.hwaddress |= ((s_hal_chip_xx_eeprom_generics.cfg.chipcfg.hwaddra2a1a0 & 0x07) << 1);
    

    // init write protection
    s_hal_chip_xx_eeprom_wrprcfg_init();
    s_hal_chip_xx_eeprom_wrprcfg_enable();
    
    // init i2c: must already be initted ...
    //hal_i2c_init(s_hal_chip_xx_eeprom_generics.cfg.chipcfg->i2cport, ??);    
 
    s_hal_chip_xx_eeprom_generics.initted  = 1;    
        
    return(hal_res_OK);
}


extern hal_result_t hal_chip_xx_eeprom_deinit(const hal_chip_xx_eeprom_cfg_t *cfg)
{
//    hal_result_t res;
    
    if(NULL == cfg)
    {
        cfg = &hal_chip_xx_eeprom_cfg_default;
    }

    // deinit i2c: dont do it
    
    // deinit write protection
    s_hal_chip_xx_eeprom_wrprcfg_deinit();
       

    s_hal_chip_xx_eeprom_generics.initted  = 0;     
    
    return(hal_res_OK);
}


extern hal_result_t hal_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes)
{   // read and block until done
    hal_result_t res = hal_res_NOK_generic;
    
    if(0 == s_hal_chip_xx_eeprom_generics.initted)
    {
        return(hal_res_NOK_generic);
    }
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_verify_rw_bounds(address, &size, buffer))
    {
        return(hal_res_NOK_generic);
    }
    
 

    hal_i2c_regaddr_t regaddr;
    regaddr.numofbytes = 2;     // all the managed eeproms use two byte addressing for internal register
    regaddr.bytes.two = (uint16_t) address;
    res = hal_i2c_read(s_hal_chip_xx_eeprom_generics.cfg.chipcfg.i2cport, s_hal_chip_xx_eeprom_generics.hwaddress, regaddr, buffer, (uint16_t) size);
 
 
    if(NULL != readbytes)
    {
        *readbytes = (hal_res_OK == res) ? (size) : (0);
    }
    
    return(res);
}


extern hal_result_t hal_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
{   // read and block until done
    hal_result_t res = hal_res_NOK_generic;
    
    if(0 == s_hal_chip_xx_eeprom_generics.initted)
    {
        return(hal_res_NOK_generic);
    }
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_verify_rw_bounds(address, &size, buffer))
    {
        return(hal_res_NOK_generic);
    } 
    
    s_hal_chip_xx_eeprom_wrprcfg_disable();

    uint16_t WriteAddr = (uint16_t) address;
    uint16_t NumByteToWrite = (uint16_t) size;
    res = s_hal_chip_xx_eeprom_writebuffer(buffer, WriteAddr, NumByteToWrite);
    
    s_hal_chip_xx_eeprom_wrprcfg_enable();
 

    if(NULL != writtenbytes)
    {
        *writtenbytes = (hal_res_OK == res) ? (size) : (0);
    }
    
    return(res);   
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


//extern uint32_t hal_chip_xx_eeprom_hid_myfunction(void)
//{
//}


extern uint32_t hal_chip_xx_eeprom_hid_getsize(const hal_base_cfg_t *cfg)
{   
    // no memory needed
    return(0);
}

extern hal_result_t hal_chip_xx_eeprom_hid_setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }
    return(hal_res_OK); 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static uint32_t s_hal_chip_xx_eeprom_dosomething(void)
//{
//    return(0x12345678);
//}


// -- functions which manage write protection

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_init(void)
{
    hal_result_t res = hal_res_OK;
    if(NULL != s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_init)
    {
        res = (hal_result_t)s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_init();
    }
    return(res);
}

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_deinit(void)
{
    hal_result_t res = hal_res_OK;
    if(NULL != s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_deinit)
    {
        res = (hal_result_t)s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_deinit();
    }
    return(res);
}

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_enable(void)
{
    hal_result_t res = hal_res_OK;
    if(NULL != s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_enable)
    {
        res = (hal_result_t)s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_enable();
    }
    return(res);
}

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_disable(void)
{
    hal_result_t res = hal_res_OK;
    if(NULL != s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_disable)
    {
        res = (hal_result_t)s_hal_chip_xx_eeprom_generics.cfg.wrprcfg.wrpr_disable();
    }
    return(res);
}


// --- utility functions

static hal_result_t s_hal_chip_xx_eeprom_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer)
{   
    if((hal_chip_xx_eeprom_chip_none == s_hal_chip_xx_eeprom_generics.cfg.chipcfg.chip) || (NULL == buffer) || (0 == *size) || (address >= s_hal_chip_xx_eeprom_generics.totalsize))
    {
        return(hal_res_NOK_generic);
    }
    
    // clip address+size to ... s_hal_chip_xx_eeprom_generics.totalsize
    if((address+(*size)) > s_hal_chip_xx_eeprom_generics.totalsize)
    {
        uint32_t ss = address+(*size) - s_hal_chip_xx_eeprom_generics.totalsize;
        (*size) -= ss; 
    }    
 

    // so far we manage only .... lower 64k bytes. to manage higher addresses we must split the reading operations or ...
    
    if(address > HAL_CHIP_XX_EEPROM_maxEEPROMsize)
    {
        return(hal_res_NOK_generic);
    }
    
    if((address+(*size)) > HAL_CHIP_XX_EEPROM_maxEEPROMsize)
    {
        return(hal_res_NOK_generic);
    }
    
    return(hal_res_OK);
    
}

   
static hal_result_t s_hal_chip_xx_eeprom_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
    uint16_t Addr = 0;

    Addr = WriteAddr % s_hal_chip_xx_eeprom_generics.pagesize;
    count = s_hal_chip_xx_eeprom_generics.pagesize - Addr;
    NumOfPage =  NumByteToWrite / s_hal_chip_xx_eeprom_generics.pagesize;
    NumOfSingle = NumByteToWrite % s_hal_chip_xx_eeprom_generics.pagesize;
 
    /*!< If WriteAddr is s_hal_chip_xx_eeprom_generics.pagesize aligned  */
    if(Addr == 0) 
    {
        /*!< If NumByteToWrite < s_hal_chip_xx_eeprom_generics.pagesize */
        if(NumOfPage == 0) 
        {
            /* Store the number of data to be written */
            s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = NumOfSingle;
            /* Start writing data */
            s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
            /* Wait operation to be complete */
            if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
            {
                return(hal_res_NOK_generic);
            }
            s_hal_chip_xx_eeprom_waiteepromstandbystate();
        }
        /*!< If NumByteToWrite > s_hal_chip_xx_eeprom_generics.pagesize */
        else  
        {
            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = s_hal_chip_xx_eeprom_generics.pagesize;        
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr +=  s_hal_chip_xx_eeprom_generics.pagesize;
                pBuffer += s_hal_chip_xx_eeprom_generics.pagesize;
            }

            if(NumOfSingle!=0)
            {
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = NumOfSingle;          
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
            }
        }
    }
    /*!< If WriteAddr is not s_hal_chip_xx_eeprom_generics.pagesize aligned  */
    else 
    {
        /*!< If NumByteToWrite < s_hal_chip_xx_eeprom_generics.pagesize */
        if(NumOfPage== 0) 
        {
            /*!< If the number of data to be written is more than the remaining space 
            in the current page: */
            if (NumByteToWrite > count)
            {
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = count;        
                /*!< Write the data contained in same page */
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();  
                
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = (NumByteToWrite - count);          
                /*!< Write the remaining data in the following page */
                s_hal_chip_xx_eeprom_writepage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();   
            }      
            else      
            {
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = NumOfSingle;         
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();  
            }     
        }
        /*!< If NumByteToWrite > s_hal_chip_xx_eeprom_generics.pagesize */
        else
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / s_hal_chip_xx_eeprom_generics.pagesize;
            NumOfSingle = NumByteToWrite % s_hal_chip_xx_eeprom_generics.pagesize;

            if(count != 0)
            {  
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = count;         
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr += count;
                pBuffer += count;
            } 

            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = s_hal_chip_xx_eeprom_generics.pagesize;          
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr +=  s_hal_chip_xx_eeprom_generics.pagesize;
                pBuffer += s_hal_chip_xx_eeprom_generics.pagesize;  
            }
            if(NumOfSingle != 0)
            {
                /* Store the number of data to be written */
                s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage = NumOfSingle;           
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
            }
        }
    }

    return(hal_res_OK);
}



static hal_result_t s_hal_chip_xx_eeprom_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite)
{ 
    hal_result_t res;

    hal_i2c_regaddr_t regaddr;
    regaddr.numofbytes = 2;
    regaddr.bytes.two = (uint16_t) WriteAddr;
    res = hal_i2c_write(s_hal_chip_xx_eeprom_generics.cfg.chipcfg.i2cport, s_hal_chip_xx_eeprom_generics.hwaddress, regaddr, pBuffer, *NumByteToWrite);
    *NumByteToWrite = 0; 
    
    return(res);
}


static hal_result_t s_hal_chip_xx_eeprom_waiteepromstandbystate(void)      
{
    return(hal_i2c_standby(s_hal_chip_xx_eeprom_generics.cfg.chipcfg.i2cport, s_hal_chip_xx_eeprom_generics.hwaddress)); 
}




static hal_result_t s_hal_chip_xx_eeprom_chip_timeoutexpired(void) 
{
//    if(NULL != s_hal_chip_xx_eeprom_generics.cfg.chipcfg.functionontimeout)
//    {
//        s_hal_chip_xx_eeprom_generics.cfg.chipcfg.functionontimeout();
//    }
//    else
//    {
        for(;;);
//    }
    
//    return(hal_res_NOK_generic);
}


static hal_result_t s_hal_chip_xx_eeprom_wait4operation2complete(void)
{
    s_hal_chip_xx_eeprom_generics.timeout = hal_chip_xx_eeprom_hid_timeout_long;
    
    while (s_hal_chip_xx_eeprom_generics.numberofbyte2writeinpage > 0)
    {
        if((s_hal_chip_xx_eeprom_generics.timeout--) == 0) {return s_hal_chip_xx_eeprom_chip_timeoutexpired();};
    }      
    
    return(hal_res_OK);    
}   



#endif//HAL_USE_CHIP_XX_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



