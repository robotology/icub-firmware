 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Timers configuration                             *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#ifndef __TIMER_H
#define __TIMER_H

extern void TIM1_Init(void);
extern void TIM2_Init(void);
extern void TIM3_Init(void);
extern void TIM4_Init(void);
extern void TIM1_UP_IRQHandler(void);
extern void TIM2_IRQHandler(void);

#endif
