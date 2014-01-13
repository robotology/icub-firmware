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
#include "hal_middleware_interface.h"
#include "hal_heap.h"

 
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

#define         HAL_CHIP_XX_EEPROM_BASICTIMEOUT                     ((uint32_t)0x00111000)
#define         HAL_CHIP_XX_EEPROM_MAXTRIALS                        (300)



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_chip_xx_eeprom_cfg_t hal_chip_xx_eeprom_cfg_default = 
{ 
    .chipcfg                 =
    {
        .chip                   = hal_chip_xx_eeprom_chip_st_m24lr64, 
        .i2cid                  = hal_i2c1,  
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


// typedef struct
// {
//     uint8_t                     initted;
//     uint8_t                     hwaddress;                 
//     uint16_t                    pagesize;
//     uint32_t                    totalsize;
//     hal_chip_xx_eeprom_cfg_t    config;
//     volatile uint16_t           numberofbyte2writeinpage;
//     volatile uint32_t           timeout;
// } hal_chip_xx_eeprom_internals_t;

typedef struct
{
    //uint8_t                     initted;
    uint8_t                     hwaddress;                 
    uint16_t                    pagesize;
    uint32_t                    totalsize;
    hal_chip_xx_eeprom_cfg_t    config;
    volatile uint16_t           numberofbyte2writeinpage;
    volatile uint32_t           timeout;
} hal_chip_xx_eeprom_internal_item_t;

typedef struct
{
    hal_bool_t                                          initted;
    hal_chip_xx_eeprom_internal_item_t*                 items[1];   
} hal_chip_xx_eeprom_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_chip_xx_eeprom_initted_clr(void);
static void s_hal_chip_xx_eeprom_initted_set(void);
static hal_bool_t s_hal_chip_xx_eeprom_initted_is(void);


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
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

const uint32_t          hal_chip_xx_eeprom_hid_timeout_flag            = HAL_CHIP_XX_EEPROM_BASICTIMEOUT;
const uint32_t          hal_chip_xx_eeprom_hid_timeout_long            = 10 * HAL_CHIP_XX_EEPROM_BASICTIMEOUT;
const uint32_t          hal_chip_xx_eeprom_hid_ackaddress_maxtrials    = HAL_CHIP_XX_EEPROM_MAXTRIALS;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// static hal_chip_xx_eeprom_internals_t s_hal_chip_xx_eeprom_internals = 
// {
//     .initted                    = 0,
//     .hwaddress                  = 0xA0,
//     .pagesize                   = 4,
//     .totalsize                  = 0,
//     .config                        =
//     {
//         .chipcfg                 =
//         {
//             .chip                   = hal_chip_xx_eeprom_chip_none, 
//             .i2cid                  = hal_i2c1, 
//             .hwaddra2a1a0           = (0 << 2) | (0 << 1) | (0 << 0)        
//         },
//         .wrprcfg                          =
//         {
//             .wrpr_init                 = NULL,
//             .wrpr_deinit               = NULL,
//             .wrpr_enable               = NULL,
//             .wrpr_disable              = NULL
//         },    
//     },
//     .numberofbyte2writeinpage   = 0,
//     .timeout                    = 0
// };

static hal_chip_xx_eeprom_theinternals_t s_hal_chip_xx_eeprom_theinternals =
{
    .initted            = hal_false,
    .items              = { NULL }   
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_chip_xx_eeprom_init(const hal_chip_xx_eeprom_cfg_t *cfg)
{
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if(NULL == cfg)
    {
        cfg = &hal_chip_xx_eeprom_cfg_default;
    }
    
    if(hal_true == s_hal_chip_xx_eeprom_initted_is())
    {
        return(hal_res_OK);
    }     
    
    if(hal_chip_xx_eeprom_chip_none == cfg->chipcfg.chip)
    {
        return(hal_res_NOK_generic);
    }
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_chip_xx_eeprom_theinternals.items[0] = hal_heap_new(sizeof(hal_chip_xx_eeprom_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }       
    
     
    // do other controls over cfg ....
    
    
    // ok, cfg is good. i copy it into ...
    
    memcpy(&intitem->config, cfg, sizeof(hal_chip_xx_eeprom_cfg_t));

 
    // init the generics according to the device
    
    switch(intitem->config.chipcfg.chip)
    {
        case hal_chip_xx_eeprom_chip_st_m24lr64:
        {
            intitem->hwaddress     = 0xA0;    
            intitem->pagesize      = 4;   
            intitem->totalsize     = 8*1024;
        } break;
        
        case hal_chip_xx_eeprom_chip_st_m24c64:
        {
            intitem->hwaddress     = 0xA0;   
            intitem->pagesize      = 32; 
            intitem->totalsize     = 8*1024;
        } break;
        
        case hal_chip_xx_eeprom_chip_st_m24512:
        {
            intitem->hwaddress     = 0xA0;   
            intitem->pagesize      = 128;
            intitem->totalsize     = 64*1024;            
        } break;
        
        case hal_chip_xx_eeprom_chip_atmel_at24c512b:
        {
            intitem->hwaddress     = 0xA0;    
            intitem->pagesize      = 128;  
            intitem->totalsize     = 64*1024;
        } break;
        
        case hal_chip_xx_eeprom_chip_atmel_at24c1024b:
        {
            intitem->hwaddress     = 0xA0;    
            intitem->pagesize      = 256;
            intitem->totalsize     = 128*1024;            
        } break;
        
        default:
        {
            intitem->hwaddress     = 0xA0;    
            intitem->pagesize      = 4;  
            intitem->totalsize     = 8*1024;            
        } break;
    }
    
    // apply extra bits
    intitem->hwaddress |= ((intitem->config.chipcfg.hwaddra2a1a0 & 0x07) << 1);
    

    // init write protection
    s_hal_chip_xx_eeprom_wrprcfg_init();
    s_hal_chip_xx_eeprom_wrprcfg_enable();
    
    // init i2c: must already be initted ...
    //hal_i2c_init(intitem->config.chipcfg->i2cid, ??);    
 
    //intitem->initted  = 1;    
    
    s_hal_chip_xx_eeprom_initted_set();
        
    return(hal_res_OK);
}


extern hal_result_t hal_chip_xx_eeprom_deinit(const hal_chip_xx_eeprom_cfg_t *cfg)
{
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    intitem = intitem;
    
    if(NULL == cfg)
    {
        cfg = &hal_chip_xx_eeprom_cfg_default;
    }

    // deinit i2c: dont do it
    
    // deinit write protection
    s_hal_chip_xx_eeprom_wrprcfg_deinit();
       

    //intitem->initted  = 0;    

    s_hal_chip_xx_eeprom_initted_clr();    
    
    return(hal_res_OK);
}


extern hal_result_t hal_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes)
{   // read and block until done
    hal_result_t res = hal_res_NOK_generic;
    
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if(hal_false == s_hal_chip_xx_eeprom_initted_is())
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
    res = hal_i2c_read(intitem->config.chipcfg.i2cid, intitem->hwaddress, regaddr, buffer, (uint16_t) size);
 
 
    if(NULL != readbytes)
    {
        *readbytes = (hal_res_OK == res) ? (size) : (0);
    }
    
    return(res);
}


extern hal_result_t hal_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
{   // read and block until done
    hal_result_t res = hal_res_NOK_generic;
    
    if(hal_false == s_hal_chip_xx_eeprom_initted_is())
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


extern hal_result_t hal_chip_xx_eeprom_hid_static_memory_init(void)
{
    memset(&s_hal_chip_xx_eeprom_theinternals, 0, sizeof(s_hal_chip_xx_eeprom_theinternals));
    return(hal_res_OK); 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_chip_xx_eeprom_initted_clr(void)
{
    s_hal_chip_xx_eeprom_theinternals.initted = hal_false;
}

static void s_hal_chip_xx_eeprom_initted_set(void)
{
    s_hal_chip_xx_eeprom_theinternals.initted = hal_true;
}

static hal_bool_t s_hal_chip_xx_eeprom_initted_is(void)
{
    return(s_hal_chip_xx_eeprom_theinternals.initted);
}

// -- functions which manage write protection

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_init(void)
{
    hal_result_t res = hal_res_OK;
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if(NULL != intitem->config.wrprcfg.wrpr_init)
    {
        res = (hal_result_t)intitem->config.wrprcfg.wrpr_init();
    }
    return(res);
}

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_deinit(void)
{
    hal_result_t res = hal_res_OK;
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if(NULL != intitem->config.wrprcfg.wrpr_deinit)
    {
        res = (hal_result_t)intitem->config.wrprcfg.wrpr_deinit();
    }
    return(res);
}

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_enable(void)
{
    hal_result_t res = hal_res_OK;
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if(NULL != intitem->config.wrprcfg.wrpr_enable)
    {
        res = (hal_result_t)intitem->config.wrprcfg.wrpr_enable();
    }
    return(res);
}

static hal_result_t s_hal_chip_xx_eeprom_wrprcfg_disable(void)
{
    hal_result_t res = hal_res_OK;
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if(NULL != intitem->config.wrprcfg.wrpr_disable)
    {
        res = (hal_result_t)intitem->config.wrprcfg.wrpr_disable();
    }
    return(res);
}


// --- utility functions

static hal_result_t s_hal_chip_xx_eeprom_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer)
{  
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    if((hal_chip_xx_eeprom_chip_none == intitem->config.chipcfg.chip) || (NULL == buffer) || (0 == *size) || (address >= intitem->totalsize))
    {
        return(hal_res_NOK_generic);
    }
    
    // clip address+size to ... intitem->totalsize
    if((address+(*size)) > intitem->totalsize)
    {
        uint32_t ss = address+(*size) - intitem->totalsize;
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
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];

    Addr = WriteAddr % intitem->pagesize;
    count = intitem->pagesize - Addr;
    NumOfPage =  NumByteToWrite / intitem->pagesize;
    NumOfSingle = NumByteToWrite % intitem->pagesize;
 
    /*!< If WriteAddr is intitem->pagesize aligned  */
    if(Addr == 0) 
    {
        /*!< If NumByteToWrite < intitem->pagesize */
        if(NumOfPage == 0) 
        {
            /* Store the number of data to be written */
            intitem->numberofbyte2writeinpage = NumOfSingle;
            /* Start writing data */
            s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
            /* Wait operation to be complete */
            if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
            {
                return(hal_res_NOK_generic);
            }
            s_hal_chip_xx_eeprom_waiteepromstandbystate();
        }
        /*!< If NumByteToWrite > intitem->pagesize */
        else  
        {
            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = intitem->pagesize;        
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr +=  intitem->pagesize;
                pBuffer += intitem->pagesize;
            }

            if(NumOfSingle!=0)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = NumOfSingle;          
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
            }
        }
    }
    /*!< If WriteAddr is not intitem->pagesize aligned  */
    else 
    {
        /*!< If NumByteToWrite < intitem->pagesize */
        if(NumOfPage== 0) 
        {
            /*!< If the number of data to be written is more than the remaining space 
            in the current page: */
            if (NumByteToWrite > count)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = count;        
                /*!< Write the data contained in same page */
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();  
                
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = (NumByteToWrite - count);          
                /*!< Write the remaining data in the following page */
                s_hal_chip_xx_eeprom_writepage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint16_t*)(&intitem->numberofbyte2writeinpage));
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
                intitem->numberofbyte2writeinpage = NumOfSingle;         
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();  
            }     
        }
        /*!< If NumByteToWrite > intitem->pagesize */
        else
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / intitem->pagesize;
            NumOfSingle = NumByteToWrite % intitem->pagesize;

            if(count != 0)
            {  
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = count;         
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
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
                intitem->numberofbyte2writeinpage = intitem->pagesize;          
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hal_res_NOK_generic == s_hal_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hal_res_NOK_generic);
                }
                s_hal_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr +=  intitem->pagesize;
                pBuffer += intitem->pagesize;  
            }
            if(NumOfSingle != 0)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = NumOfSingle;           
                s_hal_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage)); 
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
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];

    hal_i2c_regaddr_t regaddr;
    regaddr.numofbytes = 2;
    regaddr.bytes.two = (uint16_t) WriteAddr;
    res = hal_i2c_write(intitem->config.chipcfg.i2cid, intitem->hwaddress, regaddr, pBuffer, *NumByteToWrite);
    *NumByteToWrite = 0; 
    
    return(res);
}


static hal_result_t s_hal_chip_xx_eeprom_waiteepromstandbystate(void)      
{
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    return(hal_i2c_standby(intitem->config.chipcfg.i2cid, intitem->hwaddress)); 
}




static hal_result_t s_hal_chip_xx_eeprom_chip_timeoutexpired(void) 
{
//    if(NULL != intitem->config.chipcfg.functionontimeout)
//    {
//        intitem->config.chipcfg.functionontimeout();
//    }
//    else
//    {
        for(;;);
//    }
    
//    return(hal_res_NOK_generic);
}


static hal_result_t s_hal_chip_xx_eeprom_wait4operation2complete(void)
{
    hal_chip_xx_eeprom_internal_item_t *intitem = s_hal_chip_xx_eeprom_theinternals.items[0];
    
    intitem->timeout = hal_chip_xx_eeprom_hid_timeout_long;
    
    while (intitem->numberofbyte2writeinpage > 0)
    {
        if((intitem->timeout--) == 0) {return s_hal_chip_xx_eeprom_chip_timeoutexpired();};
    }      
    
    return(hal_res_OK);    
}   



#endif//HAL_USE_CHIP_XX_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



