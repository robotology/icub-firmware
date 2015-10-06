/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

#include "EOtheCANprotocol_functions.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMStransceiver.h"

#include "EOemsController.h"

#include "hal_sys.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMotionController.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMotionController_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_motioncontrol_onstop_search4focs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static eOresult_t s_eo_motioncontrol_SetCurrentSetpoint(EOtheMotionController *p, int16_t *pwmList, uint8_t size);

static void s_eo_motioncontrol_UpdateJointStatus(EOtheMotionController *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMotionController s_eo_themotcon = 
{
    .initted                = eobool_false,
    .active                 = eobool_false,
    .protindex              = 0,
    .id32                   = eo_prot_ID32dummy,
    .command                = {0},
    .canboardproperties     = NULL,
    .canentitydescriptor    = NULL,
    .servconfig             = { .type = eomn_serv_NONE },
    .candiscoverytarget     = {0},
    .onverify               = NULL,
    .activateafterverify    = eobool_false
};

//static const char s_eobj_ownname[] = "EOtheMotionController";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMotionController* eo_motioncontrol_Initialise(void)
{
    if(eobool_true == s_eo_themotcon.initted)
    {
        return(&s_eo_themotcon);
    }
    
    s_eo_themotcon.active = eobool_false;

    
    s_eo_themotcon.protindex = 0;
    s_eo_themotcon.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_themotcon.protindex, eoprot_tag_none);

    s_eo_themotcon.servconfig.type = eomn_serv_NONE;
    
    // up to to 12 mc4 + 1 mais 
    s_eo_themotcon.canboardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), eo_motcon_maxCANboards, NULL, NULL, NULL, NULL);
    
    // up to 12 jomos
    s_eo_themotcon.canentitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_motcon_maxENTITIES, NULL, NULL, NULL, NULL);
    
    s_eo_themotcon.encoderreader = eo_encoderreader_Initialise();
    
    s_eo_themotcon.numofjomos = 0;
    
        
    s_eo_themotcon.initted = eobool_true;
    
    return(&s_eo_themotcon);   
}


extern EOtheMotionController* eo_motioncontrol_GetHandle(void)
{
    if(eobool_true == s_eo_themotcon.initted)
    {
        return(&s_eo_themotcon);
    }
    
    return(NULL);
}


extern eOresult_t eo_motioncontrol_Verify(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg, eOmotcon_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }  
 
// DONT Deactivate ... we may want just to check again ....    
//    if(eobool_true == s_eo_themotcon.active)
//    {
//        eo_motioncontrol_Deactivate(p);        
//    }   
    
    #warning EOtheMotionController MUST: verify also the encoders ...

    s_eo_themotcon.onverify = onverify;
    s_eo_themotcon.activateafterverify = activateafterverify;


    memset(&s_eo_themotcon.candiscoverytarget, 0, sizeof(s_eo_themotcon.candiscoverytarget));
    s_eo_themotcon.candiscoverytarget.boardtype = eobrd_cantype_1foc;
    s_eo_themotcon.candiscoverytarget.protocolversion.major = servcfg->data.mc.foc_based.versionoffoc.protocol.major; 
    s_eo_themotcon.candiscoverytarget.protocolversion.minor = servcfg->data.mc.foc_based.versionoffoc.protocol.minor;
    s_eo_themotcon.candiscoverytarget.firmwareversion.major = servcfg->data.mc.foc_based.versionoffoc.firmware.major; 
    s_eo_themotcon.candiscoverytarget.firmwareversion.minor = servcfg->data.mc.foc_based.versionoffoc.firmware.minor;   
    
    EOconstarray* carray = eo_constarray_Load((EOarray*)&servcfg->data.mc.foc_based.arrayofjomodescriptors);
    
    uint8_t numofjomos = eo_constarray_Size(carray);
    uint8_t i = 0;
    for(i=0; i<numofjomos; i++)
    {
        const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
        eo_common_hlfword_bitset(&s_eo_themotcon.candiscoverytarget.canmap[jomodes->actuator.foc.canloc.port], jomodes->actuator.foc.canloc.addr);         
    }
       

    eOcandiscovery_onstop_t onstop = 
    {
        .function   = s_eo_motioncontrol_onstop_search4focs,
        .parameter  = (void*)servcfg
    };
        
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_themotcon.candiscoverytarget, &onstop);   

    
    return(eores_OK);   
}


