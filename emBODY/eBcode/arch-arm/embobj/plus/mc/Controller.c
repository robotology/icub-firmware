#include "EoCommon.h"

#include "EOtheMemoryPool.h"

#include "EOmcController.h"

#include "EOemsControllerCfg.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"

#include "Controller.h"

#include "hal_led.h"

MController* smc = NULL;

//static char invert_matrix(float** M, float** I, char n);
//static void MController_config_motor_set(MController* o);
//static void MController_config_encoder_set(MController* o);

MController* MController_new(uint8_t nJoints) //
{
    if (!smc) smc = NEW(MController, 1);
    
    MController* o = smc;

    if (!o) return NULL;
    
    o->nJoints = nJoints;
    o->nSets = nJoints;
    
    o->jointSet = JointSet_new(nJoints);
    
    o->set_dim = NEW(uint8_t, nJoints);
    
    o->jos = NEW(uint8_t*, nJoints);
    o->mos = NEW(uint8_t*, nJoints);
    o->eos = NEW(uint8_t*, nJoints);
    
    o->j2s = NEW(uint8_t, nJoints);
    o->m2s = NEW(uint8_t, nJoints);
    o->e2s = NEW(uint8_t, nJoints);
    
    o->joint = Joint_new(nJoints);
    o->motor = Motor_new(nJoints); 
    
    o->absEncoder = NEW(AbsEncoder, nJoints);
    
    o->Jjm = NEW(float*, nJoints);
    o->Jmj = NEW(float*, nJoints);
    
    o->Sjm = NEW(float*, nJoints);
    o->Sje = NEW(float*, nJoints);
    
    for (int i=0; i<nJoints; ++i)
    {
        o->jos[i] = NEW(uint8_t, nJoints);
        o->mos[i] = NEW(uint8_t, nJoints);
        o->eos[i] = NEW(uint8_t, nJoints);
        
        o->Jjm[i] = NEW(float, nJoints);
        o->Jmj[i] = NEW(float, nJoints);
        
        o->Sjm[i] = NEW(float, nJoints);
        o->Sje[i] = NEW(float, nJoints);
    }
    
    MController_init();
        
    return o;
}

void MController_init() //
{
    MController *o = smc;
    
    if (!o) return;
    
    o->nSets = o->nJoints;
    
    for (int i=0; i<o->nJoints; ++i)
    {
        o->j2s[i] = i;
        o->m2s[i] = i;
        o->e2s[i] = i;
        
        o->set_dim[i] = 1;
        
        o->jos[i][0] = i;
        o->mos[i][0] = i;
        o->eos[i][0] = i;
        
        for (int k=0; k<o->nJoints; ++k)
        {
            o->Jjm[i][k] = o->Jmj[i][k] = o->Sje[i][k] = o->Sjm[i][k] = (i==k?1.0f:0.0f);
        }
    }
}

