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


/* @file       stm32ee.c
	@brief      This file implements management of some i2c EEPROMs using stm32 apis.
	@author     marco.accame@iit.it
    @date       09/14/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
//#include "hal_brdcfg_modules.h"

//#ifdef HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_stm32xx_include.h"

//#include "stm32gpio_hid.h"

//#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "stm32ee.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "stm32ee_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define         STM32EE_maxEEPROMsize                   (64*1024)


//#define STM32EE_USE_OLDMODE

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const stm32ee_cfg_t stm32_cfg_default = 
{ 
    .devcfg                 =
    {
        .device                 = stm32ee_device_st_m24lr64, 
        .i2cport                = 0,  
        .hwaddra2a1a0           = (0 << 2) | (0 << 1) | (0 << 0)
//        ,
//        .wpval                  = stm32gpio_valNONE,
//        .wppin                  =
//        {
//            .port                   = stm32gpio_portNONE,
//            .pin                    = stm32gpio_pinNONE        
//        }          
    },

    .i2cext                 =
    {
        .i2cinit                = NULL,
        .i2cdeinit              = NULL,
        .i2cpar                 = NULL,
        .i2cread                = NULL,
        .i2cwrite               = NULL,
        .i2cstandby             = NULL
    }
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



typedef struct
{
    uint8_t                     initted;
    uint8_t                     hwaddress;                 // EEPROM HW address (typically 0xA0 ... but some bits may change .....
    uint16_t                    pagesize;
    uint32_t                    totalsize;
    stm32ee_cfg_t               cfg;
//    I2C_TypeDef*                i2cx;
    volatile uint16_t           numberofbyte2writeinpage;
    volatile uint32_t           timeout;
} stm32ee_generic_container_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#warning --> u may take out the wp initialisation and use three (or four) function pointers ...

static stm32ee_result_t s_stm32ee_wpext_init(void);
static stm32ee_result_t s_stm32ee_wpext_deinit(void);
static stm32ee_result_t s_stm32ee_wpext_enable(void);
static stm32ee_result_t s_stm32ee_wpext_disable(void);


//static stm32ee_result_t s_stm32ee_wp_init(const stm32ee_dev_cfg_t *devcfg);
//static stm32ee_result_t s_stm32ee_wp_deinit(const stm32ee_dev_cfg_t *devcfg);
//static stm32ee_result_t s_stm32ee_wp_enable(const stm32ee_dev_cfg_t *devcfg);
//static stm32ee_result_t s_stm32ee_wp_disable(const stm32ee_dev_cfg_t *devcfg);



static stm32ee_result_t s_stm32ee_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer);

static stm32ee_result_t s_stm32ee_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite);

static stm32ee_result_t s_stm32ee_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
static stm32ee_result_t s_stm32ee_timeoutexpired(void);
static stm32ee_result_t s_stm32ee_waiteepromstandbystate(void);

static stm32ee_result_t s_stm32ee_wait4operation2complete(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#define stm32ee_BASICTIMEOUT                ((uint32_t)0x00111000)
#define stm32ee_MAXTRIALS                   (300)

const uint32_t          stm32ee_hid_timeout_flag            = stm32ee_BASICTIMEOUT;
const uint32_t          stm32ee_hid_timeout_long            = 10 * stm32ee_BASICTIMEOUT;
const uint32_t          stm32ee_hid_ackaddress_maxtrials    = stm32ee_MAXTRIALS;



static stm32ee_generic_container_t s_stm32ee_generics = 
{
    .initted                    = 0,
    .hwaddress                  = 0xA0,
    .pagesize                   = 4,
    .totalsize                  = 0,
    .cfg                        =
    {
        .devcfg                 =
        {
            .device                 = stm32ee_device_none, 
            .i2cport                = 0, 
            .hwaddra2a1a0           = (0 << 2) | (0 << 1) | (0 << 0)
//            ,
//            .wpval                  = stm32gpio_valNONE,            
//            .wppin                  =
//            {
//                .port                   = stm32gpio_portNONE,
//                .pin                    = stm32gpio_pinNONE        
//            }          
        },
        .i2cext                         =
        {
            .i2cinit                = NULL,
            .i2cdeinit              = NULL,
            .i2cpar                 = NULL,
            .i2cread                = NULL,
            .i2cwrite               = NULL,
            .i2cstandby             = NULL
        },
        .wpext                          =
        {
            .wpinit                 = NULL,
            .wpdeinit               = NULL,
            .wpenable               = NULL,
            .wpdisable              = NULL
        },    
    },
//    .i2cx                       = NULL,
    .numberofbyte2writeinpage   = 0,
    .timeout                    = 0
};




#if     defined(USE_STM32F1)

                                                               
static const GPIO_InitTypeDef  s_stm32ee_gpio_wp_pin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_Out_PP,
}; 

static const GPIO_InitTypeDef  s_stm32ee_gpio_wp_floatpin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_Mode      = GPIO_Mode_IPD// or GPIO_Mode_IN_FLOATING,
};                                                                 
    
#elif   defined(USE_STM32F4)

static const GPIO_InitTypeDef  s_stm32ee_gpio_wp_pin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Mode      = GPIO_Mode_OUT,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_OType     = GPIO_OType_PP,
    .GPIO_PuPd      = GPIO_PuPd_UP
}; 

static const GPIO_InitTypeDef  s_stm32ee_gpio_wp_floatpin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Mode      = GPIO_Mode_IN,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_OType     = GPIO_OType_PP,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
}; 
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern stm32ee_result_t stm32ee_init(const stm32ee_cfg_t *cfg)
{
    stm32ee_result_t res = stm32ee_res_NOK;
    
    if(NULL == cfg)
    {
        cfg = &stm32_cfg_default;
    }
    
    if((stm32ee_device_none == cfg->devcfg.device) || (1 == s_stm32ee_generics.initted))
    {
        return(stm32ee_res_NOK);
    }
    
 
   if(cfg->devcfg.i2cport > 4)
   {
       return(stm32ee_res_NOK);
   }
    
     
    // do other controls over cfg ....
    
    
    // ok, cfg is good. i copy it into ...
    
    memcpy(&s_stm32ee_generics.cfg, cfg, sizeof(stm32ee_cfg_t));

 
    // init the generics according to the device
    
    switch(s_stm32ee_generics.cfg.devcfg.device)
    {
        case stm32ee_device_st_m24lr64:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 4;   
            s_stm32ee_generics.totalsize     = 8*1024;
        } break;
        
        case stm32ee_device_st_m24c64:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 32; 
            s_stm32ee_generics.totalsize     = 8*1024;
        } break;
        
        case stm32ee_device_st_m24512:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 128;
            s_stm32ee_generics.totalsize     = 64*1024;            
        } break;
        
        case stm32ee_device_atmel_at24c512b:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 128;  
            s_stm32ee_generics.totalsize     = 64*1024;
        } break;
        
        case stm32ee_device_atmel_at24c1024b:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 256;
            s_stm32ee_generics.totalsize     = 128*1024;            
        } break;
        
        default:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 4;  
            s_stm32ee_generics.totalsize     = 8*1024;            
        } break;
    }
    
    // apply extra bits
    s_stm32ee_generics.hwaddress |= ((s_stm32ee_generics.cfg.devcfg.hwaddra2a1a0 & 0x07) << 1);
    
   
    s_stm32ee_wpext_init();
    s_stm32ee_wpext_enable();
//    // init wp if needed
//    if((stm32gpio_portNONE != s_stm32ee_generics.cfg.devcfg.wppin.port) && (stm32gpio_pinNONE != s_stm32ee_generics.cfg.devcfg.wppin.pin) && ((stm32gpio_valNONE != s_stm32ee_generics.cfg.devcfg.wpval)))
//    {
//        s_stm32ee_wp_init(&s_stm32ee_generics.cfg.devcfg);
//        s_stm32ee_wp_enable(&s_stm32ee_generics.cfg.devcfg);    
//    }
    
    
    // init i2c 
    
    if((NULL == s_stm32ee_generics.cfg.i2cext.i2cread) || (NULL == s_stm32ee_generics.cfg.i2cext.i2cwrite) || (NULL == s_stm32ee_generics.cfg.i2cext.i2cstandby))
    {
        return(stm32ee_res_NOK);
    }
    
    
    if((NULL != s_stm32ee_generics.cfg.i2cext.i2cinit))
    {
    
        res = (stm32ee_result_t)s_stm32ee_generics.cfg.i2cext.i2cinit(s_stm32ee_generics.cfg.devcfg.i2cport, s_stm32ee_generics.cfg.i2cext.i2cpar);
        if(stm32ee_res_NOK == res)
        {
            return(stm32ee_res_NOK);
        }
        
    }
    

    s_stm32ee_generics.initted  = 1;    
        
    return(stm32ee_res_OK);
}


extern stm32ee_result_t stm32ee_deinit(const stm32ee_cfg_t *cfg)
{
    stm32ee_result_t res;
    
    if(NULL == cfg)
    {
        cfg = &stm32_cfg_default;
    }

   if(cfg->devcfg.i2cport > 4)
   {
       return(stm32ee_res_NOK);
   }
    
    
    if((NULL == cfg->i2cext.i2cdeinit))
    {
        return(stm32ee_res_NOK);
    }
    
    res = (stm32ee_result_t)cfg->i2cext.i2cdeinit(cfg->devcfg.i2cport, cfg->i2cext.i2cpar);
    if(stm32ee_res_NOK == res)
    {
        return(stm32ee_res_NOK);
    }
    
    s_stm32ee_wpext_deinit();
       
//    if((stm32gpio_portNONE != cfg->devcfg.wppin.port) && (stm32gpio_pinNONE != cfg->devcfg.wppin.pin) && ((stm32gpio_valNONE != cfg->devcfg.wpval)))
//    {
//        s_stm32ee_wp_deinit(&cfg->devcfg);  
//    } 

    s_stm32ee_generics.initted  = 0;     
    
    return(stm32ee_res_OK);
}


extern stm32ee_result_t stm32ee_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes)
{   // read and block until done
    stm32ee_result_t res = stm32ee_res_NOK;
    
    if(0 == s_stm32ee_generics.initted)
    {
        return(stm32ee_res_NOK);
    }
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(stm32ee_res_NOK == s_stm32ee_verify_rw_bounds(address, &size, buffer))
    {
        return(stm32ee_res_NOK);
    }
    
    
//    old mode
//    uint16_t ReadAddr = (uint16_t) address;
//    uint16_t NumByteToRead = (uint16_t) size;    
//    res = s_stm32ee_readbuffer(buffer, ReadAddr, &NumByteToRead);

    stm32ee_regaddr_t regaddr;
    regaddr.numofbytes = 2;                 // all the managed eeproms use two byte addressing for internal register
    regaddr.bytes.two = (uint16_t) address;
    res = (stm32ee_result_t)s_stm32ee_generics.cfg.i2cext.i2cread(s_stm32ee_generics.cfg.devcfg.i2cport, s_stm32ee_generics.hwaddress, regaddr, buffer, (uint16_t) size);


    if(NULL != readbytes)
    {
        *readbytes = (stm32ee_res_OK == res) ? (size) : (0);
    }
    
    return(res);
}


extern stm32ee_result_t stm32ee_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
{   // read and block until done
    stm32ee_result_t res = stm32ee_res_NOK;
    
    if(0 == s_stm32ee_generics.initted)
    {
        return(stm32ee_res_NOK);
    }
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(stm32ee_res_NOK == s_stm32ee_verify_rw_bounds(address, &size, buffer))
    {
        return(stm32ee_res_NOK);
    } 
    
    s_stm32ee_wpext_disable();
    //s_stm32ee_wp_disable(&s_stm32ee_generics.cfg.devcfg);

    uint16_t WriteAddr = (uint16_t) address;
    uint16_t NumByteToWrite = (uint16_t) size;
    res = s_stm32ee_writebuffer(buffer, WriteAddr, NumByteToWrite);
    
    s_stm32ee_wpext_enable();
    //s_stm32ee_wp_enable(&s_stm32ee_generics.cfg.devcfg);

    if(NULL != writtenbytes)
    {
        *writtenbytes = (stm32ee_res_OK == res) ? (size) : (0);
    }
    
    return(res);   
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


//extern uint32_t stm32ee_hid_myfunction(void)
//{
//}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static uint32_t s_stm32ee_dosomething(void)
//{
//    return(0x12345678);
//}


// -- functions which manage write protection

static stm32ee_result_t s_stm32ee_wpext_init(void)
{
    stm32ee_result_t res = stm32ee_res_OK;
    if(NULL != s_stm32ee_generics.cfg.wpext.wpinit)
    {
        res = (stm32ee_result_t)s_stm32ee_generics.cfg.wpext.wpinit();
    }
    return(res);
}

static stm32ee_result_t s_stm32ee_wpext_deinit(void)
{
    stm32ee_result_t res = stm32ee_res_OK;
    if(NULL != s_stm32ee_generics.cfg.wpext.wpdeinit)
    {
        res = (stm32ee_result_t)s_stm32ee_generics.cfg.wpext.wpdeinit();
    }
    return(res);
}

static stm32ee_result_t s_stm32ee_wpext_enable(void)
{
    stm32ee_result_t res = stm32ee_res_OK;
    if(NULL != s_stm32ee_generics.cfg.wpext.wpenable)
    {
        res = (stm32ee_result_t)s_stm32ee_generics.cfg.wpext.wpenable();
    }
    return(res);
}

static stm32ee_result_t s_stm32ee_wpext_disable(void)
{
    stm32ee_result_t res = stm32ee_res_OK;
    if(NULL != s_stm32ee_generics.cfg.wpext.wpdisable)
    {
        res = (stm32ee_result_t)s_stm32ee_generics.cfg.wpext.wpdisable();
    }
    return(res);
}

#if 0
static stm32ee_result_t s_stm32ee_wp_init(const stm32ee_dev_cfg_t *devcfg)
{
    // configure as a output low speed.
    if(stm32gpio_valNONE != devcfg->wpval)
    {   
        GPIO_InitTypeDef  GPIO_InitStructure;
 
#if     defined(USE_STM32F1) 
        // enable the gpio clock
        RCC_APB2PeriphClockCmd(stm32gpio_hid_thegpioclocks[devcfg->wppin.port], ENABLE);
#elif   defined(USE_STM32F4)         
         // enable the gpio clock
        RCC_AHB1PeriphClockCmd(stm32gpio_hid_thegpioclocks[devcfg->wppin.port], ENABLE);
#endif        
        memcpy(&GPIO_InitStructure, &s_stm32ee_gpio_wp_pin, sizeof(GPIO_InitTypeDef));
        
        GPIO_InitStructure.GPIO_Pin = stm32gpio_hid_thepins[devcfg->wppin.pin];
            
        GPIO_Init(stm32gpio_hid_thegpioports[devcfg->wppin.port], &GPIO_InitStructure);
    }
    
    return(stm32ee_res_OK);
}

static stm32ee_result_t s_stm32ee_wp_deinit(const stm32ee_dev_cfg_t *devcfg)
{
    if(stm32gpio_valNONE != devcfg->wpval)
    {
        GPIO_InitTypeDef  GPIO_InitStructure;
        
        memcpy(&GPIO_InitStructure, &s_stm32ee_gpio_wp_floatpin, sizeof(GPIO_InitTypeDef));
        
        GPIO_InitStructure.GPIO_Pin = stm32gpio_hid_thepins[devcfg->wppin.pin];
           
        GPIO_Init(stm32gpio_hid_thegpioports[devcfg->wppin.port], &GPIO_InitStructure);
    }
    
    return(stm32ee_res_OK);
}

static stm32ee_result_t s_stm32ee_wp_enable(const stm32ee_dev_cfg_t *devcfg)
{
    if(stm32gpio_valNONE != devcfg->wpval)
    {
        GPIO_WriteBit(stm32gpio_hid_thegpioports[devcfg->wppin.port], stm32gpio_hid_thepins[devcfg->wppin.pin], (stm32gpio_valLOW == devcfg->wpval) ? (Bit_RESET) : (Bit_SET)); 
    }
 
    return(stm32ee_res_OK);
}

static stm32ee_result_t s_stm32ee_wp_disable(const stm32ee_dev_cfg_t *devcfg)
{
    if(stm32gpio_valNONE != devcfg->wpval)
    {
        GPIO_WriteBit(stm32gpio_hid_thegpioports[devcfg->wppin.port], stm32gpio_hid_thepins[devcfg->wppin.pin], (stm32gpio_valHIGH == devcfg->wpval) ? (Bit_RESET) : (Bit_SET)); 
    }

    return(stm32ee_res_OK);
}
#endif

// --- utility functions

static stm32ee_result_t s_stm32ee_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer)
{   
    if((stm32ee_device_none == s_stm32ee_generics.cfg.devcfg.device) || (NULL == buffer) || (0 == *size) || (address >= s_stm32ee_generics.totalsize))
    {
        return(stm32ee_res_NOK);
    }
    
    // clip address+size to ... s_stm32ee_generics.totalsize
    if((address+(*size)) > s_stm32ee_generics.totalsize)
    {
        uint32_t ss = address+(*size) - s_stm32ee_generics.totalsize;
        (*size) -= ss; 
    }    
 

    // so far we manage only .... lower 64k bytes. to manage higher addresses we must split the reading operations or ...
    
    if(address > STM32EE_maxEEPROMsize)
    {
        return(stm32ee_res_NOK);
    }
    
    if((address+(*size)) > STM32EE_maxEEPROMsize)
    {
        return(stm32ee_res_NOK);
    }
    
    return(stm32ee_res_OK);
    
}

   
static stm32ee_result_t s_stm32ee_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
    uint16_t Addr = 0;

    Addr = WriteAddr % s_stm32ee_generics.pagesize;
    count = s_stm32ee_generics.pagesize - Addr;
    NumOfPage =  NumByteToWrite / s_stm32ee_generics.pagesize;
    NumOfSingle = NumByteToWrite % s_stm32ee_generics.pagesize;
 
    /*!< If WriteAddr is s_stm32ee_generics.pagesize aligned  */
    if(Addr == 0) 
    {
        /*!< If NumByteToWrite < s_stm32ee_generics.pagesize */
        if(NumOfPage == 0) 
        {
            /* Store the number of data to be written */
            s_stm32ee_generics.numberofbyte2writeinpage = NumOfSingle;
            /* Start writing data */
            s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
            /* Wait operation to be complete */
            if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
            {
                return(stm32ee_res_NOK);
            }
            s_stm32ee_waiteepromstandbystate();
        }
        /*!< If NumByteToWrite > s_stm32ee_generics.pagesize */
        else  
        {
            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = s_stm32ee_generics.pagesize;        
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();
                WriteAddr +=  s_stm32ee_generics.pagesize;
                pBuffer += s_stm32ee_generics.pagesize;
            }

            if(NumOfSingle!=0)
            {
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = NumOfSingle;          
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();
            }
        }
    }
    /*!< If WriteAddr is not s_stm32ee_generics.pagesize aligned  */
    else 
    {
        /*!< If NumByteToWrite < s_stm32ee_generics.pagesize */
        if(NumOfPage== 0) 
        {
            /*!< If the number of data to be written is more than the remaining space 
            in the current page: */
            if (NumByteToWrite > count)
            {
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = count;        
                /*!< Write the data contained in same page */
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();  
                
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = (NumByteToWrite - count);          
                /*!< Write the remaining data in the following page */
                s_stm32ee_writepage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();   
            }      
            else      
            {
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = NumOfSingle;         
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();  
            }     
        }
        /*!< If NumByteToWrite > s_stm32ee_generics.pagesize */
        else
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / s_stm32ee_generics.pagesize;
            NumOfSingle = NumByteToWrite % s_stm32ee_generics.pagesize;

            if(count != 0)
            {  
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = count;         
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();
                WriteAddr += count;
                pBuffer += count;
            } 

            while(NumOfPage--)
            {
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = s_stm32ee_generics.pagesize;          
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage));
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();
                WriteAddr +=  s_stm32ee_generics.pagesize;
                pBuffer += s_stm32ee_generics.pagesize;  
            }
            if(NumOfSingle != 0)
            {
                /* Store the number of data to be written */
                s_stm32ee_generics.numberofbyte2writeinpage = NumOfSingle;           
                s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&s_stm32ee_generics.numberofbyte2writeinpage)); 
                /* Wait operation to be complete */
                if(stm32ee_res_NOK == s_stm32ee_wait4operation2complete())
                {
                    return(stm32ee_res_NOK);
                }
                s_stm32ee_waiteepromstandbystate();
            }
        }
    }

    return(stm32ee_res_OK);
}



