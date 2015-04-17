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

#include "eOcommon.h"
#include "EOtheErrorManager.h"

#include "EOemsController.h"

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

static eOresult_t s_eo_mcserv_init_jomo(EOmcService *p);

static eOresult_t s_eo_mcserv_can_discovery_start(EOmcService *p);

static eOresult_t s_eo_mcserv_do_mc4plus(EOmcService *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOmcService s_eo_mcserv = 
{
    .initted                = eobool_false,
    .config                 = 
    {
        .type               = eomcserv_type_undefined,
        .jomosnumber        = 0
    },
    .resourcesareready      = eobool_false,
    .thejoints              = {NULL},
    .themotors              = {NULL},
    .thelocalcontroller     = NULL,
    .thelocalencoderreader  = NULL,
    .valuesencoder          = {0},
    .valuespwm              = {0}
};

//static const char s_eobj_ownname[] = "EOmcService";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOmcService* eo_mcserv_Initialise(eOmcserv_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        return(NULL);
    }
    
    // remove later on
    if(eomcserv_type_mc4plus != cfg->type)
    {
        return(NULL);
    }    
    
    memcpy(&s_eo_mcserv.config, cfg, sizeof(eOmcserv_cfg_t));
    
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

extern eOresult_t eo_mcserv_CheckResources(EOmcService *p)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    switch(p->config.type)
    {
        case eomcserv_type_mc4plus:
        {
            // nothing to verify, .... so far. we thus set everything to ready, so taht eo_mcserv_AreResourcesReady() can returns true.
            p->resourcesareready = eobool_true;
            res = eores_OK;        
        } break;
        case eomcserv_type_mc4can:
        {
            // must start the discovery procedure for mc4 boards and mais on can bus.
            res = s_eo_mcserv_can_discovery_start(p);     
        } break;   
        case eomcserv_type_2foc:
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
        case eomcserv_type_mc4plus:
        {
            #warning TBD: in eo_mcserv_Start() put the first reading of encoders eo_appEncReader_StartRead() or similar
            res = eores_OK;        
        } break;
        case eomcserv_type_mc4can:
        {
            // must send the broadcast policy to mc4 ... what else
            res = eores_OK;     
        } break;   
        case eomcserv_type_2foc:
        {
            // must start the first reading of encoders and ... enable teh joints and ........ 
            #warning TBD: in eo_mcserv_Start() put the first reading of encoders eo_appEncReader_StartRead() or similar
            res = eores_OK;     
        } break;    
        default:
        {       
        } break;    
    }
    
    return(res);
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
        case eomcserv_type_mc4plus:
        {
            #warning TBD: read all the encoders, compute pwm, send pwm to hal peripheral, update joint-motor status
            res = s_eo_mcserv_do_mc4plus(p);      
        } break;
        case eomcserv_type_mc4can:
        {
            // must perform motion done and send virtual strain values
            // for motiondone, you could implement some functions: motiondone_init() called at entry on run, motiondone_tick() called in here.
            // the same for virtualstrain.
            res = eores_OK;     
        } break;   
        case eomcserv_type_2foc:
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
        case eomcserv_type_mc4plus:
        {
            #warning TBD: maybe stop pwm , disable teh joints and ??? see what 2foc does 
            res = eores_OK;        
        } break;
        case eomcserv_type_mc4can:
        {
            // must send the broadcast policy to mc4 ... what else
            res = eores_OK;     
        } break;   
        case eomcserv_type_2foc:
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



