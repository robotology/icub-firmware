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

#include "hal_sys.h"
#include "hal_dc_motorctl.h"
#include "hal_adc.h"
#include "hal_quad_enc.h"

#include "eOcommon.h"
#include "EOtheErrorManager.h"

#include "EOtheMemoryPool.h"

#include "EOappEncodersReader.h"
#include "EOMtheEMStransceiver.h"

#include "EoProtocol.h"
#include "EOMtheIPnet.h"
#include "EOtheEntities.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOmcService.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOmcService_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define LOCAL_ENCODER(enc_type)     ((enc_type == 0) || (enc_type == 1) || (enc_type == 2) || (enc_type == 3)) // AEA, AMO, INC, ADH 
#define SPI_ENCODER(enc_type)       ((enc_type == 0) || (enc_type == 1)) // AEA, AMO 

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

static eOresult_t s_eo_mcserv_protocol_load_mc_endpoint(EOmcService *p);

static eOresult_t s_eo_mcserv_init_jomo(EOmcService *p);

static void s_eo_mcserv_init_motors_adc_feedbacks(void);

static int16_t s_eo_mcserv_get_current_adc(uint8_t motor);

static uint32_t s_eo_mcserv_get_quad_enc (uint8_t motor);

static void s_eo_mcserv_enable_all_motors(EOmcService *p);

static void s_eo_mcserv_disable_all_motors(EOmcService *p);

static eOresult_t s_eo_mcserv_can_discovery_start(EOmcService *p);

static eOresult_t s_eo_mcserv_do_mc4plus(EOmcService *p);

static eOemscontroller_board_t s_eo_mcserv_getboardcontrol(void);

static eOresult_t s_eo_mcserv_MaisValues2Pos(eOprotID32_t maisID, uint8_t motor, uint8_t joint, uint32_t* val);

static void s_eo_mcserv_pwm_enable(uint8_t i);

static void s_eo_mcserv_pwm_disable(uint8_t i);

static void s_eo_mcserv_pwm_set(uint8_t i, int16_t v);

static eObool_t s_eo_mcserv_are_motors_ext_faulted(void);

static void s_eo_mcserv_reenable_external_fault_isr(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOmcService s_eo_mcserv = 
{
    .initted                = eobool_false,
    .config                 = 
    {
        .type               = eOmcconfig_type_undefined,
        .jomosnumber        = 0
    },
    .resourcesareready      = eobool_false,
    .thejoints              = NULL,
    .themotors              = NULL,
    .thelocalcontroller     = NULL,
    .thelocalencoderreader  = NULL,
    .valuesencoder          = NULL,
    .valuesencoder_extra    = NULL,
    .valuespwm              = NULL
};

//static const char s_eobj_ownname[] = "EOmcService";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOmcService* eo_mcserv_Initialise(eOmcconfig_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        return(NULL);
    }
    
    // remove later on
    if(eOmcconfig_type_mc4plus != cfg->type)
    {
        return(NULL);
    }  

    if(eobool_true == s_eo_mcserv.initted)
    {
        return(&s_eo_mcserv);
    }        
    
    memcpy(&s_eo_mcserv.config, cfg, sizeof(eOmcconfig_cfg_t));
    
    
    if(eobool_false == eoprot_endpoint_configured_is(eoprot_board_localboard, eoprot_endpoint_motioncontrol))
    {    
        s_eo_mcserv_protocol_load_mc_endpoint(&s_eo_mcserv);
    }
    
    s_eo_mcserv_init_jomo(&s_eo_mcserv);
       
    s_eo_mcserv.initted = eobool_true;  
       
    return(&s_eo_mcserv);
}


extern EOmcService* eo_mcserv_GetHandle(void)
{
    if(eobool_false == s_eo_mcserv.initted)
    {
        return(NULL);
    }
    return(&s_eo_mcserv); 
}

extern uint8_t eo_mcserv_NumberOfJoints(EOmcService *p)
{
    if(NULL == p)
    {
        return(0);
    } 

    return(p->config.jomosnumber);
}

extern eOmc_joint_t* eo_mcserv_GetJoint(EOmcService *p, uint8_t index)
{
    if((NULL == p) || (index >= p->config.jomosnumber))
    {
        return(NULL);
    } 
    
    return(p->thejoints[index]);  
}


extern uint8_t eo_mcserv_NumberOfMotors(EOmcService *p)
{
    if(NULL == p)
    {
        return(0);
    } 

    return(p->config.jomosnumber);
}

