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

/* @file       hal_brdcfg_ems4rd.c
	@brief      This file implements support for board ems4rd
	@author     marco.accame@iit.it
    @date       11/14/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_middleware_interface.h"


#include "hal_base.h"
#include "hal_base_hid.h"

#include "hal_cpu.h"
#include "hal_cpu_hid.h"

#if defined(HAL_USE_UTILITY_BITS)
#include "hal_utility_bits_hid.h" 
#endif
#if defined(HAL_USE_UTILITY_CRC07)
#include "hal_utility_crc07_hid.h" 
#endif
#if defined(HAL_USE_UTILITY_CRC16)
#include "hal_utility_crc16_hid.h" 
#endif
#if defined(HAL_USE_UTILITY_CRC32)
#include "hal_utility_crc32_hid.h"
#endif
#if defined(HAL_USE_UTILITY_FIFO)
#include "hal_utility_fifo_hid.h" 
#endif

#include "hal_periph_can_hid.h"
#include "hal_periph_spi_hid.h"
#include "hal_periph_eth_hid.h"
#include "hal_periph_trace_hid.h"
#include "hal_eeprom.h"
#include "hal_timer.h"
#include "hal_i2c.h"
#include "hal_watchdog.h"

#include "hal_switch.h"
#include "hal_device_ethtransceiver_hid.h"

//#include "hal_chip_xx_eeprom_hid.h"
//#include "hal_chip_micrel_ks8893_hid.h"
//#include "hal_chip_st_l3g4200d_hid.h"
//#include "hal_chip_st_lis3dh_hid.h"
//#include "hal_chip_generic_ethtransceiver_hid.h"


#include "hl_cfg_plus_target.h"
#include "hl_cfg_plus_modules.h"


#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)
    #warning --> using micrel as managed device w/ EMS4RD_USE_MICREL_AS_MANAGED_DEVICE
#else
    #warning --> using micrel as automatic un-mananaged device
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_ems4rd.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(HAL_USE_PERIPH_ETH) || defined(HAL_USE_DEVICE_ETHTRANSCEIVER) || defined(HAL_USE_DEVICE_SWITCH)
    #define HAL_ETH_PHYMODE_THEONE2USE     hal_eth_phymode_fullduplex100mbps
#endif





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// -- core

#ifdef  HAL_USE_BASE
    extern const hal_base_hid_brdcfg_t hal_brdcfg_base__theconfig  =
    {
        .dummy  = 0
    };    
#endif//HAL_USE_BASE  
    
    
#ifdef  HAL_USE_CPU
    extern const hal_cpu_hid_brdcfg_t hal_brdcfg_cpu__theconfig  =
    {
        .architecture       = hal_cpu_arc_armcm4,
        .family             = hal_cpu_fam_stm32f4,
        .name               = hal_cpu_nam_stm32f407,
        .speeds             =
        {   // these values must be coherent w/ HL_CFG_MPUSPEED_STM32xxx valued defined in hl_cfg_plus_target.h 
            .max                = 168000000,
            .cpu                = 168000000,
            .fastbus            =  84000000,
            .slowbus            =  42000000
        }//,
//         .clockcfg                      =
//         {
//             .keepinternalclockatstartup     = hal_false,      // false is the preferred mode
//             .refclock                       = hal_cpu_refclock_pll_on_external_osc,  
//             .intclockspeed                  =  16000000,
//             .extclockspeed                  =  50000000,
//             .pllcfg                         =
//             {   // pllfreq = n*(source/m), speedcpu = pllfreq/p, 48mhz = pllfreq/q
//                 .m                  =  50, 
//                 .n                  = 336,
//                 .p                  =   2,
//                 .q                  =   7
//             }
//         }   
    };    
#endif//HAL_USE_CPU  

#ifdef  HAL_USE_FLASH
    extern const hal_flash_hid_brdcfg_t hal_brdcfg_flash__theconfig =
    {
        .baseaddress    =   0x08000000,                 // on every stm
        .totalsize      =
        #if     defined(HAL_USE_CPU_NAM_STM32F107)
                            256*1024
        #elif   defined(HAL_USE_CPU_NAM_STM32F407)
                            1024*1024
        #endif
    };
#endif//HAL_USE_FLASH
 

#ifdef  HAL_USE_HEAP
    extern const hal_heap_hid_brdcfg_t hal_brdcfg_heap__theconfig =
    {
        .nothingsofar   = 0
    };
#endif//HAL_USE_HEAP    
   
    
#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_brdcfg_t hal_brdcfg_sys__theconfig  =
    {
        .dummy             = 0
    };    
#endif//HAL_USE_SYS   
   

// --  extra: mpu peripherals

#ifdef  HAL_USE_PERIPH_CAN
    extern const hal_can_hid_brdcfg_t hal_brdcfg_can__theconfig =
    {
        .supported_mask     = (1 << hal_can1) | (1 << hal_can2),
       .gpiomap            =
        {   
            {   // hal_can1 
                .rx = 
                {
                    .gpio   =
                    {
                        .port   = hal_gpio_portD, 
                        .pin    = hal_gpio_pin0
                    },
                    .af32       = GPIO_AF_CAN1          
                }, 
                .tx = 
                {
                    .gpio   =
                    {
                        .port   = hal_gpio_portD, 
                        .pin    = hal_gpio_pin1
                    },
                    .af32       = GPIO_AF_CAN1          
                } 
            }, 
            {   // hal_can2 
                .rx = 
                {
                    .gpio   =
                    {
                        .port   = hal_gpio_portB, 
                        .pin    = hal_gpio_pin5
                    },
                    .af32       = GPIO_AF_CAN2          
                }, 
                .tx = 
                {
                    .gpio   =
                    {
                        .port   = hal_gpio_portB, 
                        .pin    = hal_gpio_pin6
                    },
                    .af32       = GPIO_AF_CAN2          
                } 
            }   
        } 
    };
#endif//HAL_USE_PERIPH_CAN

#ifdef  HAL_USE_PERIPH_CRC
    extern const hal_crc_hid_brdcfg_t hal_brdcfg_crc__theconfig =
    {
        .supported_mask     = (1 << hal_crc1) | (1 << hal_crc2)
    };
#endif//HAL_USE_PERIPH_CRC
    
#ifdef  HAL_USE_PERIPH_DMA
    extern const hal_dma_hid_brdcfg_t hal_brdcfg_dma__theconfig =
    {
        .supported_mask     = (1 << hal_dma1) | (1 << hal_dma2) | (1 << hal_dma3) | (1 << hal_dma4) | (1 << hal_dma5) |
                              (1 << hal_dma6) | (1 << hal_dma7) | (1 << hal_dma8) | (1 << hal_dma9) | (1 << hal_dma10)|
                              (1 << hal_dma11)| (1 << hal_dma12)

    };
#endif//HAL_USE_PERIPH_DMA       

#ifdef  HAL_USE_PERIPH_ETH
    
    extern const hal_eth_hid_brdcfg_t hal_brdcfg_eth__theconfig     =
    {
        .supported          = hal_true,
        .gpiomap            =
        {
            .mif            = hal_eth_mif_rmii,
            .gpio_mif.rmii  =
            {   // if ETH_RMII_CRS_DV is on portD <-> use GPIO_Remap_ETH on rx-rmii
                .ETH_RMII_REF_CLK   =            
                {   
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin1 }, 
                    .af32   = GPIO_AF_ETH
                },  
                .ETH_RMII_TX_EN     =            
                {   
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin11 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_TXD0      =            
                {   
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin12 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_TXD1      =            
                {   
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin13 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_CRS_DV    =            
                {   
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin7 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_RXD0      =            
                {   
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin4 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_RXD1      =            
                {   
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin5 }, 
                    .af32   = GPIO_AF_ETH
                } 
            },
            .gpio_smi       =
            {
                .ETH_MDC        =            
                {   
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin1 }, 
                    .af32   = GPIO_AF_ETH
                },     
                .ETH_MDIO       =            
                {   
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin2 }, 
                    .af32   = GPIO_AF_ETH
                }     
            }      
        }
    };
    
    extern const uint8_t hal_eth_capacityofrxfifoofframes = 4;
    extern const uint8_t hal_eth_capacityoftxfifoofframes = 2;
    
    
#warning --> taken from hal1. 
    
extern void hal_brdcfg_switch__reg_write_byI2C(uint8_t* pBuffer, uint16_t WriteAddr);
extern void hal_brdcfg_switch__reg_read_byI2C(uint8_t* pBuffer, uint16_t ReadAddr); 
    
static uint8_t hal_brdcfg_eth__get_errors_code(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype);
static void s_hal_brdcfg_eth__get_links_status(hal_eth_phy_status_t* status, uint8_t phy_num);
    
    
    
#warning --> verify if eth_smi is already iniited
extern hal_result_t hal_brdcfg_eth__check_links(uint8_t *linkst_mask, uint8_t *links_num)
{   // ok. verified. it can be used even if switch is in automatic mode (not managed). 
    uint16_t status_link;
    uint8_t  PHYaddr = 0x1;
    uint8_t  REGaddr = 0x1;
    #define  LINK_IS_UP 0x00000004
    
    if((NULL == linkst_mask) || (NULL == links_num))
    {
        return(hal_res_NOK_nullpointer);
    }
    *linkst_mask = 0;
    *links_num = 2;
    
    
    status_link = hal_eth_smi_read(PHYaddr, REGaddr);
    if((status_link &LINK_IS_UP) == LINK_IS_UP)
    {
       *linkst_mask |= 0x01;
    }
    
    PHYaddr = 0x2;
    status_link = hal_eth_smi_read(PHYaddr, REGaddr);
    if((status_link &LINK_IS_UP) == LINK_IS_UP)
    {
        *linkst_mask |= 0x02;
    }
    return(hal_res_OK);
}   

#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)  
static void s_hal_brdcfg_eth__get_links_status(hal_eth_phy_status_t* status, uint8_t phy_num)
{   // it can be used only if the switch is in managed mode
    uint16_t regaddr_status0, regaddr_status1;
    uint8_t  buff_read = 0xFF;
 //   char str[60];
    
    switch(phy_num)
    {
        case 0:
        {
            regaddr_status0 = 0x1E;
            regaddr_status1 = 0x1F;
        }break;
        
        case 1:
        {
            regaddr_status0 = 0x2E;
            regaddr_status1 = 0x2F;
        }break;
        
        case 2:
        {
            regaddr_status0 = 0x0; //reserved, not applied to port 3. (see datasheet)
            regaddr_status1 = 0x3F;
        }break;
        
        default:
        {
            return;
        }
    }

    
    memset(status, 0, sizeof(hal_eth_phy_status_t));
    
    if(phy_num <2)
    {
        hal_brdcfg_switch__reg_read_byI2C(&buff_read, regaddr_status0); //port1
        if(buff_read&0x0040)// autoneg completed
        {
            status->autoNeg_done = 1;
        }
        if(buff_read&0x0020)// link is good
        {
            status->linkisgood = 1;
        }
    }
//     snprintf(str, sizeof(str), "get_links_status phynum=%d read=0x%x",phy_num, buff_read);
//     hal_trace_puts(str);
    
    buff_read = 0;
    hal_brdcfg_switch__reg_read_byI2C(&buff_read, regaddr_status1); //port1
    if(buff_read&0x0004)// link speed 1==>100
    {
        status->linkspeed = 1;
    }
    if(buff_read&0x0002)// duplex 1==>full
    {
        status->linkduplex = 1;
    }
//     snprintf(str, sizeof(str), "get_links_status phynum=%d read=0x%x",phy_num, buff_read);
//     hal_trace_puts(str);
    
}

#else
static void s_hal_brdcfg_eth__get_links_status(hal_eth_phy_status_t* status, uint8_t phy_num)
{
    memset(status, 0, sizeof(hal_eth_phy_status_t));
    if(phy_num <2)
    {
        status->autoNeg_done = 1;
        status->linkisgood = 1;
    }
    
    status->linkspeed = 1;
    status->linkduplex = 1;
}
#endif

extern hal_result_t hal_brdcfg_eth__get_links_status(hal_eth_phy_status_t* link_list, uint8_t links_num)
{
    uint8_t i; 
    
    if((NULL == link_list) || (links_num>3))
    {
        return(hal_res_NOK_nullpointer);
    }

    for(i=0; (i<links_num); i++)
    {
        s_hal_brdcfg_eth__get_links_status(&link_list[i], i);
    }

    return(hal_res_OK);
}
    
static uint8_t hal_brdcfg_eth__get_errors_code(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype)
{
    switch(phynum)
    {
        case 0:
        {
            return(errortype);
        }
        
        case 1:
        {
            return(0x20+errortype);
        }
        
        case 2:
        {
            return(0x40+errortype);
        }
        default:
        {
            return(0);
        }
    };
//     if(0 == phynum)
//     {
//         return(errortype);
//     }
//     else
//     {
//         return(0x20+errortype);
//     }
}
// static uint8_t has_finished_to_read(void)
// {
//     uint8_t read_op = 0;
//     hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7B);
//     while((read_op & 0x80) ==0x80)
//     {
// //         asm("nop");
// //         asm("nop");
//         hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7B);
//     }
//     return 1;
// }

#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE) 
extern hal_result_t hal_brdcfg_eth__get_errors_info(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype, hal_eth_phy_errorsinfo_t *result)
{

    uint8_t errorcode;
    uint8_t buff_write = 0x1c; // read MIB counters selected
    uint8_t buff_read[4] = {0};
    
//    char str[150];

    
    if(phynum>2)
    {
        return(hal_res_NOK_nodata);
    }
    
    if(NULL == result)
    {
        return(hal_res_NOK_nullpointer);
    }
   
    errorcode = hal_brdcfg_eth__get_errors_code(phynum, errortype);
    
    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x79);

    buff_write = errorcode;
    hal_brdcfg_switch__reg_write_byI2C(&buff_write, 0x7A);
    
//     hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x79);
//     snprintf(str, sizeof(str), "in HAL: read_op addr 0x79 =0x%x", read_op);
//     hal_trace_puts(str);
//     
//     hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7a);
//     snprintf(str, sizeof(str), "in HAL: read_op addr 0x7a =0x%x errorcode=%0x", read_op, errorcode);
//     hal_trace_puts(str);
//     
//     hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7B);
//     snprintf(str, sizeof(str), "in HAL: read_op addr 0x7b =0x%x", read_op);
//     hal_trace_puts(str);
    
    hal_brdcfg_switch__reg_read_byI2C(&buff_read[3], 0x80); //reads bits from 24-31
    //has_finished_to_read();
    
//      hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7B);
//     snprintf(str, sizeof(str), "in HAL: read_op addr 0x7b =0x%x", read_op);
//     hal_trace_puts(str);
    
    hal_brdcfg_switch__reg_read_byI2C(&buff_read[2], 0x81); //reads bits from 23-16
    //has_finished_to_read();
//      hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7B);
//     snprintf(str, sizeof(str), "in HAL: read_op addr 0x7b =0x%x", read_op);
//     hal_trace_puts(str);
    
    hal_brdcfg_switch__reg_read_byI2C(&buff_read[1], 0x82); //reads bits from 15-8
    //has_finished_to_read();
//      hal_brdcfg_switch__reg_read_byI2C(&read_op, 0x7B);
//     snprintf(str, sizeof(str), "in HAL: read_op addr 0x7b =0x%x", read_op);
//     hal_trace_puts(str);
    
    hal_brdcfg_switch__reg_read_byI2C(&buff_read[0], 0x83); //reads bits from 7-0
    //has_finished_to_read();
    
    result->value = (((uint32_t)(buff_read[3]&0x3F))<< 24) | (((uint32_t)buff_read[2])<<16) | (((uint32_t)buff_read[1])<<8) | buff_read[0];
    //result->value = aux3 <<24 | aux2<<16 | aux1<<8 | aux0;

    
    if((buff_read[3]&0x80) == 0x80)
    {
        result->counteroverflow = 1;
    }
    else
    {
        result->counteroverflow = 0;
    }
    
    if((buff_read[3]&0x40) == 0x40)  
    {
        result->validvalue = 1;
    }
    else
    {
        result->validvalue = 0;
    }
    return(hal_res_OK);
}  
#else
extern hal_result_t hal_brdcfg_eth__get_errors_info(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype, hal_eth_phy_errorsinfo_t *result)
{   
    result->value = 0;
    result->counteroverflow = 0;
    result->validvalue = 1;
    
    return(hal_res_OK);
} 
#endif

#endif//HAL_USE_PERIPH_ETH



#ifdef HAL_USE_PERIPH_GPIO
    extern const hal_gpio_hid_brdcfg_t hal_brdcfg_gpio__theconfig =
    {
        .supported_mask_byport    =
        {   // ok, i enable every pin of every port. 
            0xffff,     // port a
            0xffff,     // port b
            0xffff,     // port c
            0xffff,     // port d
            0xffff,     // port e
            0xffff,     // port f
            0xffff,     // port g
            0xffff,     // port h
            0xffff      // port i
        }
    };
#endif//HAL_USE_PERIPH_GPIO


#ifdef  HAL_USE_PERIPH_I2C
    extern const hal_i2c_hid_brdcfg_t hal_brdcfg_i2c__theconfig =
    {
        .supported_mask         = (1 << hal_i2c1) | (0 << hal_i2c2) | (1 << hal_i2c3),
        .gpiomap                =
        {
            {   // hal_i2c1 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin8 }, 
                    .af32   = GPIO_AF_I2C1
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin9 }, 
                    .af32   = GPIO_AF_I2C1
                },   
            },   

            {   // hal_i2c2 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },   
            }, 
            
            {   // hal_i2c3 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin8 }, 
                    .af32   = GPIO_AF_I2C3 
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin9 }, 
                    .af32   = GPIO_AF_I2C3 
                },   
            }     
        }       
    };
    
#endif//HAL_USE_PERIPH_I2C


#if     defined(HAL_USE_PERIPH_SPI) | defined(HAL_USE_PERIPH_SPI_MINIMAL)

    extern const hal_spi_hid_brdcfg_t hal_brdcfg_spi__theconfig =
    {
        .supported_mask         = (0 << hal_spi1) | (1 << hal_spi2) | (1 << hal_spi3),
        .gpiomap                =
        {   
            {   // hal_spi1
                .sck                =
                {
                    .gpio   = { .port = hal_gpio_portNONE,     .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                     
                },
                .miso               =
                {
                    .gpio   = { .port = hal_gpio_portNONE,     .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                     
                },
                .mosi               =
                {
                    .gpio   = { .port = hal_gpio_portNONE,     .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                } 
            },
            {   // hal_spi2
                .sck                =
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin10 }, 
                    .af32   = GPIO_AF_SPI2                     
                },
                .miso               =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin2 }, 
                    .af32   = GPIO_AF_SPI2                     
                },
                .mosi               =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin3 }, 
                    .af32   = GPIO_AF_SPI2                     
                } 
            },
            {   // hal_spi3
                .sck                =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin10 }, 
                    .af32   = GPIO_AF_SPI3                     
                },
                .miso               =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin11 }, 
                    .af32   = GPIO_AF_SPI3                     
                },
                .mosi               =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin12 }, 
                    .af32   = GPIO_AF_SPI3                     
                } 
            }                   
        }
           
    };
    
#endif//HAL_USE_PERIPH_SPI/HAL_USE_PERIPH_SPI_MINIMAL
    
      
#ifdef  HAL_USE_PERIPH_TIMER
    extern const hal_timer_hid_brdcfg_t hal_brdcfg_timer__theconfig =
    {
        .supported_mask     = (1 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) | (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7) | (1 << hal_timer8)
    };       
#endif//HAL_USE_PERIPH_TIMER

    
#ifdef  HAL_USE_PERIPH_TRACE
    extern const hal_trace_hid_brdcfg_t hal_brdcfg_trace__theconfig =
    {
        .supported          = hal_true
    };       
#endif//HAL_USE_PERIPH_TRACE
  

#ifdef  HAL_USE_PERIPH_UNIQUEID
    extern const hal_uniqueid_hid_brdcfg_t hal_brdcfg_uniqueid__theconfig =
    {
        .supported_mask     = (1 << hal_uniqueid_id64bit) | (0 << hal_uniqueid_macaddr),
        .macoui             = 0
    };       
#endif//HAL_USE_PERIPH_UNIQUEID
    

#ifdef  HAL_USE_PERIPH_WATCHDOG
    extern const hal_watchdog_hid_brdcfg_t hal_brdcfg_watchdog__theconfig =
    {
        .supported_mask     = (1 << hal_watchdog1_normal) | (1 << hal_watchdog2_window)
    };       
#endif//HAL_USE_PERIPH_WATCHDOG


// -- devices

#ifdef  HAL_USE_DEVICE_ACCELEROMETER   
    extern const hal_device_accelerometer_hid_brdcfg_t hal_brdcfg_device_accelerometer__theconfig =
    {
        .supported_mask         = 0x1,
        .devcfg         =
        {
            {   // hal_accelerometer1
                .chipif             =      
                {   // use the lis3dh chip by st
                    .init       = (hal_res_fp_voidp_t)hal_chip_st_lis3dh_init,
                    .initpar    = NULL,
                    .read       = hal_chip_st_lis3dh_accel_get  
                }                    
            }
         }
    };       
#endif//HAL_USE_DEVICE_ACCELEROMETER


#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER
    
    static void s_hal_brdcfg_ems4rd_vaux_5v0_init(void);
    static void s_hal_brdcfg_ems4rd_vaux_5v0_on(void);
    static void s_hal_brdcfg_ems4rd_vaux_5v0_off(void);

    static hal_result_t s_hal_device_cantransceiver_simple_init(hal_cantransceiver_t id, void* initpar);
    static hal_result_t s_hal_device_cantransceiver_simple_enable(hal_cantransceiver_t id);
    static hal_result_t s_hal_device_cantransceiver_simple_disable(hal_cantransceiver_t id);
    
    static const hal_gpio_maP_t s_hal_device_cantransceiver__gpiomapof_enable[hal_cantransceivers_number] = 
    {
        {   // hal_cantransceiver1 
            .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin14 }, 
            .af32   = hal_NA32
        },        
        {   // hal_cantransceiver2 
            .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin15 }, 
            .af32   = hal_NA32
        }        
    };
    
    static const hal_gpio_cfg_t s_hal_device_cantransceiver__gpiocfgof_enable[hal_cantransceivers_number] =
    {
        { .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL },
        { .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL }
    };
       
    static hal_result_t s_hal_device_cantransceiver_simple_init(hal_cantransceiver_t id, void* initpar)
    {
        initpar = initpar;
        const hal_gpio_maP_t* map = &s_hal_device_cantransceiver__gpiomapof_enable[(uint8_t)id];        
        const hal_gpio_cfg_t* cfg = &s_hal_device_cantransceiver__gpiocfgof_enable[(uint8_t)id];   
        hal_gpio_init(map->gpio, cfg);        
        s_hal_device_cantransceiver_simple_disable(id);     
        s_hal_brdcfg_ems4rd_vaux_5v0_init();
        s_hal_brdcfg_ems4rd_vaux_5v0_on();
        return(hal_res_OK);
    }

    static hal_result_t s_hal_device_cantransceiver_simple_enable(hal_cantransceiver_t id)
    {  
        const hal_gpio_maP_t* map = &s_hal_device_cantransceiver__gpiomapof_enable[(uint8_t)id];          
        hal_gpio_setval(map->gpio, hal_gpio_valLOW);
        return(hal_res_OK);
    }

    static hal_result_t s_hal_device_cantransceiver_simple_disable(hal_cantransceiver_t id)
    {   
        const hal_gpio_maP_t* map = &s_hal_device_cantransceiver__gpiomapof_enable[(uint8_t)id];         
        hal_gpio_setval(map->gpio, hal_gpio_valHIGH);
        return(hal_res_OK);
    }
    
    extern const hal_device_cantransceiver_hid_brdcfg_t hal_brdcfg_device_cantransceiver__theconfig =
    {
        .supported_mask     =  (1 << hal_cantransceiver1) | (1 << hal_cantransceiver2),
        .devcfg             =
        {            
            .chipif             =
            {
                .init               = s_hal_device_cantransceiver_simple_init,
                .initpar            = NULL,
                .enable             = s_hal_device_cantransceiver_simple_enable,
                .disable            = s_hal_device_cantransceiver_simple_disable
            }
        }   
    };    
#endif//HAL_USE_DEVICE_CANTRANSCEIVER     
    




#ifdef  HAL_USE_DEVICE_EEPROM
    
    #include "hl_chip_xx_eeprom.h"
    
    const hl_chip_xx_eeprom_cfg_t hl_eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portD, .pin = hl_gpio_pin10 }           
    };        
    static hal_result_t s_chip_xx_eeprom_init(const hl_chip_xx_eeprom_cfg_t * cfg)
    {
        return((hal_result_t)hl_chip_xx_eeprom_init(cfg));
    }
    static hal_result_t s_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
    {
        return((hal_result_t)hl_chip_xx_eeprom_write(address, size, buffer, writtenbytes));        
    }
    static hal_result_t s_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
    {
        return((hal_result_t)hl_chip_xx_eeprom_read(address, size, buffer, writtenbytes));        
    }   
    
    static const hal_gpio_val_t hal_brdcfg_device_eeprom__wp_gpio_enableval    = hal_gpio_valHIGH;      // write protection is on value high                          
    static const hal_gpio_val_t hal_brdcfg_device_eeprom__wp_gpio_disableval   = hal_gpio_valLOW;       // write protection is on value high
    
    static const hal_gpio_maP_t hal_brdcfg_device_eeprom__gpiomapof_wp  = 
    {    
        .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin10 }, 
        .af32   = hal_NA32
    };
    static const hal_gpio_cfg_t hal_brdcfg_device_eeprom__gpiocfgof_wp =
    {
        .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_high,       .altcfg = NULL
    };

    

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_init(void)
    {
        const hal_gpio_maP_t* map = &hal_brdcfg_device_eeprom__gpiomapof_wp;
        return(hal_gpio_init(map->gpio, &hal_brdcfg_device_eeprom__gpiocfgof_wp));
    }

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_enable(void)
    {
        const hal_gpio_maP_t* map = &hal_brdcfg_device_eeprom__gpiomapof_wp;
        return(hal_gpio_setval(map->gpio, hal_brdcfg_device_eeprom__wp_gpio_enableval));
    }

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_disable(void)
    {
        const hal_gpio_maP_t* map = &hal_brdcfg_device_eeprom__gpiomapof_wp;
        return(hal_gpio_setval(map->gpio, hal_brdcfg_device_eeprom__wp_gpio_disableval));
    }

    
    // used by hal_brdcfg_device_eeprom__theconfig
//     static const hal_chip_xx_eeprom_cfg_t s_initpar_eeprom_i2c_01 =
//     {
//         .chipcfg    =
//         {
//             .chip           = hal_chip_xx_eeprom_chip_atmel_at24c512b,
//             .i2cid          = hal_i2c1,
//             .hwaddra2a1a0   = 0 // a0 = a1 = a2 = 0
//         },
//         .wrprcfg    =
//         {
//             .wrpr_init              = s_hal_brdcfg_device_eeprom__wp_init,
//             .wrpr_deinit            = NULL,
//             .wrpr_enable            = s_hal_brdcfg_device_eeprom__wp_enable,
//             .wrpr_disable           = s_hal_brdcfg_device_eeprom__wp_disable            
//         }
//     };    

    extern const hal_device_eeprom_hid_brdcfg_t hal_brdcfg_device_eeprom__theconfig =
    {
        .supported_mask     =  (0 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01) | (0 << hal_eeprom_i2c_02),
        .devcfg             =
        {            
            {   // hal_eeprom1_emulatedflash 
                .flashemul  =
                {
                    .baseaddress    = 0x08000000,
                    .totalsize      = 0
                }
            },
            {   // hal_device_eeprom2_i2c_01
                .i2cbased   =
                {
                    .baseaddress    = 0,
                    .totalsize      = 8*1024,
                    .i2cid          = hal_i2c1,
                    .chipif         =
                    {   // use the chip xx_eeprom
                        .init           = (hal_res_fp_voidp_t)s_chip_xx_eeprom_init, //(hal_res_fp_voidp_t)hal_chip_xx_eeprom_init,
                        .initpar        = (void*)&hl_eepromcfg, //(void*)&s_initpar_eeprom_i2c_01,
                        .read           = s_chip_xx_eeprom_read, //hal_chip_xx_eeprom_read,
                        .write          = s_chip_xx_eeprom_write, //hal_chip_xx_eeprom_write                               
                    }                    
                }           
            },
            {   // hal_device_eeprom3_i2c_02
                .i2cbased   =
                {
                    .baseaddress    = 0,
                    .totalsize      = 0,
                    .i2cid          = hal_i2c1,
                    .chipif         =
                    {   // dont use any chip
                        .init           = NULL,
                        .initpar        = NULL,
                        .read           = NULL,
                        .write          = NULL                               
                    }                    
                }           
            }                
        }
    
    };    
#endif//HAL_USE_DEVICE_EEPROM 


#ifdef HAL_USE_DEVICE_ENCODER

    extern const hal_device_encoder_hid_brdcfg_t hal_brdcfg_device_encoder__theconfig =
    {
        .supported_mask             = (1 << hal_encoder1) | (1 << hal_encoder2) | (1 << hal_encoder3) | (1 << hal_encoder4) | (1 << hal_encoder5) | (1 << hal_encoder6), 
        .spimap                     =
        {
            {   // hal_encoder1:    P6 on ems4rd
                .spiid = hal_spi2,  .muxid = hal_mux2,  .muxsel = hal_mux_selA                
            },
            {   // hal_encoder2:    P8 on ems4rd
                .spiid = hal_spi2,  .muxid = hal_mux2,  .muxsel = hal_mux_selB                
            },            
            {   // hal_encoder3:    P10 on ems4rd
                .spiid = hal_spi2,  .muxid = hal_mux2,  .muxsel = hal_mux_selC                
            },
            {   // hal_encoder4:    P7 on ems4rd
                .spiid = hal_spi3,  .muxid = hal_mux3,  .muxsel = hal_mux_selA                
            },
            {   // hal_encoder5:    P9 on ems4rd
                .spiid = hal_spi3,  .muxid = hal_mux3,  .muxsel = hal_mux_selB                
            },            
            {   // hal_encoder6:    P11 on ems4rd
                .spiid = hal_spi3,  .muxid = hal_mux3,  .muxsel = hal_mux_selC                
            }                        
        }

    };
#endif//HAL_USE_DEVICE_ENCODER  
    
#ifdef HAL_USE_DEVICE_ENCODER_FAKE
    #include "hal_encoder.h"
    #include "hal_device_encoder_hid.h"
    extern const hal_device_encoder_hid_brdcfg_t hal_brdcfg_device_encoder__theconfig =
    {
        .supported_mask             = (0 << hal_encoder1) | (0 << hal_encoder2) | (0 << hal_encoder3) | (0 << hal_encoder4) | (0 << hal_encoder5) | (0 << hal_encoder6)
    };
#endif//HAL_USE_DEVICE_ENCODER_FAKE      
  

#ifdef  HAL_USE_DEVICE_ETHTRANSCEIVER
    
#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)      
        
    #include "hl_chip_micrel_ks8893.h"
    
    static hl_result_t s_hal_brdcfg_ks8893__extclock_init(void)
    {
        return(hl_res_OK);    
    }    
    
    extern const hl_chip_micrel_ks8893_cfg_t ks8893_config = 
    {
        .i2cid              = hl_i2c3,
        .resetpin           = { .port = hl_gpio_portB,     .pin = hl_gpio_pin2 },
        .resetval           = hl_gpio_valRESET,
        .extclockinit       = s_hal_brdcfg_ks8893__extclock_init,
        .targetphymode      = hl_ethtrans_phymode_auto //hl_ethtrans_phymode_fullduplex10mbps // hl_ethtrans_phymode_halfduplex10mbps //hl_ethtrans_phymode_auto        
    };    
    
    static hal_result_t s_hal_micrel_init(void* param)
    {
        // i2c3 must be initted.
        hal_i2c_init((hal_i2c_t)ks8893_config.i2cid, NULL);        
        hl_result_t r = hl_chip_micrel_ks8893_init((const hl_chip_micrel_ks8893_cfg_t*)param);
        return((hal_result_t)r);
    }    
       
    //extern const hal_chip_micrel_ks8893_cfg_t s_micrel_ks8893_cfg;
    extern const hal_device_ethtransceiver_hid_brdcfg_t hal_brdcfg_device_ethtransceiver__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode      = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif             =
            {   // use the micrel 
                .init           = s_hal_micrel_init,
                .initpar        = (void*)&ks8893_config,
                .config         = (hal_device_ethtransceiver_hid_fn_config_t)hl_chip_micrel_ks8893_configure, 
                .getphymode     = (hal_device_ethtransceiver_hid_fn_getphymode_t)hl_chip_micrel_ks8893_mii_getphymode                
            }
         }
    }; 

#else

    static hal_result_t s_hal_switch_init(void* param)
    {
        return(hal_res_OK);
    }   
    static hal_result_t s_hal_switch_configure(hal_eth_phymode_t* usedphymode)
    {
        *usedphymode = hal_eth_phymode_fullduplex100mbps;
        return(hal_res_OK);
    }   
    static hal_result_t s_hal_switch_getphymode(hal_eth_phymode_t* usedphymode)
    {
        *usedphymode = hal_eth_phymode_fullduplex100mbps;
        return(hal_res_OK);
    }    
    extern const hal_device_ethtransceiver_hid_brdcfg_t hal_brdcfg_device_ethtransceiver__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode      = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif             =
            {   // use the micrel 
                .init           = s_hal_switch_init,
                .initpar        = NULL,
                .config         = s_hal_switch_configure, 
                .getphymode     = s_hal_switch_getphymode                
            }
         }
    }; 
    
#endif
    
#endif//0

    
#ifdef  HAL_USE_DEVICE_GYROSCOPE
    extern const hal_device_gyroscope_hid_brdcfg_t hal_brdcfg_device_gyroscope__theconfig =
    {
        .supported_mask         = (1 << hal_gyroscope1),
        .devcfg         =
        {
            {   // hal_gyroscope_port1
                .chipif             =      
                {   // use the l3g4200d chip by st
                    .init       = (hal_res_fp_voidp_t)hal_chip_st_l3g4200d_init,
                    .initpar    = NULL,
                    .read       = hal_chip_st_l3g4200d_angrate_get
                }                    
            }
         }
    };          
#endif//HAL_USE_DEVICE_GYROSCOPE
 

#ifdef HAL_USE_DEVICE_LED
    extern const hal_device_led_hid_brdcfg_t hal_brdcfg_device_led__theconfig =
    {
        .supported_mask             = (1 << hal_led0) | (1 << hal_led1) | (1 << hal_led2) | (1 << hal_led3) | (1 << hal_led4) | (1 << hal_led5), // = 0x3F: only first 6 leds ...
        .value_on                   = hal_gpio_valLOW,
        .value_off                  = hal_gpio_valHIGH,
        .gpiomap                    =
        {
            {   // hal_led0 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin8 }, 
                    .af32   = hal_NA32
                }
            },
            {   // hal_led1 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin10}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led2 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin12}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led3 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin15}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led4 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin11}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led5 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin7}, 
                    .af32   = hal_NA32
                }
            },                      
            {   // hal_led6 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE}, 
                    .af32   = hal_NA32
                }
            },              
            {   // hal_led7 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE}, 
                    .af32   = hal_NA32
                }
            }  
        }
    };
#endif//HAL_USE_DEVICE_LED
    
    
#ifdef HAL_USE_DEVICE_MUX
#warning --> configure mux
    extern const hal_device_mux_hid_brdcfg_t hal_brdcfg_device_mux__theconfig =
    {
        .supported_mask             = (0 << hal_mux1) | (1 << hal_mux2) | (1 << hal_mux3) | (0 << hal_mux4), 
        .gpiomap                    =
        {
            {   // hal_mux1   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                }
            }, 
            {   // hal_mux2   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin2 }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin3 }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin4 }, 
                    .af32   = hal_NA32                    
                }
            },
            {   // hal_mux3   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin5 }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin6 }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin7 }, 
                    .af32   = hal_NA32                    
                }
            }, 
            {   // hal_mux4   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                }
            }            
        }
    };
    
//     extern const hal_device_mux_hid_brdcfg_t hal_brdcfg_device_mux__theconfig =
//     {
//         .supported_mask             = (0 << hal_mux1) | (0 << hal_mux2) | (0 << hal_mux3) | (0 << hal_mux4), 
//         .gpio_sel0                  =
//         {
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             },             
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }, 
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }, 
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }            
//         },
//         .gpio_sel1                  =
//          {
//            {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             },             
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }, 
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }, 
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }                 
//         }, 
//         .gpio_enable                =
//         {
//            {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             },             
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }, 
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }, 
//             {    
//                 .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
//                 .config = { .dir = hal_gpio_dirNONE,    .speed = hal_gpio_speed_default,    .altcfg = NULL }
//             }                 
//         },    
//     };    
#endif//HAL_USE_DEVICE_MUX      
    

#ifdef HAL_USE_DEVICE_SWITCH

#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)   

#include "hl_chip_micrel_ks8893.h"
    
//extern const hl_chip_micrel_ks8893_cfg_t ems4rd_micrel_ks8893_config;  // defined in ... hl somewhere

extern const hl_chip_micrel_ks8893_cfg_t ks8893_config;
    
static hal_result_t chip_micrel_ks8893_init(void *cfg)
{ 
    // i2c3 must be initted.
    hal_i2c_init((hal_i2c_t)ks8893_config.i2cid, NULL);    
    return((hal_result_t)hl_chip_micrel_ks8893_init((const hl_chip_micrel_ks8893_cfg_t *)cfg));
}

static hal_result_t chip_micrel_ks8893_configure(hal_eth_phymode_t* usedphymode)
{
    return((hal_result_t)hl_chip_micrel_ks8893_configure((hl_ethtrans_phymode_t*)usedphymode));
}


    extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode  = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif         =
            {
                .init       = chip_micrel_ks8893_init,
                .initpar    = (void*)&ks8893_config,
                .config     = chip_micrel_ks8893_configure,          
            }
        }
    };
    
    

#include "hl_i2c.h"

// i dont verify constants. i just map to i2c1 and address of micrel 0xbe

#define HL_I2CID                    hl_i2c3
#define HL_MICREL_DEVADR            0xBE  

extern void hal_brdcfg_switch__reg_write_byI2C(uint8_t* pBuffer, uint16_t WriteAddr)
{
    hl_i2c_regaddr_t regaddr = { .numofbytes = 1, .bytes.one = 0 };
    regaddr.bytes.one = (uint8_t) (WriteAddr & 0xff);
    hl_i2c_write(HL_I2CID, HL_MICREL_DEVADR, regaddr, pBuffer, 1);    
}

extern void hal_brdcfg_switch__reg_read_byI2C(uint8_t* pBuffer, uint16_t ReadAddr)
{
    hl_i2c_regaddr_t regaddr = { .numofbytes = 1, .bytes.one = 0 };
    regaddr.bytes.one = (uint8_t) (ReadAddr & 0xff);
    hl_i2c_read(HL_I2CID, HL_MICREL_DEVADR, regaddr, pBuffer, 1);     
}

    
    
#else

    static hal_result_t s_switch_init(void* p)
    {
        return(hal_res_OK);
    }
    static hal_result_t s_switch_configure(hal_eth_phymode_t* usedphymode)
    {
        *usedphymode = hal_eth_phymode_fullduplex100mbps;
        return(hal_res_OK);
    }    
    extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode  = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif         =
            {
                .init       = s_switch_init,
                .initpar    = NULL,
                .config     = s_switch_configure,          
            }
        }
    };
    
#endif    

#endif//HAL_USE_DEVICE_SWITCH
        
    
#ifdef  HAL_USE_DEVICE_TERMOMETER 
    extern const hal_device_termometer_hid_brdcfg_t hal_brdcfg_device_termometer__theconfig =
    {
        .supported_mask         = 0x1,
        .devcfg         =
        {
            {   // hal_termometer_port1
                .chipif             =      
                {   // use the l3g4200d chip by st
                    .init       = (hal_res_fp_voidp_t)hal_chip_st_l3g4200d_init,
                    .initpar    = NULL,
                    .read       = hal_chip_st_l3g4200d_temp_get
                }                    
            }
         }
    };           
#endif//HAL_USE_DEVICE_TERMOMETER
// -- chips
      
   
    

// -- extbrds
// none
    


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_brdcfg_ems4rd_vaux_5v0_init(void);
static void s_hal_brdcfg_ems4rd_vaux_5v0_on(void);

extern hal_result_t hal_brdcfg__start(void)
{
    s_hal_brdcfg_ems4rd_vaux_5v0_init();
    s_hal_brdcfg_ems4rd_vaux_5v0_on();

    return(hal_res_OK);    
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions
// --------------------------------------------------------------------------------------------------------------------





#ifdef  HAL_USE_DEVICE_SWITCH

static void s_hal_brdcfg_device_switch__mco2_init(void)
{
    // --- mco2: PC9
    
    #define SLR100MHZ               0x000C0000
    #define SLR050MHZ               0x00080000
    // configure pc9 af, push pull, 50mhz
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    // clocks port port c
    RCC->AHB1ENR    |= 0x00000004;

    GPIOC->MODER    &= ~0x000C0000;              // reset pc9
    GPIOC->MODER    |=  0x00080000;              // alternate function
    GPIOC->OTYPER   &= ~0x00000200;              // output push-pull (reset state)  
    GPIOC->OSPEEDR  |=  SLR100MHZ;               // slew rate as 100MHz pin (0x000C0000) or 50mhz (0x00080000)
    GPIOC->PUPDR    &= ~0x000C0000;              // no pull up, pull down

    GPIOC->AFR[1]   &= ~0x000000F0;
    GPIOC->AFR[1]   |=  0x00000000;              // AF0 (system) 
}



static void s_hal_brdcfg_device_switch__mco_initialise(void)
{    
#define HAL_SWITCH_USE_HSE   
    
    s_hal_brdcfg_device_switch__mco2_init();

#if     defined(HAL_SWITCH_USE_PLLI2S)    
    RCC_PLLI2SCmd(DISABLE);
    RCC_PLLI2SConfig(200, 2); // 50mhz: 1mhz*200/2 = 100.  then we divide by 2 again
    RCC_PLLI2SCmd(ENABLE);
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
    // connect mco2 with plli2s divided by 2
    RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);
#elif   defined(HAL_SWITCH_USE_HSE)
 
    // connect mco2 with hse divided by 1
    RCC_MCO2Config(RCC_MCO2Source_HSE, RCC_MCO2Div_1);
#endif    
    
    
    //hal_eth_hid_rmii_refclock_init();
}


static hal_result_t s_hal_brdcfg_device_switch__extclock_init(void)
{
    s_hal_brdcfg_device_switch__mco_initialise();  
    return(hal_res_OK);    
}

#endif//HAL_USE_DEVICE_SWITCH


//#ifdef HAL_USE_DEVICE_SWITCH  
//
// #if 0
// static void hal_brdcfg_device_switch__mco1_init(void)
// {
//     // configure pa8 af, push pull, 50mhz
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);  
//     // clocks port port a
//     RCC->AHB1ENR    |= 0x00000001;

//     GPIOA->MODER   &= ~0x00030000;              // reset pa8
//     GPIOA->MODER   |=  0x00020000;              // alternate function
//     GPIOA->OTYPER  &= ~0x00000100;              // output push-pull (reset state)  
//     GPIOA->OSPEEDR |=  0x00030000;              // slew rate as 100MHz pin (3) or 50mhz (2)
//     GPIOA->PUPDR   &= ~0x00030000;              // no pull up, pull down

//     GPIOA->AFR[1]  &= ~0x0000000F;
//     GPIOA->AFR[1]  |=  0x00000000;              // AF0 (system) 

// }
// #endif

// static void s_hal_brdcfg_device_switch__mco2_init(void)
// {
//     #define SLR100MHZ               0x000C0000
//     #define SLR050MHZ               0x00080000
//     // configure pc9 af, push pull, 50mhz
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);  
//     // clocks port port c
//     RCC->AHB1ENR    |= 0x00000004;

//     GPIOC->MODER    &= ~0x000C0000;              // reset pc9
//     GPIOC->MODER    |=  0x00080000;              // alternate function
//     GPIOC->OTYPER   &= ~0x00000200;              // output push-pull (reset state)  
//     GPIOC->OSPEEDR  |=  SLR100MHZ;               // slew rate as 100MHz pin (0x000C0000) or 50mhz (0x00080000)
//     GPIOC->PUPDR    &= ~0x000C0000;              // no pull up, pull down

//     GPIOC->AFR[1]   &= ~0x000000F0;
//     GPIOC->AFR[1]   |=  0x00000000;              // AF0 (system) 
// }



// static void s_hal_brdcfg_device_switch__mco_initialise(void)
// {    
// #define HAL_SWITCH_USE_HSE   
//     
//     s_hal_brdcfg_device_switch__mco2_init();

// #if     defined(HAL_SWITCH_USE_PLLI2S)    
//     RCC_PLLI2SCmd(DISABLE);
//     RCC_PLLI2SConfig(200, 2); // 50mhz: 1mhz*200/2 = 100.  then we divide by 2 again
//     RCC_PLLI2SCmd(ENABLE);
//     // wait until it is ready
//     while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
//     // connect mco2 with plli2s divided by 2
//     RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);
// #elif   defined(HAL_SWITCH_USE_HSE)
//  
//     // connect mco2 with hse divided by 1
//     RCC_MCO2Config(RCC_MCO2Source_HSE, RCC_MCO2Div_1);
// #endif    
//     
//     
//     //hal_eth_hid_rmii_refclock_init();
// }

// static void hal_brdcfg_device_switch__rmii_init(void)
// {
//     hal_eth_hid_rmii_prepare();
//     hal_eth_hid_rmii_rx_init(); 
//     hal_eth_hid_rmii_tx_init(); 
//     hal_eth_hid_rmii_refclock_init();  
//     hal_eth_hid_smi_init();    
// }

// #ifdef  HAL_USE_CHIP_MICREL_KS8893
// static hal_result_t s_hal_brdcfg_device_switch__extclock_init(void)
// {
//     // --- mco2: PC9
//     s_hal_brdcfg_device_switch__mco_initialise();  
//     return(hal_res_OK);    
// }
// #endif//#ifdef  HAL_USE_CHIP_MICREL_KS8893

// extern hal_result_t hal_brdcfg_device_switch__chip_initialise(void* p)
// {
//     hal_result_t res = hal_res_OK;
//     
//     const hal_gpio_cfg_t hal_brdcfg_device_switch__gpio_reset = 
//     { 
//         .port = hal_gpio_portB, 
//         .pin = hal_gpio_pin2,   
//         .dir = hal_gpio_dirOUT, 
//         .speed = hal_gpio_speed_low 
//     }; 
//     
//     // --- mco2: PC9
//     s_hal_brdcfg_device_switch__mco_initialise();  
//     
//      // --- reset pin: PB2
//     hal_gpio_configure(hal_brdcfg_device_switch__gpio_reset, NULL);
//     hal_gpio_setval(hal_brdcfg_device_switch__gpio_reset.port, hal_brdcfg_device_switch__gpio_reset.pin, hal_gpio_valLOW);
//     hal_sys_delay(10*1000);
//     hal_gpio_setval(hal_brdcfg_device_switch__gpio_reset.port, hal_brdcfg_device_switch__gpio_reset.pin, hal_gpio_valHIGH);
//     hal_sys_delay(100);
//   
//     // proviamo anche questo ....
// //    hal_eth_hid_rmii_refclock_init();
//     
//     // --- mco: PC9
// //    hal_brdcfg_device_switch__mco_initialise();  
//     
//     #warning --> test on it
//     //hal_brdcfg_device_switch__rmii_init();
//     
// //    // --- i2c for communication 
// //    hal_i2c_init(hal_i2c_port1, NULL); // use default configuration    


//     res = hal_chip_micrel_ks8893_init(NULL);
//    
//     return(res);
// }

// static void s_hal_brdcfg_device_switch__mii_phymode_get(hal_eth_phymode_t* phymode)
// {
//     #define MIIHALFD    (1 << 6)
//     #define MII10MBS    (1 << 4)
//     uint8_t mux = 0;
//     uint8_t speed = 0;
//     uint8_t read = 0xFF; 
//     hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
//     regadr.bytes.one = 0x06;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &read, 1);
//     if( (MIIHALFD & read) == MIIHALFD)
//     {
//         mux = 0;
//     }
//     else
//     {
//         mux = 1;
//     }
//     if( (MII10MBS & read) == MII10MBS)
//     {
//         speed = 0;
//     }
//     else
//     {
//         speed = 1;
//     }
//     if((0==mux)&&(0==speed))        *phymode = hal_eth_phymode_halfduplex10mbps;
//     else if((0==mux)&&(1==speed))   *phymode = hal_eth_phymode_halfduplex100mbps; 
//     else if((1==mux)&&(0==speed))   *phymode = hal_eth_phymode_fullduplex10mbps; 
//     else if((1==mux)&&(1==speed))   *phymode = hal_eth_phymode_fullduplex100mbps;          
//     
// }
 
// extern hal_result_t hal_brdcfg_device_switch__configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t* phymode)
// {
//     hal_result_t res = hal_res_NOK_generic;
//     
//     hal_eth_phymode_t phymode2use = targetphymode; //hal_eth_phymode_fullduplex100mbps;
//     const uint8_t fd100 = 0x60;
//     const uint8_t fd010 = 0x20;
//     uint8_t buff_write = 0x60; // FORCE FULL DUPLEX AND 100T
//     uint8_t buff_read = 0xFF; 
//     volatile uint32_t i = 1;
//     hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
//     
//     if(NULL != phymode)
//     {
//         phymode2use = *phymode;
//     }


//     regadr.bytes.one = 0x01;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     if((buff_read&0x01))
//     {   // already initted. to be initted again must pass through a reset
//         s_hal_brdcfg_device_switch__mii_phymode_get(phymode);
//         return(hal_res_OK);
//     }
//     
//     
//     if(hal_eth_phymode_none == phymode2use)
//     {
//         if(NULL != phymode)
//         {
//             *phymode = hal_eth_phymode_none;    
//         }   
//         return(hal_res_OK);        
//     }


//     

// #if 0    
//     // configure mii
//     regadr.bytes.one = 0x06;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     buff_write  = buff_read;
//     buff_write |= 0x10; 
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
// #endif

//     // 1. configure  switch's ports 1 and 2
//     switch(phymode2use)
//     {
//         case hal_eth_phymode_auto:                  buff_write = 0x9F; break;
//         case hal_eth_phymode_halfduplex10mbps:      buff_write = 0x00; break;
//         case hal_eth_phymode_halfduplex100mbps:     buff_write = 0x40; break;
//         case hal_eth_phymode_fullduplex10mbps:      buff_write = 0x20; break;
//         case hal_eth_phymode_fullduplex100mbps:     buff_write = 0x60; break;
//         default:                                    buff_write = 0x00; break;
//     }    
//     
//     
//     // port 1
//     regadr.bytes.one = 0x1C;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//      
//     // port 2 
//     regadr.bytes.one = 0x2C;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);   
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);

//     // 2. start the switch
//     buff_write = 0x1;  
//     regadr.bytes.one = 0x01;    
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     

//     // 3. read back to verify
//     regadr.bytes.one = 0x01;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     if(!(buff_read&0x01))
//     {
//         hal_base_on_fatalerror(hal_fatalerror_runtimefault, "hal_brdcfg_device_switch__configure(): SWITCH not configured");
//     }
//     
//     s_hal_brdcfg_device_switch__mii_phymode_get(phymode);
//     
//     return(hal_res_OK);
//     
// }
//
//#endif//HAL_USE_DEVICE_SWITCH



#ifdef HAL_USE_ETH

// extern hal_bool_t hal_brdcfg_eth__phy_initialise(void)
// {
//     #warning --> in here we just init the switch ... put in reset mode, exit from reset, do the mco, init the i2c if not initted, etc. 
//     if(hal_false == hal_device_switch_initted_is())
//     {
//         hal_device_switch_init(NULL);
//     }
//     
//     return(hal_true);
// }

// extern void hal_brdcfg_eth__phy_configure(hal_eth_phymode_t *phymode)
// {
//     hal_eth_phymode_t target = hal_brdcfg_eth__phymode;
//  
// //    if((hal_eth_mux_auto == target.mux) || (hal_eth_mux_none == target.mux) || (hal_eth_speed_auto == target.speed) || (hal_eth_speed_none == target.speed))
//     if(hal_eth_phymode_none == target)
//     {
//         if(NULL != phymode)
//         {
//             *phymode  = hal_eth_phymode_none;
//         }   
//         return;        
//     }    
//     
//     hal_device_switch_start(&target);
//     
//     if(NULL != phymode)
//     {
//         *phymode  = target;     
//     }
// }

#endif//HAL_USE_ETH



static const hal_gpio_maP_t s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap = 
{
    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin1 }, 
    .af32   = hal_NA32      
};

static const hal_gpio_cfg_t s_hal_brdcfg_ems4rd_vaux_5v0_gpiocfg =
{
    .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL
};
    
static void s_hal_brdcfg_ems4rd_vaux_5v0_init(void)
{
    static uint8_t initted = 0;
    if(1 == initted)
    {
        return;
    }
    hal_gpio_init(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, &s_hal_brdcfg_ems4rd_vaux_5v0_gpiocfg); 
    initted = 1;
    s_hal_brdcfg_ems4rd_vaux_5v0_on();    
}

static void s_hal_brdcfg_ems4rd_vaux_5v0_on(void)
{
    hal_gpio_setval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, hal_gpio_valHIGH);   
}

static void s_hal_brdcfg_ems4rd_vaux_5v0_off(void)
{
    hal_gpio_setval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, hal_gpio_valLOW);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