extern eOresult_t eo_motioncontrol_Deactivate(EOtheMotionController *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_themotcon.active)
    {
        return(eores_OK);        
    } 
    
//    // send stop messages to strain, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object
//    // what else .... ?
//    
//        
//    if(eobool_true == s_eo_themotcon.itistransmitting)
//    {
//        eo_motioncontrol_TXstop(&s_eo_themotcon);
//    }
//    
//    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_themotcon.canentitydescriptor); 
//    
//    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_themotcon.canboardproperties);
//     
//    
//    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 0);
//    
//    s_eo_themotcon.strain = NULL;
//    
//    memset(&s_eo_themotcon.servconfig, 0, sizeof(eOmn_serv_configuration_t));
//    s_eo_themotcon.servconfig.type = eomn_serv_NONE;
//    
//    eo_vector_Clear(s_eo_themotcon.canboardproperties);
//    eo_vector_Clear(s_eo_themotcon.canentitydescriptor);
//    
//    

    s_eo_themotcon.active = eobool_false;

    s_eo_themotcon.numofjomos = 0;
    
    return(eores_OK);
}


extern eOresult_t eo_motioncontrol_Activate(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_true == s_eo_themotcon.active)
    {
        eo_motioncontrol_Deactivate(p);        
    }   


    EOconstarray* carray = eo_constarray_Load((EOarray*)&servcfg->data.mc.foc_based.arrayofjomodescriptors);
    
    uint8_t numofjomos = eo_constarray_Size(carray);
    
    eo_entities_SetNumOfJoints(eo_entities_GetHandle(), numofjomos);
    eo_entities_SetNumOfMotors(eo_entities_GetHandle(), numofjomos);
    

    if(0 == eo_entities_NumOfJoints(eo_entities_GetHandle()))
    {
        s_eo_themotcon.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {                
        memcpy(&s_eo_themotcon.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
      
        s_eo_themotcon.numofjomos = numofjomos;
        
        // now... use the servcfg
        uint8_t i = 0;
        
        EOconstarray* carray = eo_constarray_Load((EOarray*)&servcfg->data.mc.foc_based.arrayofjomodescriptors);

        
        // load the can mapping 
        for(i=0; i<numofjomos; i++)
        {
            const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
            
            eOcanmap_board_properties_t prop = {0};
            
            prop.type = eobrd_cantype_1foc;
            prop.location.port = jomodes->actuator.foc.canloc.port;
            prop.location.addr = jomodes->actuator.foc.canloc.addr;
            prop.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
            prop.requiredprotocol.major = servcfg->data.mc.foc_based.versionoffoc.protocol.major;
            prop.requiredprotocol.minor = servcfg->data.mc.foc_based.versionoffoc.protocol.minor;
            
            eo_vector_PushBack(s_eo_themotcon.canboardproperties, &prop);            
        }
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_themotcon.canboardproperties); 
        
        // load the entity mapping.
        for(i=0; i<numofjomos; i++)
        {
            const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
            
            eOcanmap_entitydescriptor_t des = {0};
            
            des.location.port = jomodes->actuator.foc.canloc.port;
            des.location.addr = jomodes->actuator.foc.canloc.addr;
            des.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
            des.index = (eOcanmap_entityindex_t)i;

            eo_vector_PushBack(s_eo_themotcon.canentitydescriptor, &des);            
        }        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, s_eo_themotcon.canentitydescriptor); 
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, s_eo_themotcon.canentitydescriptor);        

        // init the encoders
        
        eo_encoderreader_Activate(p->encoderreader, &servcfg->data.mc.foc_based.arrayofjomodescriptors);

        
        // init the emscontroller.
        
        s_eo_themotcon.controller = eo_emsController_Init((eOemscontroller_board_t)servcfg->data.mc.foc_based.boardtype4mccontroller, emscontroller_actuation_2FOC, numofjomos);   
                
        s_eo_themotcon.active = eobool_true;        
    }

    
    return(eores_OK);   
}



