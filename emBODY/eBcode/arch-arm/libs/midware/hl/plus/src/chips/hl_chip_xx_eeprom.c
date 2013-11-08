/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hl_chip_xx_eeprom.c
	@brief      This file implements management of some i2c EEPROMs using stm32 apis.
	@author     marco.accame@iit.it
    @date       10/29/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_CHIP_XX_EEPROM)


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_chip_xx_eeprom.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "hl_chip_xx_eeprom_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define         HL_CHIP_XX_EEPROM_maxEEPROMsize                     (64*1024)

#define         HL_CHIP_XX_EEPROM_BASICTIMEOUT                      ((uint32_t)0x00111000)
#define         HL_CHIP_XX_EEPROM_MAXTRIALS                         (300)



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hl_chip_xx_eeprom_cfg_t hl_chip_xx_eeprom_cfg_default = 
{ 
    .chip           = hl_chip_xx_eeprom_chip_st_m24lr64, 
    .i2cid          = hl_i2c1,  
    .hwaddra2a1a0   = (0 << 2) | (0 << 1) | (0 << 0),       
    .wp_val         = hl_gpio_valUNDEF,                 
    .wp_gpio        = 
    { 
        .port       = hl_gpio_portNONE, 
        .pin        = hl_gpio_pinNONE 
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    uint8_t                     hwaddress;                 
    uint16_t                    pagesize;
    uint32_t                    totalsize;
    hl_chip_xx_eeprom_cfg_t     config;
    volatile uint16_t           numberofbyte2writeinpage;
    volatile uint32_t           timeout;
} hl_chip_xx_eeprom_internal_item_t;

typedef struct
{
    hl_bool_t                               initted;
    hl_chip_xx_eeprom_internal_item_t*      items[1];   // we use an array so that in the future we may use more EEPROMs in parallel   
} hl_chip_xx_eeprom_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_xx_eeprom_initted_clr(void);
static void s_hl_chip_xx_eeprom_initted_set(void);
static hl_bool_t s_hl_chip_xx_eeprom_initted_is(void);


static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_init(hl_chip_xx_eeprom_cfg_t* cfg);
static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_deinit(hl_chip_xx_eeprom_cfg_t* cfg);
static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_enable(hl_chip_xx_eeprom_cfg_t* cfg);
static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_disable(hl_chip_xx_eeprom_cfg_t* cfg);



static hl_result_t s_hl_chip_xx_eeprom_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer);

static hl_result_t s_hl_chip_xx_eeprom_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite);

static hl_result_t s_hl_chip_xx_eeprom_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
static hl_result_t s_hl_chip_xx_eeprom_waiteepromstandbystate(void);

