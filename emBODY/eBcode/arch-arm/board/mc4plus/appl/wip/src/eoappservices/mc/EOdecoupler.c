
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/

//ciao
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
            o->motor_state_watchdog[m] = 0;
            o->motor_fault_mask2[m] = 0;

            JOINTS(j)
            {
                o->J[m][j] = o->Ji[m][j] = 0;
            }
        }
    }

    return o;
}

extern void eo_motors_reset_wdog(EOmotors *o, uint8_t motor)
{
    o->motor_state_watchdog[motor] = 0;
}

extern void eo_motors_check_wdog(EOmotors *o)
{
    MOTORS(m)
    {
        if (o->motor_state_watchdog[m]>100)
        {
            o->motor_fault_mask2[m] |= MOTOR_CAN_NOT_RESPONDING;
        }
        else
        {
            ++o->motor_state_watchdog[m];
        }
    }
}

extern eObool_t eo_motors_are_coupled(EOmotors *o, uint8_t ma, uint8_t mb)
{
    return o->J[ma][mb] || o->J[mb][ma];
}

/*
    candata.b[0] = SysError.b[0];    
    //candata.b[0] |= SysError.AS5045CalcError << 5; //adjust some values;

        b0  unsigned UnderVoltageFailure:1;      Undervoltage       
        b1  unsigned OverVoltageFailure:1;       Overvoltage
        b2  unsigned ExternalFaultAsserted:1;    External Fault
        b3  unsigned OverCurrentFailure:1;       OverCurrent
        b4  unsigned DHESInvalidValue:1;         an invalid value of HES has been detected
        b5  unsigned AS5045CSumError:1;          //ATTENTION IN MSG PUNT IN OR WITH    AS5045CalcError:1;
        b6  unsigned DHESInvalidSequence:1;      an invalid sequence of HES activation has been detected
        b7  unsigned CANInvalidProtocol:1;       can protocol incompatible with EMS

    candata.b[1] = CanIcubProtoGetcontrol_mode();  //in case of error??

    candata.b[2] = SysError.b[2];
        b0  unsigned ADCCalFailure:1;            ADC calibration failure 
        b1  unsigned I2TFailure:1;               I2T protection                     
        b2  unsigned EMUROMFault:1;              EMUROM Fault
        b3  unsigned EMUROMCRCFault:1;           EMUROM CRC Fault
        b4  unsigned EncoderFault:1;             Encoder Fault. 1 when homing with zero signal fail 
        b5  unsigned FirmwareSPITimingError:1;   SPI reading has been interrupted before finishing by foc loop	
        b6  unsigned AS5045CalcError:1;
        b7  unsigned FirmwarePWMFatalError:1;    This is true when the FOC loop tried to delay the PWM update (see below)
                                                 but the wait for the PWM counter timed out (PWM counter freezed?)
                                                 This should never happen, and that may indicate
                                                 a firmware BUG or abnormal firmware behaviour due to
                                                 unpredictable and exotic reasons (malfunctions, voodoo
                                                 magic, hardware bugs, gravity inversions, alien invasions, 
                                                 end of the world).
                                                 In any case please consider that this is certainly NOT due
                                                 to the firmware developer, but more likely it's electronic 
                                                 eng. full responsibility :-)

    candata.b[3] = SysError.b[3];
        b0  unsigned CAN_TXWasPasv:1;           can has been in TX passive mode at least one time.
        b1  unsigned CAN_RXWasPasv:1;           can has been in RX passive
        b2  unsigned CAN_RTRFlagActive:1;       an RTR flag has been seen over the wire. This is not OK for LorCan
        b3  unsigned CAN_WasWarn:1;             can has been in bus warn at least one time
        b4  unsigned CAN_DLCError:1;            at least one DLC error has been seen
        b5  unsigned SiliconRevisionFault:1;    see comments below
        b6  unsigned PositionLimitUpper:1; 
        b7  unsigned PositionLimitLower:1; 

    candata.b[4] = SysError.b[1];

        b0  unsigned CAN_BufferOverRun:1;        A CAN fifo full event has happened. 
                                                 This should never happen because this FW on the 2FOC should 
                                                 be able to handle CAN with full load. This might happen in 
                                                 certain blocking operation are requested like save to eeprom 
                                                 parameters. TODO: verify it.
        b1  unsigned unused1:1;                  UNUSED: put here for mad in msg
        b2  unsigned CAN_TXIsPasv:1;             can IS is TX passive mode
        b3  unsigned CAN_RXIsPasv:1;             can IS in RX passive mode
        b4  unsigned CAN_IsWarnTX:1;             can IS in bus warn in tx (exist only one error for rx and tx, so the bits are used together)
        b5  unsigned CAN_IsWarnRX:1;             can IS in bus warn in rx
        b6  unsigned unused3:1;                  UNUSED: put here for mad in msg
        b7  unsigned unused4:1;                  UNUSED: put here for mad in msg

    candata.b[5] = SysError.I2TFailure << 5;
    
    candata.b[6] = SysStatus.b[3];
*/