void MController_config_board(uint8_t part_type, uint8_t actuation_type)
{
    MController *o = smc;
    
    o->part_type      = part_type;
    o->actuation_type = actuation_type;
    
    float **Jjm = o->Jjm;
    float **Jmj = o->Jmj;
  
    float **Sjm = o->Sjm;
    float **Sje = NULL; //o->Sje;
    
    switch (part_type)
    {
    case emscontroller_board_ANKLE:                   //= 1,    //2FOC
        o->nJoints = 2;
        o->nSets   = 2;
    
        //invert_matrix(Jjm, Jmj, 2);
    
        o->j2s[0] = 0; o->m2s[0] = 0;
        o->j2s[1] = 1; o->m2s[1] = 1;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
        
        break;
    
    case emscontroller_board_CER_UPPER_ARM:               //= 17,    //2FOC
        o->nJoints = 2;
        o->nSets   = 2;
    
        o->j2s[0] = 0; o->m2s[0] = 0;
        o->j2s[1] = 1; o->m2s[1] = 1;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        o->jointSet[1].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        
        break;
        
    case emscontroller_board_UPPERLEG:                //= 2,    //2FOC
        o->nJoints = 4;
        o->nSets   = 4;
    
        Sjm[0][0] = 50.0f/75.0f;
        Jjm[0][0] = Sjm[0][0];
    
        //invert_matrix(Jjm, Jmj, 4);
    
        o->j2s[0] = 0; o->m2s[0] = 0;
        o->j2s[1] = 1; o->m2s[1] = 1;
        o->j2s[2] = 2; o->m2s[2] = 2;
        o->j2s[3] = 3; o->m2s[3] = 3;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
    
        break;
    case emscontroller_board_WAIST:                   //= 3,    //2FOC
    {    
        o->nJoints = 3;
        o->nSets   = 1;

        // |j0|   |   0.5   0.5    0   |   |m0|
        // |j1| = |  -0.5   0.5    0   | * |m1|
        // |j2|   | 22/80 22/80  44/80 |   |m2|
        
        float alfa = 22.0f/80.0f;
    
        Sjm[0][0] =  0.5f; Sjm[0][1] =  0.5f; Sjm[0][2] =  0.0f; 
        Sjm[1][0] = -0.5f; Sjm[1][1] =  0.5f; Sjm[1][2] =  0.0f; 
        Sjm[2][0] =  alfa; Sjm[2][1] =  alfa; Sjm[2][2] =  2.0f*alfa;
    
        for (int j=0; j<3; ++j)
            for (int m=0; m<3; ++m)
                Jjm[j][m] = Sjm[j][m];
        
        // beware: the 3rd joint (yaw) is considered independent in position control
        //invert_matrix(Jjm, Jmj, 3);
        Jmj[0][0] =  0.5f; Jmj[0][1] = -0.5f;
        Jmj[1][0] =  0.5f; Jmj[1][1] =  0.5f;
        
        o->j2s[0] = 0; o->m2s[0] = 0;
        o->j2s[1] = 0; o->m2s[1] = 0;
        o->j2s[2] = 0; o->m2s[2] = 0;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
        
        break;
    }
    case emscontroller_board_SHOULDER:                //= 4,    //2FOC
    {
        o->nJoints = 4;
        o->nSets   = 2;
        
        // |j0|    | 1     0       0   |   |m0|
        // |j1|  = | 1   40/65     0   | * |m1|
        // |j2|    | 0  -40/65   40/65 |   |m2|

        float alfa = 40.0f/65.0f;
    
        Sjm[0][0] =  1.0f; Sjm[0][1] =  0.0f; Sjm[0][2] =  0.0f; Sjm[0][3] =  0.0f;
        Sjm[1][0] =  1.0f; Sjm[1][1] =  alfa; Sjm[1][2] =  0.0f; Sjm[1][3] =  0.0f;
        Sjm[2][0] =  0.0f; Sjm[2][1] = -alfa; Sjm[2][2] =  alfa; Sjm[2][3] =  0.0f;
        Sjm[3][0] =  0.0f; Sjm[3][1] =  0.0f; Sjm[3][2] =  0.0f; Sjm[3][3] =  1.0f;

        for (int j=0; j<4; ++j)
            for (int m=0; m<4; ++m)
                Jjm[j][m] = Sjm[j][m];
        
        // beware: the 3rd joint is considered independent in position control
        //invert_matrix(Jjm, Jmj, 4);
        Jmj[1][0] = -1.0f/alfa; Jmj[1][1] =  1.0f/alfa;
        Jmj[2][2] =  1.0f/alfa;
        
        #if defined(V1_MECHANICS)
        // |j0|   |  1     0    0   |   |e0|     
        // |j1| = |  0     1    0   | * |e1|
        // |j2|   |  1    -1  40/65 |   |e2|
        Sje = o->Sje;
        Sje[2][0] = 1.0f; Sje[2][1] = -1.0f; Sje[2][2] = alfa;
        #endif
        
        o->j2s[0] = 0; o->m2s[0] = 0;
        o->j2s[1] = 0; o->m2s[1] = 0;
        o->j2s[2] = 0; o->m2s[2] = 0;
        o->j2s[3] = 1; o->m2s[3] = 1;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
        
        break;
    }
    case emscontroller_board_CER_WAIST:               //= 15,   //2FOC
        o->nJoints = 4;
        o->nSets   = 2;

        o->j2s[0] = 0; o->m2s[0] = 0; // tripod
        o->j2s[1] = 0; o->m2s[1] = 0; // tripod
        o->j2s[2] = 0; o->m2s[2] = 0; // tripod
        o->j2s[3] = 1; o->m2s[3] = 1; // yaw
        
        for (int k = 0; k<3; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
        
        o->joint[3].CAN_DO_TRQ_CTRL = TRUE;
        o->joint[3].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        o->motor[3].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        o->motor[3].HARDWARE_TYPE = HARDWARE_2FOC;
    
        break;
	case emscontroller_board_CER_BASE:                //= 21    //2FOC
    {
        o->nJoints = 4;
        o->nSets   = 4;
        
        o->j2s[0] = 0; o->m2s[0] = 0;
        o->j2s[1] = 1; o->m2s[1] = 1;
        o->j2s[2] = 2; o->m2s[2] = 2;
        o->j2s[3] = 3; o->m2s[3] = 3;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].HARDWARE_TYPE = HARDWARE_2FOC;
        }
    }
        break;
    case emscontroller_board_HEAD_neckpitch_neckroll: //= 5,    //MC4plus
        break;
    case emscontroller_board_HEAD_neckyaw_eyes:       //= 6,    //MC4plus
        break;
    case emscontroller_board_FACE_eyelids_jaw:        //= 7,    //MC4plus
        break;
    case emscontroller_board_FACE_lips:               //= 8,    //MC4plus
        break;
    case emscontroller_board_HAND_1:                  //= 9,    //MC4plus
        break;
    case emscontroller_board_HAND_2:                  //= 10,   //MC4plus
        break;
    case emscontroller_board_FOREARM:                 //= 11,   //MC4plus
        break;
    case emscontroller_board_CER_WRIST:               //= 12,   //MC4plus
        break;
    default:
        return;
    }
    
    for (int s=0; s<o->nSets; ++s) o->set_dim[s] = 0;
 
    for (int j=0; j<o->nJoints; ++j)
    {
        int s = o->j2s[j];

        o->jos[s][(o->set_dim[s])++] = j;
    }

    for (int s=0; s<o->nSets; ++s) o->set_dim[s] = 0;
 
    for (int m=0; m<o->nJoints; ++m)
    {
        int s = o->m2s[m];

        o->mos[s][(o->set_dim[s])++] = m;
    }

    for (int j=0; j<o->nJoints; ++j)
    {
        o->e2s[j] = o->j2s[j]; 
    }
    
    for (int s=0; s<o->nSets; ++s)
    {
        for (int n=0; n<o->set_dim[n]; ++n)
        {
            o->eos[s][n] = o->jos[s][n];
        }
    }
    
    for (int s=0; s<o->nSets; ++s)
    {
        JointSet_config
        (
            o->jointSet+s,
            o->set_dim+s,
            o->jos[s],
            o->mos[s],
            o->eos[s],
            o->joint, 
            o->motor, 
            Jjm,
            Jmj,
            Sje,
            Sjm,
            o->absEncoder
        );
        
        o->jointSet[s].led = (hal_led_t)(hal_led1 + s);
    }
}

