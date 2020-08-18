 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Timers configuration                             *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#include "stm32f10x.h"
#include "define.h"
#include "globals.h"

// ----------------------------------------------------------------------------
// initialize TIM1	-> 100us
// ----------------------------------------------------------------------------
void TIM1_Init (void) {

    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;         // enable clock for TIM1
	
    TIM1->PSC   = 3000-1;                       // set prescaler   = 10kHz
    TIM1->ARR   = 2-1;                          // set auto-reload
    TIM1->RCR   = 1-1;                          // set repetition counter

    TIM1->DIER = TIM_DIER_UIE;                  // Update Interrupt enable
    NVIC_EnableIRQ(TIM1_UP_IRQn);               // TIM1   Interrupt enable

    TIM1->CR1  |= TIM_CR1_CEN;                  // timer enable
}


// ----------------------------------------------------------------------------
// initialize TIM2	-> 10ms
// ----------------------------------------------------------------------------
void TIM2_Init (void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;         // enable clock for TIM2

    TIM2->PSC   = (3000)-1;                     // set prescaler   = 10kHz
    TIM2->ARR   = 200-1;                        // set auto-reload (es: 2000 = 100 ms)
    TIM2->RCR   = 1-1;                          // set repetition counter

    TIM2->DIER = TIM_DIER_UIE;                  // Update Interrupt enable
    NVIC_EnableIRQ(TIM2_IRQn);                  // TIM2   Interrupt enable

    TIM2->CR1  |= TIM_CR1_CEN;                  // timer enable
}


// ----------------------------------------------------------------------------
// initialize TIM3	-> 10ms
// ----------------------------------------------------------------------------
void TIM3_Init (void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;         // enable clock for TIM3

    TIM3->PSC   = (3000)-1;                     // set prescaler   = 10kHz
    TIM3->ARR   = 200-1;                        // set auto-reload (es: 20000 = 1s)
    TIM3->RCR   = 1-1;                          // set repetition counter

    TIM3->DIER = TIM_DIER_UIE;                  // Update Interrupt enable
    NVIC_EnableIRQ(TIM3_IRQn);                  // TIM3   Interrupt enable

    //TIM3->CR1  |= TIM_CR1_CEN;                // timer enable
}


// ----------------------------------------------------------------------------
// initialize TIM4	-> 10ms
// ----------------------------------------------------------------------------
void TIM4_Init (void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;         // enable clock for TIM4

    TIM4->PSC   = (3000)-1;                     // set prescaler   = 10kHz
    TIM4->ARR   = 200-1;                        // set auto-reload (es: 20000 = 1s)
    TIM4->RCR   = 1-1;                          // set repetition counter

    TIM4->DIER = TIM_DIER_UIE;                  // Update Interrupt enable
    NVIC_EnableIRQ(TIM4_IRQn);                  // TIM4   Interrupt enable

    //TIM4->CR1  |= TIM_CR1_CEN;                // timer enable
}



// ----------------------------------------------------------------------------
// Timer1 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM1_UP_IRQHandler (void) {

    if (TIM1->SR & (TIM_SR_UIF)) {              // UIF set?
        timer_100us=1;
            
    TIM1->SR &= ~(TIM_SR_UIF);                // clear UIF flag
    }
}


// ----------------------------------------------------------------------------
// Timer2 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM2_IRQHandler (void) {

    if (TIM2->SR & (TIM_SR_UIF)) {              // UIF set?

        
    TIM2->SR &= ~(TIM_SR_UIF);                // clear UIF flag
    }
}


// ----------------------------------------------------------------------------
// Timer3 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM3_IRQHandler (void) {

  if (TIM3->SR & (TIM_SR_UIF)) {                        // UIF set?
		
		if(!(GPIOC->IDR & PB1)){                        // se PB1 è premuto
			if(filter_PB1<timer_debounce){              //  
				filter_PB1++;                           //
			}
			else if(filter_PB1==timer_debounce){        // 
				PB1_debounce=1;                         // comando valido
				filter_PB1++;
			}
			else if(filter_PB1<timer_restart && V12board_F==1){     // 
				filter_PB1++;
			}
			else if(V12board_F==1){
				TIM3->CR1  &= ~TIM_CR1_CEN;             // timer3 disable
				PB1_restart=1;                          // comando valido per il restart dopo il fault
				PB1_debounce=0;
			}
		}
		
		TIM3->SR &= ~(TIM_SR_UIF);                      // clear UIF flag
  }
}


// ----------------------------------------------------------------------------
// Timer4 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM4_IRQHandler (void) {

  if (TIM4->SR & (TIM_SR_UIF)) {                        // UIF set?
		
		if(!(GPIOC->IDR & PB2)){                        // se PB2 è premuto
			if(filter_PB2<timer_debounce){              //  
				filter_PB2++;                           //
			}
			else if(filter_PB2==timer_debounce){        // 
				PB2_debounce=1;                         // comando valido
				filter_PB2++;
			}
			else if(filter_PB2<timer_restart && V12motor_F==1){		// 
				filter_PB2++;
			}
			else if(V12motor_F==1){
				TIM4->CR1  &= ~TIM_CR1_CEN;             // timer4 disable
				PB2_restart=1;                          // comando valido per il restart dopo il fault
				PB2_debounce=0;
			}
		}
				
    TIM4->SR &= ~(TIM_SR_UIF);                          // clear UIF flag
  }
}

