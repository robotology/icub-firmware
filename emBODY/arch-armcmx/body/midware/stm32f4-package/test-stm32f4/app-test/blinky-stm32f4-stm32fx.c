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


#include "brd_mcbstm32x.h"  
#include "systickservices.h" 

#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "stm32f4.h"


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


static void test_timer(uint16_t microsecs);

static void test_flash(void);

static void test_eeprom(void);


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
 
    brd_mcbstm32x_led_init();
    
    test_eeprom();
    
    //for(;;);
    
    test_flash();
    
    
    systickserv_start_systick(1000, myonsystick);
    
    
    test_timer(10*1000);    // 10 microsecs
    
    for(;;)
    {
        brd_mcbstm32x_led_on(brd_mcbstm32x_led_0);
        
        systickserv_wait_for(500*1000);
        
        brd_mcbstm32x_led_off(brd_mcbstm32x_led_0);
        
        systickserv_wait_for(500*1000);
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



static void test_timer(uint16_t microsecs)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* TIM7 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);


    /* Enable the TIM7 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/

    // set the counter to expiry at multiples of one microsec.
    // the clock is divided by 1, we work at 72MHz, thus use a prescaler of 71 to be at 1 MHz (1 microsec).
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
    TIM_TimeBaseStructure.TIM_Period = 1*microsecs;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM7, &TIM_TimeBaseStructure );
    // important: stop the timer 7 in debug mode
    DBGMCU_Config(DBGMCU_TIM7_STOP, ENABLE);
    TIM_Cmd( TIM7, ENABLE );

    // enable isr
    TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );
}


void TIM7_IRQHandler(void)
{
    static volatile uint32_t delta = 0;
    static volatile uint32_t prev = 0;
    static volatile uint8_t val = 0;

    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

        delta = systickserv_numofticks - prev;
        prev = systickserv_numofticks;

        delta = delta;
        
        //val = (0 == val) ? (1) : (0);
        val ++;
        
        if(20 == val)
        {
            brd_mcbstm32x_led_on(brd_mcbstm32x_led_7);
        }
        else if(40 == val)
        {
            brd_mcbstm32x_led_off(brd_mcbstm32x_led_7);
            val = 0;
        }
    }

}


static void test_flash(void)
{
    // erase first sector of 64k at address 0x08010000
    // write some data on it. read it back
    
    uint32_t address = 0x08010000; //+64*1024;
    uint8_t dataread[64] = {0};
    uint8_t datawrite[64] = {0x1};
    
    int8_t result = -1;

    // unlock
    FLASH_Unlock();


    // read first
    {
        const void *flashaddr = (const void*)address;
        memcpy(dataread, flashaddr, sizeof(dataread));
    }
    
    // delete
    {
        FLASH_Status status = FLASH_COMPLETE;
        uint32_t pageindex = FLASH_Sector_4;
        
        
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
        
        //FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

        status = FLASH_EraseSector(pageindex, VoltageRange_3);

        result = ((FLASH_COMPLETE==status)?(0):(-1));     
        result = result;
    }
    
    // write
    {
        uint32_t addr = address;
        uint32_t size = sizeof(datawrite);
        uint32_t *data = (uint32_t*)datawrite;
        
        volatile FLASH_Status status = FLASH_COMPLETE;
        uint32_t EndAddr = 0;

       

        EndAddr = addr + size; 

        while((addr < EndAddr) && (status == FLASH_COMPLETE))
        {
            // cannot write on non-0xffffffff flash
            if(0xFFFFFFFF != *((volatile uint32_t*)addr))
            {
                status = FLASH_ERROR_OPERATION;
                break;
            }

            // no need to write if already 0xffffffff
            if(0xFFFFFFFF == *data)
            {
                status = FLASH_COMPLETE;
            }
            else
            {
                status = FLASH_ProgramWord(addr, *data);
            }

            addr = addr + 4;
            data++;
        }
        
        result = ((FLASH_COMPLETE == status) ? (0) :(-1));
        result = result;
    }


}




static void test_eeprom(void)
{


}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






