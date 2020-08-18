 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Interrupt configuration                          *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#ifndef __EXTI_H
#define __EXTI_H


#define EXTI0_FALLING 	(1<<0)
#define EXTI1_FALLING	(1<<1)
#define EXTI11_FALLING	(1<<2)
#define EXTI12_FALLING	(1<<3)
#define EXTI0_RISING 	(1<<4)
#define EXTI1_RISING	(1<<5)
#define EXTI11_RISING	(1<<6)
#define EXTI12_RISING	(1<<7)


//extern void ExtIInit(void (*handler0)(void),void (*handler1)(void),void (*handler11)(void),
// void (*handler12)(void),char flags );

extern void EXTI_Init(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI4_IRQHandler(void);
extern void EXTI9_5_IRQHandler(void);
extern void EXTI15_10_IRQHandler(void);

#endif
