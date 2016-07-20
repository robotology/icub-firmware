#include "EoCommon.h"

#include "EOtheMemoryPool.h"

#include "EOmcController.h"

#include "EOemsControllerCfg.h"

#include "EOtheErrorManager.h"

#include "EoError.h"

#include "EOtheEntities.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"

#include "Controller.h"

#include "hal_led.h"

MController* smc = NULL;

static char invert_matrix(float** M, float** I, char n);
//static void MController_config_motor_set(MController* o);
//static void MController_config_encoder_set(MController* o);

static void send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64)
{

    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = jid;
    errdes.par16            = par16;
    errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);
}

MController* MController_new(uint8_t nJoints, uint8_t nEncods) //
{
    if (!smc) smc = NEW(MController, 1);
    
    MController* o = smc;

    if (!o) return NULL;
        
    o->nEncods = nEncods;
    o->nJoints = nJoints;
    o->nSets   = nJoints;
    
    o->jointSet = JointSet_new(MAX_JOINTS_PER_BOARD);
    
    o->set_dim     = NEW(uint8_t, MAX_JOINTS_PER_BOARD);
    o->enc_set_dim = NEW(uint8_t, MAX_JOINTS_PER_BOARD);
    //o->enc_set_dim = NEW(uint8_t, MAX_ENCODS_PER_BOARD);
    
    o->jos = NEW(uint8_t*, MAX_JOINTS_PER_BOARD);
    o->mos = NEW(uint8_t*, MAX_MOTORS_PER_BOARD);
    o->eos = NEW(uint8_t*, MAX_JOINTS_PER_BOARD);
    //o->eos = NEW(uint8_t*, MAX_ENCODS_PER_BOARD);
    
    o->j2s = NEW(uint8_t, MAX_JOINTS_PER_BOARD);
    o->m2s = NEW(uint8_t, MAX_MOTORS_PER_BOARD);
    o->e2s = NEW(uint8_t, MAX_ENCODS_PER_BOARD);
    
    //o->multi_encs = NEW(uint8_t, MAX_JOINTS_PER_BOARD);
    
    o->joint = Joint_new(MAX_JOINTS_PER_BOARD);
    o->motor = Motor_new(MAX_MOTORS_PER_BOARD); 
    
    o->absEncoder = AbsEncoder_new(MAX_ENCODS_PER_BOARD);
    
    o->Jjm = NEW(float*, MAX_JOINTS_PER_BOARD);
    o->Jmj = NEW(float*, MAX_MOTORS_PER_BOARD);
    
    o->Sjm = NEW(float*, MAX_JOINTS_PER_BOARD);
    o->Sje = NEW(float*, MAX_JOINTS_PER_BOARD);
    
    for (int i=0; i<MAX_JOINTS_PER_BOARD; ++i)
    {
        o->jos[i] = NEW(uint8_t, MAX_JOINTS_PER_BOARD);
        
        o->Jjm[i] = NEW(float, MAX_MOTORS_PER_BOARD);
        o->Sjm[i] = NEW(float, MAX_MOTORS_PER_BOARD);
        o->Sje[i] = NEW(float, MAX_ENCODS_PER_BOARD);
    }

    for (int i=0; i<MAX_MOTORS_PER_BOARD; ++i)
    {
        o->mos[i] = NEW(uint8_t, MAX_MOTORS_PER_BOARD);
        
        o->Jmj[i] = NEW(float, MAX_JOINTS_PER_BOARD);
    }
    
    //for (int i=0; i<MAX_ENCODS_PER_BOARD; ++i)
    for (int i=0; i<MAX_JOINTS_PER_BOARD; ++i)
    {
        o->eos[i] = NEW(uint8_t, MAX_ENCODS_PER_BOARD);
    }
    
    MController_init();
        
    return o;
}