extern eOmc_motor_t* eo_mcserv_GetMotor(EOmcService *p, uint8_t index)
{
    if((NULL == p) || (index >= p->config.jomosnumber))
    {
        return(NULL);
    } 
    
    return(p->themotors[index]);   
}

extern eOmcconfig_cfg_t* eo_mcserv_GetMotionControlConfig (EOmcService *p)
{
    if((NULL == p) || p->initted != eobool_true)
    {
        return(NULL);
    }
    
    return(&p->config);    
}

extern eOresult_t eo_mcserv_CheckResources(EOmcService *p)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_true == p->resourcesareready)
    {
        return eores_OK;
    }
    
    switch(p->config.type)
    {
        case eOmcconfig_type_mc4plus:
        {
            // nothing to verify, .... so far. we thus set everything to ready, so taht eo_mcserv_AreResourcesReady() can returns true.
            p->resourcesareready = eobool_true;
            res = eores_OK;        
        } break;
        case eOmcconfig_type_mc4can:
        {
            // must start the discovery procedure for mc4 boards and mais on can bus.
            res = s_eo_mcserv_can_discovery_start(p);     
        } break;   
        case eOmcconfig_type_2foc:
        {
            // must start the discovery procedure for mc4 boards and mais on can bus.
            res = s_eo_mcserv_can_discovery_start(p);     
        } break;    
        default:
        {       
        } break;    
    }
    
    return(res);
}


extern eObool_t eo_mcserv_AreResourcesReady(EOmcService *p, uint32_t *readyresourcesdescriptor)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    if(eobool_true == p->resourcesareready)
    {
        if(NULL != readyresourcesdescriptor)
        {
            *readyresourcesdescriptor = 666;
        }
    }
    
    return(p->resourcesareready);
}



extern eOresult_t eo_mcserv_Start(EOmcService *p)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    switch(p->config.type)
    {
        case eOmcconfig_type_mc4plus:
        {
            // must start the first reading of encoders and ... enable the joints and...?
            eo_appEncReader_StartRead(p->thelocalencoderreader);
            s_eo_mcserv_enable_all_motors(p);
            res = eores_OK;            
        } break;
        case eOmcconfig_type_mc4can:
        {
            // must send the broadcast policy to mc4 ... what else?
            res = eores_OK;     
        } break;   
        case eOmcconfig_type_2foc:
        {
            // must start the first reading of encoders and ... enable the joints and...? 
            eo_appEncReader_StartRead(p->thelocalencoderreader);    
            res = eores_OK;     
        } break;    
        default:
        {       
        } break;    
    }
    
    return(res);
}

extern eOresult_t eo_mcserv_EnableMotor(EOmcService *p, uint8_t joint_index)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eOemscontroller_board_t board_control = s_eo_mcserv_getboardcontrol();
    
    //check coupled joints
    if((emscontroller_board_SHOULDER == board_control) || (emscontroller_board_WAIST == board_control))    
    {
      if (joint_index <3) 
      {
        s_eo_mcserv_pwm_enable(p->config.jomos[0].actuator.local.index);
        s_eo_mcserv_pwm_enable(p->config.jomos[1].actuator.local.index);
        s_eo_mcserv_pwm_enable(p->config.jomos[2].actuator.local.index);
      }
      else
      {
        s_eo_mcserv_pwm_enable(p->config.jomos[joint_index].actuator.local.index);
      }
    }
    else if(emscontroller_board_HEAD_neckpitch_neckroll == board_control)  
    {
       s_eo_mcserv_pwm_enable(p->config.jomos[0].actuator.local.index);
       s_eo_mcserv_pwm_enable(p->config.jomos[1].actuator.local.index);
    }
    else if(emscontroller_board_HEAD_neckyaw_eyes == board_control) 
    {
      if((joint_index == 2) || (joint_index == 3) ) 
      {
        s_eo_mcserv_pwm_enable(p->config.jomos[2].actuator.local.index);
        s_eo_mcserv_pwm_enable(p->config.jomos[3].actuator.local.index);
      }
      else
      {
         s_eo_mcserv_pwm_enable(p->config.jomos[joint_index].actuator.local.index);
      }      
    }
    else  //the joint is not coupled to any other joint 
    { 
         s_eo_mcserv_pwm_enable(p->config.jomos[joint_index].actuator.local.index);
    }      
    
    return eores_OK;
}
extern eOresult_t eo_mcserv_EnableFaultDetection(EOmcService *p)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    s_eo_mcserv_reenable_external_fault_isr();

    return eores_OK;
}
extern eObool_t eo_mcserv_AreMotorsExtFaulted(EOmcService *p)
{    
    if(NULL == p)
    {
        return eobool_false;
    }
    
    return s_eo_mcserv_are_motors_ext_faulted();
}

