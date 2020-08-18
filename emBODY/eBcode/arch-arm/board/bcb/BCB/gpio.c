 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  GPIO configuration                               *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#include "stm32f10x.h"
#include "gpio.h"

// ----------------------------------------------------------------------------
// initialize GPIO Pins
// ----------------------------------------------------------------------------
void BCB_GPIO_Init (void) {
	
	RCC->APB2ENR |=   1 << 0;                       // enable clock for AFIO
	AFIO->MAPR =  0;                                // reset AFIO
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_NOJNTRST;       // Full SWJ (JTAG-DP + SW-DP) but without JNTRST
	
	RCC->APB2ENR |=	(RCC_APB2ENR_IOPAEN |           /* Enable GPIOA clock */
                     RCC_APB2ENR_IOPBEN |           /* Enable GPIOB clock */
                     RCC_APB2ENR_IOPCEN  );         /* Enable GPIOC clock */
  
	// GPIOA ---------------------------------------------------
	GPIOA->CRH	 &= ~0x0000000F;	// RESET PA8
	GPIOA->CRH	 |= 0x00000003;		// PA8		OUTPUT		RESET_BT
	
	//GPIOA->ODR   |= 0x00000100;		// PA8 = "1"
	
	// GPIOB ---------------------------------------------------
	GPIOB->CRL   &= ~0x00000F0F;	// RESET PB2, PB0
	GPIOB->CRH   &= ~0xFFF000FF;	// RESET PB15, PB14, PB13, PB9, PB8
	
	GPIOB->CRL   |= 0x00000008;		// PB0 		INPUT       SWT
	GPIOB->CRL   |= 0x00000300;		// PB2 		OUTPUT		CAN_LED	(DL3)
	
	GPIOB->CRH   |= 0x00000003;		// PB8		OUTPUT		FAN1
	GPIOB->CRH   |= 0x00000030;		// PB9		OUTPUT		FAN2
	GPIOB->CRH   |= 0x00300000;		// PB13		OUTPUT		LED_BT
	GPIOB->CRH   |= 0x03000000;		// PB14		OUTPUT		LED1_ETH
	GPIOB->CRH   |= 0x30000000;		// PB15		OUTPUT		LED2_ETH
	
	GPIOB->ODR   |= 0x00002001;		// Pull-up on PB0	
	
	// GPIOC ---------------------------------------------------
	GPIOC->CRL	 &= ~0xFFFF000F;	// RESET	PC7, PC6, PC5, PC4, PC0
	GPIOC->CRH	 &= ~0x00FFFFFF;	// RESET	PC9, PC8
	
	GPIOC->CRL   |=	0x00000008;		// PC0		INPUT       VBATTMON
	GPIOC->CRL   |= 0x00080000;		// PC4		INPUT       CPU             (PB1)
	GPIOC->CRL   |= 0x00300000;		// PC5		OUTPUT      CPU_Green       (PB1 led green)
	GPIOC->CRL   |= 0x03000000;		// PC6		OUTPUT      CPU_Red         (PB1 led red)
	GPIOC->CRL   |= 0x80000000;		// PC7		INPUT       MOTORS          (PB2)
	
	GPIOC->CRH   |= 0x00000003;		// PC8		OUTPUT		MOTORS_Green	(PB2 led green)
	GPIOC->CRH   |= 0x00000030;		// PC9		OUTPUT		MOTORS_RED		(PB2 led red)
	GPIOC->CRH   |= 0x00000300;		// PC10     OUTPUT      Remote fault
	GPIOC->CRH   |= 0x00008000;		// PC11     INPUT       iCub fault
	GPIOC->CRH   |= 0x00080000;		// PC12     INPUT       External fault  (red pushbutton)
	GPIOC->CRH   |= 0x00800000;		// PC13     INPUT       Jack 3.5mm
	
    GPIOC->ODR   |= 0x00003890;		// Pull-up on PC13 PC12 PC11 PC7 PC4
}