void MController_init() //
{
    MController *o = smc;
    
    if (!o) return;
    
    o->nSets = o->nJoints;
    
    o->multi_encs = 1;
    
    for (int i=0; i<MAX_JOINTS_PER_BOARD; ++i)
    {
        //o->multi_encs[i] = 1;
        
        o->set_dim[i] = 1;
        
        o->j2s[i] = i;

        o->jos[i][0] = i;
        
        for (int k=0; k<MAX_MOTORS_PER_BOARD; ++k)
        {
            o->Jjm[i][k] = o->Sjm[i][k] = (i==k?1.0f:0.0f);
        }
        
        for (int k=0; k<MAX_ENCODS_PER_BOARD; ++k)
        {
            o->Sje[i][k] = (i==k?1.0f:0.0f);
        }
        
        Joint_init(o->joint+i);
        
        o->enc_set_dim[i] = 1;
        
        o->eos[i][0] = i;
    }
    
    for (int i=0; i<MAX_MOTORS_PER_BOARD; ++i)
    {
        o->m2s[i] = i;
        
        o->mos[i][0] = i;

        for (int k=0; k<MAX_JOINTS_PER_BOARD; ++k)
        {
            o->Jmj[i][k] = (i==k?1.0f:0.0f);
        }
        
        Motor_init(o->motor+i);
    }
    
    for (int i=0; i<MAX_ENCODS_PER_BOARD; ++i)
    {        
        AbsEncoder_init(o->absEncoder+i);
    }
}

void MController_deinit()
{
    MController_init();
}

