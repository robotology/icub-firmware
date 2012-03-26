
/** @file       main_cmsis_ems001.c
    @brief      This file implements a very basic test of ems001 board
    @author     marco.accame@iit.it
    @date       02/15/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "cmsis_stm32.h"

// extern void SysTick_Handler(void);


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// 1000 is 1ms
#define SYSTICKDIVIDER  1000



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_dosomething(void);
static void s_delay(uint32_t dlyTicks);
static void init_leds(void);
static void blink_leds(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks;                         


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void) 
{
        

    if(SysTick_Config(SystemCoreClock / SYSTICKDIVIDER)) 
    { 
        while (1);                                  
    }

 	init_leds();


    while(1) 
    {
        s_delay(3000);  
        s_dosomething();
		blink_leds();
        s_delay(7000);                                
    }
  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void SysTick_Handler(void) 
{
    msTicks++;                        
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_delay(uint32_t dlyTicks) 
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}


static void s_dosomething(void)
{
    static uint32_t counter = 0;
    
    counter ++;
    
    if(100 == counter)
    {
        counter = 1;
    }
}

static void blink_leds(void)
{
	static uint8_t status = 0; //leds off

	if(!status) //if leds off
	{
		//then switch on (its value must be 0) 
		GPIOE->BRR |= 0x00000100; //led red
		GPIOE->BRR |= 0x00000400; //led green 
		GPIOE->BRR |= 0x00001000; //led yellow
		GPIOE->BRR |= 0x00008000; //led orange

		/* If you want switch on all leds together, you must use only this instruction:
  	 		GPIOE->BRR |= 0x00009500;
  		*/

		status = 1;
	}
	else
	{
		//then switch off (its value must be 1)
		GPIOE->BSRR |= 0x00000100; //led red
		GPIOE->BSRR |= 0x00000400; //led green 
		GPIOE->BSRR |= 0x00001000; //led yellow
		GPIOE->BSRR |= 0x00008000; //led orange

		/* If you want switch off all leds together, you must use this instruction only:
  	 		GPIOE->BSRR |= 0x00009500;
  		*/
		
		status = 0;	
	}
}

static void init_leds(void)
{
  //led red
  RCC->APB2ENR |= 0x00000040;	  //enable clock on port E. All leds use pin on port E.


  //configure pins for leds as	General purpose output push pull - 2 MHz
  
  GPIOE->CRH  |= 0x00000002;	  //led red		(pin E8)

  GPIOE->CRH  |= 0x00000200;	  //led green	(pin E10)

  GPIOE->CRH  |= 0x00020000;	  //led yellow	(pin E12)

  GPIOE->CRH  |= 0x20000000;	  //led orange	(pin E15)

  /* If you want set all leds together, you must use this instruction only :
  	 GPIOE->CRH  |= 0x20020202;
  */
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




