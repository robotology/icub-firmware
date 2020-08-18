 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
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
void GPIO_Init (void) {
	
	RCC->APB2ENR |=   1 << 0;                       // enable clock for AFIO
	AFIO->MAPR =  0;                                // reset AFIO
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_NOJNTRST;       // Full SWJ (JTAG-DP + SW-DP) but without JNTRST
	
	RCC->APB2ENR |=	(RCC_APB2ENR_IOPAEN |           // Enable GPIOA clock
                     RCC_APB2ENR_IOPBEN );          // Enable GPIOB clock
  
	// GPIOA ---------------------------------------------------
	GPIOA->CRL	 &= ~0xFFFFFFFF;	// RESET PA7..PA0
	GPIOA->CRH	 &= ~0x00000FFF;	// RESET PA10, PA9, PA8
	
	GPIOA->CRL   |= 0x00000000;		// PA0		INPUT       HSM_IMON
	GPIOA->CRL   |= 0x00000000;		// PA1		INPUT       IMON_V12motor
	GPIOA->CRL   |= 0x00000000;		// PA2		INPUT       IMON_V12board
	GPIOA->CRL   |= 0x00000000;		// PA3		INPUT       VBATTMON
	GPIOA->CRL   |= 0x00000000;		// PA4		INPUT       V12motor_MON
	GPIOA->CRL   |= 0x00000000;		// PA5		INPUT       V12board_MON
	GPIOA->CRL	 |= 0x03000000;		// PA6		OUTPUT		V48motor_PG		(DL1)
	GPIOA->CRL	 |= 0x30000000;		// PA7		OUTPUT		V48motor_FLT	(DL2)
	
	GPIOA->CRH	 |= 0x00000003;		// PA8		OUTPUT		LED1					(DL3)
	GPIOA->CRH	 |= 0x00000030;		// PA9		OUTPUT		EN_V12motor
	GPIOA->CRH	 |= 0x00000300;		// PA10		OUTPUT		EN_V12board
	
	GPIOA->ODR	 |= 0x000000C0;		// Imposta le uscite PA6, PA7 a "1"
	
	// GPIOB ---------------------------------------------------
	GPIOB->CRL   &= ~0x000F0FFF;	// RESET PB4, PB2, PB1, PB0
	
	GPIOB->CRL   |= 0x00000003;		// PB0		OUTPUT		HSM_EN
	GPIOB->CRL   |= 0x00000080;		// PB1		INPUT       HSM_nFLT
	GPIOB->CRL   |= 0x00000800;		// PB2		INPUT       HSM_nPG
	GPIOB->CRL   |= 0x00030000;		// PB4		OUTPUT		DL7 CAN_LED
	//GPIOB->ODR   |= 0x00000010;		// Imposta l'uscita PB4 a "1"
	GPIOB->ODR   |= 0x00000006;		// Pull-up on PB2 and PB1
}

