/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __STM32HAL_BOARD_H_
#define __STM32HAL_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------  
    
#include "stm32hal_define.h"
#include "stm32hal_driver.h"
    
// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types -------------------------------------------------------------------------    
// empty-section
    
    
// - declaration of extern public functions ---------------------------------------------------------------------------
 
// called by stm32hal_init() if stm32hal_config_t::initbsp is true
    
extern void stm32hal_board_init(void);     
    
// - public interface: begin  -----------------------------------------------------------------------------------------  
// it contains whatever cube-mx generates.


#if     defined(STM32HAL_BOARD_NUCLEOL4)

    // one version only: v1D3
    #if  (STM32HAL_DRIVER_VERSION == 0x1D3)
        
        #include "../src/config/stm32hal_driver_cfg_of_nucleol4_v1D3.h"

        #include "../src/board/nucleol4/v1D3/inc/gpio.h"
        #include "../src/board/nucleol4/v1D3/inc/main.h"
        #include "../src/board/nucleol4/v1D3/inc/rng.h"
        
    #else
        #error unsupported driver version for nucleol4
    #endif
    
#elif   defined(STM32HAL_BOARD_MTB4)
        
    #if (STM32HAL_DRIVER_VERSION == 0x190)

        #include "../src/config/stm32hal_driver_cfg_of_mtb4_v190.h"

        #include "../src/board/mtb4/v190/inc/adc.h"
        #include "../src/board/mtb4/v190/inc/can.h"
        #include "../src/board/mtb4/v190/inc/dma.h"
        #include "../src/board/mtb4/v190/inc/gpio.h"
        #include "../src/board/mtb4/v190/inc/i2c.h"
        #include "../src/board/mtb4/v190/inc/rng.h"
        #include "../src/board/mtb4/v190/inc/main.h"
        #include "../src/board/mtb4/v190/inc/tim.h"
        #include "../src/board/mtb4/v190/inc/usart.h"

        #include "../src/board/mtb4/v190/inc/stm32l4xx_it.h"

    #elif (STM32HAL_DRIVER_VERSION == 0x1D3)

        #include "../src/config/stm32hal_driver_cfg_of_mtb4_v1D3.h"

        #include "../src/board/mtb4/v1D3/inc/adc.h"
        #include "../src/board/mtb4/v1D3/inc/can.h"
        #include "../src/board/mtb4/v1D3/inc/dma.h"
        #include "../src/board/mtb4/v1D3/inc/gpio.h"
        #include "../src/board/mtb4/v1D3/inc/i2c.h"
        #include "../src/board/mtb4/v1D3/inc/rng.h"
        #include "../src/board/mtb4/v1D3/inc/main.h"
        #include "../src/board/mtb4/v1D3/inc/tim.h"
        #include "../src/board/mtb4/v1D3/inc/usart.h"

        #include "../src/board/mtb4/v1D3/inc/stm32l4xx_it.h"

    #else
        #error unsupported driver version for mtb4
    #endif

#elif   defined(STM32HAL_BOARD_STRAIN2)	

    // three cases: v190, v1B0, V1D3
    #if (STM32HAL_DRIVER_VERSION == 0x190)

        #include "../src/config/stm32hal_driver_cfg_of_strain2_v190.h"

        #include "../src/board/strain2/v190/inc/adc.h"
        #include "../src/board/strain2/v190/inc/can.h"
        #include "../src/board/strain2/v190/inc/dma.h"
        #include "../src/board/strain2/v190/inc/gpio.h"
        #include "../src/board/strain2/v190/inc/i2c.h"
        #include "../src/board/strain2/v190/inc/main.h"
        #include "../src/board/strain2/v190/inc/rng.h"
        #include "../src/board/strain2/v190/inc/tim.h"
        #include "../src/board/strain2/v190/inc/usart.h"

        #include "../src/board/strain2/v190/inc/stm32l4xx_it.h"

    #elif(STM32HAL_DRIVER_VERSION == 0x1B0)

				#if !defined(CPU_AT_80MHZ)
				#define CPU_AT_80MHZ
				#endif
				
        #include "../src/config/stm32hal_driver_cfg_of_strain2_v1B0.h"

        #include "../src/board/strain2/v1B0/inc/adc.h"
        #include "../src/board/strain2/v1B0/inc/can.h"
        #include "../src/board/strain2/v1B0/inc/dma.h"
        #include "../src/board/strain2/v1B0/inc/gpio.h"
        #include "../src/board/strain2/v1B0/inc/i2c.h"
        #include "../src/board/strain2/v1B0/inc/main.h"
        #include "../src/board/strain2/v1B0/inc/rng.h"
        #include "../src/board/strain2/v1B0/inc/tim.h"
        #include "../src/board/strain2/v1B0/inc/usart.h"

        #include "../src/board/strain2/v1B0/inc/stm32l4xx_it.h"


    #elif(STM32HAL_DRIVER_VERSION == 0x1D3)
				
        #include "../src/config/stm32hal_driver_cfg_of_strain2_v1D3.h"

        #include "../src/board/strain2/v1D3/inc/adc.h"
        #include "../src/board/strain2/v1D3/inc/can.h"
        #include "../src/board/strain2/v1D3/inc/dma.h"
        #include "../src/board/strain2/v1D3/inc/gpio.h"
        #include "../src/board/strain2/v1D3/inc/i2c.h"
        #include "../src/board/strain2/v1D3/inc/main.h"
        #include "../src/board/strain2/v1D3/inc/rng.h"
        #include "../src/board/strain2/v1D3/inc/tim.h"
        #include "../src/board/strain2/v1D3/inc/usart.h"

        #include "../src/board/strain2/v1D3/inc/stm32l4xx_it.h"

    #else
        #error unsupported driver version for strain2
    #endif


#elif   defined(STM32HAL_BOARD_PSC)	

    // one case only: v190
    #if (STM32HAL_DRIVER_VERSION == 0x190)

        #include "../src/config/stm32hal_driver_cfg_of_psc_v190.h"
        
        #include "../src/board/psc/v190/inc/main.h"

        #include "../src/board/psc/v190/inc/can.h"
        #include "../src/board/psc/v190/inc/dma.h"
        #include "../src/board/psc/v190/inc/gpio.h"
        #include "../src/board/psc/v190/inc/i2c.h"        
        #include "../src/board/psc/v190/inc/rng.h"
        #include "../src/board/psc/v190/inc/tim.h"

        #include "../src/board/psc/v190/inc/stm32l4xx_it.h"

    #else
        #error unsupported driver version for psc
    #endif
    
#elif   defined(STM32HAL_BOARD_RFE) 

    // one case only: v183
    #if (STM32HAL_DRIVER_VERSION == 0x183)

        #include "../src/config/stm32hal_driver_cfg_of_rfe.h"

        #include "../src/board/rfe/v183/inc/adc.h"
        #include "../src/board/rfe/v183/inc/can.h"
        #include "../src/board/rfe/v183/inc/dma.h"
        #include "../src/board/rfe/v183/inc/gpio.h"
        #include "../src/board/rfe/v183/inc/i2c.h"
        #include "../src/board/rfe/v183/inc/main.h"
        #include "../src/board/rfe/v183/inc/spi.h"
        #include "../src/board/rfe/v183/inc/rng.h"
        #include "../src/board/rfe/v183/inc/tim.h"

        #include "../src/board/rfe/v183/inc/stm32l4xx_it.h"

    #else
        #error unsupported driver version for rfe
    #endif

#elif   defined(STM32HAL_BOARD_SG3) 

    // one case only: v190
    #if (STM32HAL_DRIVER_VERSION == 0x190)

        #include "../src/config/stm32hal_driver_cfg_of_sg3_v190.h"

        #include "../src/board/sg3/v190/inc/adc.h"
        #include "../src/board/sg3/v190/inc/can.h"
        #include "../src/board/sg3/v190/inc/dma.h"
        #include "../src/board/sg3/v190/inc/gpio.h"
        #include "../src/board/sg3/v190/inc/i2c.h"
        #include "../src/board/sg3/v190/inc/main.h"
        #include "../src/board/sg3/v190/inc/tim.h"

        #include "../src/board/sg3/v190/inc/stm32l4xx_it.h"

    #else
        #error unsupported driver version for sg3
    #endif