static hl_result_t s_hl_chip_xx_eeprom_wait4operation2complete(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

const uint32_t          hl_chip_xx_eeprom_hid_timeout_flag            = HL_CHIP_XX_EEPROM_BASICTIMEOUT;
const uint32_t          hl_chip_xx_eeprom_hid_timeout_long            = 10 * HL_CHIP_XX_EEPROM_BASICTIMEOUT;
const uint32_t          hl_chip_xx_eeprom_hid_ackaddress_maxtrials    = HL_CHIP_XX_EEPROM_MAXTRIALS;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_chip_xx_eeprom_internal_item_t s_hl_chip_xx_eeprom_theonlyitem = { 0 };   // so far it is the only one.

static hl_chip_xx_eeprom_theinternals_t s_hl_chip_xx_eeprom_theinternals =
{
    .initted            = hl_false,
    .items              = { NULL }   
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hl_result_t hl_chip_xx_eeprom_init(const hl_chip_xx_eeprom_cfg_t *cfg)
{
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];
    
    if(NULL == cfg)
    {
        cfg = &hl_chip_xx_eeprom_cfg_default;
    }
    
    if(hl_true == s_hl_chip_xx_eeprom_initted_is())
    {
        return(hl_res_OK);
    }     

    // do some controls over cfg ....

    // is the target chip valid?
    if(hl_chip_xx_eeprom_chip_none == cfg->chip)
    {
        return(hl_res_NOK_generic);
    }
        
    // is i2c bus already initialised?
    if(hl_false == hl_i2c_initted_is(cfg->i2cid))
    {
        return(hl_res_NOK_generic);
    }    
    
    // if it does not have an item yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        // those of you scared of heap may use static ram
        // intitem = s_hl_chip_xx_eeprom_theinternals.items[0] = hl_sys_heap_new(sizeof(hl_chip_xx_eeprom_internal_item_t));
        intitem = s_hl_chip_xx_eeprom_theinternals.items[0] = &s_hl_chip_xx_eeprom_theonlyitem;
    }       
    
         
    // ok, cfg is good. i copy it into ...
    
    memcpy(&intitem->config, cfg, sizeof(hl_chip_xx_eeprom_cfg_t));

 
    // init the generics according to the device
    
    switch(intitem->config.chip)
    {
        case hl_chip_xx_eeprom_chip_st_m24lr64:
        {
            intitem->hwaddress     = 0xA0;    
            intitem->pagesize      = 4;   
            intitem->totalsize     = 8*1024;
        } break;
        
        case hl_chip_xx_eeprom_chip_st_m24c64:
        {
            intitem->hwaddress     = 0xA0;   
            intitem->pagesize      = 32; 
            intitem->totalsize     = 8*1024;
        } break;
        
        case hl_chip_xx_eeprom_chip_st_m24512:
        {
            intitem->hwaddress     = 0xA0;   
            intitem->pagesize      = 128;
            intitem->totalsize     = 64*1024;            
        } break;
        
        case hl_chip_xx_eeprom_chip_atmel_at24c512b:
        {
            intitem->hwaddress     = 0xA0;    
            intitem->pagesize      = 128;  
            intitem->totalsize     = 64*1024;
        } break;
        
        case hl_chip_xx_eeprom_chip_atmel_at24c1024b:
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
    intitem->hwaddress |= ((intitem->config.hwaddra2a1a0 & 0x07) << 1);
    

    // init write protection
    s_hl_chip_xx_eeprom_wrprcfg_init(&intitem->config);
    s_hl_chip_xx_eeprom_wrprcfg_enable(&intitem->config);
    

    s_hl_chip_xx_eeprom_initted_set();
        
    return(hl_res_OK);
}


extern hl_result_t hl_chip_xx_eeprom_deinit(const hl_chip_xx_eeprom_cfg_t *cfg)
{
//    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];
//    intitem = intitem;
    
    if(NULL == cfg)
    {
        cfg = &hl_chip_xx_eeprom_cfg_default;
    }

    // deinit i2c: dont do it
    
    // deinit write protection
    s_hl_chip_xx_eeprom_wrprcfg_deinit((hl_chip_xx_eeprom_cfg_t*)cfg);
       

    s_hl_chip_xx_eeprom_initted_clr();    
    
    return(hl_res_OK);
}


extern hl_result_t hl_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes)
{   // read and block until done
    hl_result_t res = hl_res_NOK_generic;    
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];
    
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    if(hl_false == s_hl_chip_xx_eeprom_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_verify_rw_bounds(address, &size, buffer))
    {
        return(hl_res_NOK_generic);
    }
    
#endif    
     
    hl_i2c_regaddr_t regaddr;
    regaddr.numofbytes = 2;     // all the managed eeproms use two byte addressing for internal register
    regaddr.bytes.two = (uint16_t) address;
    res = hl_i2c_read(intitem->config.i2cid, intitem->hwaddress, regaddr, buffer, (uint16_t) size);
 
    if(NULL != readbytes)
    {
        *readbytes = (hl_res_OK == res) ? (size) : (0);
    }
    
    return(res);
}


extern hl_result_t hl_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
{   // read and block until done
    hl_result_t res = hl_res_NOK_generic;
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];

#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
   
    if(hl_false == s_hl_chip_xx_eeprom_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_verify_rw_bounds(address, &size, buffer))
    {
        return(hl_res_NOK_generic);
    } 
    