void MController_config_joint(int j, eOmc_joint_config_t* config) //
{
    MController *o = smc;
    
    Joint_config(o->joint+j, j, config);
    
    // TODOALE move to motor config
    Motor_config_trqPID(o->motor+j, &(config->pidtorque));
    Motor_config_filter(o->motor+j,   config->tcfiltertype);
    Motor_config_friction(o->motor+j, config->motor_params.bemf_value, config->motor_params.ktau_value);
    
    switch(config->jntEncoderType)
    {
        case eomc_encoder_AEA:
            AbsEncoder_config(o->absEncoder+j, j, config->jntEncoderResolution, AEA_DEFAULT_SPIKE_MAG_LIMIT, AEA_DEFAULT_SPIKE_CNT_LIMIT);
            break;
        
        default:
            AbsEncoder_config_fake(o->absEncoder+j, j);
            break;
    }
}

void MController_config_motor(int m, eOmc_motor_config_t* config) //
{
    Motor_config(smc->motor+m, m, config);
}

void MController_config_motor_friction(int m, eOmc_motor_params_t* friction) //
{
    Motor_config_friction(smc->motor+m, friction->bemf_value, friction->ktau_value);
}

void MController_config_joint_impedance(int j, eOmc_impedance_t* impedance) //
{
    Joint_set_impedance(smc->joint+j, impedance);
}