extern eOresult_t eo_mcserv_SetMotorFaultMask(EOmcService *p, uint8_t motor, uint8_t* fault_mask)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eOemscontroller_board_t board_control = s_eo_mcserv_getboardcontrol();
    
    //check coupled joints
    if((emscontroller_board_SHOULDER == board_control) || (emscontroller_board_WAIST == board_control))    
    {
      if (motor <3) 
      {
        // don't need to use p->config.jomos[motor].actuator.local.index, cause the emsController (and related objs) use the same indexing of the highlevel
        eo_motor_set_motor_status(eo_motors_GetHandle(),0, fault_mask);
        eo_motor_set_motor_status(eo_motors_GetHandle(),1, fault_mask);
        eo_motor_set_motor_status(eo_motors_GetHandle(),2, fault_mask);
      }
      else
      {
        eo_motor_set_motor_status(eo_motors_GetHandle(),motor, fault_mask);
      }
    }
    else if(emscontroller_board_HEAD_neckpitch_neckroll == board_control)  
    {
       eo_motor_set_motor_status(eo_motors_GetHandle(),0, fault_mask);
       eo_motor_set_motor_status(eo_motors_GetHandle(),1, fault_mask);
    }
    else if(emscontroller_board_HEAD_neckyaw_eyes == board_control) 
    {
      if((motor == 2) || (motor == 3) ) 
      {
        eo_motor_set_motor_status(eo_motors_GetHandle(),2, fault_mask);
        eo_motor_set_motor_status(eo_motors_GetHandle(),3, fault_mask);
      }
      else
      {
         eo_motor_set_motor_status(eo_motors_GetHandle(),motor, fault_mask);
      }      
    }
    else  // marco.accame: the joint is not coupled to any other joint 
    { 
         eo_motor_set_motor_status(eo_motors_GetHandle(),motor, fault_mask);
    }      
    
    return eores_OK;
}

extern uint32_t eo_mcserv_GetMotorFaultMask(EOmcService *p, uint8_t motor)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    // don't need to use p->config.jomos[motor].actuator.local.index, cause the emsController (and related objs) use the same indexing of the highlevel
    uint32_t state_mask = eo_get_motor_fault_mask(eo_motors_GetHandle(), motor);
    return state_mask;
}

extern int16_t eo_mcserv_GetMotorCurrent(EOmcService *p, uint8_t joint)
{
    if(NULL == p)
    {
        return(NULL);
    }
   
    int16_t curr_val = 0;
    
    // if local motor (MC4plus)
    if(1 == p->config.jomos[joint].actuator.local.type)
    {
        curr_val = s_eo_mcserv_get_current_adc (p->config.jomos[joint].actuator.local.index);
    }
    
    // if remote (EMS4rd-2foc or mc4) --> should return what's inside the array ems->motor_current [motor]
    // the structure is filled with the CAN callbacks
    else if (0 == p->config.jomos[joint].actuator.local.type)
    {
    }
    return curr_val;
}

extern uint32_t eo_mcserv_GetMotorPositionRaw(EOmcService *p, uint8_t joint)
{
    if(NULL == p)
    {
        return(NULL);
    }
   
    uint32_t pos_val = 0;
    
    // if local motor (MC4plus)
    if(1 == p->config.jomos[joint].actuator.local.type)
    {
        pos_val = s_eo_mcserv_get_quad_enc (p->config.jomos[joint].actuator.local.index);
    }
    
    // if remote (EMS4rd-2foc or mc4) --> should return what's inside the array ems->motor_position[motor]
    // the structure is filled with the CAN callbacks
    else if (0 == p->config.jomos[joint].actuator.local.type)
    {
    }
    return pos_val;
}

extern eOresult_t eo_mcserv_Actuate(EOmcService *p)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    switch(p->config.type)
    {
        case eOmcconfig_type_mc4plus:
        {
            //read all the encoders, compute pwm, send pwm to hal peripheral, update joint-motor status
            res = s_eo_mcserv_do_mc4plus(p);      
        } break;
        case eOmcconfig_type_mc4can:
        {
            // must perform motion done and send virtual strain values
            // for motiondone, you could implement some functions: motiondone_init() called at entry on run, motiondone_tick() called in here.
            // the same for virtualstrain.
            res = eores_OK;     
        } break;   
        case eOmcconfig_type_2foc:
        {
            // must start the first reading of encoders and ... enable teh joints and ........ 
            res = eores_OK;     
        } break;    
        default:
        {       
        } break;    
    }
    
    return(res);


}



