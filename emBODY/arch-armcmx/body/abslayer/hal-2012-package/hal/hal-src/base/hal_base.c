/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/* @file       hal_mpu_base.c
	@brief      This file keeps implementation of the base hal module for stm32.
	@author     marco.accame@iit.it
    @date       09/12/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_mpu_stm32xx_include.h"

#include "hal_brdcfg.h"


#include "hal_utility_bits_hid.h" 
#include "hal_utility_crc16_hid.h" 
#include "hal_utility_crc32_hid.h" 
#include "hal_utility_fifo_hid.h" 

#include "hal_mpu_arch_hid.h"
#include "hal_mpu_can_hid.h" 
#include "hal_mpu_crc_hid.h" 
#include "hal_mpu_eth_hid.h" 
#include "hal_mpu_flash_hid.h" 
#include "hal_mpu_gpio_hid.h" 
#include "hal_mpu_i2c_hid.h"
#include "hal_mpu_spi_hid.h"
#include "hal_mpu_sys_hid.h"
#include "hal_mpu_timer_hid.h"
#include "hal_mpu_trace_hid.h"
#include "hal_mpu_watchdog_hid.h"

#include "hal_device_led_hid.h" 

#include "hal_device_display_hid.h" 
#include "hal_device_eeprom_hid.h"
#include "hal_device_ethtransceiver_hid.h"
#include "hal_device_switch_hid.h"

#include "hal_device_accelerometer_hid.h" 
#include "hal_device_gyroscope_hid.h"
#include "hal_device_termometer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_base.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_base_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern hal_cfg_t hal_base_hid_params = 
{ 
    .cpu_freq = 0
};




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if 0
    // removed by acemor on 09-mar-2011 to avoid problems of ... 
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Heap_Size (referred from hal_stm32.o).
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Stack_Size (referred from hal_stm32.o).
__asm int s_hal_getstacksize (void) {
        IMPORT  Stack_Size
        LDR     R0,=Stack_Size
        BX      LR
}


__asm int s_hal_getheapsize (void) {
        IMPORT  Heap_Size
        LDR     R0,=Heap_Size
        BX      LR
}
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static void (*s_hal_fn_osal_system_scheduling_suspend)(void) = NULL;
static void (*s_hal_fn_osal_system_scheduling_restart)(void) = NULL;
static void (*s_hal_fn_on_error)(hal_fatalerror_t, const char *) = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern uint32_t hal_base_memory_getsize(const hal_cfg_t *cfg, uint32_t *size04aligned)
{
    uint32_t retval = 0;
 
    if(NULL == cfg)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingconfiguration, "hal_base_memory_getsize() needs a cfg");
        return(0);
    }


    // utilities
    
#ifdef  HAL_USE_UTILITY_BITS
    retval += hal_utility_bits_hid_getsize(cfg);
#endif//HAL_USE_UTILITY_BITS

#ifdef  HAL_USE_UTILITY_CRC16
    retval += hal_utility_crc16_hid_getsize(cfg);
#endif//HAL_USE_UTILITY_CRC16    

#ifdef  HAL_USE_UTILITY_CRC32
    retval += hal_utility_crc32_hid_getsize(cfg);
#endif//HAL_USE_UTILITY_CRC32        
    
#ifdef  HAL_USE_UTILITY_FIFO
    retval += hal_utility_fifo_hid_getsize(cfg);
#endif//HAL_USE_UTILITY_FIFO    
    
    
    // peripherals
        
#ifdef  HAL_USE_ARCH
    retval += hal_arch_arm_hid_getsize(cfg);
#endif//HAL_USE_ARCH

#ifdef  HAL_USE_BASE
    retval += hal_base_hid_getsize(cfg);
#endif//HAL_USE_BASE
    
#ifdef  HAL_USE_CAN
    retval += hal_can_hid_getsize(cfg);
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    retval += hal_crc_hid_getsize(cfg);
#endif//HAL_USE_CRC

#ifdef HAL_USE_ETH
    retval += hal_eth_hid_getsize(cfg);
#endif//HAL_USE_ETH
                      
#ifdef HAL_USE_FLASH
    retval += hal_flash_hid_getsize(cfg);
#endif//HAL_USE_FLASH  

#ifdef  HAL_USE_GPIO
    retval += hal_gpio_hid_getsize(cfg);
#endif//HAL_USE_GPIO

#ifdef  HAL_USE_I2C
    retval += hal_i2c_hid_getsize(cfg);
#endif//HAL_USE_I2C

#ifdef  HAL_USE_SPI
    retval += hal_spi_hid_getsize(cfg);
#endif//HAL_USE_SPI

#ifdef  HAL_USE_SYS
    retval += hal_sys_hid_getsize(cfg);
#endif//HAL_USE_SYS 

#ifdef  HAL_USE_TIMER
    retval += hal_timer_hid_getsize(cfg);
#endif//HAL_USE_TIMER       

#ifdef  HAL_USE_TRACE
    retval += hal_trace_hid_getsize(cfg);
#endif//HAL_USE_TRACE 

#ifdef  HAL_USE_WATCHDOG
    retval += hal_watchdog_hid_getsize(cfg);
#endif//HAL_USE_WATCHDOG 


    // actuators
    
#ifdef  HAL_USE_DEVICE_LED
    retval += hal_device_led_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_LED  


    // devices

#ifdef  HAL_USE_DEVICE_DISPLAY
    retval += hal_device_display_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_DISPLAY  

#ifdef  HAL_USE_DEVICE_EEPROM
    retval += hal_device_eeprom_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_EEPROM  

#ifdef  HAL_USE_DEVICE_ETHTRANSCEIVER
    retval += hal_device_ethtransceiver_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER  

#ifdef  HAL_USE_DEVICE_SWITCH
    retval += hal_device_switch_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_SWITCH 


    // sensors

#ifdef  HAL_USE_DEVICE_ACCELEROMETER
    retval += hal_device_accelerometer_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_ACCELEROMETER   

#ifdef HAL_USE_SENSOR_ENCODER
    retval += hal_sensor_encoder_hid_getsize(cfg);
#endif//HAL_USE_SENSOR_ENCODER   

#ifdef  HAL_USE_DEVICE_GYROSCOPE
    retval += hal_device_gyroscope_hid_getsize(cfg);
#endif//HAL_USE_DEVICE_GYROSCOPE   

#ifdef  HAL_USE_SENSOR_TEMP
    retval += hal_device_termometer_hid_getsize(cfg);
#endif//HAL_USE_SENSOR_TEMP 


    // board (hence chips)
    
    retval += hal_brdcfg_chips__getsize(cfg);



    if(NULL != size04aligned)
    {
        *size04aligned = retval;
    }
    
    return(retval);
}


extern hal_result_t hal_base_initialise(const hal_cfg_t *cfg, uint32_t *data04aligned) 
{
    if(NULL == cfg)
    {   
        hal_base_hid_on_fatalerror(hal_fatalerror_missingconfiguration, "hal_base_initialise() needs cfg");
        return(hal_res_NOK_generic);
    }


    // override functions
    s_hal_fn_on_error = cfg->extfn.usr_on_fatal_error;
    s_hal_fn_osal_system_scheduling_suspend = cfg->extfn.osal_system_scheduling_suspend;
    s_hal_fn_osal_system_scheduling_restart = cfg->extfn.osal_system_scheduling_restart;



//     if(cfg->cpu_freq != SystemCoreClock)
//     {   
//         hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect frequency");
//         return(hal_res_NOK_generic);
//     }

#if 0 
    // removed by acemor on 09-mar-2011 to avoid problems of ... 
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Heap_Size (referred from hal_stm32.o).
    // .\obj\shalPART.axf: Error: L6218E: Undefined symbol Stack_Size (referred from hal_stm32.o).
    if(cfg->sys_stacksize != s_hal_getstacksize())
    {   
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect stack size");
        return(hal_res_NOK_generic);
    }

    if(cfg->sys_heapsize != s_hal_getheapsize())
    {   
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_base_initialise(): incorrect heap size");
        return(hal_res_NOK_generic);
    }
#endif


    
    // utilities
    
#ifdef  HAL_USE_UTILITY_BITS
    if(hal_res_OK != hal_utility_bits_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_utility_bits_hid_getsize(cfg)/4;
#endif//HAL_USE_UTILITY_BITS

#ifdef  HAL_USE_UTILITY_CRC16
    if(hal_res_OK != hal_utility_crc16_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_utility_crc16_hid_getsize(cfg)/4;
#endif//HAL_USE_UTILITY_CRC16    

#ifdef  HAL_USE_UTILITY_CRC32
    if(hal_res_OK != hal_utility_crc32_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_utility_crc32_hid_getsize(cfg)/4;
#endif//HAL_USE_UTILITY_CRC32        
    
#ifdef  HAL_USE_UTILITY_FIFO
    if(hal_res_OK != hal_utility_fifo_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_utility_fifo_hid_getsize(cfg)/4;
#endif//HAL_USE_UTILITY_FIFO    
 
    // peripherals
    
#ifdef  HAL_USE_ARCH
    if(hal_res_OK != hal_arch_arm_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_arch_arm_hid_getsize(cfg)/4;
#endif//HAL_USE_ARCH


#ifdef  HAL_USE_BASE
    if(hal_res_OK != hal_base_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_base_hid_getsize(cfg)/4;
#endif//HAL_USE_BASE


#ifdef  HAL_USE_CAN
    if(hal_res_OK != hal_can_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_can_hid_getsize(cfg)/4;
#endif//HAL_USE_CAN


#ifdef  HAL_USE_CRC
    if(hal_res_OK != hal_crc_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_crc_hid_getsize(cfg)/4;
#endif//HAL_USE_CRC


#ifdef  HAL_USE_ETH
    if(hal_res_OK != hal_eth_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_eth_hid_getsize(cfg)/4;
#endif//HAL_USE_ETH 

#ifdef  HAL_USE_FLASH
    if(hal_res_OK != hal_flash_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_flash_hid_getsize(cfg)/4;
#endif//HAL_USE_FLASH   

#ifdef  HAL_USE_GPIO
    if(hal_res_OK != hal_gpio_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_gpio_hid_getsize(cfg)/4;
#endif//HAL_USE_GPIO
    
#ifdef  HAL_USE_I2C
    if(hal_res_OK != hal_i2c_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_i2c_hid_getsize(cfg)/4;
#endif//HAL_USE_I2C     
    
#ifdef HAL_USE_SPI
    if(hal_res_OK != hal_spi_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_spi_hid_getsize(cfg)/4;
#endif//HAL_USE_SPI    

#ifdef HAL_USE_SYS
    if(hal_res_OK != hal_sys_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_sys_hid_getsize(cfg)/4;
#endif//HAL_USE_SYS

#ifdef HAL_USE_TIMER
    if(hal_res_OK != hal_timer_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_timer_hid_getsize(cfg)/4;
#endif//HAL_USE_TIMER

#ifdef HAL_USE_TRACE
    if(hal_res_OK != hal_trace_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_trace_hid_getsize(cfg)/4;
#endif//HAL_USE_TRACE

#ifdef HAL_USE_WATCHDOG
    if(hal_res_OK != hal_watchdog_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_watchdog_hid_getsize(cfg)/4;
#endif//HAL_USE_WATCHDOG


    //  actuators

#ifdef  HAL_USE_DEVICE_LED
    if(hal_res_OK != hal_device_led_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_led_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_LED
 
    
    // devices
    
#ifdef HAL_USE_DEVICE_DISPLAY
    if(hal_res_OK != hal_device_display_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_display_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_DISPLAY
    

#ifdef HAL_USE_DEVICE_EEPROM
    if(hal_res_OK != hal_device_eeprom_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_eeprom_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICVE_EEPROM

    
#ifdef HAL_USE_DEVICE_ETHTRANSCEIVER
    if(hal_res_OK != hal_device_ethtransceiver_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_ethtransceiver_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER    
    

#ifdef HAL_USE_DEVICE_SWITCH
    if(hal_res_OK != hal_device_switch_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_switch_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_SWITCH
        

    // sensors
    
#ifdef HAL_USE_DEVICE_ACCELEROMETER
    if(hal_res_OK != hal_device_accelerometer_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_accelerometer_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_ACCELEROMETER    
    
    
#ifdef HAL_USE_DEVICE_ENCODER
    if(hal_res_OK != hal_sensor_encoder_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_sensor_encoder_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_ENCODER    
    
#ifdef HAL_USE_DEVICE_GYROSCOPE
    if(hal_res_OK != hal_device_gyroscope_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_gyroscope_hid_getsize(cfg)/4;
#endif//HAL_USE_DEVICE_GYROSCOPE    
 
    
#ifdef HAL_USE_SENSOR_TEMP
    if(hal_res_OK != hal_device_termometer_hid_setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_device_termometer_hid_getsize(cfg)/4;
#endif//HAL_USE_SENSOR_TEMP    
    
    
    // board (hence chips)
    
    if(hal_res_OK != hal_brdcfg_chips__setmem(cfg, data04aligned))
    {
        return(hal_res_NOK_generic);
    }
    data04aligned += hal_brdcfg_chips__getsize(cfg)/4;    
    
    
    // finally ... sets used config
    memcpy(&hal_base_hid_params, cfg, sizeof(hal_cfg_t));

    
    return(hal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern uint32_t hal_base_hid_getsize(const hal_cfg_t *cfg)
{   
    // no memory needed
    return(0);
}

extern hal_result_t hal_base_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }
    return(hal_res_OK); 
}

// #if 0
// // use hal_tools_fifo_ instead
// void hal_base_hid_genericfifo_init(hal_base_genericfifo_t *gf, uint8_t capacity, uint8_t sizeofitem, uint8_t *data)
// {
//     // avoid checks .... be careful in calling
//     
//     gf->capacity = capacity;
//     gf->sizeofitem = sizeofitem;
//     gf->data = data;
//     
//     hal_base_hid_genericfifo_reset(gf);
// }


// void hal_base_hid_genericfifo_reset(hal_base_genericfifo_t *gf)
// {
//     // avoid checks .... be careful in calling
//     
//     gf->size = 0;
//     gf->index = 0;
// }

// void hal_base_hid_genericfifo_clear(hal_base_genericfifo_t *gf)
// {
//     // avoid checks .... be careful in calling
//     
//     memset(gf->data, 0, gf->capacity * gf->sizeofitem);
//     
//     gf->size = 0;
//     gf->index = 0;
// }


// hal_result_t hal_base_hid_genericfifo_put(hal_base_genericfifo_t *gf, uint8_t *data)
// {
//     // avoid checks .... be careful in calling
//     
//     if(gf->size == gf->capacity)
//     {
//         return(hal_res_NOK_generic);
//     }
//     
//     memcpy(gf->data + (gf->sizeofitem)*(gf->index), data, gf->sizeofitem);
//     gf->size ++;
//     gf->index ++;
//     if(gf->index == gf->capacity)
//     {
//         gf->index = 0;
//     }
//     
//     return(hal_res_OK);
// }


// hal_result_t hal_base_hid_genericfifo_get(hal_base_genericfifo_t *gf, uint8_t *data, uint8_t *size)
// {
//     // avoid checks .... be careful in calling
//     
//     uint8_t start = 0;
//     
//     if(0 == gf->size)
//     {
//         return(hal_res_NOK_nodata);
//     }
//     
//     start = (gf->capacity + gf->index - gf->size);
//     if(start >= gf->capacity)
//     {
//         start -= gf->capacity;
//     }
//     
//     memcpy(data, gf->data + (gf->sizeofitem)*(start), gf->sizeofitem);
//     gf->size --;
// 	
// 	*size = gf->size;
//     
//     return(hal_res_OK);
// }


// uint8_t * hal_base_hid_genericfifo_front(hal_base_genericfifo_t *gf)
// {
//     // avoid checks .... be careful in calling
//     
//     uint8_t start = 0;
//     
//     if(0 == gf->size)
//     {
//         return(NULL);
//     }
//     
//     start = (gf->capacity + gf->index - gf->size);
//     if(start >= gf->capacity)
//     {
//         start -= gf->capacity;
//     }
//     
//     return(gf->data + (gf->sizeofitem)*(start));
// }


// void hal_base_hid_genericfifo_pop(hal_base_genericfifo_t *gf)
// {
//     // avoid checks .... be careful in calling

//     if(0 != gf->size)
//     {
//         gf->size --;
//     }
//     
//     return;
// }


// uint8_t hal_base_hid_genericfifo_size(hal_base_genericfifo_t *gf)
// {
//     return(gf->size);
// }
// #endif

extern void hal_base_hid_osal_scheduling_suspend(void)
{
	if(0 != (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
	{	// no need if it is called by an isr
	 	return;
	}

    if(NULL != s_hal_fn_osal_system_scheduling_suspend)
    {
		s_hal_fn_osal_system_scheduling_suspend();
    }
}


extern void hal_base_hid_osal_scheduling_restart(void)
{
	if(0 != (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
	{	// no need if it is called by an isr
	 	return;
	}

    if(NULL != s_hal_fn_osal_system_scheduling_restart)
    {
		s_hal_fn_osal_system_scheduling_restart();
    }
} 


extern void hal_base_hid_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
    if(NULL != s_hal_fn_on_error)
    {
        s_hal_fn_on_error(errorcode, errormsg);
    } 
    else
    {   
        for(;;)
        {
            errorcode = errorcode;
        }
    }
}

 
void hal_hid_link_to_all_files(void)
{
    // removed ...
}


extern void hal_base_hid_ram_basic_init(void)
{
    // in here we set values of memory which is required in hal_base_memory_getsize() and in hal_base_initialise()
    // before any specific initialisation.

    // for nzi data
    memset(&hal_base_hid_params, 0, sizeof(hal_base_hid_params));

    s_hal_fn_on_error = NULL;
    s_hal_fn_osal_system_scheduling_suspend = NULL;
    s_hal_fn_osal_system_scheduling_restart = NULL;

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



#endif//HAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