/*
void MController_config_Jjm(float **Jjm) //
{
    MController *o = smc;
    
    int N = o->nJoints;
    
    for (int j=0; j<N; ++j)
    {
        for (int m=0; m<N; ++m)
        {
            o->Jjm[j][m] = Jjm[j][m];
        }
    }
    
    invert_matrix(Jjm, o->Jmj, N);

    for (int m=0; m<N; ++m)
    {
        for (int j1=0; j1<N; ++j1)
        {
            for (int j2=0; j2<N; ++j2) if (j1!=j2)
            {
                if (Jjm[j1][m] != 0.0f && Jjm[j2][m] != 0.0f)
                {
                    if (o->j2s[j2] > o->j2s[j1])
                    {
                        int s = o->j2s[j2];

                        for (int j=0; j<N; ++j)
                        {
                            if (o->j2s[j] == s) o->j2s[j] = o->j2s[j1]; 
                        }
                    }
                }
            }
        }
    }
    
    for (int s=0; s<N; ++s)
    {   
        o->set_dim[s] = 0;
    }
    
    for (int j=0; j<N; ++j)
    {
        int s = o->j2s[j];

        o->jos[s][o->set_dim[s]++] = j;
    }

    o->nSets = 0;
    
    for (int s=0; s<N; ++s)
    {
        if (o->set_dim[s])
        {
            o->set_dim[o->nSets] = o->set_dim[s];
            
            for (int j=0; j<o->set_dim[s]; ++j)
            {
                o->jos[o->nSets][j] = o->jos[s][j];
            }

            ++o->nSets;
        }
    }
    
    MController_config_motor_set(o);
    MController_config_encoder_set(o);
}
*/
/*
void MController_config_Jje(float **Jje) //
{
    MController *o = smc;
    
    int N = o->nJoints;

    for (int j=0; j<N; ++j)
    { 
        for (int e=0; e<N; ++e)
        {
            o->Jje[j][e] = Jje[j][e];
        }
    }
        
    for (int e=0; e<N; ++e)
    {
        for (int j1=0; j1<N; ++j1)
        {
            for (int j2=0; j2<N; ++j2) if (j1!=j2)
            {
                if (Jje[j1][e] != 0.0f && Jje[j2][e] != 0.0f)
                {
                    if (o->j2s[j2] > o->j2s[j1])
                    {
                        int s = o->j2s[j2];

                        for (int j=0; j<N; ++j)
                        {
                            if (o->j2s[j] == s) o->j2s[j] = o->j2s[j1];
                        }
                    }
                }
            }
        }
    }
    
    for (int s=0; s<N; ++s)
    {   
        o->set_dim[s] = 0;
    }
    
    for (int j=0; j<N; ++j)
    {
        int s = o->j2s[j];

        o->jos[s][o->set_dim[s]++] = j;
    }
    
    for (int s=0; s<N; ++s)
    {
        if (o->set_dim[s])
        {
            o->set_dim[o->nSets] = o->set_dim[s];
            
            for (int j=0; j<o->set_dim[s]; ++j)
            {
                o->jos[o->nSets][j] = o->jos[s][j];
            }

            ++o->nSets;
        }
    }

    MController_config_motor_set(o);
    MController_config_encoder_set(o);
}
*/