#elif   defined(STM32HAL_BOARD_NUCLEOH7) 

    // two cases: v180 and v190
    #if (STM32HAL_DRIVER_VERSION == 0x180)

        #include "../src/config/stm32hal_driver_cfg_of_nucleoh7_v180.h"

        #include "../src/board/nucleoh7/v180/inc/dma.h"
        #include "../src/board/nucleoh7/v180/inc/gpio.h"
        #include "../src/board/nucleoh7/v180/inc/i2c.h"
        #include "../src/board/nucleoh7/v180/inc/main.h"
        #include "../src/board/nucleoh7/v180/inc/usb_otg.h"
        #include "../src/board/nucleoh7/v180/inc/usart.h"

        #include "../src/board/nucleoh7/v180/inc/stm32h7xx_it.h"

    #elif (STM32HAL_DRIVER_VERSION == 0x190)

        #include "../src/config/stm32hal_driver_cfg_of_nucleoh7_v190.h"

        #include "../src/board/nucleoh7/v190/inc/dma.h"
        #include "../src/board/nucleoh7/v190/inc/gpio.h"
        #include "../src/board/nucleoh7/v190/inc/i2c.h"
        #include "../src/board/nucleoh7/v190/inc/main.h"
        #include "../src/board/nucleoh7/v190/inc/usb_otg.h"
        #include "../src/board/nucleoh7/v190/inc/usart.h"

        #include "../src/board/nucleoh7/v190/inc/stm32h7xx_it.h"

    #else
        #error unsupported driver version for nucleoh7
    #endif

#elif   defined(STM32HAL_BOARD_STM32G4EVAL) 

    // two cases: v111 and v120
    #if (STM32HAL_DRIVER_VERSION == 0x111)

        #include "../src/config/stm32hal_driver_cfg_of_stm32g4eval_v111.h"

        #include "../src/board/stm32g4eval/v111/inc/main.h"
        #include "../src/board/stm32g4eval/v111/inc/adc.h"
        #include "../src/board/stm32g4eval/v111/inc/comp.h"
        #include "../src/board/stm32g4eval/v111/inc/dac.h"
        #include "../src/board/stm32g4eval/v111/inc/dma.h"
        #include "../src/board/stm32g4eval/v111/inc/fdcan.h"
        #include "../src/board/stm32g4eval/v111/inc/fmac.h"
        #include "../src/board/stm32g4eval/v111/inc/fmc.h"
        #include "../src/board/stm32g4eval/v111/inc/gpio.h"
        #include "../src/board/stm32g4eval/v111/inc/hrtim.h"
        #include "../src/board/stm32g4eval/v111/inc/i2c.h"
        #include "../src/board/stm32g4eval/v111/inc/rng.h"
        #include "../src/board/stm32g4eval/v111/inc/spi.h"
        #include "../src/board/stm32g4eval/v111/inc/tim.h"
        #include "../src/board/stm32g4eval/v111/inc/usart.h"
        #include "../src/board/stm32g4eval/v111/inc/usb.h"
        
        #include "../src/board/stm32g4eval/v111/inc/stm32g4xx_it.h"
        
    #elif (STM32HAL_DRIVER_VERSION == 0x120)

        #include "../src/config/stm32hal_driver_cfg_of_stm32g4eval_v120.h"

        #include "../src/board/stm32g4eval/v120/inc/main.h"
        #include "../src/board/stm32g4eval/v120/inc/adc.h"
        #include "../src/board/stm32g4eval/v120/inc/comp.h"
        #include "../src/board/stm32g4eval/v120/inc/dac.h"
        #include "../src/board/stm32g4eval/v120/inc/dma.h"
        #include "../src/board/stm32g4eval/v120/inc/fdcan.h"
        #include "../src/board/stm32g4eval/v120/inc/fmac.h"
        #include "../src/board/stm32g4eval/v120/inc/fmc.h"
        #include "../src/board/stm32g4eval/v120/inc/gpio.h"
        #include "../src/board/stm32g4eval/v120/inc/hrtim.h"
        #include "../src/board/stm32g4eval/v120/inc/i2c.h"
        #include "../src/board/stm32g4eval/v120/inc/rng.h"
        #include "../src/board/stm32g4eval/v120/inc/spi.h"
        #include "../src/board/stm32g4eval/v120/inc/tim.h"
        #include "../src/board/stm32g4eval/v120/inc/usart.h"
        #include "../src/board/stm32g4eval/v120/inc/usb.h"
        
        #include "../src/board/stm32g4eval/v120/inc/stm32g4xx_it.h"
    #else
        #error unsupported driver version for stm32g4eval
    #endif

