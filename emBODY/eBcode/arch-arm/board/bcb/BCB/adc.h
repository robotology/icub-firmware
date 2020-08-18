 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  ADC configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#ifndef __ADC_H
#define __ADC_H

#define ADC_VALUE_MAX      (0xFFF)

extern uint16_t AD_last;
extern uint8_t  AD_done;

extern int *ADC_Conv(void);

extern void ADC_Init    (void);
extern void ADC_StartCnv(void);
extern void ADC_StopCnv (void);

#endif