extern eOresult_t eo_mcserv_Stop(EOmcService *p)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    switch(p->config.type)
    {
        case eOmcconfig_type_mc4plus:
        {
            // disable PWM for all the motors
            s_eo_mcserv_disable_all_motors(p);
            res = eores_OK;        
        } break;
        case eOmcconfig_type_mc4can:
        {
            // must send the broadcast policy to mc4 ... what else
            res = eores_OK;     
        } break;   
        case eOmcconfig_type_2foc:
        {
            // must disable the joints and ........ 
            res = eores_OK;     
        } break;    
        default:
        {       
        } break;    
    }
    
    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_mcserv_protocol_load_mc_endpoint(EOmcService *p)
{
    eOresult_t res = eores_OK;

    EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 

    // load mc endpoint specific to this board using a proper number of joint motors
    eOprot_EPcfg_t epcfgmc = {0};
    epcfgmc.endpoint    = eoprot_endpoint_motioncontrol;
    epcfgmc.numberofentities[0] = p->config.jomosnumber;
    epcfgmc.numberofentities[1] = p->config.jomosnumber;
    epcfgmc.numberofentities[2] = 1; // one controller


    if(eobool_true == eoprot_EPcfg_isvalid(&epcfgmc))
    {
        eo_nvset_LoadEP(nvset, &epcfgmc, eobool_true);
        eo_entities_Refresh(eo_entities_GetHandle());
    }                        
        
    // now we must define the .... proxy rules
    // e.g., if we have board number equal to 1 or 3 ... (eb2 or eb4) then we set it for mc only
    // in teh future we can set this proxy mode on teh basis of the board number received
    //eOprotBRD_t localboard = eoprot_board_local_get();

    return(res);
}


