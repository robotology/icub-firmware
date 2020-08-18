 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
 *  Timers configuration                             *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#include "stm32f10x.h"
#include "define.h"
#include "globals.h"

// ----------------------------------------------------------------------------
// initialize TIM1: 1ms
// ----------------------------------------------------------------------------
void TIM1_Init (void) {

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;		// enable clock for TIM1
	
  TIM1->PSC   = 3000-1;									// set prescaler   = 10KHz
  TIM1->ARR   = 2-1;										// set auto-reload
  TIM1->RCR   = 1-1;										// set repetition counter

  TIM1->DIER = TIM_DIER_UIE;						// Update Interrupt enable
  NVIC_EnableIRQ(TIM1_UP_IRQn);					// TIM1   Interrupt enable

  TIM1->CR1  |= TIM_CR1_CEN;						// timer enable
}


// ----------------------------------------------------------------------------
// initialize TIM2
// ----------------------------------------------------------------------------
void TIM2_Init (void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		// enable clock for TIM2

  TIM2->PSC   = (3000)-1;								// set prescaler   = 10kHz
  TIM2->ARR   = 2000-1;									// set auto-reload (es: 2000 = 100 ms)
  TIM2->RCR   = 1-1;										// set repetition counter

  TIM2->DIER = TIM_DIER_UIE;						// Update Interrupt enable
  NVIC_EnableIRQ(TIM2_IRQn);						// TIM1   Interrupt enable

  TIM2->CR1  |= TIM_CR1_CEN;						// timer enable
}


// ----------------------------------------------------------------------------
// initialize TIM3	-> 10ms
// ----------------------------------------------------------------------------
void TIM3_Init (void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;		// enable clock for TIM3

  TIM3->PSC   = (3000)-1;								// set prescaler   = 10kHz
  TIM3->ARR   = 200-1;									// set auto-reload (es: 20000 = 1s)
  TIM3->RCR   = 1-1;										// set repetition counter

  TIM3->DIER = TIM_DIER_UIE;						// Update Interrupt enable
  NVIC_EnableIRQ(TIM3_IRQn);						// TIM1   Interrupt enable

  //TIM3->CR1  |= TIM_CR1_CEN;						// timer enable
}


// ----------------------------------------------------------------------------
// Timer1 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM1_UP_IRQHandler (void) {

  if (TIM1->SR & (TIM_SR_UIF)) {                  /* UIF set?                 */
		
		timer_100us=1;
    					
    TIM1->SR &= ~(TIM_SR_UIF);                    /* clear UIF flag           */
  }
}


// ----------------------------------------------------------------------------
// Timer2 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM2_IRQHandler (void) {

  if (TIM2->SR & (TIM_SR_UIF)) {                  /* UIF set?                 */
		
		toggle_timer ^= 1;
		
    //if (toggle_timer == 0)	{LED1_ON;}
    //else										{LED1_OFF;}		
		
    TIM2->SR &= ~(TIM_SR_UIF);                    /* clear UIF flag           */
  }
}


// ----------------------------------------------------------------------------
// Timer3 Update Interrupt Handler
// ----------------------------------------------------------------------------
void TIM3_IRQHandler (void) {

  if (TIM3->SR & (TIM_SR_UIF)) {              // UIF set?
		
		
		
		TIM3->SR &= ~(TIM_SR_UIF);                // clear UIF flag
  }
}
