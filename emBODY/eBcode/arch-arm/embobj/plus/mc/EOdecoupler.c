
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOemsControllerCfg.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler.h"
#include "iCubCanProto_types.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define ZERO_ROTATION_TORQUE 0

#define CABLE_WARNING_x_100  2000

#define MOTORS(m) for (uint8_t m=0; m<o->nMotors; ++m)
#define JOINTS(j) MOTORS(j) 

#define MOTOR_EXTERNAL_FAULT     0x00000004

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static char invert_matrix(float** M, float** I, char n);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotors* eo_motors_New(uint8_t nMotors, eOemscontroller_board_t board) 
{
    if (!nMotors) return NULL;
    if (nMotors > MAX_NAXLES) nMotors = MAX_NAXLES;
    
    EOmotors *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotors), 1);

    if (o)
    {
        o->board = board;
        o->nMotors = nMotors;
        o->Jok = eobool_false;
        
        MOTORS(m)
        {
            o->motor_watchdog[m] = 100;
            o->motor_run_state_last[m] = icubCanProto_controlmode_notConfigured;
            o->motor_run_state_req[m] = icubCanProto_controlmode_notConfigured;
            o->motor_run_state[m] = icubCanProto_controlmode_notConfigured;
            o->motor_run_state_req_wdog[m] = 100;
            o->motor_fault_mask[m] = 0;
            
            JOINTS(j)
            {
                o->J[m][j] = o->Ji[m][j] = 0;
            }
        }
    }

    return o;
}

extern void eo_motors_new_state_req(EOmotors *o, uint8_t motor, uint8_t control_mode)
{
    o->motor_run_state_req[motor] = control_mode;
    o->motor_run_state_req_wdog[motor] = 50;
}

extern void eo_motors_rearm_wdog(EOmotors *o, uint8_t motor)
{
    o->motor_watchdog[motor] = 50;
}

extern void eo_motors_check_wdog(EOmotors *o)
{
    MOTORS(m)
    {
        if (o->motor_watchdog[m]>0)
        {
            --o->motor_watchdog[m];
        }
    }
}

extern eObool_t eo_motors_are_coupled(EOmotors *o, uint8_t ma, uint8_t mb)
{
    return o->J[ma][mb] || o->J[mb][ma];
}

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, uint8_t *state)
{   
    if (!state) return;

    o->motor_run_state[m] = state[0];
    o->motor_fault_mask[m] = ((uint32_t*)state)[1];
}

#include "EoError.h"
#include "EOtheErrorManager.h"

extern eObool_t eo_motor_check_state_req(EOmotors *o, uint8_t m)
{
    if (o->motor_run_state[m] == o->motor_run_state_req[m])
    {
        o->motor_run_state_last[m] = o->motor_run_state[m];
        
        o->motor_run_state_req_wdog[m] = 50;
    
        return eobool_false;
    }
    
    if (o->motor_run_state_last[m] != o->motor_run_state[m])
    {
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = m; // unless required
        descriptor.par64 = (((uint16_t)(o->motor_run_state[m]))<<8)||(o->motor_run_state_req[m]);
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_generic_error);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                
        return eobool_true;
    }
    
    if (o->motor_run_state_req_wdog[m]>0)
    {
        --o->motor_run_state_req_wdog[m];
        
        return eobool_false;
    }
    
    {
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = m; // unless required
        descriptor.par64 = (((uint16_t)(o->motor_run_state[m]))<<8)||(o->motor_run_state_req[m]);
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_generic_error);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    return eobool_true;
}

extern eObool_t eo_get_motor_timeout(EOmotors *o, uint8_t m)
{
    return o->motor_watchdog[m]==0;
}

extern uint32_t eo_get_motor_fault_mask(EOmotors *o, uint8_t m)
{
    return o->motor_fault_mask[m];
}

extern eObool_t eo_is_motor_ext_fault(EOmotors *o, uint8_t m)
{
    return o->motor_fault_mask[m] & MOTOR_EXTERNAL_FAULT;
}

extern eObool_t eo_is_motor_hard_fault(EOmotors *o, uint8_t m)
{
    return o->motor_run_state[m] == icubCanProto_controlmode_hwFault;
}