void MController_update_joint_torque_fbk(uint8_t j, CTRL_UNITS trq_fbk) //
{
    Joint_update_torque_fbk(smc->joint+j, trq_fbk);
}

void MController_update_absEncoder_fbk(uint8_t e, int32_t position) //
{
    AbsEncoder_update(smc->absEncoder+e, position);
}

void MController_update_motor_state_fbk(uint8_t m, void* state)
{
    Motor_update_state_fbk(smc->motor+m, state);
}

void MController_invalid_absEncoder_fbk(uint8_t e, hal_spiencoder_errors_flags error_flags) //
{
    AbsEncoder_invalid(smc->absEncoder+e, error_flags);
}

void MController_timeout_absEncoder_fbk(uint8_t e) //
{
    AbsEncoder_timeout(smc->absEncoder+e);
}

int32_t MController_get_absEncoder(uint8_t j)
{
    return AbsEncoder_position(smc->absEncoder+j);
}

void MController_do()
{    
    for (int s=0; s<smc->nSets; ++s)
    {
        JointSet_do(smc->jointSet+s);
    }
    
    Motor_actuate(smc->motor, smc->nJoints);
}

BOOL MController_set_control_mode(uint8_t j, eOmc_controlmode_command_t control_mode) //
{
    return JointSet_set_control_mode(smc->jointSet+smc->j2s[j], control_mode);
}    

void MController_set_interaction_mode(uint8_t j, eOmc_interactionmode_t interaction_mode) //
{
    JointSet_set_interaction_mode(smc->jointSet+smc->j2s[j], interaction_mode);
} 

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

/*
static void MController_config_motor_set(MController *o)
{
    int N = o->nJoints;
    
    uint8_t set_dim[MAX_PER_BOARD];
    
    for (int s=0; s<o->nSets; ++s)
    {
        for (int i=0; i<o->set_dim[s]; ++i)
        {
            int j = o->jos[s][i];

            //o->j2s[j] = s;

            for (int m=0; m<N; ++m)
            {
                if (o->Jjm[j][m] != 0.0f)
                {
                    o->m2s[m] = s;
                }
            }
        }
        
        set_dim[s] = 0;
    }

    for (int m=0; m<N; ++m)
    {
        int s = o->m2s[m];

        o->mos[s][set_dim[s]++] = m;
    }
}
*/
/*
static void MController_config_encoder_set(MController *o)
{
    int N = o->nJoints;
    
    uint8_t set_dim[MAX_PER_BOARD];
    
    for (int s=0; s<o->nSets; ++s)
    {
        for (int i=0; i<o->set_dim[s]; ++i)
        {
            int j = o->jos[s][i];

            //o->j2s[j] = s;

            for (int e=0; e<N; ++e)
            {
                if (o->Jje[j][e] != 0.0f)
                {
                    o->e2s[e] = s;
                }
            }
        }
        
        set_dim[s] = 0;
    }

    for (int e=0; e<N; ++e)
    {
        int s = o->e2s[e];

        o->eos[s][set_dim[s]++] = e;
    }
}
*/

#define FOR(i) for (int i=0; i<n; ++i)
#define SCAN(r,c) FOR(r) FOR(c)

/*
static char invert_matrix(float** M, float** I, char n)
{
    float B[MAX_PER_BOARD][MAX_PER_BOARD];
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
*/

void MController_calibrate(uint8_t e, eOmc_calibrator_t *calibrator)
{
    JointSet_calibrate(smc->jointSet+smc->e2s[e], e, calibrator);
}

void MController_go_idle(void)
{
    for (uint8_t s=0; s<smc->nSets; ++s)
    {
        JointSet_set_control_mode(smc->jointSet+s, eomc_controlmode_cmd_force_idle);
    }
}