#elif   defined(STM32HAL_BOARD_PMC) 

    // two cases: v111 and v120
    #if (STM32HAL_DRIVER_VERSION == 0x111)

        #include "../src/config/stm32hal_driver_cfg_of_pmc_v111.h"

        #include "../src/board/pmc/v111/inc/main.h"
        #include "../src/board/pmc/v111/inc/adc.h"
        #include "../src/board/pmc/v111/inc/comp.h"
        #include "../src/board/pmc/v111/inc/cordic.h"
        #include "../src/board/pmc/v111/inc/crc.h"
        #include "../src/board/pmc/v111/inc/dma.h"
        #include "../src/board/pmc/v111/inc/fdcan.h"
        #include "../src/board/pmc/v111/inc/fmac.h"
        #include "../src/board/pmc/v111/inc/gpio.h"
        #include "../src/board/pmc/v111/inc/i2c.h"
        #include "../src/board/pmc/v111/inc/rng.h"
        #include "../src/board/pmc/v111/inc/rtc.h"
        #include "../src/board/pmc/v111/inc/spi.h"
        #include "../src/board/pmc/v111/inc/tim.h"
        //#include "../src/board/pmc/v111/inc/usart.h"
        #include "../src/board/pmc/v111/inc/usb.h"
        
        //#include "../src/board/pmc/v120/inc/stm32g4xx_it.h"    
    #elif (STM32HAL_DRIVER_VERSION == 0x120)

        #include "../src/config/stm32hal_driver_cfg_of_pmc_v120.h"

        #include "../src/board/pmc/v120/inc/main.h"
        #include "../src/board/pmc/v120/inc/adc.h"
        #include "../src/board/pmc/v120/inc/comp.h"
        #include "../src/board/pmc/v120/inc/cordic.h"
        #include "../src/board/pmc/v120/inc/crc.h"
        #include "../src/board/pmc/v120/inc/dma.h"
        #include "../src/board/pmc/v120/inc/fdcan.h"
        #include "../src/board/pmc/v120/inc/fmac.h"
        #include "../src/board/pmc/v120/inc/gpio.h"
        #include "../src/board/pmc/v120/inc/i2c.h"
        #include "../src/board/pmc/v120/inc/rng.h"
        #include "../src/board/pmc/v120/inc/rtc.h"
        #include "../src/board/pmc/v120/inc/spi.h"
        #include "../src/board/pmc/v120/inc/tim.h"
        //#include "../src/board/pmc/v120/inc/usart.h"
        #include "../src/board/pmc/v120/inc/usb.h"
        
        //#include "../src/board/pmc/v120/inc/stm32g4xx_it.h"
        

    #else
        #error unsupported driver version for pmc
    #endif

#elif   defined(STM32HAL_BOARD_AMCBLDC) 

    // two cases: v120 and v122
    #if (STM32HAL_DRIVER_VERSION == 0x120)

        #include "../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"

        #include "../src/board/amcbldc/v120/inc/main.h"
        #include "../src/board/amcbldc/v120/inc/adc.h"
        #include "../src/board/amcbldc/v120/inc/cordic.h"
        #include "../src/board/amcbldc/v120/inc/crc.h"
        #include "../src/board/amcbldc/v120/inc/dma.h"
        #include "../src/board/amcbldc/v120/inc/fdcan.h"
        #include "../src/board/amcbldc/v120/inc/fmac.h"
        #include "../src/board/amcbldc/v120/inc/gpio.h"
        #include "../src/board/amcbldc/v120/inc/i2c.h"
        #include "../src/board/amcbldc/v120/inc/rng.h"
        #include "../src/board/amcbldc/v120/inc/tim.h"
        #include "../src/board/amcbldc/v120/inc/usb.h"
        
        #include "../src/board/amcbldc/v120/inc/stm32g4xx_it.h"        
    #elif (STM32HAL_DRIVER_VERSION == 0x122)

        #include "../src/config/stm32hal_driver_cfg_of_amcbldc_v122.h"

        #include "../src/board/amcbldc/v122/inc/main.h"
        #include "../src/board/amcbldc/v122/inc/adc.h"
        #include "../src/board/amcbldc/v122/inc/cordic.h"
        #include "../src/board/amcbldc/v122/inc/crc.h"
        #include "../src/board/amcbldc/v122/inc/dma.h"
        #include "../src/board/amcbldc/v122/inc/fdcan.h"
        #include "../src/board/amcbldc/v122/inc/fmac.h"
        #include "../src/board/amcbldc/v122/inc/gpio.h"
        #include "../src/board/amcbldc/v122/inc/i2c.h"
        #include "../src/board/amcbldc/v122/inc/rng.h"
        #include "../src/board/amcbldc/v122/inc/tim.h"
        #include "../src/board/amcbldc/v122/inc/usb.h"
        
        #include "../src/board/amcbldc/v122/inc/stm32g4xx_it.h"
    #else
        #error unsupported driver version for amcbldc
    #endif
    