// speed_motor  = J^-1 * speed_axis
// torque_motor = Jt   * torque_axis

#if defined(USE_JACOBIAN)

extern void eo_motors_decouple_PWM(EOmotors *o, float *pwm_joint, float *pwm_motor, eObool_t* stiff)
{
    MOTORS(m)
    {
        pwm_motor[m] = 0;

        JOINTS(j)
        {
            pwm_motor[m] += ((stiff[j]?o->Ji[m][j]:o->J[j][m])*pwm_joint[j]);
        }
        
        pwm_motor[m] >>= 14;
        
        LIMIT(pwm_motor[m], NOMINAL_CURRENT);
    }
}

#else

extern void eo_motors_decouple_PWM(EOmotors *o, float *pwm_joint, float *pwm_motor, eObool_t* stiff)
{
    //if (!o) return;
    
    if(emscontroller_board_SHOULDER == o->board)
    {
        //             | 1     0       0   |
        // J = dq/dm = | 1   40/65     0   |
        //             | 0  -40/65   40/65 | 

        //        |    1       0       0   |
        // J^-1 = | -65/40   65/40     0   |
        //        | -65/40   65/40   65/40 |

        //      | 1     1       0   |
        // Jt = | 0   40/65  -40/65 |
        //      | 0     0     40/65 | 

        int32_t buff;
        
        pwm_motor[0] = pwm_joint[0];
        
        if (stiff[0])
        {    
            pwm_motor[1] = (-65*pwm_joint[0])/40;
            #ifdef SHOULDER_3rd_JOINT_COUPLING
            pwm_motor[2] = pwm_motor[1];
            #else
            pwm_motor[2] = 0;
            #endif
        }
        else
        {
            pwm_motor[0] = pwm_joint[0];
        }
        
        if (stiff[1])
        {
            buff = (65*pwm_joint[1])/40;
            pwm_motor[1] += buff;
            #ifdef SHOULDER_3rd_JOINT_COUPLING
            pwm_motor[2] += buff;
            #endif
        }
        else
        {
            pwm_motor[1] = pwm_joint[1];
        }
        
        if (stiff[2])
        {
            pwm_motor[2] += (65*pwm_joint[2])/40;
        }
        else
        {
            pwm_motor[2] = pwm_joint[2];
        }
        // stiff
        //pwm_motor[0] = (int16_t)(pwm_joint[0]);
        //pwm_motor[1] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]))/40);
        //pwm_motor[2] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]+pwm_joint[2]))/40);
        
        // compliant
        //pwm_motor[0] = (int16_t)(pwm_joint[0]+pwm_joint[1]);
        //pwm_motor[1] = (int16_t)((40*(pwm_joint[1]-pwm_joint[2]))/65);
        //pwm_motor[2] = (int16_t)((40*pwm_joint[2])/65);
    
        pwm_motor[3] = pwm_joint[3];
    }        
    else if(emscontroller_board_WAIST == o->board)
    {
        //             |   1     1     0   |
        // J = dq/dm = |  -1     1     0   |
        //             | 44/80 44/80 44/80 | 

        //        | 1/2  -1/2    0   |
        // J^-1 = | 1/2   1/2    0   |
        //        | -1     0   80/44 |

        //      | 1  -1  44/80 |
        // Jt = | 1   1  44/80 |
        //      | 0   0  44/80 | 
        
        if (stiff[0]) {pwm_motor[0] = (pwm_joint[0]-pwm_joint[1])/2;} else {pwm_motor[0] = pwm_joint[0];}
        if (stiff[1]) {pwm_motor[1] = (pwm_joint[0]+pwm_joint[1])/2;} else {pwm_motor[1] = pwm_joint[1];}
        if (stiff[2]) {pwm_motor[2] =  pwm_joint[2];                } else {pwm_motor[2] = pwm_joint[2];}
    }        
    else if (emscontroller_board_UPPERLEG == o->board)
    {    
        pwm_motor[0] = pwm_joint[0];
        pwm_motor[1] = pwm_joint[1];
        pwm_motor[2] = pwm_joint[2];
        pwm_motor[3] = pwm_joint[3];
    }        
    else if (emscontroller_board_ANKLE == o->board)
    {    
        pwm_motor[0] = pwm_joint[0];
        pwm_motor[1] = pwm_joint[1];
    }
    /*
    else if((emscontroller_board_HEAD_neckpitch_neckroll == ems->board) || (emscontroller_board_HEAD_neckyaw_eyes == ems->board))
    {
        #warning TODO: for head v3
        // marco.accame: questo e' un placeholder per mettere le azioni specifiche riguardanti la scheda della head-v3.
        // ovviamente si deve sviluppare gli if-else (o un bel switch-case) per tutte le board head v3. 
        // qui se la scheda NON presenta coupled joints, allora si procede come nel caso del emscontroller_board_UPPERLEG.  
        // altrimenti si procede come nel caso del emscontroller_board_WAIST.
        // attenzione al caso di solo alcuni coupled joints (vedi emscontroller_board_SHOULDER)  
    }
    */
    else    // marco.accame: this board does not have coupled joints
    {    
        MOTORS(m) pwm_motor[m] = 0;
    }
    //#endif
}