/* from 2FOC
extern void CanIcubProtoTrasmitter_SendStatusMsg()
{
    memset(&candata.b[0], 0, 8);
       
    //prapare the payload
    candata.b[0] = SysError.b[0];    
    b0  unsigned UnderVoltageFailure:1;     Undervoltage       
    b1  unsigned OverVoltageFailure:1;      Overvoltage
    b2  unsigned ExternalFaultAsserted:1;   External Fault
    b3  unsigned OverCurrentFailure:1;      OverCurrent                                        
    b4  unsigned DHESInvalidValue:1;        an invalid value of HES has been detected 
    b5  unsigned AS5045CSumError:1;         | AS5045CalcError:1;
    b6  unsigned DHESInvalidSequence:1;     an invalid sequence of HES activation has been detected
    b7  unsigned CANInvalidProtocol:1;      can protocol incompatible with EMS
    
    candata.b[4] = SysError.b[1];
    b0  unsigned CAN_BufferOverRun:1;        A CAN fifo full event has happened. 
    b1  unsigned unused1:1;                  UNUSED: put here for mad in msg       
    b2  unsigned CAN_TXIsPasv:1;             can IS is TX passive mode
    b3  unsigned CAN_RXIsPasv:1;             can IS in RX passive mode
    b4  unsigned CAN_IsWarnTX:1;             can IS in bus warn in tx (exist only one error for rx and tx, so the bits are used together)
    b5  unsigned CAN_IsWarnRX:1;             can IS in bus warn in rx
    b6  unsigned unused3:1;                  UNUSED: put here for mad in msg
    b7  unsigned unused4:1;                  UNUSED: put here for mad in msg
    
    candata.b[5] = SysError.I2TFailure << 5;
}
*/

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, uint8_t *state)//uint8_t motor_error, uint8_t can_error)
{
    o->motor_fault_mask2[m] = 0; // 
    
    if (state)
    {
        uint8_t motor_error1 = state[0];
        
        if (motor_error1)
        {
            if (motor_error1 & 0x03) o->motor_fault_mask2[m] |= MOTOR_UNDEROVERVOLTAGE;
            if (motor_error1 & 0x04) o->motor_fault_mask2[m] |= MOTOR_EXTERNAL_FAULT;
            if (motor_error1 & 0x08) o->motor_fault_mask2[m] |= MOTOR_OVERCURRENT_FAULT;
            
            if (motor_error1 & 0x50) o->motor_fault_mask2[m] |= MOTOR_HALLSENSORS_FAULT;
            if (motor_error1 & 0x20) o->motor_fault_mask2[m] |= MOTOR_AS5045ERROR;
            if (motor_error1 & 0x80) o->motor_fault_mask2[m] |= MOTOR_CAN_INVALID_PROT;    
        }
        
        uint8_t motor_error2 = state[2];
        
        if (motor_error2)
        {
            if (motor_error2 & 0x02) o->motor_fault_mask2[m] |= MOTOR_I2T_LIMIT_FAULT;
            if (motor_error2 & 0x10) o->motor_fault_mask2[m] |= MOTOR_QENCODER_FAULT;
        }
        
        uint8_t can_error = state[4];
        
        if (can_error)
        {
            o->motor_fault_mask2[m] |= MOTOR_CAN_GENERIC_FAULT;
        }
        
        // old 2FOC fw compatibility
        motor_error2 = state[5];
        
        if (motor_error2)
        {
            if (motor_error2 & 0x20) o->motor_fault_mask2[m] |= MOTOR_I2T_LIMIT_FAULT;
        }
    }
}