////////////////////////////////////////////////////////////////////////////////

void MController_get_joint_state(int j, eOmc_joint_status_t* joint_state)
{
    Joint_get_state(smc->joint+j, joint_state);
}


void MController_get_pid_state(int j, eOmc_joint_status_ofpid_t* pid_state, BOOL decoupled_pwm)
{    
    if (Joint_get_pid_state(smc->joint+j, pid_state))
    {
        Motor_get_pid_state(smc->motor+j, pid_state);
    }
    
    if (decoupled_pwm)
    {
        pid_state->complpos.output = smc->motor[j].output;
    }
}

void MController_get_motor_state(int m, eOmc_motor_status_t* motor_status)
{
    Motor_get_state(smc->motor+m, motor_status);
}

void MController_update_motor_pos_fbk(int m, int32_t position)
{
    Motor_update_pos_fbk(smc->motor+m, position);
}

void MController_update_motor_current_fbk(int m, int16_t current)
{
    Motor_update_current_fbk(smc->motor+m, current);
}

void MController_config_pos_pid(int j, eOmc_PID_t *pid_conf)
{
    PID_config(&smc->joint[j].posPID, pid_conf);
}

void MController_config_trq_pid(int m, eOmc_PID_t *pid_conf)
{
    PID_config(&smc->motor[m].trqPID, pid_conf);
}

void MController_config_joint_pos_limits(int j, int32_t pos_min, int32_t pos_max)
{
    Joint_set_limits(smc->joint+j, pos_min, pos_max);
}

void MController_config_joint_vel_ref_timeout(int j, int32_t timeout_ms)
{
    WatchDog_set_base_time_msec(&smc->joint[j].vel_ref_wdog, timeout_ms);
}

BOOL MController_set_joint_pos_ref(int j, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref)
{
    return Joint_set_pos_ref(smc->joint+j, pos_ref, vel_ref);
}

BOOL MController_set_joint_vel_ref(int j, CTRL_UNITS vel_ref, CTRL_UNITS acc_ref)
{
    return Joint_set_vel_ref(smc->joint+j, vel_ref, acc_ref);
}

BOOL MController_set_joint_pos_raw(int j, CTRL_UNITS pos_ref)
{
    return Joint_set_pos_raw(smc->joint+j, pos_ref);
}

BOOL MController_set_joint_trq_ref(int j, CTRL_UNITS trq_ref)
{
    return Joint_set_trq_ref(smc->joint+j, trq_ref);
}

BOOL MController_set_joint_out_ref(int j, CTRL_UNITS out_ref)
{
    return Joint_set_out_ref(smc->joint+j, out_ref);
}

void MController_stop_joint(int j)
{
    Joint_stop(smc->joint+j);
}

void MController_config_motor_gearbox_ratio(int m, int32_t gearbox_ratio)
{
    Motor_config_gearbox_ratio(smc->motor+m, gearbox_ratio);
}

void empty_fake_MController_config_motor_encoder(int m, int32_t resolution){}
    
int16_t MController_config_motor_pwm_limit(int m, int16_t pwm_limit)
{
    return Motor_config_pwm_limit(smc->motor+m, pwm_limit);
}

void MController_update_motor_odometry_fbk_can(int m, void* data)
{
    Motor_update_odometry_fbk_can(smc->motor+m, data);
}

void MController_motor_raise_fault_i2t(int m)
{
    Motor_raise_fault_i2t(smc->motor+m);
}

void MController_motor_raise_fault_overcurrent(int m)
{
    Motor_raise_fault_overcurrent(smc->motor+m);
}

void MController_motor_raise_fault_external(int m)
{
    Motor_raise_fault_external(smc->motor+m);
}

BOOL MController_motor_is_external_fault(int m)
{
    return Motor_is_external_fault(smc->motor+m);
}

////////////////////////////////////////////////////////////////////////////////