#endif
    
    s_hl_chip_xx_eeprom_wrprcfg_disable(&intitem->config);

    uint16_t WriteAddr = (uint16_t) address;
    uint16_t NumByteToWrite = (uint16_t) size;
    res = s_hl_chip_xx_eeprom_writebuffer(buffer, WriteAddr, NumByteToWrite);
    
    s_hl_chip_xx_eeprom_wrprcfg_enable(&intitem->config);
 

    if(NULL != writtenbytes)
    {
        *writtenbytes = (hl_res_OK == res) ? (size) : (0);
    }
    
    return(res);   
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_chip_xx_eeprom_initted_clr(void)
{
    s_hl_chip_xx_eeprom_theinternals.initted = hl_false;
}

static void s_hl_chip_xx_eeprom_initted_set(void)
{
    s_hl_chip_xx_eeprom_theinternals.initted = hl_true;
}

static hl_bool_t s_hl_chip_xx_eeprom_initted_is(void)
{
    return(s_hl_chip_xx_eeprom_theinternals.initted);
}

// -- functions which manage write protection

static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_init(hl_chip_xx_eeprom_cfg_t* cfg)
{
    hl_result_t res = hl_res_OK;
    
    static const hl_gpio_init_t wpgpioinit = 
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)
        .f1.port        = hl_gpio_portNONE,
        .f1.mode        =
        {
            .gpio_pins  = 0,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_mode  = GPIO_Mode_Out_PP            
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
        .fx.port        = hl_gpio_portNONE,
        .fx.mode        =
        {
            .gpio_pins  = 0,
            .gpio_mode  = GPIO_Mode_OUT,
            .gpio_speed = GPIO_Speed_50MHz,
            .gpio_otype = GPIO_OType_PP,
            .gpio_pupd  = GPIO_PuPd_UP
        }
#else
    #error ERROR --> choose a HL_USE_MPU_ARCH_*
#endif        
    };
    
    if(hl_gpio_valUNDEF != cfg->wp_val)
    {
        hl_gpio_init_t gpioinit;
        hl_gpio_map_t gpiomap;
        memcpy(&gpioinit, &wpgpioinit, sizeof(hl_gpio_init_t));
        gpiomap.gpio.port = cfg->wp_gpio.port;
        gpiomap.gpio.pin  = cfg->wp_gpio.pin;
        gpiomap.af32      = hl_NA32;
        hl_gpio_fill_init(&gpioinit, &gpiomap);
        res = hl_gpio_init(&gpioinit);  
        // no altcfg ...
    }
    
    return(res);
}

static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_deinit(hl_chip_xx_eeprom_cfg_t* cfg)
{
    hl_result_t res = hl_res_OK;
    
    s_hl_chip_xx_eeprom_wrprcfg_disable(cfg);

    return(res);
}

static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_enable(hl_chip_xx_eeprom_cfg_t* cfg)
{
    hl_result_t res = hl_res_OK;
   
    if(hl_gpio_valUNDEF != cfg->wp_val)
    {       
        hl_gpio_pin_write(cfg->wp_gpio, cfg->wp_val);        
    }
    
    return(res);
}

static hl_result_t s_hl_chip_xx_eeprom_wrprcfg_disable(hl_chip_xx_eeprom_cfg_t* cfg)
{
    hl_result_t res = hl_res_OK;

    if(hl_gpio_valUNDEF != cfg->wp_val)
    {  
        hl_gpio_val_t val = (hl_gpio_valSET == cfg->wp_val) ? (hl_gpio_valRESET) : (hl_gpio_valSET);
        hl_gpio_pin_write(cfg->wp_gpio, val);        
    }    
    
    return(res);
}


// --- utility functions