static stm32ee_result_t s_stm32ee_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite)
{ 
    stm32ee_result_t res;

    stm32ee_regaddr_t regaddr;
    regaddr.numofbytes = 2;
    regaddr.bytes.two = (uint16_t) WriteAddr;
    res = (stm32ee_result_t)s_stm32ee_generics.cfg.i2cext.i2cwrite(s_stm32ee_generics.cfg.devcfg.i2cport, s_stm32ee_generics.hwaddress, regaddr, pBuffer, *NumByteToWrite);
    *NumByteToWrite = 0; 
    
    return(res);
}


static stm32ee_result_t s_stm32ee_waiteepromstandbystate(void)      
{
    if(NULL != s_stm32ee_generics.cfg.i2cext.i2cstandby)
    {        
        s_stm32ee_generics.cfg.i2cext.i2cstandby(s_stm32ee_generics.cfg.devcfg.i2cport, s_stm32ee_generics.hwaddress); 
    }   
    
    return stm32ee_res_OK;
}




static stm32ee_result_t s_stm32ee_timeoutexpired(void) 
{
//    if(NULL != s_stm32ee_generics.cfg.devcfg.functionontimeout)
//    {
//        s_stm32ee_generics.cfg.devcfg.functionontimeout();
//    }
//    else
//    {
        for(;;);
//    }
    
    return(stm32ee_res_NOK);
}