static eOresult_t s_eo_mcserv_init_jomo(EOmcService *p)
{
    eOresult_t res = eores_OK;
    uint8_t jm = 0;
    eOemscontroller_board_t board_control = emscontroller_board_NO_CONTROL;
    
    // in here we must init whatever is needed for the management of joint-motor.
    // in the future: eth-protocol, can-mapping maybe, 
    // surely now: encoder-reader object, actuator on hal-pwm if present, etc.
    
    // eth-protocol: TBD when needed
    
    
    // initialise pointers to joints and motors
    p->thejoints = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eOmc_joint_t*), p->config.jomosnumber);
    p->themotors = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eOmc_motor_t*), p->config.jomosnumber);
    
    // retrieve pointers to joints and motors   
    for(jm=0; jm<p->config.jomosnumber; jm++)
    {
        p->thejoints[jm] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jm); 
        p->themotors[jm] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, jm);
    }
    
    // ems controller. it is used only in some control types
    if((eOmcconfig_type_2foc == p->config.type) || (eOmcconfig_type_mc4plus == p->config.type))
    {
      
        board_control = s_eo_mcserv_getboardcontrol();
        
        //use the ankle for test experimental setup
        p->thelocalcontroller = eo_emsController_Init(board_control, emscontroller_actuation_LOCAL, p->config.jomosnumber);        
    }
    else
    {
        p->thelocalcontroller = NULL;
    }
    
    // actuator
    // reserve some memory for pwm values
    p->valuespwm = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(int16_t), p->config.jomosnumber);
   
    //low level init for motors and adc
    s_eo_mcserv_init_motors_adc_feedbacks();
   
    // encoder reader
    if((eOmcconfig_type_2foc == p->config.type) || (eOmcconfig_type_mc4plus == p->config.type))
    {
        // reserve some memory used by encoder reader to store values        
        p->valuesencoder       = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(uint32_t), p->config.jomosnumber);
        p->valuesencoder_extra = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(uint32_t), p->config.jomosnumber);
        
        // encoder-reader (so far we only have encoders supported by the encoder reader object).
        //init enc config
        eOappEncReader_cfg_t encoder_reader_cfg;
        memset(&encoder_reader_cfg, 0xFF, sizeof(eOappEncReader_cfg_t)); //0xFF is the invalid value
        encoder_reader_cfg.SPI_callbackOnLastRead = NULL, encoder_reader_cfg.SPI_callback_arg = NULL; //not using SPI callback
        encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].numberof = 0, encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].numberof = 0; //reset number of encoders on SPIs
        
        eOmcconfig_jomo_cfg_t current_jomo = {0};
        uint8_t enc_joint_index = 0, etype_primary = 0, etype_extra = 0;
        for(jm=0; jm<p->config.jomosnumber; jm++)
        {
            current_jomo    = p->config.jomos[jm];
            etype_primary   = p->config.jomos[jm].encoder.etype;
            etype_extra     = p->config.jomos[jm].extra_encoder.etype;
            
            if (p->config.jomos[jm].encoder.isthere)
            {
                // fill in the primary encoders field
                if (LOCAL_ENCODER(etype_primary))
                {
                    //set type, position and pos_type (primary encoder)
                    encoder_reader_cfg.joints[jm].primary_encoder           = (eo_appEncReader_enc_type_t) current_jomo.encoder.etype;
                    encoder_reader_cfg.joints[jm].primary_enc_position      = (eo_appEncReader_encoder_position_t) current_jomo.encoder.index;
                    encoder_reader_cfg.joints[jm].primary_encoder_pos_type  = (eo_appEncReader_detected_position_t) current_jomo.encoder.pos_type;
                    
                    //store the encoder joint value --> use it when you get the value
                    p->config.jomos[jm].encoder.enc_joint = enc_joint_index;
                    enc_joint_index++;
                    
                    //only for SPI encoders
                    if (SPI_ENCODER(etype_primary))
                    {
                        if (current_jomo.encoder.index % 2 == 0)
                        {
                           encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].numberof++; // even
                              if (encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].numberof == 1) //if not set yet
                                  encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].type = (hal_encoder_type) etype_primary;
                        }                      
                        else
                        {
                           encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].numberof++; // odd
                              if (encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].numberof == 1) //if not set yet
                                  encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].type = (hal_encoder_type) etype_primary; 
                        }
                    }
                }
                // here I could check if it's a MAIS encoder. If it's the case, I could set as a primary encoder a quad enc, so that I can get it's value
                // in the control loop and apply the limit
                else
                {
                    
                }
            }
            
            if (p->config.jomos[jm].extra_encoder.isthere)
            {
                // fill in the extra encoders field
                if (LOCAL_ENCODER(etype_extra))
                {
                    //set type, position and pos_type (extra encoder)
                    encoder_reader_cfg.joints[jm].extra_encoder             = (eo_appEncReader_enc_type_t) current_jomo.extra_encoder.etype;
                    encoder_reader_cfg.joints[jm].extra_enc_position        = (eo_appEncReader_encoder_position_t) current_jomo.extra_encoder.index;
                    encoder_reader_cfg.joints[jm].extra_encoder_pos_type    = (eo_appEncReader_detected_position_t) current_jomo.extra_encoder.pos_type;
                    
                    //store the encoder joint value --> use it when you get the value
                    p->config.jomos[jm].extra_encoder.enc_joint = enc_joint_index;
                    
                    //only for SPI encoders
                    if (SPI_ENCODER(etype_extra))
                    {
                        if (current_jomo.extra_encoder.index % 2 == 0)
                        {
                           encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].numberof++; // even
                              if (encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].numberof == 1) //if not set yet
                                  encoder_reader_cfg.SPI_streams[eo_appEncReader_stream0].type = (hal_encoder_type) etype_extra;
                        }                      
                        else
                        {
                           encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].numberof++; // odd
                              if (encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].numberof == 1) //if not set yet
                                  encoder_reader_cfg.SPI_streams[eo_appEncReader_stream1].type = (hal_encoder_type) etype_extra; 
                        }
                    }
                }
                // here I could check if it's a MAIS encoder. If it's the case, I could set as a primary encoder a quad enc, so that I can get it's value
                // in the control loop and apply the limit
                else
                {
                    
                }
            }
         
        }
        
        // in here we init the encoder reader
        p->thelocalencoderreader = (void*)eo_appEncReader_New(&encoder_reader_cfg);
    }
    else
    {
        p->thelocalencoderreader = NULL;
        p->valuesencoder         = NULL;
        p->valuesencoder_extra   = NULL;
        p->valuespwm             = NULL;
    }
    
    return(res);
}

static void s_eo_mcserv_init_motors_adc_feedbacks(void)
{
    //currently the motors are initialized all together and without config
    hal_motors_extfault_handling_init();
        
    //first initialize all ADC as indipendents
    hal_adc_common_structure_init();
        
    // init the ADC to read the (4) current values of the motor and (4) analog inputs (hall_sensors, if any) using ADC1/ADC3
    // always initialized at the moment, but the proper interface for reading the values is in EOappEncodersReader
    hal_adc_dma_init_ADC1_ADC3_hall_sensor_current ();
}

static int16_t s_eo_mcserv_get_current_adc(uint8_t motor)
{
    return hal_adc_get_current_motor_mA(motor);
}

static uint32_t s_eo_mcserv_get_quad_enc(uint8_t motor)
{
    return hal_quad_enc_getCounter(motor);
}

