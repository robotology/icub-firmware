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

#include "hl_cfg_plus_modules.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "hl_core.h"

#include "hl_sys.h"

#include "hl_i2c.h"

#include "hl_chip_xx_eeprom.h"
#include "hl_chip_st_l3g4200d.h"
#include "hl_chip_st_lis3x.h"


#include "board.h"  
#include "systickservices.h" 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void myonsystick(void);

static void test_i2c(void);

static void test_eeprom(void);

static void test_timer(uint16_t microsecs);

static void test_mems_init(void);
static void test_mems_get(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    
    SystemCoreClockUpdate();
 
    board_led_init();
    
    test_i2c();
    
    test_eeprom();
    
    test_mems_init();
    
    
    systickserv_start_systick(1000, myonsystick);
    
    
    test_timer(10*1000);    // 10 millisecs
    
    for(;;)
    {
        board_led_on(board_led_0);
        
        //systickserv_wait_for(500*1000);
        hl_sys_delay(500*1000);
        
        board_led_off(board_led_0);
        
        //systickserv_wait_for(500*1000);
        hl_sys_delay(500*1000);
        
        test_mems_get();
    }

    
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



static void myonsystick(void)
{

}


static void test_i2c(void)
{
    const hl_i2c_devaddr_t devaddr = 0xA0;
    
    hl_result_t r = hl_res_NOK_generic;
    
    // init i2c1
    
    r = hl_i2c_init(hl_i2c1, NULL);
    
    // ping eeprom at address 0xA0    
    
    r = hl_i2c_ping(hl_i2c1, devaddr);
    
    r = r;    
    
}


static void test_eeprom(void)
{
    
#if     defined(HL_USE_CHIP_XX_EEPROM)    

#if     defined(HL_USE_BRD_EMS001)
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portB, .pin = hl_gpio_pin10 }           
    };
#else    
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_st_m24c64, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valUNDEF,
        .wp_gpio        = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE }           
    };
#endif    
    hl_result_t r = hl_res_NOK_generic;
    
    r = hl_chip_xx_eeprom_init(&eepromcfg);
    
    uint32_t address = 258;
    uint32_t size = 14;
    static uint8_t buffer[32] = {0};
    static uint8_t target[10] = {0, 0, 2, 3, 4, 5, 6, 7, 8, 9};
    uint32_t readbytes = 0;
    uint32_t writtenbytes = 0;
    
    r = hl_chip_xx_eeprom_read(address, size, buffer, &readbytes);
    
    r = hl_chip_xx_eeprom_write(address, 10, target, &writtenbytes);
    
    r = hl_chip_xx_eeprom_read(address, size, buffer, &readbytes);
    

    r = r;    
#endif    
}

static void test_mems_init(void)
{
    hl_result_t r = hl_res_NOK_generic;
    
#if     defined(HL_USE_CHIP_ST_L3G4200D)   
    r = hl_chip_st_l3g4200d_init(NULL);
    r = r;
#endif    


#if     defined(HL_USE_CHIP_ST_LIS3X)   
    r = hl_chip_st_lis3x_init(NULL);
    r = r;
#endif 
    
}



static void test_mems_get(void)
{
    char str[128];
    hl_result_t r = hl_res_NOK_generic;
    int32_t x, y, z;
    
#if     defined(HL_USE_CHIP_ST_L3G4200D)    
    r = hl_chip_st_l3g4200d_angrate_get(&x, &y, &z);
    r = r;
    snprintf(str, sizeof(str), "l3g4200d: r=%d, m=(%d, %d, %d)", r, x, y, z);
    hl_sys_itm_puts(str);    
#endif    


#if     defined(HL_USE_CHIP_ST_LIS3X)     
    r = hl_chip_st_lis3x_accel_get(&x, &y, &z);
    r = r;
    snprintf(str, sizeof(str), "lis3x:   r=%d, m=(%d, %d, %d)", r, x, y, z);
    hl_sys_itm_puts(str);  
#endif     
    
}


static void test_timer(uint16_t microsecs)
{
    // this code is based onto 72Mhz
    //uint32_t div = SystemCoreClock/1000000 - 1;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

//     /* TIM7 clock enable */
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

//     /* Enable the TIM7 global Interrupt */
//     NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/

    // set the counter to expiry at multiples of one microsec.
    // the clock is divided by 1, we work at 72MHz, thus use a prescaler of 71 to be at 1 MHz (1 microsec).
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
    TIM_TimeBaseStructure.TIM_Period = 1*microsecs;
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // div
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
//     TIM_TimeBaseInit( TIM7, &TIM_TimeBaseStructure );
//     // important: stop the timer 7 in debug mode
//     DBGMCU_Config(DBGMCU_TIM7_STOP, ENABLE);
//     TIM_Cmd( TIM7, ENABLE );

//     // enable isr
//     TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );

    TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );
    // important: stop the timer 4 in debug mode
    DBGMCU_Config(DBGMCU_TIM4_STOP, ENABLE);
    TIM_Cmd( TIM4, ENABLE );

    // enable isr
    TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );
}


//void TIM7_IRQHandler(void)
void TIM4_IRQHandler(void)
{
    static volatile uint32_t delta = 0;
    static volatile uint32_t prev = 0;
    static volatile uint32_t val = 0;

    //if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        //TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        delta = systickserv_numofticks - prev;
        prev = systickserv_numofticks;

        delta = delta;
        
        //val = (0 == val) ? (1) : (0);
        val ++;
        
        if(50 == val)
        {
            board_led_on(board_led_3);
        }
        else if(100 == val)
        {
            board_led_off(board_led_3);
            val = 0;
        }
    }

}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