static stm32ee_result_t s_stm32ee_wait4operation2complete(void)
{
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_long;
    
    while (s_stm32ee_generics.numberofbyte2writeinpage > 0)
    {
        if((s_stm32ee_generics.timeout--) == 0) {return s_stm32ee_timeoutexpired();};
    }      
    
    return(stm32ee_res_OK);    
}   


// --- old functions

#if 0


static stm32ee_result_t s_stm32ee_readbuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead)
{  
    /*  Set the pointer to the Number of data to be read. This pointer will be used 
        by the DMA Transfer Completer interrupt Handler in order to reset the 
        variable to 0. User should check on this variable in order to know if the 
        DMA transfer has been complete or not. */
    s_stm32ee_generics.prt2bytes2read = NumByteToRead;
  
    /*!< While the bus is busy */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_long;
    while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_BUSY))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }
  
    /*!< Send START condition */
    I2C_GenerateSTART(s_stm32ee_generics.i2cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }
  
    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(s_stm32ee_generics.i2cx, s_stm32ee_generics.hwaddress, I2C_Direction_Transmitter);

    /*!< Test on EV6 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    } 


//#elif defined (sEE_M24C64_32)

    /*!< Send the EEPROM's internal address to read from: MSB of the address first */
    I2C_SendData(s_stm32ee_generics.i2cx, (uint8_t)((ReadAddr & 0xFF00) >> 8));    

    /*!< Test on EV8 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }

    /*!< Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(s_stm32ee_generics.i2cx, (uint8_t)(ReadAddr & 0x00FF));    
  
//#endif /*!< sEE_M24C08 */

    /*!< Test on EV8 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_BTF) == RESET)
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }
  
    /*!< Send START condition a second time */  
    I2C_GenerateSTART(s_stm32ee_generics.i2cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    } 
  
    /*!< Send EEPROM address for read */
    I2C_Send7bitAddress(s_stm32ee_generics.i2cx, s_stm32ee_generics.hwaddress, I2C_Direction_Receiver);  

    /* If number of data to be read is 1, then DMA couldn't be used */
    /* One Byte Master Reception procedure (POLLING) ---------------------------*/
    if ((uint16_t)(*NumByteToRead) < 2)
    {
        /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_ADDR) == RESET)
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }     

        /*!< Disable Acknowledgement */
        I2C_AcknowledgeConfig(s_stm32ee_generics.i2cx, DISABLE);   


        //#warning --> in stm32f1 they use a sEE_EnterCriticalSection_UserCallback(); it generally disalbe interrupts

        /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
        (void)(s_stm32ee_generics.i2cx)->SR2;

        /*!< Send STOP Condition */
        I2C_GenerateSTOP(s_stm32ee_generics.i2cx, ENABLE);
            

        //#warning --> in stm32f1 they use a sEE_ExitCriticalSection_UserCallback();

        /* Wait for the byte to be received */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_RXNE) == RESET)
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }

        /*!< Read the byte received from the EEPROM */
        *pBuffer = I2C_ReceiveData(s_stm32ee_generics.i2cx);

        /*!< Decrement the read bytes counter */
        (uint16_t)(*NumByteToRead)--;        

        /* Wait to make sure that STOP control bit has been cleared */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while((s_stm32ee_generics.i2cx)->CR1 & I2C_CR1_STOP)
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }  

        /*!< Re-Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(s_stm32ee_generics.i2cx, ENABLE);    

        return(stm32ee_res_OK);
    }
    else if(0 == s_stm32ee_generics.cfg.dmacfg.dontuse) /* More than one Byte Master Reception procedure (DMA) -----------------*/
    {
        /*!< Test on EV6 and clear it */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }  

        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        //s_stm32ee_dma_config((uint32_t)pBuffer, (uint16_t)(*NumByteToRead), sEE_DIRECTION_RX);
        s_stm32ee_dma_config_rx((uint32_t)pBuffer, (uint16_t)(*NumByteToRead));

        /* Inform the DMA that the next End Of Transfer Signal will be the last one */
        I2C_DMALastTransferCmd(s_stm32ee_generics.i2cx, ENABLE); 

        /* Enable the DMA Rx Stream */
        DMA_Cmd(stm32ee_hid_dma_stream_rx, ENABLE);  

        // and now ... waits for the dma reception
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_long;
        while ((*s_stm32ee_generics.prt2bytes2read) > 0)
        {
            if((s_stm32ee_generics.timeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }

    }
    else
    {   // must use transfer without dma
 
        /*!< Test on EV6 and clear it */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }  

        while(*NumByteToRead)
        {
        
            if(1 == (*NumByteToRead))
            {
                /*!< Disable Acknowledgement */
                I2C_AcknowledgeConfig(s_stm32ee_generics.i2cx, DISABLE);          
                /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
                (void)(s_stm32ee_generics.i2cx)->SR2;
                /*!< Send STOP Condition */
                I2C_GenerateSTOP(s_stm32ee_generics.i2cx, ENABLE);            
            }
        
            /* Wait for the byte to be received */
            s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
            while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_RXNE) == RESET)
            {
                if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
            }
            
            /*!< Read the byte received from the EEPROM */
            *pBuffer = I2C_ReceiveData(s_stm32ee_generics.i2cx);
            
            pBuffer++;
            
            /*!< Decrement the read bytes counter */
            (uint16_t)(*NumByteToRead)--;              
        
        }
    
    
        /* Wait to make sure that STOP control bit has been cleared */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while((s_stm32ee_generics.i2cx)->CR1 & I2C_CR1_STOP)
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }  
        
        /*!< Re-Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(s_stm32ee_generics.i2cx, ENABLE);    

    }
  
    /* If all operations OK, return OK (0) */
    return stm32ee_res_OK;
}

static stm32ee_result_t s_stm32ee_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite)
{ 

#if !defined(STM32EE_USE_OLDMODE)

    stm32ee_result_t res;

    stm32ee_regaddr_t regaddr;
    regaddr.numofbytes = 2;
    regaddr.bytes.two = (uint16_t) WriteAddr;
    res = (stm32ee_result_t)s_stm32ee_generics.cfg.i2cext.i2cwrite(s_stm32ee_generics.cfg.devcfg.i2cport, s_stm32ee_generics.hwaddress, regaddr, pBuffer, *NumByteToWrite);
    *NumByteToWrite = 0; 
    
    return(res);

#else
    /*  Set the pointer to the Number of data to be written. This pointer will be used 
        by the DMA Transfer Completer interrupt Handler in order to reset the 
        variable to 0. User should check on this variable in order to know if the 
        DMA transfer has been complete or not. */
    s_stm32ee_generics.prt2bytes2write = NumByteToWrite;  
  
    /*!< While the bus is busy */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_long;
    while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_BUSY))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }

    /*!< Send START condition */
    I2C_GenerateSTART(s_stm32ee_generics.i2cx, ENABLE);

    /*!< Test on EV5 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }
  
    /*!< Send EEPROM address for write */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    I2C_Send7bitAddress(s_stm32ee_generics.i2cx, s_stm32ee_generics.hwaddress, I2C_Direction_Transmitter);

    /*!< Test on EV6 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }

//#ifdef sEE_M24C08
//  
//  /*!< Send the EEPROM's internal address to write to : only one byte Address */
//  I2C_SendData(s_stm32ee_generics.i2cx, WriteAddr);
//  
//#elif defined(sEE_M24C64_32)
  
  
    /*!< Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(s_stm32ee_generics.i2cx, (uint8_t)((WriteAddr & 0xFF00) >> 8));

    /*!< Test on EV8 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;  
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }  

    /*!< Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(s_stm32ee_generics.i2cx, (uint8_t)(WriteAddr & 0x00FF));
  
//#endif /*!< sEE_M24C08 */  
  
    /*!< Test on EV8 and clear it */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag; 
    while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    } 

    if(0 == s_stm32ee_generics.cfg.dmacfg.dontuse)
    { 
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        //s_stm32ee_dma_config((uint32_t)pBuffer, (uint32_t)(*NumByteToWrite), sEE_DIRECTION_TX);
        s_stm32ee_dma_config_tx((uint32_t)pBuffer, (uint32_t)(*NumByteToWrite));

        /* Enable the DMA Tx Stream */
        DMA_Cmd(stm32ee_hid_dma_stream_tx, ENABLE);
    }
    else
    {
       
        while(*NumByteToWrite)
        {       
            /*!< Send the byte to be written */
            I2C_SendData(s_stm32ee_generics.i2cx, *pBuffer); 
            
            pBuffer++;
            (*NumByteToWrite)--;
            
            /*!< Test on EV8 and clear it */
            s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag; 
            while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
            {
                if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
            }  
        }        
 
        /*!< Send STOP condition */
        I2C_GenerateSTOP(s_stm32ee_generics.i2cx, ENABLE);
        
        /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
        (void)(s_stm32ee_generics.i2cx)->SR1;
        (void)(s_stm32ee_generics.i2cx)->SR2;        
        // end        
    }
  
    /* If all operations OK, return OK (0) */
    return stm32ee_res_OK;
    
