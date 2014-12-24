#include "reference_decoupling.h"
#include "pid.h"
#include "can1.h" 
#include "control_enable.h"

/***************************************************************************/
/**
 * this function decouples PWM (new version joint version).
 ***************************************************************************/


float _des_out [JN] = INIT_ARRAY(0);
Int32 counter=0;
//#define DEBUG_REF 1
     
void decouple_reference()
{      

//-----------------------------------------------------------------------------------
#if VERSION == 0x0163

    //  Neck Differential coupling 
    //    |Me1| |  1    -1 |  |Je1|
    //    |Me2|=|  1     1 |* |Je2|
 
 	#ifdef DEBUG_REF
    if (counter==5000)            
    {
    	can_printf("B: %f %f",_desired[0],_desired[1]);
    }
    #endif
    
    _des_out[0] = (float)(+ _desired[0] + _desired[1]);
    _des_out[1] = (float)(- _desired[0] + _desired[1]);
    _des_out[0] = _des_out[0] * 80.83788;
    _des_out[1] = _des_out[1] * 80.83788;
    _desired_decoupled[0] = (Int32)(_des_out[0]);
    _desired_decoupled[1] = (Int32)(_des_out[1]);
    
    #ifdef DEBUG_REF
    if (counter==5000)            
    {
    	can_printf("A: %f %f",_desired_decoupled[0],_desired_decoupled[1]);
    	counter=0;
    }
    if (counter==1000)            
    {
    	can_printf("%f %f %f %f",_position_enc[0],_position_enc[1],_position[0],_position[1]); 
    }
    counter++;
    #endif
    
#endif

}
