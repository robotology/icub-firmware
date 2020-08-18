 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Interrupt configuration                          *
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

    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;       // clear used pin           
    AFIO->EXTICR[0] |=  AFIO_EXTICR1_EXTI0_PB;    // set PB.0 to use          
    EXTI->IMR       |=  EXTI_IMR_MR0;             // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR0;             // no event                 
    EXTI->RTSR      &= ~EXTI_RTSR_TR0;            // no rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR0;            // set falling edge trigger 

    NVIC_EnableIRQ(EXTI0_IRQn);                   // enable IRQ EXTI 0        

	
    AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI4;      // clear used pin           
    AFIO->EXTICR[1] |=  AFIO_EXTICR2_EXTI4_PC;   // set PC.4 to use         
    EXTI->IMR       |=  EXTI_IMR_MR4;            // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR4;            // unmask event             
    EXTI->RTSR      |=  EXTI_RTSR_TR4;           // set rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR4;           // set falling edge trigger 
    	
    NVIC_EnableIRQ(EXTI4_IRQn);
    
    AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI7;      // clear used pin           
    AFIO->EXTICR[1] |=  AFIO_EXTICR2_EXTI7_PC;   // set PC.7 to use         
    EXTI->IMR       |=  EXTI_IMR_MR7;            // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR7;            // unmask event             
    EXTI->RTSR      |=  EXTI_RTSR_TR7;           // set rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR7;           // set falling edge trigger 

    NVIC_EnableIRQ(EXTI9_5_IRQn);
    
    
    AFIO->EXTICR[2] &= ~AFIO_EXTICR3_EXTI11;      // clear used pin           
    AFIO->EXTICR[2] |=  AFIO_EXTICR3_EXTI11_PC;   // set PC.11 to use         
    EXTI->IMR       |=  EXTI_IMR_MR11;            // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR11;            // unmask event             
    EXTI->RTSR      |=  EXTI_RTSR_TR11;           // set rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR11;           // set falling edge trigger 
    
    
    AFIO->EXTICR[3] &= ~AFIO_EXTICR4_EXTI12;      // clear used pin           
    AFIO->EXTICR[3] |=  AFIO_EXTICR4_EXTI12_PC;   // set PC.12 to use         
    EXTI->IMR       |=  EXTI_IMR_MR12;            // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR12;            // unmask event             
    EXTI->RTSR      |=  EXTI_RTSR_TR12;           // set rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR12;           // set falling edge trigger 
    
    
    AFIO->EXTICR[3] &= ~AFIO_EXTICR4_EXTI13;      // clear used pin           
    AFIO->EXTICR[3] |=  AFIO_EXTICR4_EXTI13_PC;   // set PC.13 to use         
    EXTI->IMR       |=  EXTI_IMR_MR13;            // unmask interrupt         
    EXTI->EMR       &= ~EXTI_EMR_MR13;            // unmask event             
    EXTI->RTSR      |=  EXTI_RTSR_TR13;           // set rising edge trigger   
    EXTI->FTSR      |=  EXTI_FTSR_TR13;           // set falling edge trigger 

    NVIC_EnableIRQ(EXTI15_10_IRQn);               // enable IRQ EXTI 10..15   
    
}


// ----------------------------------------------------------------------------
// EXTI0 Interrupt Handler for SWT button connected to GPIOB.0
// ----------------------------------------------------------------------------
void EXTI0_IRQHandler(void) {
	//static int count = 0;
	
	EXTI->PR |= (1UL << 0);                       // clear pending interrupt  
	
	if((GPIOB->IDR & SWT) == 0){					// check for debounce
	//if(count < DEBOUNCE_CNT)	{count++;}
	//else{
		//count=0;
    //if ((ledExti ^=1) == 0) {CAN_LED_OFF;}
	//	else										{CAN_LED_ON;}
	//}
	}

}


// ----------------------------------------------------------------------------
// EXTI4 Interrupt Handler for button PB1 connected to GPIOC.4
// ----------------------------------------------------------------------------
void EXTI4_IRQHandler(void) {
	EXTI->PR |= (1UL << 4);                 // clear pending interrupt
			
	if(!(GPIOC->IDR & PB1)){                // check for debounce
		TIM3->CR1  |= TIM_CR1_CEN;          // timer3 enable
	}
	else if((GPIOC->IDR & PB1)){
		TIM3->CR1  &= ~TIM_CR1_CEN;         // timer3 disable
		filter_PB1=0;
	}
}


// ----------------------------------------------------------------------------
// EXTI9..5 Interrupt Handler for button PB2 connected to GPIOC.7
// ----------------------------------------------------------------------------
void EXTI9_5_IRQHandler(void) {
	EXTI->PR |= (1UL << 7);                 // clear pending interrupt
	
	if(!(GPIOC->IDR & PB2)){                // check for debounce
		if(!V12board_F){                    // check for no fault on V12board
			TIM4->CR1  |= TIM_CR1_CEN;      // timer4 enable
		}
	}
	else if((GPIOC->IDR & PB2)){
		TIM4->CR1  &= ~TIM_CR1_CEN;         // timer4 disable
		filter_PB2=0;
	}
	
}


// ----------------------------------------------------------------------------
// EXTI15..10 Interrupt Handler for button PB2 connected to GPIOC.13
// ----------------------------------------------------------------------------
void EXTI15_10_IRQHandler(void) {
	
    if( (EXTI->PR) & (1UL << 11) ){
        EXTI->PR |= (1UL << 11);                // clear pending interrupt
        
        int_fault = 1;
        
        //if((GPIOC->IDR & FAULT) == 0 )     {fault=0;}
        //else                               {fault=1;}
    }
    
    if( (EXTI->PR) & (1UL << 12) ){
        EXTI->PR |= (1UL << 12);                // clear pending interrupt
        
        int_fault = 1;
        //if((GPIOC->IDR & FAULT_GPIO) == 0 )     {LED_BT_OFF;}
        //else                                    {LED_BT_ON;}
    }
    
    if( (EXTI->PR) & (1UL << 13) ){
        EXTI->PR |= (1UL << 13);                // clear pending interrupt
        int_fault = 1;
        //FAULT_REM_OFF;
        //if(!(GPIOC->IDR & JACK) == 0 )      {FAULT_REM_ON;}
        //else                                {FAULT_REM_OFF;}
    }
	
}