#elif   defined(STM32HAL_BOARD_STM32H745DISCO) 

    // two cases: v190 and v1A0
    #if (STM32HAL_DRIVER_VERSION == 0x190)
    
        #include "../src/config/stm32hal_driver_cfg_of_stm32h745disco_v190.h"

        #include "../src/board/stm32h745disco/v190/inc/gpio.h"
        #include "../src/board/stm32h745disco/v190/inc/main.h"
        #include "../src/board/stm32h745disco/v190/inc/eth.h"

        #include "../src/board/stm32h745disco/v190/inc/stm32h7xx_it.h"

    #elif (STM32HAL_DRIVER_VERSION == 0x1A0)
    
        #include "../src/config/stm32hal_driver_cfg_of_stm32h745disco_v1A0.h"

        #include "../src/board/stm32h745disco/v1A0/inc/gpio.h"
        #include "../src/board/stm32h745disco/v1A0/inc/main.h"
        #include "../src/board/stm32h745disco/v1A0/inc/eth.h"

        #include "../src/board/stm32h745disco/v1A0/inc/stm32h7xx_it.h"
        #include "../src/board/stm32h745disco/v1A0/drv/Driver_ETH_MAC.h"
        #include "../src/board/stm32h745disco/v1A0/drv/Driver_ETH_PHY.h"
		
    #else
        #error unsupported driver version for stm32h745disco
    #endif

#elif   defined(STM32HAL_BOARD_AMC) 

    // two cases: v1A0 and v1B1
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
    
        #include "../src/config/stm32hal_driver_cfg_of_amc_v1A0.h"

        #include "../src/board/amc/v1A0/inc/gpio.h"
        #include "../src/board/amc/v1A0/inc/main.h"
        #include "../src/board/amc/v1A0/inc/eth.h"
        #include "../src/board/amc/v1A0/inc/spi.h"
        //#include "../src/board/amc/v1A0/inc/fdcan.h"

        #include "../src/board/amc/v1A0/inc/stm32h7xx_it.h"

    #elif (STM32HAL_DRIVER_VERSION == 0x1B1)
    
        #include "../src/config/stm32hal_driver_cfg_of_amc_v1B1.h"

        #include "../src/board/amc/v1B1/inc/gpio.h"
        #include "../src/board/amc/v1B1/inc/main.h"
        #include "../src/board/amc/v1B1/inc/eth.h"
        #include "../src/board/amc/v1B1/inc/spi.h"
        #include "../src/board/amc/v1B1/inc/fdcan.h"

        #include "../src/board/amc/v1B1/inc/stm32h7xx_it.h"
    #else
        #error unsupported driver version for amc
    #endif

#elif   defined(STM32HAL_BOARD_AMC2C) 


    // one case: v1A0
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
       
        #include "../src/config/stm32hal_driver_cfg_of_amc2c_v1A0.h"

        #include "../src/board/amc2c/v1A0/inc/gpio.h"
        #include "../src/board/amc2c/v1A0/inc/main.h"

        #include "../src/board/amc2c/v1A0/inc/stm32h7xx_it.h"
		
    #else
        #error unsupported driver version for amc2c
    #endif

#elif   defined(STM32HAL_BOARD_AMCFOCM7) 

    // one case: v1A0
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
    
        #include "../src/config/stm32hal_driver_cfg_of_amcfocm7_v1A0.h"
        #include "../src/board/amcfocm7/v1A0/inc/gpio.h"
        #include "../src/board/amcfocm7/v1A0/inc/stm32h7xx_it.h"
		
    #else
        #error unsupported driver version for amcfocm7
    #endif

#elif   defined(STM32HAL_BOARD_AMCFOCM4) 


    // one case: v1A0
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
       
        #include "../src/config/stm32hal_driver_cfg_of_amcfocm4_v1A0.h"
//        #include "../src/board/amcfocm4/v1A0/inc/gpio.h"
        #include "../src/board/amcfocm4/v1A0/inc/stm32h7xx_it.h"
		
    #else
        #error unsupported driver version for amcfocm4
    #endif
    
