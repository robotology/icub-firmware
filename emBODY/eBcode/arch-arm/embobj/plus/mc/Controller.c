/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#include "EoCommon.h"

#include "EOtheMemoryPool.h"


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

#include "hal_trace.h"

MController* smc = NULL;

#include "stdio.h"

static char invert_matrix(float** M, float** I, char n);
//static void MController_config_motor_set(MController* o);
//static void MController_config_encoder_set(MController* o);


//static void send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64)
//{

//    eOerrmanDescriptor_t errdes = {0};

//    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = jid;
//    errdes.par16            = par16;
//    errdes.par64            = par64;
//    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, message, NULL, &errdes);
//}


static char s_trace_string[128] = {0};

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
    
    o->Jjm = NEW(float*, MAX_JOINTS_PER_BOARD); o->Sjm = NEW(float*, MAX_JOINTS_PER_BOARD); 
    
    o->Jmj = NEW(float*, MAX_MOTORS_PER_BOARD); o->Smj = NEW(float*, MAX_MOTORS_PER_BOARD);
    
    o->Sje = NEW(float*, MAX_JOINTS_PER_BOARD);
    
    for (int i=0; i<MAX_JOINTS_PER_BOARD; ++i)
    {
        o->jos[i] = NEW(uint8_t, MAX_JOINTS_PER_BOARD);
        
        o->Jjm[i] = NEW(float, MAX_MOTORS_PER_BOARD); o->Sjm[i] = NEW(float, MAX_MOTORS_PER_BOARD);
        
        o->Sje[i] = NEW(float, MAX_ENCODS_PER_BOARD);
    }

    for (int i=0; i<MAX_MOTORS_PER_BOARD; ++i)
    {
        o->mos[i] = NEW(uint8_t, MAX_MOTORS_PER_BOARD);
        
        o->Jmj[i] = NEW(float, MAX_JOINTS_PER_BOARD); o->Smj[i] = NEW(float, MAX_JOINTS_PER_BOARD);
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
        
        o->e2s[i] = i;

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
            o->Jmj[i][k] = o->Smj[i][k] = (i==k?1.0f:0.0f);
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

static uint8_t getNumberOfSets(const eOmc_4jomo_coupling_t *jomoCouplingInfo)
{
    uint8_t n=0;
    for(uint8_t i=0; i<MAX_JOINTS_PER_BOARD; i++)
    {
        if((jomoCouplingInfo->joint2set[i] != eomc_jointSetNum_none) && (jomoCouplingInfo->joint2set[i] > n))
        {
            n = jomoCouplingInfo->joint2set[i];
        }
    }
    n+=1;
    return (n);
}

static void updateEntity2SetMaps(const eOmc_4jomo_coupling_t *jomoCouplingInfo,  EOconstarray* carray)
{
    MController *o = smc;
    for(uint8_t i=0; i<MAX_JOINTS_PER_BOARD; i++)
    {
        if(eomc_jointSetNum_none == jomoCouplingInfo->joint2set[i])
            continue; //leave init values
        o->j2s[i] = o->m2s[i] = jomoCouplingInfo->joint2set[i];
    }
    
    //in the following code I fill data in e2s vector.
    //in almost all boards, for each joint j:  (o->j2s[j] == o->e2s[j]== o->m2s[j] == jomoCouplingInfo->joint2set[j] ) is true
    //but in CER HAND board we have only two joints and three encoder of each joint; therefore the e2s={0,0,0, 1,1,1}
    //The following code fills data in e2s vecotor keeping in mind the CER hand board expection.
    uint8_t currentpos = 0;
    const eOmc_jomo_descriptor_t *jomodes ;
    for(uint8_t j=0; j<o->nJoints; j++)
    {
        jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, j);
        uint8_t multi_enc = (uint8_t) eomc_encoder_get_numberofcomponents((eOmc_encoder_t)jomodes->encoder1.type);
        //VALE2: horrible work around about multi_encs
        if (multi_enc == 0) //when in encoder1 xml group there is none.
            multi_enc = 1;
        for(uint8_t i=currentpos; i<currentpos+multi_enc; i++)
        {
            o->e2s[i] = jomoCouplingInfo->joint2set[j];
        }
        currentpos += multi_enc;
    }
    
    //currently all joint of same board have the the same number of multiple encoder. so i use j0
    jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, 0);
    o->multi_encs = (uint8_t) eomc_encoder_get_numberofcomponents((eOmc_encoder_t)jomodes->encoder1.type);
    //VALE: horrible work around about multi_encs
    if (o->multi_encs == 0) //when in encoder1 xml group there is none.
        o->multi_encs = 1;
    o->nEncods = o->multi_encs *o->nJoints;
    
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
}


