

#ifndef __brushless_commh__
#define __brushless_commh__

#include "dsp56f807.h"

typedef struct sDutyControl_tag
{
	byte    Dir;	
	Int16   Duty;
} sDutyControlBL;
 
//All THESE VALUE ARE VALID ONLY IF THE PWM IS CENTER ALIGNED 

#define  MIN_DUTY 3  // MIN_DUTY 3 at 30KHz, 3 at 25KHz 
#define  MAX_DUTY 760// 625 at 30KHz, 760 at 25KHz 
#define  DEAD_TIME 5 //deadtime 5 at 30KHz, 5 at 25KHz 
#define  STEP 5//10     // 5 at 30KHz 6 at 25KHz   
#define  PWMFREQ 800 // 666= 30KHz 800= 25KHz 
#define HALL			   0	
#define ENCODER            1
#define ALIGNEMENT_PWM   100
#define HALLSENSOR0 ((getRegBits(GPIO_B_DR,GPIO_B2)<<2) | (getRegBits(GPIO_B_DR,GPIO_B3)<<1) | (getRegBits(GPIO_B_DR,GPIO_B4)));
#define HALLSENSOR1 ((getRegBits(GPIO_D_DR,GPIO_D2)<<2) | (getRegBits(GPIO_D_DR,GPIO_D3)<<1) | (getRegBits(GPIO_D_DR,GPIO_D4)));
 

void TD0_interrupt(void);

void TD0_Enable(void);


void TD0_Disable(void);

//It initialize the brushless commutation for axis_number axis (1 or 2 axis)
void Init_Brushless_Comm(byte axis_number, byte commutation_mode);  //  
void TD0_init (void);
void PWM_generate_BLL(byte , Int16);
Int32 get_speed(byte axis);
Int32 get_commutations(byte axis);
void set_commutations(byte axis, Int32 value);

void PositionAlignment(void);
#endif 