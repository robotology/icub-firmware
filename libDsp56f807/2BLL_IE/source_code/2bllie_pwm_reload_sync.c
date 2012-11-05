

#include "phase_hall_sens.h"
#include "pwm_interface.h"
#include "qd0.h"
#include "qd1.h" 
#include "leds_interface.h"
#include "2bllie_brushless_comm.h"
#include "currents_interface.h"
#include "qd0.h"
#include "qd1.h"
#include "can1.h"


//volatile UInt8 _current_sign[4]={0,0,0,0};

volatile   UInt16 status0 = 0;
volatile   UInt16 status1 = 0;
volatile   UInt16 old_status0 = 0;
volatile   UInt16 old_status1 = 0;
//volatile   bool phase_changed[2]={0,0};    
volatile   UInt8 hall_error[2]={HALL_ERROR_0K,HALL_ERROR_0K};
volatile   UInt8 tmp,val; //used in the interrupt routine

Int16  	   DirectionSpinning0 = 1;	/* direction of spinning (1,-1) */
volatile Int16	   CommutationCounter0;		/* counter of commutation events */
volatile UWord16	   Timer0Value;			/* actual value of position Timer0 */
volatile Int16	   Timer0Difference1;		/* difference between Timer0Value and COMPARE1 register */
volatile Int16	   Timer0Difference2;		/* difference between Timer0Value and COMPARE2 register */
extern volatile byte COMMUTATION_MODE;
const Int16  COMMUTATION_CONVERSION_TABLE[8] = {0, 1, 3, 2, 6, 4, 5, 0};	/* order of commutation states */

//*********************************************************
// definizione varibili																			 
volatile sPwmControlBL *pTable0;
volatile sPwmControlBL *pTable1;

const UWord16 DIRECTION_TABLE[8] = {0, 5, 3, 1, 6, 4, 2, 0};
const UWord16 DIRECTION_TABLE_INV[8] = {0, 3, 6, 2, 5, 1, 4, 0};

volatile Int32 comm_enc[2]={0,0}; 
sPwmControlBL PWMState[2];



//*********************************************************
void Init_Hall_Effect_0(void)
{
	clrRegBits(GPIO_B_DDR,GPIO_B2);  //     HX1 
	clrRegBits(GPIO_B_PER,GPIO_B2);  
	clrRegBits(GPIO_B_DDR,GPIO_B3);  //     HY1 
	clrRegBits(GPIO_B_PER,GPIO_B3);  	
 	clrRegBits(GPIO_B_DDR,GPIO_B4);  //     HZ1 
	clrRegBits(GPIO_B_PER,GPIO_B4);									
}				

//*********************************************************
void Init_Hall_Effect_1(void)
{
	clrRegBits(GPIO_D_DDR,GPIO_D2);  //     HX2 
	clrRegBits(GPIO_D_PER,GPIO_D2);  
	clrRegBits(GPIO_D_DDR,GPIO_D3);  //     HY2 
	clrRegBits(GPIO_D_PER,GPIO_D3);  	
 	clrRegBits(GPIO_D_DDR,GPIO_D4);  //     HZ2 
	clrRegBits(GPIO_D_PER,GPIO_D4);									
}				



/*********************************************************
* 
* In the interrupt it are read the hall-sensors and it is change the commutation of the phases 
*
*********************************************************/


#pragma interrupt
void PWMAReload_Interrupt(void)
{	
	//clear the interrupt flag of the pwm reload interrupt
	clrRegBits(PWMA_PMCTL, PWMA_PMCTL_PWMF_MASK);
	//read the hall sensors	
	status0=HALLSENSOR0;
	if (old_status0!= status0) 
	{
	  	if (status0 == DIRECTION_TABLE[old_status0]) 
	  	{
	  		comm_enc[0]++; 		
	  		
	  		//			phase_changed[0]=1;
			// write mask to PWM Channel Control Register 
			PWMState[0]= pTable0[status0];
			tmp = getReg(PWMA_PMOUT) & 0x8000;
			val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
			setReg(PWMA_PMOUT,val); 
			old_status0 = status0;	
	  	}
		else if (status0 == DIRECTION_TABLE_INV[old_status0]) 
			{
				comm_enc[0]--;	
				//			phase_changed[0]=1;
				// write mask to PWM Channel Control Register 
				PWMState[0]= pTable0[status0];
				tmp = getReg(PWMA_PMOUT) & 0x8000;
				val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
				setReg(PWMA_PMOUT,val); 
				old_status0 = status0;	
			}
			else
			{
				hall_error[0]=HALL_ERROR_TABLE;
				PWM_outputPadDisable(0);
			#ifdef DEBUG_CAN_MSG	
				can_printf("HALL ERROR 0");
			#endif
			} 

	}

		
}



