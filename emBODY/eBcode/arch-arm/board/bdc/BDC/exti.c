 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
 *  Ext int configuration                            *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#include "stm32f10x.h"
#include "exti.h"
#include "define.h"
#include "globals.h"

// ----------------------------------------------------------------------------
// External Interrupt initialisation
// ----------------------------------------------------------------------------
void EXTI_Init(void) {

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;           // enable clock for AFIO    

    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI1;       // clear used pin           
    AFIO->EXTICR[0] |=  AFIO_EXTICR1_EXTI1_PB;    // set PB.1 to use          
    EXTI->IMR       |=  EXTI_IMR_MR1;             // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR1;             // no event                 
    EXTI->RTSR      &= ~EXTI_RTSR_TR1;            // no rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR1;            // set falling edge trigger 
    NVIC_EnableIRQ(EXTI1_IRQn);                   // enable IRQ EXTI 1        

    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI2;       // clear used pin           
    AFIO->EXTICR[0] |=  AFIO_EXTICR1_EXTI2_PB;    // set PB.2 to use          
    EXTI->IMR       |=  EXTI_IMR_MR2;             // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR2;             // no event                 
    EXTI->RTSR      |=  EXTI_RTSR_TR2;            // set rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR2;            // set falling edge trigger 
    NVIC_EnableIRQ(EXTI2_IRQn);                   // enable IRQ EXTI 2        
}


// ----------------------------------------------------------------------------
// EXTI Interrupt Handler for HSM_nFLT button connected to GPIOB.1
// ----------------------------------------------------------------------------
void EXTI1_IRQHandler(void) {
	
	EXTI->PR |= (1 << 1);							// clear pending interrupt
		
	if((GPIOB->IDR & HSM_nFLT)==0)  {V48motor_FLT_ON;       HSM_F=1;}
	//else                            {V48motor_FLT_OFF;}      //HSM_F=0;}
}


// ----------------------------------------------------------------------------
// EXTI Interrupt Handler for HSM_nPG button connected to GPIOB.2
// ----------------------------------------------------------------------------
void EXTI2_IRQHandler(void) {

    EXTI->PR |= (1 << 2);							// clear pending interrupt
        
    if((GPIOB->IDR & HSM_nPG)==0)   {V48motor_PG_ON;        HSM_PG=1;}
    else                            {V48motor_PG_OFF;       HSM_PG=0;}
}