static hl_result_t s_hl_chip_xx_eeprom_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer)
{  
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];
    
    if((hl_chip_xx_eeprom_chip_none == intitem->config.chip) || (NULL == buffer) || (0 == *size) || (address >= intitem->totalsize))
    {
        return(hl_res_NOK_generic);
    }
    
    // clip address+size to ... intitem->totalsize
    if((address+(*size)) > intitem->totalsize)
    {
        uint32_t ss = address+(*size) - intitem->totalsize;
        (*size) -= ss; 
    }    
 

    // so far we manage only .... lower 64k bytes. to manage higher addresses we must split the reading operations or ...
    
    if(address > HL_CHIP_XX_EEPROM_maxEEPROMsize)
    {
        return(hl_res_NOK_generic);
    }
    
    if((address+(*size)) > HL_CHIP_XX_EEPROM_maxEEPROMsize)
    {
        return(hl_res_NOK_generic);
    }
    
    return(hl_res_OK);
    
}

   
static hl_result_t s_hl_chip_xx_eeprom_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
    uint16_t Addr = 0;
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];

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
            s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
            /* Wait operation to be complete */
            if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
            {
                return(hl_res_NOK_generic);
            }
            s_hl_chip_xx_eeprom_waiteepromstandbystate();
        }
        /*!< If NumByteToWrite > intitem->pagesize */
        else  
        {
            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = intitem->pagesize;        
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr +=  intitem->pagesize;
                pBuffer += intitem->pagesize;
            }

            if(NumOfSingle!=0)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = NumOfSingle;          
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();
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
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();  
                
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = (NumByteToWrite - count);          
                /*!< Write the remaining data in the following page */
                s_hl_chip_xx_eeprom_writepage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();   
            }      
            else      
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = NumOfSingle;         
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();  
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
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr += count;
                pBuffer += count;
            } 

            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = intitem->pagesize;          
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();
                WriteAddr +=  intitem->pagesize;
                pBuffer += intitem->pagesize;  
            }
            if(NumOfSingle != 0)
            {
                /* Store the number of data to be written */
                intitem->numberofbyte2writeinpage = NumOfSingle;           
                s_hl_chip_xx_eeprom_writepage(pBuffer, WriteAddr, (uint16_t*)(&intitem->numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(hl_res_NOK_generic == s_hl_chip_xx_eeprom_wait4operation2complete())
                {
                    return(hl_res_NOK_generic);
                }
                s_hl_chip_xx_eeprom_waiteepromstandbystate();
            }
        }
    }

    return(hl_res_OK);
}



static hl_result_t s_hl_chip_xx_eeprom_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite)
{ 
    hl_result_t res;
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];

    hl_i2c_regaddr_t regaddr;
    regaddr.numofbytes = 2;
    regaddr.bytes.two = (uint16_t) WriteAddr;
    res = hl_i2c_write(intitem->config.i2cid, intitem->hwaddress, regaddr, pBuffer, *NumByteToWrite);
    *NumByteToWrite = 0; 
    
    return(res);
}


static hl_result_t s_hl_chip_xx_eeprom_waiteepromstandbystate(void)      
{
    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];
    
    return(hl_i2c_standby(intitem->config.i2cid, intitem->hwaddress)); 
}



#if 0
static hl_result_t s_hl_chip_xx_eeprom_chip_timeoutexpired(void) 
{
//    if(NULL != intitem->config.functionontimeout)
//    {
//        intitem->config.functionontimeout();
//    }
//    else
//    {
        for(;;);
//    }
    
//    return(hl_res_NOK_generic);
}
#endif

// this function is useless for current implementation of s_hl_chip_xx_eeprom_writepage() which uses a blocking mode for i2c writing
// with hl_i2c_write(), and which internally sets numberofbyte2writeinpage to zero.
// however we keep it in case ...
static hl_result_t s_hl_chip_xx_eeprom_wait4operation2complete(void)
{
//    hl_chip_xx_eeprom_internal_item_t *intitem = s_hl_chip_xx_eeprom_theinternals.items[0];
//    
//    intitem->timeout = hl_chip_xx_eeprom_hid_timeout_long;
//    
//    while (intitem->numberofbyte2writeinpage > 0)
//    {
//        if((intitem->timeout--) == 0) {return s_hl_chip_xx_eeprom_chip_timeoutexpired();};
//    }      
//    
    return(hl_res_OK);    
}   



#endif//defined(HL_USE_CHIP_XX_EEPROM)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



