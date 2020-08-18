 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
 *  Ext int configuration                            *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#ifndef __EXTI_H
#define __EXTI_H

extern void EXTI_Init(void);
extern void EXTI1_IRQHandler(void);
extern void EXTI2_IRQHandler(void);

#endif