static void debug_printsMatrix4X4(float **m)
{
    snprintf(s_trace_string, sizeof(s_trace_string), "----- START -----");
    hal_trace_puts(s_trace_string);
    
    for(uint8_t r=0; r<4; r++)
    {
        snprintf(s_trace_string, sizeof(s_trace_string), "%.4f  %.4f  %.4f  %.4f", m[r][0], m[r][1], m[r][2], m[r][3]);
        hal_trace_puts(s_trace_string);
    }
    
    snprintf(s_trace_string, sizeof(s_trace_string), "----- END -----");
    hal_trace_puts(s_trace_string);

}

static void debug_printsMatrix4X6(float **m)
{
    snprintf(s_trace_string, sizeof(s_trace_string), "----- START -----");
    hal_trace_puts(s_trace_string);
    
    for(uint8_t r=0; r<4; r++)
    {
        snprintf(s_trace_string, sizeof(s_trace_string), "%.4f  %.4f  %.4f  %.4f  %.4f  %.4f", m[r][0], m[r][1], m[r][2], m[r][3], m[r][4], m[r][5]);
        hal_trace_puts(s_trace_string);
    }
    
    snprintf(s_trace_string, sizeof(s_trace_string), "----- END -----");
    hal_trace_puts(s_trace_string);

}

static void copyMatrix4X4(float **dst, const eOmc_4x4_matrix_t src)
{
    int N = MAX_JOINTS_PER_BOARD <= 4 ? MAX_JOINTS_PER_BOARD : 4;
    
    for(uint8_t r=0; r<N; r++)
    {
        for(uint8_t c=0; c<N; c++)
        {
            dst[r][c] = eo_common_Q17_14_to_float(src[r][c]);
        }
    }
}


static void copyMatrix4X6(float **dst, const eOmc_4x6_matrix_t src)
{
    int R = MAX_JOINTS_PER_BOARD <= 4 ? MAX_JOINTS_PER_BOARD : 4;
    int C = MAX_ENCODS_PER_BOARD <= 6 ? MAX_ENCODS_PER_BOARD : 6;
        
    for(uint8_t r=0; r<R; r++)
    {
        for(uint8_t c=0; c<C; c++)
        {
            dst[r][c] = eo_common_Q17_14_to_float(src[r][c]);
        }
    }
}

