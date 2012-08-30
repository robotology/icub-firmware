
#include "2bllie_brushless_comm.h"
#include "phase_hall_sens.h"
#include "pwm_interface.h"
#include "pwm_a.h"
#include "pwm_b.h"
#include "asc.h"
#include "can1.h"
#include "leds_interface.h"
#include "currents_interface.h"
#include "qd0.h"
#include "qd1.h"
// variabili per l'inseguimento del duty cycle
sDutyControlBL DutyCycle[2];
sDutyControlBL DutyCycleReq[2];

extern sPwmControlBL *pTable0;
extern sPwmControlBL *pTable1;
extern UInt16 status0 ;
extern UInt16 status1 ;
extern UInt16 old_status0 ;
extern UInt16 old_status1 ;
extern sPwmControlBL PWMState[2];
extern Int32 comm_enc[2]; 
extern UInt8 hall_error[2];
Int32 comm_count[2] = {0, 0};
//extern volatile UInt8 	_current_sign[];
volatile byte COMMUTATION_MODE=HALL;   //the commutation method  
extern COMMUTATION_CONVERSION_TABLE[8];
const Int16  HALL_TO_COMMCOUNTER[8] = {0, 1, 3, 2, 5, 6, 4, 0};	/* order of commutation states */
#define INITIAL_COMMUTATION_COUNTER_VALUE  5;
const UInt16 COMMUTATION_SEQUENCE[6] ={ 4,6,2,3,1,5};
extern CommutationCounter0;
//extern CommutationCounter1;
//debug

/* extracting from a short */
#define BYTE_L(x) (__shr(x, 8))
#define BYTE_H(x) (x & 0xff)
canmsg_t _can;					// buffer to prepare messages for send 
//end 

//**********************************************************************
sPwmControlBL bldcCommutationTableComp[8] = {{0x003F, 0x0000} ,
					 {0x000F, 0x0002} ,
					 {0x0033, 0x0020} ,
					 {0x0033, 0x0002} ,
					 {0x003C, 0x0008} ,
					 {0x000F, 0x0008} ,
					 {0x003C, 0x0020} ,
					 {0x003F, 0x0000} };
																							 
sPwmControlBL bldcCommutationTableCompInv[8] = {{0x003F, 0x0000} ,
					{0x003C, 0x0020} ,
		 			{0x000F, 0x0008} ,
					{0x003C, 0x0008} ,
					{0x0033, 0x0002} ,
					{0x0033, 0x0020} ,
					{0x000F, 0x0002} ,
					{0x003F, 0x0000} };
					
//**********************************************************************
void TD0_Enable(void)
{
	setRegBits (TMRD0_CTRL, 0x2000);
}

void TD0_Disable(void)
{
	clrRegBits (TMRD0_CTRL, 0x2000);	
}

//**********************************************************************
/**
 * returns the speed of the brushless
 */
Int32 get_speed(byte axis)
{
	if (axis>=0 && axis <2)	return comm_count[axis];
}

//*********************************
/**
/* returns the hall sensor commutations
*/
Int32 get_commutations(byte axis)
{
	if (axis>=0 && axis <2)	return comm_enc[axis];
}

//*********************************
/**
/* returns the hall sensor commutations
*/
void set_commutations(byte axis, Int32 value)
{
	if (axis>=0 && axis <2)	comm_enc[axis]=value;
}


//**********************************************************************
/**
 * initializes the counter/timer. the timer is initialized w/ 1ms period.
 */
void TD0_init (void)
{
	/* TMRD0_CTRL: CM=0,PCS=0,SCS=0,ONCE=0,LENGTH=1,DIR=0,Co_INIT=0,OM=0 */
	setReg (TMRD0_CTRL, 0x20);           /* Stop all functions of the timer */

	/* TMRD0_SCR: TCF=0,TCFIE=1,TOF=0,TOFIE=0,IEF=0,IEFIE=0,IPS=0,INPUT=0,Capture_Mode=0,MSTR=0,EEOF=0,VAL=0,FORCE=0,OPS=0,OEN=0 */
	setReg (TMRD0_SCR, 0x4000);
	setReg (TMRD0_LOAD, 0);                /* Reset load register */
	setReg (TMRD0_CMP1, 39999);            /* Store appropriate value to the compare register according to the selected high speed CPU mode */

	clrRegBits (TMRD0_CTRL, 0x1e00);
	setRegBits (TMRD0_CTRL, 4096);    /* Set prescaler register according to the selected high speed CPU mode */
	setReg 	   (TMRD0_CNTR, 0); 		   /* Reset counter */
		
	clrRegBits (TMRD0_CTRL, 0xe000);
	TD0_Enable();						   /* counter on! */
}

/**
 * isr timer. 
 */
#pragma interrupt saveall
void TD0_interrupt(void)
{
	UInt8 tmp,val;
	static Int16 counter = 0;

	if ((status0==0x00) || (status0==0x07))
	{
		hall_error[0] |=HALL_ERROR_TD0;
	}
	if ((status1==0x00) || (status1==0x07))
	{
    	hall_error[1] |=HALL_ERROR_TD0;
	}
   
	if (DutyCycleReq[0].Duty < MIN_DUTY)
		DutyCycleReq[0].Duty=MIN_DUTY;
	if (DutyCycleReq[0].Duty > MAX_DUTY)
		DutyCycleReq[0].Duty=MAX_DUTY;
	if (DutyCycleReq[1].Duty < MIN_DUTY)
		DutyCycleReq[1].Duty=MIN_DUTY;
	if (DutyCycleReq[1].Duty > MAX_DUTY)
		DutyCycleReq[1].Duty=MAX_DUTY;

	if (DutyCycleReq[0].Dir != DutyCycle[0].Dir) 
	{
		if ((DutyCycle[0].Duty-STEP) < (MIN_DUTY)) //then direction change 
		{
			DutyCycle[0].Duty=DutyCycle[0].Duty-STEP;
			
			if (DutyCycle[0].Duty<(-MIN_DUTY))
			{
		 		DutyCycle[0].Duty=-DutyCycle[0].Duty;
				if (DutyCycle[0].Duty>DutyCycleReq[0].Duty) DutyCycle[0].Duty=DutyCycleReq[0].Duty; 	
			}
			else
			{
				DutyCycle[0].Duty=MIN_DUTY;
			}
			
			if (DutyCycleReq[0].Dir) 
			{
				pTable0 = bldcCommutationTableCompInv;
		//		_current_sign[0]=0;
			
			}
			
			else
			{
				pTable0 = bldcCommutationTableComp;
		//		_current_sign[0]=1;
				
			} 
		    // after a direction change the pwm must be changed immediately, before a hall sensor changed  
		status0=HALLSENSOR0; 
		
			PWMState[0] = pTable0[status0];
			tmp = getReg(PWMA_PMOUT) & 0x8000;
			val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
			setReg(PWMA_PMOUT,val);			
			DutyCycle[0].Dir = DutyCycleReq[0].Dir;
			
		}		
		else //no direction change
		{	
			DutyCycle[0].Duty=DutyCycle[0].Duty-STEP;
		}
	}
	else {
			if (DutyCycleReq[0].Duty > DutyCycle[0].Duty) 
			{
				if (DutyCycleReq[0].Duty-DutyCycle[0].Duty>=STEP)
				{
					DutyCycle[0].Duty=DutyCycle[0].Duty+STEP;
				}		
				else
					DutyCycle[0].Duty=DutyCycleReq[0].Duty;
			}
			else if (DutyCycleReq[0].Duty < DutyCycle[0].Duty) 
			{
				if (DutyCycle[0].Duty-DutyCycleReq[0].Duty>=STEP)
					DutyCycle[0].Duty=DutyCycle[0].Duty-STEP;
				else
					DutyCycle[0].Duty=DutyCycleReq[0].Duty;
			}
		
	//	else TD0_Disable();
	}
	
	if (DutyCycleReq[1].Dir != DutyCycle[1].Dir) 
	{
		if ((DutyCycle[1].Duty-STEP) < (MIN_DUTY)) //then direction change 
		{
			DutyCycle[1].Duty=DutyCycle[1].Duty-STEP;
			
			if (DutyCycle[1].Duty<(-MIN_DUTY))
			{
		 		DutyCycle[1].Duty=-DutyCycle[1].Duty;
				if (DutyCycle[1].Duty>DutyCycleReq[1].Duty) DutyCycle[1].Duty=DutyCycleReq[1].Duty; 	
			}
			else
			{
				DutyCycle[1].Duty=MIN_DUTY;
			}
			
			if (DutyCycleReq[1].Dir) pTable1 = bldcCommutationTableCompInv;
			else pTable1 = bldcCommutationTableComp; 
		    
			// after a direction change the pwm must be changed immediately, before a hall sensor changed  

		
			status1=HALLSENSOR1;
			PWMState[1] = pTable1[status1];
			tmp = getReg(PWMB_PMOUT) & 0x8000;
			val=tmp | PWMState[1].MaskOut | (PWMState[1].Mask<<8);
			setReg(PWMB_PMOUT,val); 
	    	
			DutyCycle[1].Dir = DutyCycleReq[1].Dir;
		}		
		else //no direction change 
		{
			DutyCycle[1].Duty=DutyCycle[1].Duty-STEP;
		}
	}
	else {
	 
			if (DutyCycleReq[1].Duty > DutyCycle[1].Duty) 
			{
				if (DutyCycleReq[1].Duty-DutyCycle[1].Duty>=STEP)
				{
					DutyCycle[1].Duty=DutyCycle[1].Duty+STEP;
				}
				
				else
					DutyCycle[1].Duty=DutyCycleReq[1].Duty;
			}
			else if (DutyCycleReq[1].Duty < DutyCycle[1].Duty) 
			{
				if (DutyCycle[1].Duty-DutyCycleReq[1].Duty>=STEP)
					DutyCycle[1].Duty=DutyCycle[1].Duty-STEP;
				else
					DutyCycle[1].Duty=DutyCycleReq[1].Duty;
			}
		
	//	else TD0_Disable();
	}
	
	PWM_generate_BLL(0, DutyCycle[0].Duty); 	
	PWM_generate_BLL(1, DutyCycle[1].Duty); 
		
	clrRegBit (TMRD0_SCR, TCF);            /* Reset interrupt request flag */
	
}

void PositionAlignment(void)
{
  UInt16 tmp,val,i,j,deb;
  UInt16 counter=0;
  UInt16 DELAY_VALUE=65000;
	  /* Disable compare interrupt */
	  QD0_DisableEvent();
	  /* Set the alignment voltage vector */
	  PWM_outputPadEnable(0);
	  DutyCycleReq[0].Duty=MIN_DUTY+ALIGNEMENT_PWM;
	  status0=4; //if I give this value and I expect to go to status0=6;
	  PWMState[0] = pTable0[status0];
	  tmp = getReg(PWMA_PMOUT) & 0x8000;
	  val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
	  setReg(PWMA_PMOUT,val); 	  
	  /* Wait till the DutyCycle reaches the alignment value */
	  while(DutyCycle[0].Duty<(DutyCycleReq[0].Duty-1)) 
	  { 
		asm
		 {
		 	NOP
		 }// delay
	  };
	  
	  for (i=1;i<=5;i++)
	  {
	      status0=COMMUTATION_SEQUENCE[i]; //if I give this value and I expect to go to status0=5;
		  PWMState[0] = pTable0[status0];
		  tmp = getReg(PWMA_PMOUT) & 0x8000;
		  val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
		  setReg(PWMA_PMOUT,val); 
	  	  DutyCycleReq[0].Duty=MIN_DUTY;
	  	  while(DutyCycle[0].Duty<=(DutyCycleReq[0].Duty+1)) 
	  	  { 
			asm
		 		{
		 		NOP
		 		}// delay
	  	  };
		  
		  for(j=0;j<50;j++)   //delay of about 1sec
		  {
		      counter=0;
		      DutyCycleReq[0].Duty=MIN_DUTY+(j<<1);	 //to reach 100 of PWM
			  /* Wait till the DutyCycle reaches the alignment value */
			  while(counter<(DELAY_VALUE)) 
			  { 
			  	 counter++;
			  	 asm
				 {
				 	NOP
				 }// delay
			  };
		  } 
	  }
	    QD0_SetCounter(HALF_COMMUTATION_INTERVAL);
	  /* Set the compare registers to half of NORMAL_COMMUTATION_INTERVAL (30 degrees) */
	  setReg(TMRA0_CMP1,HALF_COMMUTATION_INTERVAL+NORMAL_COMMUTATION_INTERVAL);
	  setReg(TMRA0_CMP2,HALF_COMMUTATION_INTERVAL-NORMAL_COMMUTATION_INTERVAL);

//	  QD0_SetCounter(HALF_COMMUTATION_INTERVAL);
	  /* Set the compare registers to half of NORMAL_COMMUTATION_INTERVAL (30 degrees) */
//	  setReg(TMRA0_CMP1,HALF_COMMUTATION_INTERVAL+NORMAL_COMMUTATION_INTERVAL);
//	  setReg(TMRA0_CMP2,HALF_COMMUTATION_INTERVAL-NORMAL_COMMUTATION_INTERVAL);
	  /* Enable compare interrupt */
	  QD0_EnableEvent();
	  setRegBits (TMRA0_CTRL, 0x8000);	   /* counter on in quadrature mode! */ 
	  /* Disable PWM output */
	  PWM_outputPadDisable(0);
	  
	  /* Set zero output voltage */
	  DutyCycleReq[0].Duty=MIN_DUTY;
	  /* Set initial value of commutation counter */
	  CommutationCounter0 = INITIAL_COMMUTATION_COUNTER_VALUE;
	  /* Set initial mask and swap bit in PWM */
	  status0=COMMUTATION_CONVERSION_TABLE[CommutationCounter0];
	  PWMState[0] = pTable0[status0];
	  tmp = getReg(PWMA_PMOUT) & 0x8000;
	  val=tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8);
	  setReg(PWMA_PMOUT,val); 

}


