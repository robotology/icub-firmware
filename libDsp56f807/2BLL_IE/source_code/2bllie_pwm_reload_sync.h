/*
 * 2bll_timer_pwm_sync.h
 * 	Interrupt syncronized with the center of the pwm 
 */

#ifndef  __2bll_pwm_reload_synch__
#define  __2bll_pwm_reload_synch__
 
#include "dsp56f807.h"


//  definizione strutture dati
typedef struct sPwmControlBL_tag
{
	UWord16    Mask;
	UWord16    MaskOut;
} sPwmControlBL;




#define NORMAL_COMMUTATION_INTERVAL  342   //resolution of the encoder for an electric turn (8192/4)/6=341.3       
#define SHORT_COMMUTATION_INTERVAL   338    //(2048-(5*342) this step is longer to compensate the 0.3 tick each step 
#define HALF_COMMUTATION_INTERVAL    171   //resolution of the encoder for an electric turn (8192/4)/6=341.3       
#define  HALL_ERROR_0K   	0x0  //no errors
#define  HALL_ERROR_TD0   	0x1  //when there is a mismatch between Hall reading in the hall_interrupt and the 1ms timer
#define  HALL_ERROR_TABLE 	0x2  //when the reading of the hall in the hall_interrupt is not congruent with the table  
#define  HALL_ERROR_GLITCH	0x4 //when there is a mismatch between the two hall reading in the hall_interrupt 

void    PWMAReload_Interrupt();
void    PWMBReload_Interrupt();
void 	Init_Hall_Effect_0();
void 	Init_Hall_Effect_1();
void QD0_OnCompare(void);
void QD1_OnCompare(void);
UInt8 Get_Sens0_Status(void);
UInt8 Get_Sens1_Status(void);
UInt8 getHallStatus(UInt16 channel);
void setHallStatus(UInt16 channel,UInt8 val);

#endif