extern eOresult_t eo_motioncontrol_Tick(EOtheMotionController *p)
{
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == s_eo_themotcon.active)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    } 
    
    
    if(eomn_serv_MC_foc == p->servconfig.type)
    {
        uint8_t error_mask = 0;
        eOresult_t res = eores_NOK_generic;
        uint32_t encvalue[4] = {0}; 
        hal_encoder_errors_flags encflags[4] = {0};
        int16_t pwm[4] = {0};
        
        uint8_t i = 0;
        
        // wait for the encoders for some time 
        for(i=0; i<30; ++i)
        {
            if(eobool_true == eo_encoderreader_IsReadingAvailable(p->encoderreader))
            {
                break;
            }
            else
            {
                hal_sys_delay(5*hal_RELTIME_1microsec);
            }
        }        
        
        // read the encoders
        
        if(eobool_true == eo_encoderreader_IsReadingAvailable(p->encoderreader))
        {    
            for(i=0; i<p->numofjomos; i++)
            {
                uint32_t extra = 0;
                res = eo_encoderreader_Read(p->encoderreader, i, &(encvalue[i]), &extra, &(encflags[i]));
                if (res != eores_OK)
                {
                    error_mask |= 1<<(i<<1);
                    //encvalue[enc] = (uint32_t)ENC_INVALID;
                }
            }        
        }
        else
        {
            error_mask = 0xAA; // timeout
        }

        // Restart the reading of the encoders
        eo_encoderreader_StartReading(p->encoderreader);
            
        eo_emsController_AcquireAbsEncoders((int32_t*)encvalue, error_mask);
        
        eo_emsController_CheckFaults();
            
        /* 2) pid calc */
        eo_emsController_PWM(pwm);

        /* 3) prepare and punt in rx queue new setpoint */
        s_eo_motioncontrol_SetCurrentSetpoint(p, pwm, 4); 
     
        /* 4) update joint status */
        s_eo_motioncontrol_UpdateJointStatus(p);
  
    }

    return(eores_OK);    
}


//extern eOresult_t eo_motioncontrol_OnDiscoveryStop(EOtheMotionController *p, EOtheCANdiscovery2 *discovery2, eObool_t searchisok)
//{   
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    // wip part. we should: 
//    // 1. send up a ok result about the strain ...
//    // 2. call eo_motioncontrol_Initialise()
//    // 3. return OK
//    
//    
//    if(NULL == p)
//    {
//        eo_motioncontrol_Initialise();
//    }

//    return(eores_OK); 
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the joints and motors ... better in here rather than elsewhere.

static const eOmc_joint_t s_joint_default_value =
{
    .config =             
    {
        .pidposition =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        },
        .pidvelocity =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        },
        .pidtorque =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        }, 
        .limitsofjoint =
        {
            .min =                   0,
            .max =                   0
        },
        .impedance =
        {
            .stiffness =             0,
            .damping =               0,
            .offset =                0,
            .filler02 =              {0xf1, 0xf2}           
        },        
        
        .velocitysetpointtimeout =   0,
       
        .motionmonitormode =         eomc_motionmonitormode_dontmonitor,
        .filler01 =                  0xe0,
        .DEPRECATED_encoderconversionfactor =   EOUTIL_EMULFLOAT32_ONE,
        .DEPRECATED_encoderconversionoffset =   EOUTIL_EMULFLOAT32_ZERO,
        .motor_params =
        {
            .bemf_value =            0,
            .ktau_value =            0,
            .bemf_scale =            0,
            .ktau_scale =            0,
            .filler02 =              {0xf1, 0xf2}
        },
        .tcfiltertype =              0,
        .jntEncoderType =            0,
        .filler02 =                  {0xf1, 0xf2}
    },
    .status =                       
    {
        .basic =
        {
            .jnt_position =          0,
            .jnt_velocity =          0,
            .jnt_acceleration =      0,
            .jnt_torque =            0,
            .motionmonitorstatus =   eomc_motionmonitorstatus_notmonitored,
            .controlmodestatus =     eomc_controlmode_idle,
        },
        .ofpid =                     {0},
        .interactionmodestatus =     eomc_imodeval_stiff,
        .chamaleon03 =               {0} //{0xd1, 0xd2, 0xd3}
    },
    .inputs =                        {0},
    .cmmnds =                       
    {
        .calibration =               {0},
        .setpoint =                  {0},
        .stoptrajectory =            0,
        .controlmode =                 eomc_controlmode_cmd_switch_everything_off,
        .interactionmode =           eomc_imodeval_stiff,
        .filler01 =                  0        
    }
}; 