static eOresult_t s_eo_mcserv_init_jomo(EOmcService *p)
{
    eOresult_t res = eores_OK;
    uint8_t jm = 0;
    
    // in here we must init whatever is needed for the management of joint-motor.
    // in the future: eth-protocol, can-mapping maybe, 
    // surely now: encoder-reader object, actuator on hal-pwm if present, etc.
    
    // eth-protocol: TBD when needed
    
    
    // retrieve pointers to joints and motors   
    for(jm=0; jm<p->config.jomosnumber; jm++)
    {
        p->thejoints[jm] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jm); 
        p->themotors[jm] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, jm);
    }
    
    // ems controller. it is used only in some control types
    if((eomcserv_type_2foc == p->config.type) || (eomcserv_type_mc4plus == p->config.type))
    {
        p->thelocalcontroller = eo_emsController_Init();        
    }
    else
    {
        p->thelocalcontroller = NULL;
    }
    
   
    if((eomcserv_type_2foc == p->config.type) || (eomcserv_type_mc4plus == p->config.type))
    {
        // encoder-reader (so far we only have encoders supported by the encoder reader object).
        #warning TBD: init the encoder-reader
        for(jm=0; jm<p->config.jomosnumber; jm++)
        {
            // in here we prepare the config array
            // p->config.jomos[jm].encoder.etype ... 0 is aea
            // p->config.jomos[jm].encoder.index ... is index strating from 0
        }
        // in here we init the encoder reader
        p->thelocalencoderreader = (void*)0x3;
    }
    else
    {
        p->thelocalencoderreader = NULL;
    }
    
    // actuator
    for(jm=0; jm<p->config.jomosnumber; jm++)
    {
        if(1 == p->config.jomos[jm].actuator.any.type)
        {   // on board
            #warning TBD: init the hal pwm
            uint8_t pwm = p->config.jomos[jm].actuator.local.index;
            pwm = pwm;
        }
    }
    
    
    return(res);
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

void myhal_pwm(uint8_t i, int16_t v)
{
    
}

void myhal_pwm_init(uint8_t i)
{
    
}

extern eOresult_t s_eo_mcserv_do_mc4plus(EOmcService *p)
{
    eOresult_t res = eores_OK;
    uint8_t jm = 0;
    
    uint8_t errormask = 0x00;
    
    #warning TBD: add what is missing for do phase
      
    // 1. wait some time until encoders are ready. if not ready by a timeout, then issue a diagnostics message and ... quit or continue?
    // tbd
    
    // 2. read encoders and put result into p->valuesencoder[] and fill an errormask
    errormask = 0x00;
    // tbd
    
    // 3. restart a new reading of the encoders 
    // tbd
    
    // 4. apply the readings to localcontroller
    eo_emsController_AcquireAbsEncoders((int32_t*)p->valuesencoder, errormask);
    
    // 5. compute the pwm using pid
    eo_emsController_PWM(p->valuespwm);
    
    // 6. apply the pwm. for the case of mc4plus we call hal_pwm();
    for(jm=0; jm<p->config.jomosnumber; jm++)
    {
        myhal_pwm(p->config.jomos[jm].actuator.local.index, p->valuespwm[jm]);
    }

    // 7. propagate the status of joint motors locally computed in localcontroller to the joints / motors in ram
    {   // so far in here. but later on move it in a function.... also 2foc mode does that 
        uint8_t transmit_decoupled_pwms = 1;
        for(jm=0; jm<p->config.jomosnumber; jm++)
        {
            // joint ...
            eOmc_joint_status_t *jstatus = &(p->thejoints[jm]->status);
            
            eo_emsController_GetJointStatus(jm, jstatus);
            eo_emsController_GetActivePidStatus(jm, &(jstatus->ofpid)); 
            if(transmit_decoupled_pwms) 
            {   //this functions is used to get the motor PWM after the decoupling matrix
                eo_emsController_GetPWMOutput(jm, &(jstatus->ofpid.output));
            }
            if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus->basic.motionmonitorstatus)
            {
                #warning TBD: see how we manage the motion done for the case of mc4plus
                /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
                - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
                message because the setpoint is alredy reached. this means that:
                    - if monitormode is forever, no new set point has been configured 
                    - if monitormode is _untilreached, the joint reached the setpoint already.
                - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
                message because pc104 is not interested in getting motion done.
                */
                if(eo_emsController_GetMotionDone(jm))
                {
                    jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
                }
            }
            
            // motor ...
            eOmc_motor_status_t *mstatus = &(p->themotors[jm]->status);
            
            eo_emsController_GetMotorStatus(jm, mstatus);
  
        }
        
    } // propagate status
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



