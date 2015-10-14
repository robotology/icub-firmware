
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

static EOmotors *o = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotors* eo_motors_New(uint8_t nMotors, eOemscontroller_board_t board) 
{
    if (!nMotors) return NULL;
    if (nMotors > MAX_NAXLES) nMotors = MAX_NAXLES;
    
    o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotors), 1);

    if (o)
    {
        o->board = board;
        o->nMotors = nMotors;
        o->Jok = eobool_false;
        
        MOTORS(m)
        {
            o->motor_watchdog[m] = 100;
            //o->motor_run_state_last[m] = icubCanProto_controlmode_notConfigured;
            o->motor_run_state_req[m] = icubCanProto_controlmode_notConfigured;
            o->motor_run_state[m] = icubCanProto_controlmode_notConfigured;
            o->motor_run_state_req_wdog[m] = 100;
            o->motor_fault_mask[m] = 0;
            o->motor_qe_error[m] = 0;
            
            JOINTS(j)
            {
                o->J[m][j] = o->Ji[m][j] = 0;
            }
        }
    }

    return o;
}

extern EOmotors* eo_motors_GetHandle()
{
    if(o)
        return o;
    else
        return NULL;
}

extern uint32_t eo_motors_getQEError(EOmotors *o, uint8_t motor)
{
    return o->motor_qe_error[motor];
}

extern void eo_motors_new_state_req(EOmotors *o, uint8_t motor, uint8_t control_mode)
{
    o->motor_run_state_req[motor] = control_mode;
    
    if (o->motor_run_state[motor] == icubCanProto_controlmode_notConfigured)
    {
        o->motor_run_state_req_wdog[motor] = 250;
    }
    else
    {
        o->motor_run_state_req_wdog[motor] = 50;
    }
}

extern void eo_motors_reset_wdog(EOmotors *o, uint8_t motor)
{
    o->motor_watchdog[motor] = 0;
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

extern void eo_motor_get_motor_status(EOmotors *o, uint8_t m, uint8_t *state, uint8_t *state_req)
{
    *state = o->motor_run_state[m];
    *state_req = o->motor_run_state_req[m];
}

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, uint8_t *state)
{   
    if (!state) return;

    //old
    /*
    o->motor_run_state[m] = state[0];
    o->motor_fault_mask[m] = ((uint32_t*)state)[1];
    o->motor_qe_error[m] = (((uint32_t)(((uint16_t*)state)[1]))<<16) | (uint32_t)state[1];
    */
    
    o->motor_run_state[m]  = state[0]; //1byte
    o->motor_fault_mask[m] = *(uint32_t*)&(state[1]); //4 bytes
    o->motor_qe_error[m]   = state[5]; //1byte (don't know if it's right)
    
    o->motor_qe_error[m] = o->motor_qe_error[m];
}

#include "EoError.h"
#include "EOtheErrorManager.h"

extern eObool_t eo_motor_check_state_req(EOmotors *o, uint8_t m)
{
    if (o->motor_run_state[m] == o->motor_run_state_req[m])
    {
        //o->motor_run_state_last[m] = o->motor_run_state[m];
        
        o->motor_run_state_req_wdog[m] = 50;
    
        return eobool_false;
    }
    /*
    if (o->motor_run_state_last[m] != o->motor_run_state[m])
    {                
        return eobool_true;
    }
    */
    if (o->motor_run_state_req_wdog[m]>0)
    {
        --o->motor_run_state_req_wdog[m];
        
        return eobool_false;
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
    else if ((emscontroller_board_UPPERLEG == o->board) || (emscontroller_board_FACE_lips == o->board))
    {    
        pwm_motor[0] = pwm_joint[0];
        pwm_motor[1] = pwm_joint[1];
        pwm_motor[2] = pwm_joint[2];
        pwm_motor[3] = pwm_joint[3];
    }        
    else if ((emscontroller_board_ANKLE == o->board) || (emscontroller_board_FACE_eyelids_jaw == o->board))
    {    
        pwm_motor[0] = pwm_joint[0];
        pwm_motor[1] = pwm_joint[1];
    }
    else if(emscontroller_board_HEAD_neckpitch_neckroll == o->board)
    {
        // use following formula:
        // m0: left motor on neck
        // m1: rigth motor on neck
        // j0: pitch (the head moves forward and backwards)
        // j1: roll (the head moves rigth and left)
        // j0 = 0.5*m0 - 0.5*m1             (we must move the motors in opposite directions to move head forward or backwards)
        // j1 = 0.5*m0 + 0.5*m1             (we must move the motors in the same directions to move head left or right)
        // or:
        // J = M * A, J = [j0, j1], M = {m0, m1], A = [0.5, -0.5], [0.5, 0.5]]
        // hence M = J * A^-1, where A^-1 = [[1, 1,], [-1, 1]].
        // m0 = +j0 + j1
        // m1 = -j0 + j1
        
        // in here i must use the inverse matrix A ....
        // however, as for the second joint we use a negative value of encoderconversionfactor in xml files, if we also want to keep the same sign 
        // of the kp in PID then the matrix is [[-1 -1] [+1 -1]]        
        {
            if (stiff[0]) {pwm_motor[0] = (-pwm_joint[0]-pwm_joint[1]);} else {pwm_motor[0] = pwm_joint[0];}
            if (stiff[1]) {pwm_motor[1] = (+pwm_joint[0]-pwm_joint[1]);} else {pwm_motor[1] = pwm_joint[1];}
        }
    }   
   	else if((emscontroller_board_HEAD_neckyaw_eyes == o->board)) 
    {
        // joints 0 and 1 are independent, 2 and 3 are dependent.

        pwm_motor[0] = pwm_joint[0];
        pwm_motor[1] = pwm_joint[1];
        
        // for j2 and j3 ...
        
        // use following formula:
        // m2: motor position of left eye (as read by encoder)              (L)
        // m3: motor position of rigth eye (as read by encoder)             (R)
        // e2: version of eyes                                              (Vs)
        // e3: vergence of eyes                                             (Vg)
        // e2 = 0.5*m2 + 0.5*m3             (we must move the motors in the same directions to move eyes to follow a direction)
        // e3 = 1.0*m2 - 1.0*m3             (we must move the motors in the opposite directions to do convergence or divergence)
        // or:
        // E = A * M, E = [e2, e3], M = {m2, m3], A = [0.5, +0.5], [1.0, -1.0]]
        // hence M = E * A^-1, where A^-1 = [[1, 0.5,], [1, -0.5]].
        // m2 = +e2 + e3
        // m3 = e2 - 0.5 * e3
        
        // in here i must use the inverse matrix A
        {
            float pwm2jo = pwm_joint[2];
            float pwm3jo = pwm_joint[3];
            uint8_t virtualCalibrationInProgress = 0;
            // notyetbothcalibrated = (0 == pwm2jo) || (0 == pwm3jo); /// qui non siamo sicuri, meglio forse usare una formula piu' complessa.
            
            virtualCalibrationInProgress = eo_emsController_IsVirtualCalibrationInProgress();
            
            if(1 == virtualCalibrationInProgress)
            {
                pwm_motor[2] = pwm_joint[2];
                pwm_motor[3] = pwm_joint[3];                
            }
            else
            {
                // we need to use the inverse of eyesDirectMatrix[] = {+0.500, +0.500, -0.500, +0.500} which is:
                // const float eyesInverseMatrix[] = {+1.000, -1.000, +1.000, +1.000};
                if (stiff[2]) {pwm_motor[2] = ( (+1.000f)*pwm2jo + (-1.000f)*pwm3jo);} else {pwm_motor[2] = pwm_joint[2];}
                if (stiff[3]) {pwm_motor[3] = ( (+1.000f)*pwm2jo + (+1.000f)*pwm3jo);} else {pwm_motor[3] = pwm_joint[3];}                 
            }
        }  
    }
    else    // marco.accame: this board does not have coupled joints
            // davide: to me seems more like that your trying to compute the PWM of an undefined board
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