extern void debug_dump_coupling_data(const eOmc_4jomo_coupling_t *jomoCouplingInfo)
{
     MController *o = smc;
    
    snprintf(s_trace_string, sizeof(s_trace_string), "Numofj=%d nEncs=%d multienc=%d numofsets=%d\n", o->nJoints, o->nEncods, o->multi_encs, o->nSets);
    //hal_trace_puts(s_trace_string);
    eo_errman_Trace(eo_errman_GetHandle(), s_trace_string, "pippo");
    
    snprintf(s_trace_string, sizeof(s_trace_string), "MATRIX Jmj");
    //hal_trace_puts(s_trace_string);
    eo_errman_Trace(eo_errman_GetHandle(), s_trace_string, "nn");
    debug_printsMatrix4X4(o->Jmj);
    
    snprintf(s_trace_string, sizeof(s_trace_string), "MATRIX Jjm");
    hal_trace_puts(s_trace_string);
    debug_printsMatrix4X4(o->Jjm);
    
    snprintf(s_trace_string, sizeof(s_trace_string), "MATRIX Sje");
    hal_trace_puts(s_trace_string);
    debug_printsMatrix4X6(o->Sje);
    
     snprintf(s_trace_string, sizeof(s_trace_string), "MATRIX Sjm");
    hal_trace_puts(s_trace_string);
    debug_printsMatrix4X4(o->Sjm);
    

    snprintf(s_trace_string, sizeof(s_trace_string), "J2S:  %d  %d  %d  %d\n", o->j2s[0], o->j2s[1], o->j2s[2], o->j2s[3]);
    hal_trace_puts(s_trace_string);
    
    snprintf(s_trace_string, sizeof(s_trace_string), "m2s:  %d  %d  %d  %d\n", o->m2s[0], o->m2s[1], o->m2s[2], o->m2s[3]);
    hal_trace_puts(s_trace_string);
    
    snprintf(s_trace_string, sizeof(s_trace_string), "e2s:  %d  %d  %d  %d  %d  %d\n", o->e2s[0], o->e2s[1], o->e2s[2], o->e2s[3], o->e2s[4], o->e2s[5]);
    hal_trace_puts(s_trace_string);
    
    
    for(int i=0; i<o->nJoints; i++)
    {
        snprintf(s_trace_string, sizeof(s_trace_string), "\nCFG JOINTSET %d", i);
        hal_trace_puts(s_trace_string);
        
        //snprintf(s_trace_string, sizeof(s_trace_string), "trq=%d  %s  mspeed=%d \n", jomoCouplingInfo->jsetcfg[i].candotorquecontrol,
        //          eomc_pidoutputtype2string((eOmc_pidoutputtype_t)(jomoCouplingInfo->jsetcfg[i].pidoutputtype), eobool_false), 
        //         jomoCouplingInfo->jsetcfg[i].usespeedfeedbackfrommotors);
        //hal_trace_puts(s_trace_string);
        
       const eOmc_jointSet_constraints_t *constr = &jomoCouplingInfo->jsetcfg[i].constraints;           
        snprintf(s_trace_string, sizeof(s_trace_string), "%s  %.2f  %.2f\n", 
                    eomc_jsetconstraint2string((eOmc_jsetconstraint_t)(constr->type), eobool_false),constr->param1, constr->param2);
        hal_trace_puts(s_trace_string);
    }


}

static void update_jointAndMotor_withJointset_constraints(void)
{
    //currently I use constarint to set configuration info that can not be set by xml.
    //In the future, (I hope ASAP) following parameters will be  set by xml file, so I'll remove this function
    // now I could not change xml configuration without change yarp interface....
    
    MController *o = smc;
    for(int s=0; s<o->nSets; s++)
    {
        if(o->jointSet[s].special_constraint == eomc_jsetconstraint_cerhand)
        {
            for(int i=0; i<o->set_dim[s]; i++)
            {
                int j = o->jos[s][i];
                //o->joint[j].dead_zone = 400.0f;
                o->joint[j].not_reversible = TRUE;
            }
        }

        if(o->jointSet[s].special_constraint == eomc_jsetconstraint_cerhand2)
        {
            for(int i=0; i<o->set_dim[s]; i++)
            {
                int j = o->jos[s][i];
                //o->joint[j].dead_zone = 400.0f;
                o->joint[j].not_reversible = TRUE;
            }
        }
        
        if(o->jointSet[s].special_constraint == eomc_jsetconstraint_trifid)
        {
            for(int i=0; i<o->set_dim[s]; i++)
            {
                int j = o->jos[s][i];
                //o->joint[j].dead_zone = 1.0f;
                o->joint[j].not_reversible = TRUE;
            }
        }
    }
}

static void update_jointAndMotor_withJointSet_configuration(void)
{    
    //provisional function. see comment inside
    update_jointAndMotor_withJointset_constraints();
}


