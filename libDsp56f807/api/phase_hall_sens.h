
#ifndef __phase_hall_sensh__
#define __phase_hall_sensh__


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

#warning "we should change it with 0x02 but iCubInterface does not read it"
#define  HALL_ERROR_TABLE 	0x1   //when the reading of the hall in the hall_interrupt is not congruent with the table  
#define  HALL_ERROR_GLITCH	0x4 //when there is a mismatch between the two hall reading in the hall_interrupt 


//If COMM_ENC is defined, the number of motor commutations is counted.
//It can be used as a motor encoder. Function get_commutations() retrieves the computed value.
//It is also mandatory for back-EMF compensation.
#define  COMM_ENC

//#define  HALL_GLITH_CHECK

/**************************************************************************************/
/**
 * Inits the Hall Effect Sensor, channel 0
 */
/**************************************************************************************/
void Init_Hall_Effect_0(void);

/**************************************************************************************/
/**
 * Inits the Hall Effect Sensor, channel 1
 */
/**************************************************************************************/
void Init_Hall_Effect_1(void);

/**************************************************************************************/
/**
 * Return the Status of the Halla Effect Sensor, channel 0
 * @return a byte with the 3 lsb bits indicating the status of the three sensors (0b00000xyz) 
 */
/**************************************************************************************/
UInt8 Get_Sens0_Status(void);

/**************************************************************************************/
/**
 * Return the Status of the Halla Effect Sensor, channel 0
 * @return a byte with the 3 lsb bits indicating the status of the three sensors (0b00000xyz) 
 */
/**************************************************************************************/
UInt8 Get_Sens1_Status(void);

UInt8 getHallStatus(UInt16 channel);
void setHallStatus(UInt16 channel,UInt8 val);

void    PWMAReload_Interrupt();
void    PWMBReload_Interrupt();


#endif 
