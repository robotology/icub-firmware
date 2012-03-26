/**************************************************************************//**
 * @file     main.c
 * @brief    CMSIS Cortex-M3 Blinky example
 *           Blink a LED using CM3 SysTick
 * @version  V1.03
 * @date     24. September 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#if 1
    #include "cmsis_stm32f1.h"
#else
    #include "stm32f10x.h"
#endif


#include "eventviewer.h"



volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
    evEntityId_t prev = eventviewer_switch_to(ev_ID_systick);
    msTicks++;                        /* increment counter necessary in Delay() */
    eventviewer_switch_to(prev);
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*------------------------------------------------------------------------------
  configer LED pins
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Config(void) {

  RCC->APB2ENR |=  1 <<  6;                     /* Enable GPIOE clock          */
  GPIOE->CRH    = 0x33333333;                   /* Configure the GPIO for LEDs */
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_On (uint32_t led) {

  GPIOE->BSRR = (led);                          /* Turn On  LED */
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Off (uint32_t led) {

  GPIOE->BRR  = (led);                          /* Turn Off LED */
}


typedef struct
{
    uint8_t     ctrl        : 2;
    uint8_t     ffus        : 2;
    uint8_t     rqst_conf   : 1;
    uint8_t     rqst_time   : 1;
    uint8_t     plus_sign   : 1;
    uint8_t     plus_time   : 1;
} bitfieldtest_t;

typedef struct 
{
    bitfieldtest_t      ctrl;
    uint8_t             ropc;
    uint16_t            nvid;            
} head_t;

static bitfieldtest_t bt = {3, 1, 1, 1, 0, 0};

static head_t head = { {3, 1, 1, 1, 0, 0}, 0xF1, 0x1001 };

uint8_t aaa = 3;

uint8_t bbb = 0;


typedef __packed struct
{
	uint8_t			doit;
	uint8_t			doalsothat;
	uint32_t		withperiodinmicrosecs;
} global_configuration_t;

typedef __packed struct
{
	uint8_t			acquisitionenabled;
	uint32_t			acquisitionperiod;
} input_configuration_t;

typedef __packed struct
{
	uint8_t			applicationenabled;
	uint32_t			applicationmode;
} output_configuration_t;

typedef __packed struct
{
	uint8_t			inputvalue;
	uint64_t			acquistiontime;
input_configuration_t	inputconfiguration;
} input_t;

typedef __packed struct
{
	uint32_t			outputvalue;
	uint64_t			applicationtime;
    output_configuration_t	outputconfiguration;
} output_t;

typedef __packed struct
{
	global_configuration_t	globalconfiguration;
	input_t			input;
	output_t			output;
} tree_structured_nvs;

static tree_structured_nvs 	treenvs = 
{ 
    {0x01, 0x00, 0x000003e8}, 						// globalconfiguration
    {0x00, 0x0000000000010000, {0x01, 0x000003e8}},			// input
    {0x00000010, 0x0000000000020000, {0x01, 0x02}}			// output
};

static uint8_t treenv_content [] = 
{ 
0x01, 0x00, 0xe8, 0x03, 0x00, 0x00, // globalconfiguration
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe8, 0x03, 0x00, 0x00, // input
0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02 // output
};

void userdef1(void)
{

}

void userdef2(void)
{

}

void userdef3(void)
{

}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

  evEntityId_t prev;

  eventviewer_init();
  eventviewer_load(ev_ID_systick, SysTick_Handler);  
  eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
  eventviewer_load(ev_ID_first_usrdef+2, userdef2);
  eventviewer_load(ev_ID_first_usrdef+3, userdef3);
    

  if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 1 msec interrupts  */
    while (1);                                  /* Capture error */
  }

  if(treenvs.globalconfiguration.doit == 1)
  {
     treenvs.globalconfiguration.doalsothat = 1;
  }

  bbb = *((uint8_t*)(&bt));

  head.nvid = head.nvid;

  if(3 == head.ctrl.ctrl)
  {
    head.ropc = 0xf2;
  }

  if(aaa == (bbb & 0x03))
  {
    head.ropc = 0xf2;
  }

  if(aaa == bt.ctrl)
  {
      bt.ctrl = 2;
      bt.ffus = 3;
      bt.ctrl = bt.ctrl;
  }

  prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
  
  LED_Config();  
  
  prev= eventviewer_switch_to(prev);              
 
  while(1) {
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
    LED_On (0x100);                             /* Turn on the LED. */
    Delay (300);                                /* delay  300 Msec */
    prev = eventviewer_switch_to(ev_ID_first_usrdef+3);
    LED_Off (0x100);                            /* Turn off the LED. */
    Delay (700);                                /* delay  700 Msec */
  }
  
}