static void get_jomo_coupling_info(const eOmc_4jomo_coupling_t *jomoCouplingInfo, EOconstarray* carray)
{
    MController *o = smc;
    
    o->nSets = getNumberOfSets(jomoCouplingInfo);

    copyMatrix4X6(o->Sje, jomoCouplingInfo->encoder2joint);
    
    copyMatrix4X4(o->Jmj, jomoCouplingInfo->joint2motor);
    copyMatrix4X4(o->Jjm, jomoCouplingInfo->motor2joint);
    copyMatrix4X4(o->Sjm, jomoCouplingInfo->motor2joint);
    
    invert_matrix(o->Sjm, o->Smj, 4);
    
    

    updateEntity2SetMaps(jomoCouplingInfo, carray);
    
    const eOmc_jointset_configuration_t   *jsetcfg = &jomoCouplingInfo->jsetcfg[0];
    
    for(int s=0; s<o->nSets; s++)
    {
        //#error implement motor input configuration 
        //o->jointSet[s].CAN_DO_TRQ_CTRL = jsetcfg[s].candotorquecontrol;
        //o->jointSet[s].default_motor_input = jsetcfg[s].pidoutputtype;
        
        o->jointSet[s].olooop_ctrl_out_type = jsetcfg[s].pid_output_types.pwm_ctrl_out_type;
        o->jointSet[s].curent_ctrl_out_type = jsetcfg[s].pid_output_types.cur_ctrl_out_type;
        
        o->jointSet[s].torque_ctrl_out_type = jsetcfg[s].pid_output_types.torque_ctrl_out_type;
        o->jointSet[s].postrj_ctrl_out_type = jsetcfg[s].pid_output_types.postrj_ctrl_out_type;
        o->jointSet[s].veltrj_ctrl_out_type = jsetcfg[s].pid_output_types.veltrj_ctrl_out_type;
        o->jointSet[s].mixtrj_ctrl_out_type = jsetcfg[s].pid_output_types.mixtrj_ctrl_out_type;
        o->jointSet[s].posdir_ctrl_out_type = jsetcfg[s].pid_output_types.posdir_ctrl_out_type;
        o->jointSet[s].veldir_ctrl_out_type = jsetcfg[s].pid_output_types.veldir_ctrl_out_type;
        
        o->jointSet[s].USE_SPEED_FBK_FROM_MOTORS = jsetcfg[s].usespeedfeedbackfrommotors;
        o->jointSet[s].special_constraint = (eOmc_jsetconstraint_t)jsetcfg[s].constraints.type;
        o->jointSet[s].special_limit = jsetcfg[s].constraints.param1;
        //NOTE: jsetcfg[s].constraints.param2 is not used currently. It is reserved for future use
        
        
#ifdef R1_HAND_MKII
        o->jointSet[s].special_constraint = eomc_jsetconstraint_cerhand2;
        o->jointSet[s].special_limit = 500;
#endif
    }   
        
        
    update_jointAndMotor_withJointSet_configuration();
    
//    char message[180];
//    snprintf(message, sizeof(message), "J2S:%d %d %d %d;T:%d,%d,%d,%d", o->j2s[0], o->j2s[1], o->j2s[2], o->j2s[3],
//        o->jointSet[0].MOTOR_CONTROL_TYPE, o->jointSet[1].MOTOR_CONTROL_TYPE, o->jointSet[2].MOTOR_CONTROL_TYPE, o->jointSet[3].MOTOR_CONTROL_TYPE);
//    send_debug_message(message,0,0,0);
    //debug_dump_coupling_data(jomoCouplingInfo);
}

eOmc_encoder_t MController_getTypeofEncoderAtJoint(const eOmc_jomo_descriptor_t *jomodes)
{
    if(jomodes->encoder1.pos == eomc_pos_atjoint)
        return((eOmc_encoder_t)jomodes->encoder1.type);
    else if(jomodes->encoder2.pos == eomc_pos_atjoint)
        return((eOmc_encoder_t)jomodes->encoder2.type);
    else
        return (eomc_enc_none);
}