static eOresult_t s_eo_mcserv_can_discovery_start(EOmcService *p)
{
    eOresult_t res = eores_OK;
    
    // think of how to ask the config task (and not the run tasks) to send get firmware versions in regular times
    // for instance, we could instantiate a timer with a callback which sends an event.
    // then in the event handler of config task we call a mc_can_check_tick function which just sends the proper can messages.
    // if the mc_can_check_tick detects that all wanted answers are arrived, then it stops the timer, it prevents the same
    // function to execute once more, and sets the discovered flag to true, so that function eo_mcserv_AreResourcesReady() can returns true.
    
    // if 
    
    
    return(res);
}

static void s_eo_mcserv_enable_all_motors(EOmcService *p)
{
    //enable PWM of the motors (if not faulted)
    if (!s_eo_mcserv_are_motors_ext_faulted())
    {
        for(uint8_t jm=0; jm<p->config.jomosnumber; jm++)
        {
            if(1 == p->config.jomos[jm].actuator.local.type)
            {   // on board 
                s_eo_mcserv_pwm_enable(p->config.jomos[jm].actuator.local.index);
            }
        }
    }
    
    return;
}

static void s_eo_mcserv_disable_all_motors(EOmcService *p)
{
     for(uint8_t jm=0; jm<p->config.jomosnumber; jm++)
     {
        if(1 == p->config.jomos[jm].actuator.local.type)
        {   // on board 
            s_eo_mcserv_pwm_set(p->config.jomos[jm].actuator.local.index, 0);
            s_eo_mcserv_pwm_disable(p->config.jomos[jm].actuator.local.index);
        }
     }
     
     return;
}

static void s_eo_mcserv_pwm_set(uint8_t i, int16_t v)
{
    //out of bound
    if (i > 3)
        return;
    
    hal_motor_pwmset((hal_motor_t)i, v);
}

static void s_eo_mcserv_pwm_enable(uint8_t i)
{
    //out of bound
    if (i > 3)
        return;
    
    hal_motor_enable((hal_motor_t)i);
}

static void s_eo_mcserv_pwm_disable(uint8_t i)
{
    //out of bound
    if (i > 3)
        return;
    
    hal_motor_disable((hal_motor_t)i);
}


static eObool_t s_eo_mcserv_are_motors_ext_faulted(void)
{
    if (hal_motor_isfault())
    {
        return eobool_true;
    }
       
    return eobool_false;
}

static void s_eo_mcserv_reenable_external_fault_isr(void)
{
    hal_motor_reenable_break_interrupts();
}


