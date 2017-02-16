

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_DRIVER_H_
#define _STM32HAL_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32hal_define.h"

#include "stm32hal_cmsis.h"


#if defined(STM32L476xx)

// this one points to the stm32l4xx_hal_conf.h generated for a given board xxx, hence it is inside src/board/xxx
#include "../src/stm32l4/stm32l4xx_hal_conf.h"

#include "../src/stm32l4/stm32l4xx_hal.h"
#include "../src/stm32l4/stm32l4xx_hal_adc.h"
#include "../src/stm32l4/stm32l4xx_hal_adc_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_can.h"
#include "../src/stm32l4/stm32l4xx_hal_comp.h"
#include "../src/stm32l4/stm32l4xx_hal_cortex.h"
#include "../src/stm32l4/stm32l4xx_hal_crc.h"
#include "../src/stm32l4/stm32l4xx_hal_crc_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_cryp.h"
#include "../src/stm32l4/stm32l4xx_hal_cryp_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_dac.h"
#include "../src/stm32l4/stm32l4xx_hal_dac_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_def.h"
#include "../src/stm32l4/stm32l4xx_hal_dfsdm.h"
#include "../src/stm32l4/stm32l4xx_hal_dma.h"
#include "../src/stm32l4/stm32l4xx_hal_firewall.h"
#include "../src/stm32l4/stm32l4xx_hal_flash.h"
#include "../src/stm32l4/stm32l4xx_hal_flash_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_flash_ramfunc.h"
#include "../src/stm32l4/stm32l4xx_hal_gpio.h"
#include "../src/stm32l4/stm32l4xx_hal_gpio_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_hcd.h"
#include "../src/stm32l4/stm32l4xx_hal_i2c.h"
#include "../src/stm32l4/stm32l4xx_hal_i2c_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_irda.h"
#include "../src/stm32l4/stm32l4xx_hal_irda_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_iwdg.h"
#include "../src/stm32l4/stm32l4xx_hal_lcd.h"
#include "../src/stm32l4/stm32l4xx_hal_lptim.h"
#include "../src/stm32l4/stm32l4xx_hal_nand.h"
#include "../src/stm32l4/stm32l4xx_hal_nor.h"
#include "../src/stm32l4/stm32l4xx_hal_opamp.h"
#include "../src/stm32l4/stm32l4xx_hal_opamp_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_pcd.h"
#include "../src/stm32l4/stm32l4xx_hal_pcd_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_pwr.h"
#include "../src/stm32l4/stm32l4xx_hal_pwr_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_qspi.h"
#include "../src/stm32l4/stm32l4xx_hal_rcc.h"
#include "../src/stm32l4/stm32l4xx_hal_rcc_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_rng.h"
#include "../src/stm32l4/stm32l4xx_hal_rtc.h"
#include "../src/stm32l4/stm32l4xx_hal_rtc_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_sai.h"
#include "../src/stm32l4/stm32l4xx_hal_sd.h"
#include "../src/stm32l4/stm32l4xx_hal_smartcard.h"
#include "../src/stm32l4/stm32l4xx_hal_smartcard_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_smbus.h"
#include "../src/stm32l4/stm32l4xx_hal_spi.h"
#include "../src/stm32l4/stm32l4xx_hal_spi_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_sram.h"
#include "../src/stm32l4/stm32l4xx_hal_swpmi.h"
#include "../src/stm32l4/stm32l4xx_hal_tim.h"
#include "../src/stm32l4/stm32l4xx_hal_tim_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_tsc.h"
#include "../src/stm32l4/stm32l4xx_hal_uart.h"
#include "../src/stm32l4/stm32l4xx_hal_uart_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_usart.h"
#include "../src/stm32l4/stm32l4xx_hal_usart_ex.h"
#include "../src/stm32l4/stm32l4xx_hal_wwdg.h"


#else
    #error STM32HAL: the MPU is undefined (e.g., STM32L476xx) 
#endif



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard
