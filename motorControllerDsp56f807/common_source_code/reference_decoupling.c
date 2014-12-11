#include "reference_decoupling.h"
#include "pid.h"
#include "can1.h" 
#include "control_enable.h"

/***************************************************************************/
/**
 * this function decouples PWM (new version joint version).
 ***************************************************************************/


Int32 _des_out [JN] = INIT_ARRAY(0);
     
void decouple_reference()
{      

//-----------------------------------------------------------------------------------
#if VERSION == 0x0162

    //  Neck Differential coupling 
    //    |Me1| |  1    -1 |  |Je1|
    //    |Me2|=|  1     1 |* |Je2|
    
    _des_out[0] = (_desired[0] + _desired[1])>>1;
    _des_out[1] = (_desired[0] + _desired[1])>>1;
    _desired[0] = _des_out[0];
    _desired[1] = _des_out[1];
                 
#endif

}
