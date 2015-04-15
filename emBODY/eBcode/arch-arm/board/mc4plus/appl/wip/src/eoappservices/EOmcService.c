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

static eOresult_t s_eo_mcserv_can_discovery_start(EOmcService *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOmcService s_eo_mcserv = 
{
    .initted                = eobool_false,
    .config                 = 
    {
        .type           = eomcserv_type_undefined,
        .jointnum       = 0,
        .motornum       = 0
    },
    .resourcesareready      = eobool_false
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

    return(p->config.jointnum);
}

extern eOmc_joint_t* eo_mcserv_GetJoint(EOmcService *p, uint8_t index)
{
    if((NULL == p) || (index >= p->config.jointnum))
    {
        return(NULL);
    } 
    
    return(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, index));    
}


extern uint8_t eo_mcserv_NumberOfMotors(EOmcService *p)
{
    if(NULL == p)
    {
        return(0);
    } 

    return(p->config.motornum);
}

extern eOmc_motor_t* eo_mcserv_GetMotor(EOmcService *p, uint8_t index)
{
    if((NULL == p) || (index >= p->config.motornum))
    {
        return(NULL);
    } 
    
    return(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, index));    
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
            #warning TBD: start the first reading of encoders
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
            res = eores_OK;        
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



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