#pragma interrupt 
void PWMBReload_Interrupt(void)
{	
	//clear the interrupt flag of the pwm reload interrupt
	clrRegBits(PWMB_PMCTL, PWMB_PMCTL_PWMF_MASK);
	//read the hall sensors
	status1=HALLSENSOR1;
	if (old_status1!= status1) 
	{
	  	if (status1 == DIRECTION_TABLE[old_status1]) 
	  	{
	  		comm_enc[1]++;
  			//		phase_changed[1]=1;		
			// write mask to PWM Channel Control Register 
			PWMState[1]= pTable1[status1];
			tmp = getReg(PWMB_PMOUT) & 0x8000;
			val=tmp | PWMState[1].MaskOut | (PWMState[1].Mask<<8);
			setReg(PWMB_PMOUT,val);
			old_status1 = status1;
	  	}
		else if (status1 == DIRECTION_TABLE_INV[old_status1]) 
			{
				comm_enc[1]--;	
				//		phase_changed[1]=1;		
				// write mask to PWM Channel Control Register 
				PWMState[1]= pTable1[status1];
				tmp = getReg(PWMB_PMOUT) & 0x8000;
				val=tmp | PWMState[1].MaskOut | (PWMState[1].Mask<<8);
				setReg(PWMB_PMOUT,val);
				old_status1 = status1;
			}
			else
			{
				hall_error[1]=HALL_ERROR_TABLE;
				PWM_outputPadDisable(1);
			#ifdef DEBUG_CAN_MSG
				can_printf("HALL ERROR 1");
			#endif
			}    

	}
}

#pragma interrupt called
void QD0_OnCompare(void)
{
  /* finding of source of interrupt (COMPARE1 or COMPARE2) */
  Timer0Value=getReg(TMRA0_CNTR);
  Timer0Difference1=abs_s((Timer0Value-QD0_GetCompare1Value()));
  Timer0Difference2=abs_s((Timer0Value-QD0_GetCompare2Value()));
//  can_printf("CMP1 %d", QD0_GetCompare1Value());
//  can_printf("CMP2 %d", QD0_GetCompare2Value());
//  can_printf("T %d",Timer0Value);
//  can_printf("DF2 %d DF1 ,%d",Timer0Difference2, Timer0Difference1);	
  /* according to finding of interrupt source */
  if (Timer0Difference1<Timer0Difference2)
    {
    /* set direction of spinning */
    DirectionSpinning0 = 1;
    
    /* increment commutation counter */
    if (CommutationCounter0<6) 
    {
      CommutationCounter0++;
    }
    else 
    {
      CommutationCounter0=1;
    }
   
    /* set new value to COMPARE1/2 registers and compensate rounding of NORMAL_COMMUTATION_INTERVAL */
    if (CommutationCounter0==4) 
    {
      QD0_SetCompare1(QD0_GetCompare1Value() + SHORT_COMMUTATION_INTERVAL);
      QD0_SetCompare2(QD0_GetCompare2Value() + SHORT_COMMUTATION_INTERVAL);
  
    }
    else 
    {
      QD0_SetCompare1(QD0_GetCompare1Value() + NORMAL_COMMUTATION_INTERVAL);
      QD0_SetCompare2(QD0_GetCompare2Value() + NORMAL_COMMUTATION_INTERVAL);
    }
  }
  else 
  {
    /* set direction of spinning */
    DirectionSpinning0 = -1;
    /* decrement commutation counter */
    if (CommutationCounter0>1) 
    {
      CommutationCounter0--;
    }
    else 
    {
      CommutationCounter0=6;
    }
    /* set new value to COMPARE1/2 registers and compensate rounding of NORMAL_COMMUTATION_INTERVAL */
    if (CommutationCounter0==3) 
    {
      QD0_SetCompare2(QD0_GetCompare2Value() - SHORT_COMMUTATION_INTERVAL);
      QD0_SetCompare1(QD0_GetCompare1Value() - SHORT_COMMUTATION_INTERVAL);
    }
    else 
    {
      QD0_SetCompare2(QD0_GetCompare2Value() - NORMAL_COMMUTATION_INTERVAL);
      QD0_SetCompare1(QD0_GetCompare1Value() - NORMAL_COMMUTATION_INTERVAL);
    }
  }
  
  status0 = COMMUTATION_CONVERSION_TABLE[CommutationCounter0];
  
  // write mask to PWM Channel Control Register 
  PWMState[0]= pTable0[status0];
  tmp = getReg(PWMA_PMOUT) & 0x8000;
  val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
  setReg(PWMA_PMOUT,val); 
  old_status0 = status0;
}

UInt8 Get_Sens0_Status(void)
{
	return status0;
}

UInt8 Get_Sens1_Status(void)
{
	return status1;
}
UInt8 getHallStatus(UInt16 channel)
{
	return hall_error[channel];
}

void setHallStatus(UInt16 channel,UInt8 val)
{
	hall_error[channel]=val;
}