void MController_config_board(const eOmn_serv_configuration_t* brd_cfg)
{
    MController *o = smc;
    
    EOconstarray* carray = NULL;
    const eOmc_4jomo_coupling_t *jomoCouplingInfo = NULL;
    
    switch (brd_cfg->type)
    {
        case eomn_serv_MC_foc:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.foc_based.arrayofjomodescriptors);
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.foc_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_2FOC;
            jomoCouplingInfo = &(brd_cfg->data.mc.foc_based.jomocoupling);
            break;
        
        case eomn_serv_MC_mc4plusmais:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.mc4plusmais_based.arrayofjomodescriptors);
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plusmais_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_MC4p;
            jomoCouplingInfo = &(brd_cfg->data.mc.mc4plusmais_based.jomocoupling);
            break;
        
        case eomn_serv_MC_mc4plus:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.mc4plus_based.arrayofjomodescriptors);
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plus_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_MC4p;
            jomoCouplingInfo = &(brd_cfg->data.mc.mc4plus_based.jomocoupling);
            break;
        case eomn_serv_MC_mc2pluspsc:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.mc2pluspsc.arrayofjomodescriptors);
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc2pluspsc.arrayofjomodescriptors.head.size; 
            o->actuation_type = HARDWARE_MC4p; 
            jomoCouplingInfo = &(brd_cfg->data.mc.mc2pluspsc.jomocoupling);            
            break;
        case eomn_serv_MC_mc4plusfaps:
            carray = eo_constarray_Load((EOarray*)&brd_cfg->data.mc.mc4plusfaps.arrayofjomodescriptors);
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plusfaps.arrayofjomodescriptors.head.size; 
            o->actuation_type = HARDWARE_MC4p; 
            jomoCouplingInfo = &(brd_cfg->data.mc.mc4plusfaps.jomocoupling);            
            break;
        
        default:
            return;
    }
    
    for (int k=0; k<o->nJoints; ++k)
    {
        const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, k);
        eOmc_encoder_t typeEncJoint = MController_getTypeofEncoderAtJoint(jomodes);
        switch(typeEncJoint)
        {
            case eomc_enc_spichainof2:
            {
                for (int e=0; e<2; ++e)
                {
                    o->absEncoder[k*2+e].type = eomc_enc_aea;
                    o->absEncoder[k*2+e].fake = FALSE;
                }
                break;
            }
            case eomc_enc_spichainof3:
            {
                for (int e=0; e<3; ++e)
                {
                    o->absEncoder[k*3+e].type = eomc_enc_aea;
                    o->absEncoder[k*3+e].fake = FALSE;
                }
                break;
            }                
            
            case eomc_enc_psc:
            {
                for (int e=0; e<4; ++e)
                {
                    o->absEncoder[k*4+e].type = eomc_enc_aea;
                    o->absEncoder[k*4+e].fake = FALSE;
                }
                break;
            }  
 
            // marco.accame on 16 dec 2020: i assume eomc_enc_pos is similar to aea because the encoder is absolute and has only one component 
            case eomc_enc_pos:
            case eomc_enc_aea:
            {
                o->absEncoder[k].type = eomc_enc_aea;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            
            case eomc_enc_mais:
            {
                o->absEncoder[k].type = eomc_enc_mais;
                o->absEncoder[k].fake = FALSE;
                break;
            }

            case eomc_enc_amo:
            {
                o->absEncoder[k].type = eomc_enc_amo;
                o->absEncoder[k].fake = FALSE;
                break;
            }            
            
            
            case eomc_enc_absanalog:
            {
                o->absEncoder[k].type = eomc_enc_absanalog;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            case eomc_enc_none:
            default:
            {
                o->absEncoder[k].fake = TRUE;
                o->absEncoder[k].type = eomc_enc_aea;
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
    
    
    get_jomo_coupling_info(jomoCouplingInfo, carray);
    
    //Now i need to check if encoder2joint matrix is identity matrix or not.
    //If it is an identity matrix, it means this jointset doesn't use encoders coupling, else otherwise.
    //if this set use encoders coupling, i need to pass the encoders coupling matrix to JointSet_config, 
    //else i pass NULL.

    float **Sje_aux = NULL;
    
    eOboolvalues_t isIdentityMatrix = eobool_true;
    for (int i=0; i<MAX_JOINTS_PER_BOARD; ++i)
    {
        for (int k=0; k<MAX_ENCODS_PER_BOARD; ++k)
        {
            if(i==k)
            {
                if(o->Sje[i][k]  != 1.0f)
                    isIdentityMatrix = eobool_false;
            }
            else
            {
                if(o->Sje[i][k]  != 0.0f)
                    isIdentityMatrix = eobool_false;
            }
        }
    }
        
    if(!isIdentityMatrix)
        Sje_aux = o->Sje;

		
		
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
            o->Jjm, o->Sjm,
            o->Jmj, o->Smj,
            Sje_aux
        );
        
        o->jointSet[s].led = (hal_led_t)(hal_led1 + s);
    }
}





#if 0

//void MController_config_board(uint8_t part_type, uint8_t actuation_type)
void MController_config_board(const eOmn_serv_configuration_t* brd_cfg)
{
    MController *o = smc;
    
    EOconstarray* carray = NULL;
    const eOmc_4jomo_coupling_t *jomoCouplingInfo = NULL;
    
    switch (brd_cfg->type)
    {
        case eomn_serv_MC_foc:
            carray = eo_constarray_Load((const EOarray*)&brd_cfg->data.mc.foc_based.arrayofjomodescriptors);
            o->part_type = brd_cfg->data.mc.foc_based.boardtype4mccontroller;
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.foc_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_2FOC;
            jomoCouplingInfo = &(brd_cfg->data.mc.foc_based.jomocoupling);
            break;
        
        case eomn_serv_MC_mc4plusmais:
            carray = eo_constarray_Load((const EOarray*)&brd_cfg->data.mc.mc4plusmais_based.arrayofjomodescriptors);
            o->part_type = brd_cfg->data.mc.mc4plusmais_based.boardtype4mccontroller;
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plusmais_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_MC4p;
            jomoCouplingInfo = &(brd_cfg->data.mc.mc4plusmais_based.jomocoupling);
            break;
        
        case eomn_serv_MC_mc4plus:
            carray = eo_constarray_Load((const EOarray*)&brd_cfg->data.mc.mc4plus_based.arrayofjomodescriptors);
            o->part_type = brd_cfg->data.mc.mc4plus_based.boardtype4mccontroller;
            o->nSets = o->nEncods = o->nJoints = brd_cfg->data.mc.mc4plus_based.arrayofjomodescriptors.head.size;
            o->actuation_type = HARDWARE_MC4p;
            jomoCouplingInfo = &(brd_cfg->data.mc.mc4plus_based.jomocoupling);
            break;
        
        default:
            return;
    }
    
    for (int k=0; k<o->nJoints; ++k)
    {
        const eOmc_jomo_descriptor_t *jomodes = (const eOmc_jomo_descriptor_t*) eo_constarray_At(carray, k);
        
        switch(jomodes->encoder1.type)
        {
            case eomc_enc_spichainof2:
            {
                for (int e=0; e<2; ++e)
                {
                    o->absEncoder[k*2+e].type = eomc_enc_aea;
                    o->absEncoder[k*2+e].fake = FALSE;
                }
                break;
            }
            case eomc_enc_spichainof3:
            {
                for (int e=0; e<3; ++e)
                {
                    o->absEncoder[k*3+e].type = eomc_enc_aea;
                    o->absEncoder[k*3+e].fake = FALSE;
                }
                break;
            }                
            case eomc_enc_aea:
            {
                o->absEncoder[k].type = eomc_enc_aea;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            
            case eomc_enc_mais:
            {
                o->absEncoder[k].type = eomc_enc_mais;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            
            case eomc_enc_absanalog:
            {
                o->absEncoder[k].type = eomc_enc_absanalog;
                o->absEncoder[k].fake = FALSE;
                break;
            }
            default:
            {
                o->absEncoder[k].fake = TRUE;
                o->absEncoder[k].type = eomc_enc_aea;
                break;
            }
        };
        
        o->motor[k].HARDWARE_TYPE = o->actuation_type;
        
        switch(o->motor[k].HARDWARE_TYPE)
        {
            case HARDWARE_MC4p:
            {
                o->motor[k].actuatorPort = jomodes->actuator.pwm.port;
                Motor_clear_ext_fault(&(o->motor[k]));
            }
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
    
    
    
    get_jomo_coupling_info(jomoCouplingInfo);
    
    //Currently not all info are stored in boardconfig, so in following switch the controller configures jointsets, joints, etc. with remaining info
    
    switch (o->part_type)
    {
    case eomc_ctrlboard_ANKLE:                   //= 1,    //2FOC
    {
        for (int k = 0; k<o->nJoints; ++k)
        {            
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = TRUE;
        }
        
        break;
    }
    
    case eomc_ctrlboard_CER_UPPER_ARM:               //= 17,    //2FOC
    {
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
        
            o->jointSet[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = TRUE;
        }
        break;
    }   

    case eomc_ctrlboard_UPPERLEG:                //= 2,    //2FOC
    {
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
        }
        break;
    }
        
        
    case eomc_ctrlboard_WAIST:                   //= 3,    //2FOC
    {

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
        
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = TRUE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = TRUE;
        
        break;
    }
    case eomc_ctrlboard_SHOULDER:                //= 4,    //2FOC
    {
        Sjm = o->Sjm;
        Jjm = o->Jjm;
        Jmj = o->Jmj;
        #if defined(ICUB_MEC_V1) || defined(ICUB_GENOVA04)
        // |j0|   |  1     0    0   |   |e0|     
        // |j1| = |  0     1    0   | * |e1|
        // |j2|   |  1    -1  40/65 |   |e2|
        Sje = o->Sje;
        #endif
        
        // |j0|    | 1     0       0   |   |m0|
        // |j1|  = | 1   40/65     0   | * |m1|
        // |j2|    | 0  -40/65   40/65 |   |m2|

        float alfa = 40.0f/65.0f; //IMPORTANT: alfa must have the same value of variable alfa in EOtheBoardConfig.c
    
        Sjm[0][0] =  1.0f; Sjm[0][1] =  0.0f; Sjm[0][2] =  0.0f; Sjm[0][3] =  0.0f;
        Sjm[1][0] =  1.0f; Sjm[1][1] =  alfa; Sjm[1][2] =  0.0f; Sjm[1][3] =  0.0f;
        Sjm[2][0] =  0.0f; Sjm[2][1] = -alfa; Sjm[2][2] =  alfa; Sjm[2][3] =  0.0f;
        Sjm[3][0] =  0.0f; Sjm[3][1] =  0.0f; Sjm[3][2] =  0.0f; Sjm[3][3] =  1.0f;

        for (int j=0; j<4; ++j)
            for (int m=0; m<4; ++m)
                Jjm[j][m] = Sjm[j][m];
        
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
    case eomc_ctrlboard_CER_WAIST:               //= 15,   //2FOC
    {
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
    }
        
    case eomc_ctrlboard_CER_LOWER_ARM:               //= 12,   //MC4plus
    {
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
    }
        
    case eomc_ctrlboard_CER_BASE:                //= 21    //2FOC
    {
        
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = VEL_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;
        }
    }
        break;
    
    case eomc_ctrlboard_CER_NECK:                   //= 22,    //mc4plus
    {
        for (int k = 0; k<o->nJoints; ++k)
        {            
            o->joint[k].dead_zone = 0;
            
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;
        }
        
        AbsEncoder_config_divisor(o->absEncoder+1, 125);
        
        break;
    }
    
    case eomc_ctrlboard_CER_HAND:                   //= 14, 16,    //mc2plus
    {
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
    }
    
    case eomc_ctrlboard_HEAD_neckpitch_neckroll:      //= 5,    //MC4plus
    {
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

        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        }
        
        o->jointSet[0].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[0].CAN_DO_TRQ_CTRL = FALSE;
        
        break;
    }
    case eomc_ctrlboard_HEAD_neckyaw_eyes:       //=6,      //MC4plus
    {

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

    case eomc_ctrlboard_HAND_thumb:                  //= 9,    //MC4plus
    {
        /*
        NOTE: (VALE)
            In this board, thumb oppose and thumb proximal joints are coupled.
            I get decoupled matrix from mc4(CAN) firmware version 0x228 function "decouple_dutycycle".
            In this function, the fw uses a different decoupled matrix if it runs on left arm or right arm.
            Currently I tested only left arm, so we need to check if the same matrix can be use also on twin board on right arm.
        */
        
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
    
    case eomc_ctrlboard_FACE_eyelids_jaw:        //= 7,    //MC4plus
    case eomc_ctrlboard_4jointsNotCoupled:               //= 8,    //MC4plus
    {
        for (int k = 0; k<o->nJoints; ++k)
        {
            o->joint[k].CAN_DO_TRQ_CTRL = FALSE;
            o->joint[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->motor[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            
            o->jointSet[k].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
            o->jointSet[k].CAN_DO_TRQ_CTRL = FALSE;

        }

        break;
    }
    case eomc_ctrlboard_HAND_2:                  //= 10,   //MC4plus
        break;
    
    case eomc_ctrlboard_FOREARM:                 //= 11,   //MC4plus
    {
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
        
        o->jointSet[2].MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR;
        o->jointSet[2].CAN_DO_TRQ_CTRL = TRUE;
    
    break;
    }    
    

    default:
        return;
    }
    
    //check multi encoder compatibility
    //currently all joint of same board have the the same number of multiple encoder. so i use j0
    const eOmc_jomo_descriptor_t *jomodes = (const eOmc_jomo_descriptor_t*) eo_constarray_At(carray, 0);
    uint8_t cfg_multienc = (uint8_t) eomc_encoder_get_numberofcomponents((eOmc_encoder_t)jomodes->encoder1.type);
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

#endif

void MController_config_joint(int j, eOmc_joint_config_t* config) //
{
    MController *o = smc;
    
    MController_config_minjerk_pid(j, &(config->pidtrajectory));
    
    MController_config_direct_pid(j, &(config->pidtrajectory));
    //MController_config_direct_pid(j, &(config->piddirect));
    
    MController_motor_config_torque_PID(j, &(config->pidtorque));
    
    Motor_config_filter(o->motor+j,   config->tcfiltertype);
    Motor_config_friction(o->motor+j, config->motor_params.bemf_value, config->motor_params.ktau_value);   
    
    Joint_config(o->joint+j, j, config);
    
    for(int e=0; e< o->multi_encs; e++)
    {
        AbsEncoder_config(o->absEncoder+j*o->multi_encs+e,   j, config->jntEncoderResolution, config->jntEncTolerance);
        AbsEncoder_config_divisor(smc->absEncoder+j, config->gearbox_E2J);
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

extern void MController_motor_config_current_PID(int m, eOmc_PID_t* pid);
extern void MController_motor_config_torque_PID(int m, eOmc_PID_t *pid);
extern void MController_motor_config_speed_PID(int m, eOmc_PID_t *pid);

void MController_motor_config_current_PID(int m, eOmc_PID_t* pid)
{
    Motor_config_current_PID(smc->motor+m, pid);
}

void MController_motor_config_torque_PID(int m, eOmc_PID_t *pid)
{
    //Motor_config_trqPID(smc->motor+m, pid);
    PID_config(&(smc->motor[m].trqPID), pid);
}

void MController_motor_config_speed_PID(int m, eOmc_PID_t* pid)
{
    Motor_config_speed_PID(smc->motor+m, pid);
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
        AbsEncoder_update(enc + k, (uint16_t)positions[k]);
    }
    
    //void JointSet_send_debug_message(char *message, uint8_t jid)
    if (0)
    {
        static int noflood[2] = { 0, 1000};
    
        if (++noflood[e]>2000)
        {
            noflood[e] = 0;
            
            eOerrmanDescriptor_t errdes = {0};

            errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = 0;
            errdes.par16            = e;
            errdes.par64            = (((uint64_t)positions[0])<<48)|(((uint64_t)positions[1])<<32)|(((uint64_t)positions[2])<<16)|(((int64_t)positions[3]));
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, "ENCODERS", NULL, &errdes);
        }
    }
    
#ifdef R1_HAND
    
    MController_update_joint_torque_fbk(e, AbsEncoder_position(enc + 2));
    
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

void MController_update_joint_targets(int j)
{
    Joint_update_status_reference(smc->joint+j);
}




void MController_config_minjerk_pid(int j, eOmc_PID_t *pid_conf)
{
    PID_config(&(smc->joint[j].minjerkPID), pid_conf);
    PID_config(&(smc->joint[j].directPID), pid_conf);
}

void MController_config_direct_pid(int j, eOmc_PID_t *pid_conf)
{
    PID_config(&(smc->joint[j].directPID), pid_conf);
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

BOOL MController_set_joint_vel_raw(int j, CTRL_UNITS vel_ref)
{
    return Joint_set_vel_raw(smc->joint+j, vel_ref);
}

BOOL MController_set_joint_trq_ref(int j, CTRL_UNITS trq_ref)
{
    return Joint_set_trq_ref(smc->joint+j, trq_ref);
}

BOOL MController_set_joint_pwm_ref(int j, CTRL_UNITS pwm_ref)
{
    return Joint_set_pwm_ref(smc->joint+j, pwm_ref);
}

BOOL MController_set_joint_cur_ref(int j, CTRL_UNITS cur_ref)
{
    return Joint_set_cur_ref(smc->joint+j, cur_ref);
}

void MController_stop_joint(int j)
{
    Joint_stop(smc->joint+j);
}

void MController_config_motor_gearbox_M2J(int m, float32_t gearbox_M2J)
{
    Motor_config_gearbox_M2J(smc->motor+m, gearbox_M2J);
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
    Motor_update_odometry_fbk_can(smc->motor+m, (CanOdometry2FocMsg*)data);
}

void MController_motor_raise_fault_i2t(int m)
{
    Motor_raise_fault_i2t(smc->motor+m);
}

////////////////////////////////////////////////////////////////////////////////