#endif   
}

static stm32ee_result_t s_stm32ee_waiteepromstandbystate(void)      
{
#if defined(STM32EE_USE_OLDMODE)   
 
    volatile uint16_t tmpSR1 = 0;
    volatile uint32_t sEETrials = 0;

    /*!< While the bus is busy */
    s_stm32ee_generics.timeout = stm32ee_hid_timeout_long;
    while(I2C_GetFlagStatus(s_stm32ee_generics.i2cx, I2C_FLAG_BUSY))
    {
        if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
    }

    /*  Keep looping till the slave acknowledge his address or maximum number 
        of trials is reached (this number is defined by stm32ee_hid_ackaddress_maxtrials define
        in stm324xg_eval_i2c_ee.h file) */
    while (1)
    {
        /*!< Send START condition */
        I2C_GenerateSTART(s_stm32ee_generics.i2cx, ENABLE);

        /*!< Test on EV5 and clear it */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_flag;
        while(!I2C_CheckEvent(s_stm32ee_generics.i2cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }    

        /*!< Send EEPROM address for write */
        I2C_Send7bitAddress(s_stm32ee_generics.i2cx, s_stm32ee_generics.hwaddress, I2C_Direction_Transmitter);

        /* Wait for ADDR flag to be set (Slave acknowledged his address) */
        s_stm32ee_generics.timeout = stm32ee_hid_timeout_long;
        do
        {     
            /* Get the current value of the SR1 register */
            tmpSR1 = (s_stm32ee_generics.i2cx)->SR1;

            /* Update the timeout value and exit if it reach 0 */
            if((s_stm32ee_generics.timeout--) == 0) return s_stm32ee_timeoutexpired();
        }
        /* Keep looping till the Address is acknowledged or the AF flag is 
        set (address not acknowledged at time) */
        while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);

        /* Check if the ADDR flag has been set */
        if (tmpSR1 & I2C_SR1_ADDR)
        {
            /* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already 
            been read) */
            (void)(s_stm32ee_generics.i2cx)->SR2;

            /*!< STOP condition */    
            I2C_GenerateSTOP(s_stm32ee_generics.i2cx, ENABLE);

            /* Exit the function */
            return stm32ee_res_OK;
        }
        else
        {
            /*!< Clear AF flag */
            I2C_ClearFlag(s_stm32ee_generics.i2cx, I2C_FLAG_AF);                  
        }
        /* Check if the maximum allowed number of trials has bee reached */
        if (sEETrials++ == stm32ee_hid_ackaddress_maxtrials)
        {
            /* If the maximum number of trials has been reached, exit the function */
            return s_stm32ee_timeoutexpired();
        }
    }
#else 

    if(NULL != s_stm32ee_generics.cfg.i2cext.i2cstandbydevice)
    {        
        s_stm32ee_generics.cfg.i2cext.i2cstandbydevice(s_stm32ee_generics.cfg.devcfg.i2cport, s_stm32ee_generics.hwaddress); 
    }   
    
    return stm32ee_res_OK;
#endif    
}


#endif 


//#endif//HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