#endif

extern eObool_t eo_motors_CableLimitAlarm(int32_t j0, int32_t j1, int32_t j2)
{
    int32_t cond = 171*(j0-j1);
    
    if (cond < -34700 + CABLE_WARNING_x_100) return eobool_true;
 
    cond -= 171*j2;
    
    if (cond < -36657 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  11242 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j1+j2;
    
    if (cond <  -6660 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  21330 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j0;
    
    if (cond <  -9600 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >    500 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j1;
    
    if (cond <          CABLE_WARNING_x_100) return eobool_true;
    if (cond >  19500 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j2;
    
    if (cond < -9000 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  9000 - CABLE_WARNING_x_100) return eobool_true;
    
    return eobool_false;
}

extern void eo_motors_set_Jacobian(EOmotors *o, int32_t **Ji32)
{
    float Ji[4][4];
    MOTORS(m) JOINTS(j)
    {
        o->Ji[m][j] = Ji32[m][j];
        Ji[m][j] = ((float)(Ji32[m][j]))/16384.0f;
    }
    
    float J[4][4];
    if (!invert_matrix((float**)Ji, (float**)J, o->nMotors)) return;
    
    o->Jok = eobool_true;
    
    MOTORS(m) JOINTS(j)
    {
        o->J[m][j]=(int32_t)(0.5f+Ji[m][j]*16348.0f);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#define FOR(i) for (int i=0; i<n; ++i)
#define SCAN(r,c) FOR(r) FOR(c)

static char invert_matrix(float** M, float** I, char n)
{
    float B[4][4];
    SCAN(r,c) { B[r][c]=M[r][c]; I[r][c]=0.0f; }
    FOR(i) I[i][i]=1.0f;
    
    for (int r=0; r<n-1; ++r)
    {
        int pivot=-1;
        float max2=0.0f;
        
        for (int d=r; d<n; ++d)
        {
            float m2=B[d][r]*B[d][r];

            if (m2>max2)
            {
                max2=m2;
                pivot=d;
            }
        }

        if (pivot==-1)
        {
            return 0;
        }

        if (pivot!=r)
        {
            FOR(c)
            {
                float tb=B[r][c]; B[r][c]=B[pivot][c]; B[pivot][c]=tb;
                float ti=I[r][c]; I[r][c]=I[pivot][c]; I[pivot][c]=ti;
            }
        }

        float P=-1.0f/B[r][r];

        for (int rr=r+1; rr<n; ++rr)
        {
            float D=P*B[rr][r]; 

            FOR(c)
            {
                B[rr][c]+=D*B[r][c];
                I[rr][c]+=D*I[r][c];
            }
        }
    }

    for (int r=n-1; r>0; --r)
    {
        float P=-1.0f/B[r][r];

        for (int rr=r-1; rr>=0; --rr)
        {
            float D=P*B[rr][r]; 

            FOR(c)
            {
                B[rr][c]+=D*B[r][c];
                I[rr][c]+=D*I[r][c];
            }
        }
    }

    FOR(r)
    {
        float D=1.0f/B[r][r];

        FOR(c) I[r][c]*=D;
    }
    
    return 1;
}    

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