extern eOresult_t s_eo_mcserv_do_mc4plus(EOmcService *p)
{
    eOresult_t res = eores_OK;
    uint8_t jm = 0;
    
    uint8_t errormask = 0x00;
    uint32_t dummy_extra = 0x00;
    hal_encoder_errors_flags fl = {0};
    EOappEncReader *app_enc_reader = p->thelocalencoderreader;
    
    #warning maybe we could add a timeout here, instead of trying to read tot times...the behaviour coded below it's the one used for ems control loop       
    uint8_t spi_stream0 = 0, spi_stream1 = 0;
    for(uint8_t i=0; i<30; ++i)
    {
        if (eo_appEncReader_isReady(app_enc_reader))
        {
            break;
        }
        // it means that the SPI encoders are not ready yet...
        else
        {
            if (!eo_appEncReader_isReadySPI_stream0(app_enc_reader)) ++spi_stream0;
            if (!eo_appEncReader_isReadySPI_stream1(app_enc_reader)) ++spi_stream1;
            hal_sys_delay(5);
        }
    }
    
    // 2. read encoders and put result into p->valuesencoder[] and p->valuesencoder_extra[]  and fill an errormask
    if (eo_appEncReader_isReady(app_enc_reader))
    {
        for(jm=0; jm<p->config.jomosnumber; jm++)
        {
            // for some type of encoder I need to get the values from EOappEncodersReader, for the others we get the value in other ways
            // at least one of the two encoder is local 
            if (LOCAL_ENCODER(p->config.jomos[jm].encoder.etype) || LOCAL_ENCODER(p->config.jomos[jm].extra_encoder.etype))
            {
                res = eo_appEncReader_GetJointValue (app_enc_reader,
                                                     (eo_appEncReader_joint_position_t)p->config.jomos[jm].encoder.enc_joint,
                                                     &p->valuesencoder[jm],
                                                     &p->valuesencoder_extra[jm],
                                                     &fl);
            }
            // MAIS is always primary
            if (p->config.jomos[jm].encoder.etype == 4) //MAIS
            {
            /* it should be a MAIS encoder! So here:
                - call a function to convert the last MAIS values (saved into the 15values array) to iCubDegrees
                - copy inside p->valuesencoder[jm] the right position value
            */
                
                //using encoder.index as prot index for getting the mais entity inside the function
                res = s_eo_mcserv_MaisValues2Pos(p->config.jomos[jm].encoder.index,
                                                 p->config.jomos[jm].actuator.local.index,
                                                 jm,
                                                 &p->valuesencoder[jm]);
            }
            
            //if the extra encoder is a rotor position, update the proper value
            if((p->config.jomos[jm].extra_encoder.isthere == 1) && (p->config.jomos[jm].extra_encoder.pos_type == 1)) // rotor position
            {
                eo_emsController_AcquireMotorPosition(jm, p->valuesencoder_extra[jm]);
            }
            
            //if the extra encoder is another joint position...where can I put those values?
            else if((p->config.jomos[jm].extra_encoder.isthere == 1) && (p->config.jomos[jm].extra_encoder.pos_type == 0)) // joint position
            {
   
            }
            
             //fill the errormask
             if (res != eores_OK)
                errormask |= 1<<(jm<<1);
        }
    }   
    else
    {
        errormask = 0xAA; // timeout
    }
    
    // 3. restart a new reading of the encoders 
    eo_appEncReader_StartRead(app_enc_reader);
    
    // 4. apply the readings (primary encoder) to localcontroller and check for possible faults
    eo_emsController_AcquireAbsEncoders((int32_t*)p->valuesencoder, errormask);
    eo_emsController_CheckFaults();
    
    // (for MAIS-controlled joints...do a check on the limits (see MC4 firmware) before entering here or inside the controller?)
    // 5. compute the pwm using pid
    eo_emsController_PWM(p->valuespwm);
    
    // 6. apply the pwm. for the case of mc4plus we call hal_pwm();
    for(jm=0; jm<p->config.jomosnumber; jm++)
    {
        s_eo_mcserv_pwm_set(p->config.jomos[jm].actuator.local.index, p->valuespwm[jm]);
    }

    // 7. propagate the status of joint motors locally computed in localcontroller to the joints / motors in ram
    {   // so far in here. but later on move it in a function.... also 2foc mode does that 
        uint8_t transmit_decoupled_pwms = 1;
        for(jm=0; jm<p->config.jomosnumber; jm++)
        {
            // joint ...
            eOmc_joint_status_t *jstatus = &(p->thejoints[jm]->status);
            if (jstatus == NULL)
            {
                continue;
            }
            
            eo_emsController_GetJointStatus(jm, jstatus);
            eo_emsController_GetActivePidStatus(jm, &(jstatus->ofpid)); 
            if(transmit_decoupled_pwms) 
            {   //this functions is used to get the motor PWM after the decoupling matrix
                eo_emsController_GetPWMOutput(jm, &(jstatus->ofpid.generic.output));
            }
            
            jstatus->modes.ismotiondone = eo_emsController_GetMotionDone(jm);

//            if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus->modes.ismotiondone)
//            {
//                #warning TBD: see how we manage the motion done for the case of mc4plus
//                /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
//                - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
//                message because the setpoint is alredy reached. this means that:
//                    - if monitormode is forever, no new set point has been configured 
//                    - if monitormode is _untilreached, the joint reached the setpoint already.
//                - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
//                message because pc104 is not interested in getting motion done.
//                */
//                if(eo_emsController_GetMotionDone(jm))
//                {
//                    jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
//                }
//            }
            
            // motor ...
            eOmc_motor_status_t *mstatus = &(p->themotors[jm]->status);
            if (mstatus == NULL)
            {
                continue;
            }
            
            eo_emsController_GetMotorStatus(jm, mstatus);
  
        }
        
    } // propagate status
    
    return(res);
}