//*********************************************************
void Init_Brushless_Comm(byte axis_number, byte commutation_mode)
{
	UInt8 tmp;
	
	COMMUTATION_MODE=commutation_mode;
	
	if (axis_number==2)
	{
	
	DutyCycle[0].Duty = MIN_DUTY;
	DutyCycle[0].Dir = 0;
	DutyCycleReq[0].Duty = MIN_DUTY;
	DutyCycleReq[0].Dir = 0;
	pTable0 = bldcCommutationTableComp;

	DutyCycle[1].Duty = MIN_DUTY;
	DutyCycle[1].Dir = 0;
	DutyCycleReq[1].Duty = MIN_DUTY;
	DutyCycleReq[1].Dir = 0;
	pTable1 = bldcCommutationTableComp;
	

	Init_Hall_Effect_0();
	Init_Hall_Effect_1();
	// inizializzazione dello stato del motore 0
	status0=HALLSENSOR0;
	// inizializzazione dello stato del motore 1
	status1=HALLSENSOR1;
	if (COMMUTATION_MODE==ENCODER)
	{
		QD0_init();
		QD1_init();
		tmp=HALLSENSOR0;
		CommutationCounter0=HALL_TO_COMMCOUNTER[tmp]; 
		status0=COMMUTATION_CONVERSION_TABLE[CommutationCounter0];
	}
	if (COMMUTATION_MODE==HALL)
	{
		// inizializzazione dello stato del motore 0
		status0=HALLSENSOR0;
		// inizializzazione dello stato del motore 1
		status1=HALLSENSOR1;
	}
	PWMState[0] = pTable0[status0];
	old_status0 = status0;

	PWMState[1] = pTable1[status1];
	old_status1 = status1;	
	
	//Init PWM
	PWM_A_init ();
	PWM_B_init ();
	
	// write mask to PWM Channel Control Register / 
	tmp = getReg(PWMA_PMOUT) & 0x8000;
	// Set output control enable to PWMOUT 
	setReg(PWMA_PMOUT,tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8)); 

	tmp = getReg(PWMB_PMOUT) & 0x8000;
	// Set output control enable to PWMOUT 
	setReg(PWMB_PMOUT,tmp | PWMState[1].MaskOut | (PWMState[1].Mask<<8)); 
	}
	
	if (axis_number==1)
	{
		
	DutyCycle[0].Duty = MIN_DUTY;
	DutyCycle[0].Dir = 0;
	DutyCycleReq[0].Duty = MIN_DUTY;
	DutyCycleReq[0].Dir = 0;
	pTable0 = bldcCommutationTableComp;
			
	Init_Hall_Effect_0();
	if (COMMUTATION_MODE==ENCODER)
	QD0_init();
	// inizializzazione dello stato del motore
	status0=HALLSENSOR0;
	PWMState[0] = pTable0[status0];
	old_status0 = status0;
	
	//Init PWM
	PWM_A_init ();
	// write mask to PWM Channel Control Register / 
	tmp = getReg(PWMA_PMOUT) & 0x8000;
	// Set output control enable to PWMOUT 
	setReg(PWMA_PMOUT,tmp | PWMState[0].MaskOut | (PWMState[0].Mask<<8)); 

	}
	//Init Current
	init_currents();
	
	// Init duty cycle timer
	TD0_init();
	
	if (COMMUTATION_MODE== ENCODER)
	{
		PositionAlignment();
	}
}