static const eOmc_motor_t s_motor_default_value =
{
    .config =             
    {
        .pidcurrent =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        },
        .gearboxratio =              0,
        .rotorEncoderResolution =    0,
        .filler01 =                  0,
        .maxvelocityofmotor =        0,
        .maxcurrentofmotor =         0,
        .rotorIndexOffset =          0,
        .motorPoles =                0,
        .hasHallSensor =             eobool_false,
        .hasTempSensor =             eobool_false,
        .hasRotorEncoder =           eobool_false,
        .hasRotorEncoderIndex =      eobool_false,
        .rotorEncoderType =          0,
        .limitsofrotor =
        {
            .max = 0,
            .min = 0
        }
    },
    .status =                       {0}
}; 

extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &s_joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t *sta = (eOmc_joint_status_t*)eo_nv_RAM(nv);
    memcpy(sta, &s_joint_default_value.status, sizeof(eOmc_joint_status_t));
}

extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv)
{
    eOmc_motor_config_t *cfg = (eOmc_motor_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &s_motor_default_value.config, sizeof(eOmc_motor_config_t));
}

extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv)
{
    eOmc_motor_status_t *sta = (eOmc_motor_status_t*)eo_nv_RAM(nv);
    memcpy(sta, &s_motor_default_value.status, sizeof(eOmc_motor_status_t));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_motioncontrol_onstop_search4focs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_themotcon.activateafterverify))
    {
        const eOmn_serv_configuration_t * mcserv = (const eOmn_serv_configuration_t *)par;
        eo_motioncontrol_Activate(&s_eo_themotcon, mcserv);        
    }
    
    if(NULL != s_eo_themotcon.onverify)
    {
        s_eo_themotcon.onverify(&s_eo_themotcon, searchisok); 
    }    
    
    return(eores_OK);   
}



// want to send a canframe with pwm onto can bus. 
static eOresult_t s_eo_motioncontrol_SetCurrentSetpoint(EOtheMotionController *p, int16_t *pwmList, uint8_t size)
{
    eOcanport_t port = eOcanport1;
    
    // now i need to assign port and command with correct values.
    
    // i manage 2foc boards. they are at most 4. they must be all in the same can bus. 
    int16_t pwmValues[4] = {0, 0, 0, 0};
    
    uint8_t i=0;
    for(i=0; i<p->numofjomos; i++)
    {
        eOcanmap_location_t loc = {0};
        // search the address of motor i-th and fill the pwmValues[] in relevant position.
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, i, 0), &loc, NULL, NULL))
        {
            port = (eOcanport_t)loc.port;  // marco.accame: i dont check if the port is not always the same ... it MUST be.          
            if((loc.addr > 0) && (loc.addr <= 4))
            {
                pwmValues[loc.addr-1] = pwmList[i];
            }            
        }        
    }
    
    // ok, now i fill command and location
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_periodicMotorControl;    
    command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
    command.value = &pwmValues[0];
    
    eOcanmap_location_t location = {0};
    location.port = port;
    location.addr = 0; // marco.accame: we put 0 just because it is periodic and this is the source address (the EMS has can address 0).
    location.insideindex = eocanmap_insideindex_first; // because all 2foc have motor on index-0. 

    // and i send the command
    return(eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location));   
}


static void s_eo_motioncontrol_UpdateJointStatus(EOtheMotionController *p)
{
    const uint8_t transmit_decoupled_pwms = 1;
    
    eOmc_joint_status_t *jstatus = NULL;
    eOmc_motor_status_t *mstatus = NULL;
    
    uint8_t jId = 0;
        
    for(jId = 0; jId<p->numofjomos; jId++)
    {
        if(NULL != (jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jId)))
        {
       
            eo_emsController_GetJointStatus(jId, jstatus);
            
            eo_emsController_GetActivePidStatus(jId, &jstatus->ofpid);
            
            if (transmit_decoupled_pwms) 
            {  
                //this functions is used to get the motor PWM after the decoupling matrix
                eo_emsController_GetPWMOutput(jId, &jstatus->ofpid.output);
            }
            
            if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus->basic.motionmonitorstatus)
            {
                /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
                - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
                message because the setpoint is alredy reached. this means that:
                    - if monitormode is forever, no new set point has been configured 
                    - if monitormode is _untilreached, the joint reached the setpoint already.
                - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
                message because pc104 is not interested in getting motion done.
                */
                if(eo_emsController_GetMotionDone(jId))
                {
                    jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
                }
            }
        }
 
    }
    
    
    for(jId = 0; jId<p->numofjomos; jId++)
    {
        if(NULL != (mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), jId)))
        {
            eo_emsController_GetMotorStatus(jId, mstatus);
        }
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



