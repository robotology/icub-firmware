//   
//  PWM related routines:
//  
//  Initialization, control and fault management 
//  
// 
#ifndef __PWM_H__
#define __PWM_H__

#include <pwm12.h>

//void SetupPWM(void);
//void PWMDisable(void);
//void PWMEnable(void);

void pwmInit(short pwm_50_duty_cycle, short pwm_deadtime, short pwm_max);
void pwmSetMax(short Vmax);
char pwmON(void);
void pwmOFF(void);
void pwmZero(void);
void pwmOut(int Va, int Vb, int Vc);
char pwmIsEnabled();

#endif