#elif   defined(STM32HAL_BOARD_MTB4C)
        
    #if(STM32HAL_DRIVER_VERSION == 0x190)

        #include "../src/config/stm32hal_driver_cfg_of_mtb4c_v190.h"

        //#include "../src/board/mtb4c/v190/inc/adc.h"
        #include "../src/board/mtb4c/v190/inc/can.h"
        #include "../src/board/mtb4c/v190/inc/dma.h"
        #include "../src/board/mtb4c/v190/inc/gpio.h"
        #include "../src/board/mtb4c/v190/inc/i2c.h"
        //#warning add rng.h in cubmx project
        //#include "../src/board/mtb4c/v190/inc/rng.h"
        #include "../src/board/mtb4c/v190/inc/main.h"
        #include "../src/board/mtb4c/v190/inc/tim.h"
        #include "../src/board/mtb4c/v190/inc/usart.h"

        #include "../src/board/mtb4c/v190/inc/stm32l4xx_it.h"

    #elif(STM32HAL_DRIVER_VERSION == 0x1D3)

        #include "../src/config/stm32hal_driver_cfg_of_mtb4c_v1D3.h"

        #include "../src/board/mtb4c/v1D3/inc/adc.h"
        #include "../src/board/mtb4c/v1D3/inc/can.h"
        #include "../src/board/mtb4c/v1D3/inc/dma.h"
        #include "../src/board/mtb4c/v1D3/inc/gpio.h"
        #include "../src/board/mtb4c/v1D3/inc/i2c.h"
        #include "../src/board/mtb4c/v1D3/inc/rng.h"
        #include "../src/board/mtb4c/v1D3/inc/main.h"
        #include "../src/board/mtb4c/v1D3/inc/tim.h"
        #include "../src/board/mtb4c/v1D3/inc/usart.h"

        #include "../src/board/mtb4c/v1D3/inc/stm32l4xx_it.h"
        
    #else
        #error unsupported driver version for mtb4c
    #endif

#elif   defined(STM32HAL_BOARD_STRAIN2C)	
    
    // two cases: v1B0 and v1D3
    #if(STM32HAL_DRIVER_VERSION == 0x1B0)
                
        #include "../src/config/stm32hal_driver_cfg_of_strain2c_v1B0.h"

        #include "../src/board/strain2c/v1B0/inc/adc.h"
        #include "../src/board/strain2c/v1B0/inc/can.h"
        #include "../src/board/strain2c/v1B0/inc/dma.h"
        #include "../src/board/strain2c/v1B0/inc/gpio.h"
        #include "../src/board/strain2c/v1B0/inc/i2c.h"
        #include "../src/board/strain2c/v1B0/inc/main.h"
        #include "../src/board/strain2c/v1B0/inc/rng.h"
        #include "../src/board/strain2c/v1B0/inc/tim.h"
        #include "../src/board/strain2c/v1B0/inc/usart.h"

        #include "../src/board/strain2c/v1B0/inc/stm32l4xx_it.h"

    #elif(STM32HAL_DRIVER_VERSION == 0x1D3)

        #include "../src/config/stm32hal_driver_cfg_of_strain2c_v1D3.h"

        #include "../src/board/strain2c/v1D3/inc/adc.h"
        #include "../src/board/strain2c/v1D3/inc/can.h"
        #include "../src/board/strain2c/v1D3/inc/dma.h"
        #include "../src/board/strain2c/v1D3/inc/gpio.h"
        #include "../src/board/strain2c/v1D3/inc/i2c.h"
        #include "../src/board/strain2c/v1D3/inc/main.h"
        #include "../src/board/strain2c/v1D3/inc/rng.h"
        #include "../src/board/strain2c/v1D3/inc/tim.h"
        #include "../src/board/strain2c/v1D3/inc/usart.h"

        #include "../src/board/strain2c/v1D3/inc/stm32l4xx_it.h"
        
    #else
        #error unsupported driver version for strain2c
    #endif
    
#elif   defined(STM32HAL_BOARD_EMS)	
    
    // nothing to include 
    
#elif   defined(STM32HAL_BOARD_MC4PLUS)	
        
    // nothing to include 
        
#else
    #error STM32HAL: you must define a STM32HAL_BOARD_${B}
#endif


// - public interface: end --------------------------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