static eOemscontroller_board_t s_eo_mcserv_getboardcontrol(void)
{
    eOemscontroller_board_t type = emscontroller_board_NO_CONTROL;
    
    uint8_t n = eoprot_board_local_get();
    
    switch(n)
    {
        // board 2-4-5-10-11 -> NO motion control
        case 1:
        case 3:
        case 9:
        case 10:
        {
            type = emscontroller_board_NO_CONTROL;
        } break;

        /* EMS Boards */
        
        // board 1-3 --> shoulders 
        case 0:
        case 2:
        {
            type = emscontroller_board_SHOULDER;
        } break;

        // board 5 --> waist
        case 4:
        {
            type = emscontroller_board_WAIST;
        } break;

        // board 6-8 --> upperlegs
        case 5:
        case 7:
        {
            type = emscontroller_board_UPPERLEG;
        } break;
        
        // board 7-9 --> ankles
        case 6:
        case 8:
        {
            type = emscontroller_board_ANKLE;
        } break;
        
        /* MC4plus Boards */
        
        // board 12 --> HeadV3: neck (pitch and roll)
        case 11:
        {
            type = emscontroller_board_HEAD_neckpitch_neckroll;
        } break;
        
        // board 13 --> HeadV3: neck (yaw) and eyes
        case 12:
        {
            type = emscontroller_board_HEAD_neckyaw_eyes;
        } break;
        
        // board 14 --> FaceV3: eyelids and jaw
        case 13:
        {
            type = emscontroller_board_FACE_eyelids_jaw;
        } break;
        
        // board 15 --> FaceV3: lips
        case 14:
        {
            type = emscontroller_board_FACE_lips;
        } break;
        
        // board 16 --> HandV3: middle (proximal, distal), index distal, little fingers
        case 15:
        {
            type = emscontroller_board_HAND_1;
        } break;
        
        // board 17 --> HandV3: thumb (proximal, abduction, distal), index proximal
        case 16:
        {
            type = emscontroller_board_HAND_2;
        } break;
        
       
        // board 17 --> ForeArmV3: wrist (differential coupling, pronosupination), finger abduction
        case 17:
        {
            type = emscontroller_board_FOREARM;
        } break;
        
        
        /* experimental boards control */
        case 97:
        {
            //type = emscontroller_board_HEAD_neckpitch_neckroll;   //2 coupled joints-mc4plus
            type = emscontroller_board_FACE_eyelids_jaw;            //indipendent joints-mc4plus
        } break;
        case 98:
        {
            type = emscontroller_board_FACE_eyelids_jaw;			//indipendent joints-mc4plus
        } break;
        
        
        default:
        {
            type = emscontroller_board_NO_CONTROL;
        } break;
       
    }
    
    return(type);   
}

static eOresult_t s_eo_mcserv_MaisValues2Pos(eOprotID32_t maisID, uint8_t motor, uint8_t joint, uint32_t* val)
{
    //how can I follow a rule based on the joint number? it seems that the rule is hardware-cabled
    
    //eOmc_joint_t *joint_ref = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, joint);
    eOmc_joint_t *joint_ref = eo_mcserv_GetJoint(eo_mcserv_GetHandle(), joint);
    float divider = joint_ref->config.jntEncoderResolution;
    if(divider < 0)
        divider = -divider;

    uint32_t temp_val;
    uint8_t n = eoprot_board_local_get();
    
    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), maisID);
    if ((status == NULL) || (joint_ref == NULL))
    {
        *val = 0;
        return eores_NOK_generic;        
    }
    
    //board 16 - 1b4 : index distal (m0), middle proximal (m1), middle distal (m2), little fingers (m3)
    if (n == 15)
    {
        switch (motor)
        {
            //index distal
            case 0:
            {
                temp_val = status->the15values.data[4] + status->the15values.data[5];
                
            } break;
            
            // middle proximal
            case 1:
            {
                temp_val = status->the15values.data[6];
                
            } break;
            
            //middle distal
            case 2:
            {
                temp_val = status->the15values.data[7] + status->the15values.data[8];
                
            } break;
            
            //little fingers
            case 3:
            {
                temp_val = status->the15values.data[11] + status->the15values.data[12] + status->the15values.data[13];
                
            } break;
            
            default:
            {
                *val = 0;
                return eores_NOK_generic;
                
            } break;
        }
        
    }
    
    //board 17 - 1b3 : thumb abduction (m0 - NO MAIS), thumb proximal (m1), thumb distal (m2), index proximal (m3)
    else if (n == 16)
    {
        switch (motor)
        {
            // motor 0 is not controlled using mais position data
            
            // thumb proximal
            case 1:
            {
                temp_val = status->the15values.data[0];
                
            } break;
            
            // thumb distal
            case 2:
            {
                temp_val = status->the15values.data[1] + status->the15values.data[2];
                
            } break;
            
            // index proximal
            case 3:
            {
                temp_val = status->the15values.data[3];
                
            } break;
            
            default:
            {
                *val = 0;
                return eores_NOK_generic;
                
            } break;
        }
        
    }
    
    *val = (float) temp_val * 65535.0 / divider; //position in iCubDegrees = (MAIS_raw * ICUB_DEGREES_full_scales) / encoder_resolution
    
    return eores_OK;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



