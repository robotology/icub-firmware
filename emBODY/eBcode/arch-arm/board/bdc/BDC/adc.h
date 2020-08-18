 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
 *  ADC configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#ifndef __ADC_H
#define __ADC_H

#define ADC_VALUE_MAX      (0xFFF)

//extern uint16_t ADC_value;
//extern int ADC_value[3];
extern uint8_t  AD_done;

extern int16_t  *ADC_Conv(void);

extern void     ADC_Init    (void);
extern void     ADC_StartCnv(void);
extern void     ADC_StopCnv (void);
extern uint16_t ADC_GetCnv  (void);

#endif