extern uint16_t eo_get_motor_fault_mask(EOmotors *o, uint8_t m)
{
    return o->motor_fault_mask2[m];
}

extern eObool_t eo_is_motor_ext_fault(EOmotors *o, uint8_t m)
{
    return o->motor_fault_mask2[m] & MOTOR_EXTERNAL_FAULT;
}

extern eObool_t eo_is_motor_hard_fault(EOmotors *o, uint8_t m)
{
    return o->motor_fault_mask2[m] & MOTOR_HARDWARE_FAULT;
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
    
    //#if defined(SHOULDER_BOARD)
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
    
        {
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
     
    }        
    else if(emscontroller_board_WAIST == o->board)
    {
    //#elif defined(WAIST_BOARD)
        
        //             |   1     1     0   |
        // J = dq/dm = |  -1     1     0   |
        //             | 44/80 44/80 44/80 | 

        //        | 1/2  -1/2    0   |
        // J^-1 = | 1/2   1/2    0   |
        //        | -1     0   80/44 |

        //      | 1  -1  44/80 |
        // Jt = | 1   1  44/80 |
        //      | 0   0  44/80 | 
        
        {
            if (stiff[0]) {pwm_motor[0] = (pwm_joint[0]-pwm_joint[1])/2;} else {pwm_motor[0] = pwm_joint[0];}
            if (stiff[1]) {pwm_motor[1] = (pwm_joint[0]+pwm_joint[1])/2;} else {pwm_motor[1] = pwm_joint[1];}
            if (stiff[2]) {pwm_motor[2] =  pwm_joint[2];                } else {pwm_motor[2] = pwm_joint[2];}
        }
    }        
    else if(emscontroller_board_UPPERLEG == o->board)
    {    
    //#elif defined(UPPERLEG_BOARD)
        {
            pwm_motor[0] = pwm_joint[0];
            pwm_motor[1] = pwm_joint[1];
            pwm_motor[2] = pwm_joint[2];
            pwm_motor[3] = pwm_joint[3];
        }
    }        
    else if((emscontroller_board_ANKLE == o->board) || (emscontroller_board_FACE_eyelids_jaw == o->board))
    {    
    //#elif defined(ANKLE_BOARD)
        {
            pwm_motor[0] = pwm_joint[0];
            pwm_motor[1] = pwm_joint[1];
        }
     
    }
    else if((emscontroller_board_HEAD_neckyaw_eyes == o->board)) 
    {
        #warning TODO: for head v3
        // marco.accame: questo e' un placeholder per mettere le azioni specifiche riguardanti la scheda della head-v3.
        // ovviamente si deve sviluppare gli if-else (o un bel switch-case) per tutte le board head v3. 
        // mettere formula di disaccoppiamento. probabilmente il neck e' come le prime due righe del waist.         
    }
    else if((emscontroller_board_FACE_lips == o->board)) 
    {
        pwm_motor[0] = pwm_joint[0];
        pwm_motor[1] = pwm_joint[1];
        pwm_motor[2] = pwm_joint[2];
        pwm_motor[3] = pwm_joint[3];        
    }
    else    // marco.accame: this board does not have coupled joints
            // davide: to me seems more like that your trying to find the PWM of an undefined board
    {    
    //#else
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