//void MController_config_board(uint8_t part_type, uint8_t actuation_type)
void MController_config_board(const eOmn_serv_configuration_t* brd_cfg)
{
    MController *o = smc;
    
    EOconstarray* carray = NULL;
    
    switch (brd_cfg->type)
    {
        case eomn_serv_MC_foc:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.foc_based.arrayofjomodescriptors);
            o->part_type = brd_cfg->data.mc.foc_based.boardtype4mccontroller;
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.foc_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_2FOC;
            break;
        
        case eomn_serv_MC_mc4plusmais:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.mc4plusmais_based.arrayofjomodescriptors);
            o->part_type = brd_cfg->data.mc.mc4plusmais_based.boardtype4mccontroller;
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plusmais_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_MC4p;
            break;
        
        case eomn_serv_MC_mc4plus:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.mc4plus_based.arrayofjomodescriptors);
            o->part_type = brd_cfg->data.mc.mc4plus_based.boardtype4mccontroller;
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plus_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_MC4p;
            break;
        
        default:
            return;
    }
    
    for (int k=0; k<o->nJoints; ++k)
    {
        const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, k);
        
        switch(jomodes->sensor.type)
        {
            case eomn_serv_mc_sensor_encoder_spichainof2:
            {
                for (int e=0; e<2; ++e)
                {
                    o->absEncoder[k*2+e].type = eomc_encoder_AEA;
                    o->absEncoder[k*2+e].fake = FALSE;
                }
                break;
            }
            case eomn_serv_mc_sensor_encoder_spichainof3:
            {
                for (int e=0; e<3; ++e)
                {
                    o->absEncoder[k*3+e].type = eomc_encoder_AEA;
                    o->absEncoder[k*3+e].fake = FALSE;
                }
                break;
            }                
            case eomn_serv_mc_sensor_encoder_aea:
            {
                o->absEncoder[k].type = eomc_encoder_AEA;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            
            case eomn_serv_mc_sensor_mais:
            {
                o->absEncoder[k].type = eomc_encoder_MAIS;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            
            case eomn_serv_mc_sensor_encoder_absanalog:
            {
                o->absEncoder[k].type = eomc_encoder_HALL_ADC;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            default:
            {
                o->absEncoder[k].fake = TRUE;
                o->absEncoder[k].type = eomc_encoder_NONE;
                break;
            }
        };
        
        o->motor[k].HARDWARE_TYPE = o->actuation_type;
        
        switch(o->motor[k].HARDWARE_TYPE)
        {
            case HARDWARE_MC4p:
                o->motor[k].actuatorPort = jomodes->actuator.pwm.port;
                break;
            
            case HARDWARE_2FOC:
                o->motor[k].actuatorPort = jomodes->actuator.foc.canloc.addr-1;
                break;

            default:
                return;
        }
        
        o->joint[k].eo_joint_ptr = eo_entities_GetJoint(eo_entities_GetHandle(), k);
    }
    
    float **Jjm = NULL; //o->Jjm;
    float **Jmj = NULL; //o->Jmj;
  
    float **Sjm = NULL; //o->Sjm;
    float **Sje = NULL; //o->Sje;
    
    switch (o->part_type)
    {
    case emscontroller_board_ANKLE:                   //= 1,    //2FOC
        o->nSets   = 2;
    
        //invert_matrix(Jjm, Jmj, 2);
    
        for (int k = 0; k<o->nJoints; ++k)
        {            
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = TRUE;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = k;
        }
        
        break;
    
    case emscontroller_board_CER_UPPER_ARM:               //= 17,    //2FOC
        o->nSets   = 4;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        
            o->jointSet[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = TRUE;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = k;
        }
        
        break;

    case emscontroller_board_UPPERLEG:                //= 2,    //2FOC
        o->nSets   = 4;
    
        Jjm = o->Jjm;
        Sjm = o->Sjm;
    
        Sjm[0][0] = 50.0f/75.0f;
        Jjm[0][0] = Sjm[0][0];
    
        //invert_matrix(Jjm, Jmj, 4);
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = TRUE;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = k;
        }
    
        break;
        
    case emscontroller_board_WAIST:                   //= 3,    //2FOC
    {    
        o->nSets   = 1;

        // |j0|   |   0.5   0.5    0   |   |m0|
        // |j1| = |  -0.5   0.5    0   | * |m1|
        // |j2|   | 22/80 22/80  44/80 |   |m2|
        
        Sjm = o->Sjm;
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        
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
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = 0;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = TRUE;
        
        break;
    }
    case emscontroller_board_SHOULDER:                //= 4,    //2FOC
    {
        o->nSets   = 2;
        
        Sjm = o->Sjm;
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        
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
        
        #if defined(ICUB_MEC_V1) || defined(ICUB_GENOVA04)
        // |j0|   |  1     0    0   |   |e0|     
        // |j1| = |  0     1    0   | * |e1|
        // |j2|   |  1    -1  40/65 |   |e2|
        Sje = o->Sje;
        Sje[2][0] = 1.0f; Sje[2][1] = -1.0f; Sje[2][2] = alfa;
        #endif
        
        o->j2s[0] = o->m2s[0] = o->e2s[0] = 0;
        o->j2s[1] = o->m2s[1] = o->e2s[1] = 0;
        o->j2s[2] = o->m2s[2] = o->e2s[2] = 0;
        o->j2s[3] = o->m2s[3] = o->e2s[3] = 1;
        
        for (int k=0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = TRUE;
        
        o->jointSet[1].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[1].CAN_DO_TRQ_CTRL = TRUE;
        
        break;
    }
    case emscontroller_board_CER_WAIST:               //= 15,   //2FOC
        o->nSets   = 2;

        o->j2s[0] = o->m2s[0] = o->e2s[0] = 0;
        o->j2s[1] = o->m2s[1] = o->e2s[1] = 0;
        o->j2s[2] = o->m2s[2] = o->e2s[2] = 0;
        o->j2s[3] = o->m2s[3] = o->e2s[3] = 1;
        
        for (int k=0; k<3; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->joint[3].CAN_DO_TRQ_CTRL = TRUE;
        o->joint[3].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        o->motor[3].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[1].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        o->jointSet[1].CAN_DO_TRQ_CTRL = TRUE;
        
        o->jointSet[0].special_constraint = TRIFID_CONSTRAINT;
        o->jointSet[0].special_limit = WAIST_TRIFID_LIMIT;
    
        break;
        
    case emscontroller_board_CER_LOWER_ARM:               //= 12,   //MC4plus
        o->nSets   = 2;

        o->j2s[0] = o->m2s[0] = o->e2s[0] = 0;
        o->j2s[1] = o->m2s[1] = o->e2s[1] = 0;
        o->j2s[2] = o->m2s[2] = o->e2s[2] = 0;
        o->j2s[3] = o->m2s[3] = o->e2s[3] = 1;
        
        //Sje = o->Sje;
        //Sje[3][3] = 1.0f/64.0f;
    
        for (int k=0; k<3; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[0].special_constraint = TRIFID_CONSTRAINT;
        o->jointSet[0].special_limit = WRIST_TRIFID_LIMIT;
        
        //////////////////////////////////////////////////////
        
        o->joint[3].CAN_DO_TRQ_CTRL = TRUE;
        o->joint[3].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->motor[3].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        
        o->jointSet[1].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[1].CAN_DO_TRQ_CTRL = TRUE;
        
        AbsEncoder_config_divisor(o->absEncoder+3, 64);
        
        break;
        
    case emscontroller_board_CER_BASE:                //= 21    //2FOC
    {
        o->nSets   = 4;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = k;
        }
    }
        break;
    
    case emscontroller_board_CER_NECK:                   //= 22,    //mc4plus
        o->nSets   = 2;
    
        for (int k = 0; k<o->nJoints; ++k)
        {            
            o->joint[k].dead_zone = 0;
            
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = k;
        }
        
        break;
    
    case emscontroller_board_CER_HAND:                   //= 14, 16,    //mc2plus
        o->nSets   = 2;
        o->nEncods = 6;
    
        o->multi_encs = 3;
    
        Sje = o->Sje;

        Sje[0][0] = -1; Sje[0][1] = 1; Sje[0][2] = 0; Sje[0][3] =  0; Sje[0][4] = 0; Sje[0][5] = 0;
        Sje[1][0] =  0; Sje[1][1] = 0; Sje[1][2] = 0; Sje[1][3] = -1; Sje[1][4] = 1; Sje[1][5] = 0;
    
        //Sje[0][0] =  0; Sje[0][1] = 0; Sje[0][2] = 1; Sje[0][3] =  0; Sje[0][4] = 0; Sje[0][5] = 0;
        //Sje[1][0] =  0; Sje[1][1] = 0; Sje[1][2] = 0; Sje[1][3] =  0; Sje[1][4] = 0; Sje[1][5] = 1;
    
        o->e2s[0] = o->e2s[1] = o->e2s[2] = 0;
        o->e2s[3] = o->e2s[4] = o->e2s[5] = 1;
    
        for (int k = 0; k<o->nJoints; ++k)
        {            
            o->joint[k].dead_zone = 400;
            
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;
            o->jointSet[k].USE_SPEED_FBK_FROM_MOTORS = FALSE;
            
            o->j2s[k] = o->m2s[k] = k;
            
            //o->multi_encs[k] = 2;
            
            o->jointSet[k].special_constraint = CER_HAND_CONSTRAINT;
        }
        
        break;    
    
    case emscontroller_board_HEAD_neckpitch_neckroll:      //= 5,    //MC4plus
    {
        o->nSets   = 1;

        // motor to joint
        // |j0|   |   0.5   -0.5  |   |m0|
        // |j1| = |   0.5    0.5  | * |m1|

        Sjm = o->Sjm;
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        
        Sjm[0][0] =  0.5f; Sjm[0][1] = -0.5f;
        Sjm[1][0] =  0.5f; Sjm[1][1] =  0.5f;
    
        for (int j=0; j<o->nJoints; ++j)
            for (int m=0; m<o->nJoints; ++m)
                Jjm[j][m] = Sjm[j][m];
        
        
        //joint to motor
        // |m0|   |   1.0    1.0  |   |j0|
        // |m1| = |  -1.0    1.0  | * |j1|
        
        Jmj[0][0] =  1.0f; Jmj[0][1] =  1.0f;
        Jmj[1][0] = -1.0f; Jmj[1][1] =  1.0f;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            //jiont, motor, encoder to set
            o->j2s[k] = o->m2s[k] = o->e2s[k] = 0;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        break;
    }
    case emscontroller_board_HEAD_neckyaw_eyes:       //=6,      //MC4plus
    {
        o->nSets   = 3;
        
        Sjm = o->Sjm;
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        
        //motor to joint
        
        // |j0|    | 1     0       0      0 |    |m0|
        // |j1|  = | 0     1       0      0 |  * |m1|
        // |j2|    | 0     0      0.5    0.5|    |m2|
        // |j3|    | 0     0     -0.5    0.5|    |m2|

        Sjm[0][0] =  1.0f; Sjm[0][1] =  0.0f; Sjm[0][2] =  0.0f; Sjm[0][3] =  0.0f;
        Sjm[1][0] =  0.0f; Sjm[1][1] =  1.0f; Sjm[1][2] =  0.0f; Sjm[1][3] =  0.0f;
        Sjm[2][0] =  0.0f; Sjm[2][1] =  0.0f; Sjm[2][2] =  0.5f; Sjm[2][3] =  0.5f;
        Sjm[3][0] =  0.0f; Sjm[3][1] =  0.0f; Sjm[3][2] = -0.5f; Sjm[3][3] =  0.5f;

        for (int j=0; j<4; ++j)
            for (int m=0; m<4; ++m)
                Jjm[j][m] = Sjm[j][m];
        

        //inverted matrix: joint to motor
        // |m0|    | 1     0       0      0 |    |j0|
        // |m1|  = | 0     1       0      0 |  * |j1|
        // |m2|    | 0     0       1     -1 |    |j2|
        // |m3|    | 0     0       1      1 |    |j2|
        
        Jmj[0][0] =  1.0f; Jmj[0][1] =  0.0f; Jmj[0][2] =  0.0f; Jmj[0][3] =  0.0f;
        Jmj[1][0] =  0.0f; Jmj[1][1] =  1.0f; Jmj[1][2] =  0.0f; Jmj[1][3] =  0.0f;
        Jmj[2][0] =  0.0f; Jmj[2][1] =  0.0f; Jmj[2][2] =  1.0f; Jmj[2][3] = -1.0f;
        Jmj[3][0] =  0.0f; Jmj[3][1] =  0.0f; Jmj[3][2] =  1.0f; Jmj[3][3] =  1.0f;
        
        
        
        //joint, motor, encoder to set
        o->j2s[0] = o->m2s[0] = o->e2s[0] = 0;
        o->j2s[1] = o->m2s[1] = o->e2s[1] = 1;
        o->j2s[2] = o->m2s[2] = o->e2s[2] = 2;
        o->j2s[3] = o->m2s[3] = o->e2s[3] = 2;
        
        for (int k=0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[1].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[1].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[2].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[2].CAN_DO_TRQ_CTRL = FALSE;
        
        break;
    }

    case emscontroller_board_HAND_thumb:                  //= 9,    //MC4plus
    {
        /*
        NOTE: (VALE)
            In this board, thumb oppose and thumb proximal joints are coupled.
            I get decoupled matrix from mc4(CAN) firmware version 0x228 function "decouple_dutycycle".
            In this function, the fw uses a different decoupled matrix if it runs on left arm or right arm.
            Currently I tested only left arm, so we need to check if the same matrix can be use also on twin board on right arm.
        */
        
        o->nSets   = 3;
        
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        
        //motor to joint
        
        // |j0|    | 1     0       0      0 |    |m0|
        // |j1|  = | 1     1       0      0 |  * |m1|
        // |j2|    | 0     0       1      0 |    |m2|
        // |j3|    | 0     0       0      1 |    |m3|

        Jjm[0][0] =  1.0f; Jjm[0][1] =  0.0f; Jjm[0][2] =  0.0f; Jjm[0][3] =  0.0f;
        Jjm[1][0] =  1.0f; Jjm[1][1] =  1.0f; Jjm[1][2] =  0.0f; Jjm[1][3] =  0.0f;
        Jjm[2][0] =  0.0f; Jjm[2][1] =  0.0f; Jjm[2][2] =  1.0f; Jjm[2][3] =  0.0f;
        Jjm[3][0] =  0.0f; Jjm[3][1] =  0.0f; Jjm[3][2] =  0.0f; Jjm[3][3] =  1.0f;


        //inverted matrix: joint to motor
        // |m0|    | 1     0       0      0 |    |j0|
        // |m1|  = |-1     1       0      0 |  * |j1|
        // |m2|    | 0     0       1      0 |    |j2|
        // |m3|    | 0     0       0      1 |    |j3|
        
        Jmj[0][0] =  1.0f; Jmj[0][1] =  0.0f; Jmj[0][2] =  0.0f; Jmj[0][3] =  0.0f;
        Jmj[1][0] = -1.0f; Jmj[1][1] =  1.0f; Jmj[1][2] =  0.0f; Jmj[1][3] =  0.0f;
        Jmj[2][0] =  0.0f; Jmj[2][1] =  0.0f; Jmj[2][2] =  1.0f; Jmj[2][3] =  0.0f;
        Jmj[3][0] =  0.0f; Jmj[3][1] =  0.0f; Jmj[3][2] =  0.0f; Jmj[3][3] =  1.0f;
        
        
        //joint, motor, encoder to set
        o->j2s[0] = o->m2s[0] = o->e2s[0] = 0;
        o->j2s[1] = o->m2s[1] = o->e2s[1] = 0;
        o->j2s[2] = o->m2s[2] = o->e2s[2] = 1;
        o->j2s[3] = o->m2s[3] = o->e2s[3] = 2;
        
        for (int k=0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[1].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[1].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[2].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[2].CAN_DO_TRQ_CTRL = FALSE;
    }    
    break;
    
    case emscontroller_board_FACE_eyelids_jaw:        //= 7,    //MC4plus
    case emscontroller_board_4jointsNotCoupled:               //= 8,    //MC4plus
        o->nSets   = 4;
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;
            
            o->j2s[k] = o->m2s[k] = o->e2s[k] = k;
        }
        break;
    case emscontroller_board_HAND_2:                  //= 10,   //MC4plus
        break;
    
    case emscontroller_board_FOREARM:                 //= 11,   //MC4plus
    {
        o->nSets   = 3;
        
        Sjm = o->Sjm;
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        
        //motor to joint
        
        // |j0|    | 1     0       0      0 |    |m0|
        // |j1|  = | 0     1       1      0 |  * |m1|
        // |j2|    | 0     0       1      0 |    |m2|
        // |j3|    | 0     0       0      1 |    |m3|

        Sjm[0][0] =  1.0f; Sjm[0][1] =  0.0f; Sjm[0][2] =  0.0f; Sjm[0][3] =  0.0f;
        Sjm[1][0] =  0.0f; Sjm[1][1] =  1.0f; Sjm[1][2] =  1.0f; Sjm[1][3] =  0.0f;
        Sjm[2][0] =  0.0f; Sjm[2][1] =  0.0f; Sjm[2][2] =  1.0f; Sjm[2][3] =  0.0f;
        Sjm[3][0] =  0.0f; Sjm[3][1] =  0.0f; Sjm[3][2] =  0.0f; Sjm[3][3] =  1.0f;

        for (int j=0; j<4; ++j)
            for (int m=0; m<4; ++m)
                Jjm[j][m] = Sjm[j][m];
        

        //inverted matrix: joint to motor
        // |m0|    | 1     0       0      0 |    |j0|
        // |m1|  = | 0     1       0      0 |  * |j1|
        // |m2|    | 0    -1       1      0 |    |j2|
        // |m3|    | 0     0       0      1 |    |j3|
        
        Jmj[0][0] =  1.0f; Jmj[0][1] =  0.0f; Jmj[0][2] =  0.0f; Jmj[0][3] =  0.0f;
        Jmj[1][0] =  0.0f; Jmj[1][1] =  1.0f; Jmj[1][2] =  0.0f; Jmj[1][3] =  0.0f;
        Jmj[2][0] =  0.0f; Jmj[2][1] = -1.0f; Jmj[2][2] =  1.0f; Jmj[2][3] =  0.0f;
        Jmj[3][0] =  0.0f; Jmj[3][1] =  0.0f; Jmj[3][2] =  0.0f; Jmj[3][3] =  1.0f;
        
        
        
        //joint, motor, encoder to set
        o->j2s[0] = o->m2s[0] = o->e2s[0] = 0;
        o->j2s[1] = o->m2s[1] = o->e2s[1] = 1;
        o->j2s[2] = o->m2s[2] = o->e2s[2] = 1;
        o->j2s[3] = o->m2s[3] = o->e2s[3] = 2;
        
        for (int k=0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[1].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[1].CAN_DO_TRQ_CTRL = FALSE;
        
        o->jointSet[2].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[2].CAN_DO_TRQ_CTRL = FALSE;
    }    
    
    
    break;    
    
    
    default:
        return;
    }
    
    //check multi encoder compatibility
    //currently all joint of same board have the the same number of multiple encoder. so i use j0
    const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, 0);
    uint8_t cfg_multienc = (uint8_t) eomn_mc_sensor_getnumofcomponets((eOmn_serv_mc_sensor_type_t)jomodes->sensor.type);
    if(o->multi_encs != cfg_multienc)
    {
        eOerrmanDescriptor_t errdes;
        char str[50];
        snprintf(str, sizeof(str), "multiEnc check: par16=mc par64=cfg");
        errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag04);
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.par16            = o->multi_encs;
        errdes.par64            = cfg_multienc;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);
    
    }
    
    
    for (int s=0; s<o->nSets; ++s) o->enc_set_dim[s] = 0;
    
    for (int e=0; e<o->nEncods; ++e)
    {
        int s = o->e2s[e];

        o->eos[s][(o->enc_set_dim[s])++] = e;
    }
    
    for (int s=0; s<o->nSets; ++s) o->set_dim[s] = 0;
 
    for (int m=0; m<o->nJoints; ++m)
    {
        int s = o->m2s[m];

        o->mos[s][(o->set_dim[s])++] = m;
    }
    
    for (int s=0; s<o->nSets; ++s) o->set_dim[s] = 0;
 
    for (int j=0; j<o->nJoints; ++j)
    {
        int s = o->j2s[j];

        o->jos[s][(o->set_dim[s])++] = j;
    }
    
    for (int s=0; s<o->nSets; ++s)
    {
        JointSet_config
        (
            o->jointSet+s,
            o->set_dim+s,
            o->enc_set_dim+s,
            o->jos[s],
            o->mos[s],
            o->eos[s],
            o->joint, 
            o->motor, 
            o->absEncoder,
            Jjm,
            Jmj,
            Sje,
            Sjm
        );
        
        o->jointSet[s].led = (hal_led_t)(hal_led1 + s);
    }
}
void MController_config_joint(int j, eOmc_joint_config_t* config) //
{
    MController *o = smc;
    
    Joint_config(o->joint+j, j, config);
    
    MController_config_vel_pid(j, &config->pidvelocity);
    
    Motor_config_trqPID(o->motor+j, &(config->pidtorque));
    Motor_config_filter(o->motor+j,   config->tcfiltertype);
    Motor_config_friction(o->motor+j, config->motor_params.bemf_value, config->motor_params.ktau_value);
    
    if (j==3 && o->part_type==emscontroller_board_CER_LOWER_ARM)
    {
        AbsEncoder_config(o->absEncoder+j, j, /*(eOmc_EncoderType_t)config->jntEncoderType,*/ config->jntEncoderResolution, 64*AEA_DEFAULT_SPIKE_MAG_LIMIT, AEA_DEFAULT_SPIKE_CNT_LIMIT);
    }
    else if (o->part_type==emscontroller_board_CER_HAND)
    {
        AbsEncoder_config(o->absEncoder+j*3,   j, /*(eOmc_EncoderType_t)config->jntEncoderType,*/ config->jntEncoderResolution, AEA_DEFAULT_SPIKE_MAG_LIMIT, AEA_DEFAULT_SPIKE_CNT_LIMIT);
        AbsEncoder_config(o->absEncoder+j*3+1, j, /*(eOmc_EncoderType_t)config->jntEncoderType,*/ config->jntEncoderResolution, AEA_DEFAULT_SPIKE_MAG_LIMIT, AEA_DEFAULT_SPIKE_CNT_LIMIT);
        AbsEncoder_config(o->absEncoder+j*3+2, j, /*(eOmc_EncoderType_t)config->jntEncoderType,*/ config->jntEncoderResolution, AEA_DEFAULT_SPIKE_MAG_LIMIT, AEA_DEFAULT_SPIKE_CNT_LIMIT);
    }
    else
    {
        int16_t spike_mag_limit;
        uint16_t spike_cnt_limit;
        if((config->jntEncoderType == eomc_encoder_MAIS) || (config->jntEncoderType == eomc_encoder_HALL_ADC))
        {
            spike_mag_limit = 0;
            spike_cnt_limit = 0;
        }
        else
        {
            spike_mag_limit = AEA_DEFAULT_SPIKE_MAG_LIMIT;
            spike_cnt_limit = AEA_DEFAULT_SPIKE_CNT_LIMIT;
        }
            
        AbsEncoder_config(o->absEncoder+j, j, /*(eOmc_EncoderType_t)config->jntEncoderType,*/ config->jntEncoderResolution, spike_mag_limit, spike_cnt_limit);
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

void MController_motor_config_current_PID(int m, eOmc_PID_t* pidcurrent)
{
    Motor_config_current_PID(smc->motor+m, pidcurrent);
}

void MController_motor_config_max_currents(int m, eOmc_current_limits_params_t* current_params)
{
    Motor_config_max_currents(smc->motor+m, current_params);
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

void MController_update_absEncoder_fbk(uint8_t e, uint32_t* positions) //
{    
    AbsEncoder* enc = smc->absEncoder + e*smc->multi_encs;
    
    for (int k=0; k<smc->multi_encs; ++k)
    {
        AbsEncoder_update(enc++, (uint16_t)positions[k]);
    }
    
#ifdef R1_HAND
    
    MController_update_joint_torque_fbk(e, AbsEncoder_position(smc->absEncoder+(e*smc->multi_encs+2)));
    
    /*
    static int repeat[2] = {0,500};
    
    if (++repeat[e]>=1000)
    {
        repeat[e] = 0;
        
        send_debug_message("Torque", e, smc->joint[e].trq_fbk, 0);
    }
    */
#endif
}

void MController_update_motor_state_fbk(uint8_t m, void* state)
{
    Motor_update_state_fbk(smc->motor+m, state);
}

void MController_invalid_absEncoder_fbk(uint8_t e, eOencoderreader_errortype_t error_type) //
{
    //AbsEncoder_invalid(smc->absEncoder+e, error_type);
    
    AbsEncoder* enc = smc->absEncoder + e*smc->multi_encs;
    
    for (int k=0; k<smc->multi_encs; ++k)
    {
        AbsEncoder_invalid(enc++, error_type);
    }
}

void MController_timeout_absEncoder_fbk(uint8_t e) //
{
    //AbsEncoder_timeout(smc->absEncoder+e);
    
    AbsEncoder* enc = smc->absEncoder + e*smc->multi_encs;
    
    for (int k=0; k<smc->multi_encs; ++k)
    {
        AbsEncoder_timeout(enc++);
    }
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

static char invert_matrix(float** M, float** I, char n)
{
    float B[MAX_JOINTS_PER_BOARD][MAX_JOINTS_PER_BOARD];
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

void MController_calibrate(uint8_t e, eOmc_calibrator_t *calibrator)
{
    JointSet_calibrate(smc->jointSet+smc->e2s[e*smc->multi_encs], e, calibrator);
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
//    static uint32_t count =0;
//    
//    count++;
    
    Joint *j_ptr= smc->joint+j;
    
    Joint_get_state(j_ptr, joint_state);
    
    AbsEncoder* enc_ptr = smc->absEncoder + j*smc->multi_encs;
    
    for (int k=0; k<smc->multi_encs; ++k)
    {
        joint_state->addinfo.multienc[k] = AbsEncoder_position(enc_ptr++);
        //joint_state->addinfo.multienc[k] = count;
    }
    
//    if(count>10000)
//        count = 0;
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

void MController_update_motor_pos_fbk(int m, int32_t position_raw)
{
    Motor_update_pos_fbk(smc->motor+m, position_raw);
}

void MController_update_motor_current_fbk(int m, int16_t current)
{
    Motor_update_current_fbk(smc->motor+m, current);
}

void MController_config_pos_pid(int j, eOmc_PID_t *pid_conf)
{
    Joint* joint = smc->joint+j;
    
    joint->scKpos   = pid_conf->kp;
    joint->scKvel   = pid_conf->kff;
    joint->scKstill = 0.1f*joint->scKpos;
    
    PID_config(&(joint->posPID), pid_conf);
}

void MController_config_vel_pid(int j, eOmc_PID_t *pid_conf)
{
    Joint* joint = smc->joint+j;
    Motor* motor = smc->motor+j;
    
    Motor_config_velPID(motor, pid_conf);
    PID_config(&(joint->velPID), pid_conf);
    
    /*
    pid_conf->control_law = 0;
    
    switch (pid_conf->control_law)
    {
        case 1:
            PID_config(&(joint->velPID), pid_conf);
            break;
        
        case 2:
            Motor_config_velPID(motor, pid_conf);
            break;
        
        default:
            break;
    }
    */
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

void MController_config_motor_encoder(int m, int32_t resolution)
{
    Motor_config_encoder(smc->motor+m, resolution);
}
    
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

////////////////////////////////////////////////////////////////////////////////
