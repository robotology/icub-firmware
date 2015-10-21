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
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

#include "EOemsController.h"

#include "hal_sys.h"

#include "EOVtheCallbackManager.h"

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

static void s_eo_motioncontrol_send_periodic_error_report(void *p);

static eOresult_t s_eo_motioncontrol_onendofverify_encoder(EOaService* s, eObool_t operationisok);

static eOresult_t s_eo_motioncontrol_onstop_search4focs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static eOresult_t s_eo_motioncontrol_SetCurrentSetpoint(EOtheMotionController *p, int16_t *pwmList, uint8_t size);

static eOresult_t s_eo_motioncontrol_onendofverify_mais(EOaService* s, eObool_t operationisok);

static eOresult_t s_eo_motioncontrol_onstop_search4mc4s(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);
    
static void s_eo_motioncontrol_UpdateJointStatus(EOtheMotionController *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMotionController s_eo_themotcon = 
{
    .service = 
    {
        .servconfig             = { .type = eomn_serv_NONE },
        .initted                = eobool_false,
        .active                 = eobool_false,
        .activateafterverify    = eobool_false,
        .running                = eobool_false,
        .onverify               = NULL            
    },
    .diagnostics = 
    {
        .reportTimer            = NULL,
        .reportPeriod           = 10*EOK_reltime1sec,
        .errorDescriptor        = {0},
        .errorType              = eo_errortype_info,
        .errorCallbackCount     = 0,
        .repetitionOKcase       = 10
    },     
    .sharedcan =
    {
        .boardproperties        = NULL,
        .entitydescriptor       = NULL,
        .discoverytarget        = {0},
        .ondiscoverystop        = {0},
        .command                = {0}, 
    },
    
    .numofjomos                 = 0,

    .mcfoc = 
    {
        .thecontroller          = NULL,
        .theencoderreader       = NULL
    },
    .mcmc4 =
    {
        .servconfigmais         = {0},
        .themais                = NULL,
        .themc4boards           = NULL
    }   
};

static const char s_eobj_ownname[] = "EOtheMotionController";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMotionController* eo_motioncontrol_Initialise(void)
{
    if(eobool_true == s_eo_themotcon.service.initted)
    {
        return(&s_eo_themotcon);
    }
    
    s_eo_themotcon.service.active = eobool_false;
    s_eo_themotcon.service.running = eobool_false;

    s_eo_themotcon.numofjomos = 0;

    s_eo_themotcon.service.servconfig.type = eomn_serv_NONE;
    
    // up to to 12 mc4 OR upto 4 foc (the MAIS is managed directly by the EOtheMAIS object)
    //#warning CHECK: we could use only 3 mc4 boards instead of 12 in candiscovery. shall we do it?
    s_eo_themotcon.sharedcan.boardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), eo_motcon_maxJOMOs, NULL, NULL, NULL, NULL);
    
    // up to 12 jomos
    s_eo_themotcon.sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_motcon_maxJOMOs, NULL, NULL, NULL, NULL);
    
    s_eo_themotcon.mcfoc.thecontroller = NULL;
    s_eo_themotcon.mcfoc.theencoderreader = eo_encoderreader_Initialise();
            
    s_eo_themotcon.mcmc4.themais = eo_mais_Initialise();
           
    s_eo_themotcon.diagnostics.reportTimer = eo_timer_New();

    s_eo_themotcon.service.initted = eobool_true;
    
    return(&s_eo_themotcon);   
}


extern EOtheMotionController* eo_motioncontrol_GetHandle(void)
{
    if(eobool_true == s_eo_themotcon.service.initted)
    {
        return(&s_eo_themotcon);
    }
    
    return(NULL);
}


extern eOmotioncontroller_mode_t eo_motioncontrol_GetMode(EOtheMotionController *p)
{
    if(NULL == p)
    {
        return(eo_motcon_mode_NONE);
    }

    return((eOmotioncontroller_mode_t)p->service.servconfig.type);
}


extern eOresult_t eo_motioncontrol_Verify(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }  
    
    
    if((eo_motcon_mode_foc != servcfg->type) && (eo_motcon_mode_mc4 != servcfg->type))
    {
        return(eores_NOK_generic);
    }

    if(eobool_true == s_eo_themotcon.service.active)
    {
        eo_motioncontrol_Deactivate(p);        
    } 

    // make sure the timer is not running
    eo_timer_Stop(s_eo_themotcon.diagnostics.reportTimer);        
    
    if(eomn_serv_MC_foc == servcfg->type)
    {
        
        s_eo_themotcon.service.onverify = onverify;
        s_eo_themotcon.service.activateafterverify = activateafterverify;

        // 1. prepare the can discovery for foc boards 
        memset(&s_eo_themotcon.sharedcan.discoverytarget, 0, sizeof(s_eo_themotcon.sharedcan.discoverytarget));
        s_eo_themotcon.sharedcan.discoverytarget.boardtype = eobrd_cantype_foc;
        s_eo_themotcon.sharedcan.discoverytarget.protocolversion.major = servcfg->data.mc.foc_based.version.protocol.major; 
        s_eo_themotcon.sharedcan.discoverytarget.protocolversion.minor = servcfg->data.mc.foc_based.version.protocol.minor;
        s_eo_themotcon.sharedcan.discoverytarget.firmwareversion.major = servcfg->data.mc.foc_based.version.firmware.major; 
        s_eo_themotcon.sharedcan.discoverytarget.firmwareversion.minor = servcfg->data.mc.foc_based.version.firmware.minor;   
        
        EOconstarray* carray = eo_constarray_Load((EOarray*)&servcfg->data.mc.foc_based.arrayofjomodescriptors);
        
        uint8_t numofjomos = eo_constarray_Size(carray);
        uint8_t i = 0;
        for(i=0; i<numofjomos; i++)
        {
            const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
            eo_common_hlfword_bitset(&s_eo_themotcon.sharedcan.discoverytarget.canmap[jomodes->actuator.foc.canloc.port], jomodes->actuator.foc.canloc.addr);         
        }
                
        s_eo_themotcon.sharedcan.ondiscoverystop.function = s_eo_motioncontrol_onstop_search4focs;
        s_eo_themotcon.sharedcan.ondiscoverystop.parameter = (void*)servcfg;
        
        // 2. at first i verify the encoders. then, function s_eo_motioncontrol_onendofverify_encoder() shall either issue an encoder error or start discovery of foc boards
         eo_encoderreader_Verify(eo_encoderreader_GetHandle(), &servcfg->data.mc.foc_based.arrayofjomodescriptors, s_eo_motioncontrol_onendofverify_encoder, eobool_true); 
        
    }
    else if(eomn_serv_MC_mc4 == servcfg->type)
    {
        
        s_eo_themotcon.service.onverify = onverify;
        s_eo_themotcon.service.activateafterverify = activateafterverify;
        
        // 1. prepare the verify of mais
        s_eo_themotcon.mcmc4.servconfigmais.type = eomn_serv_AS_mais;
        memcpy(&s_eo_themotcon.mcmc4.servconfigmais.data.as.mais, &servcfg->data.mc.mc4_based.mais, sizeof(eOmn_serv_config_data_as_mais_t)); 

        // 2. prepare the can discovery of mc4 boards
        memset(&s_eo_themotcon.sharedcan.discoverytarget, 0, sizeof(s_eo_themotcon.sharedcan.discoverytarget));
        s_eo_themotcon.sharedcan.discoverytarget.boardtype = eobrd_cantype_mc4;
        s_eo_themotcon.sharedcan.discoverytarget.protocolversion.major = servcfg->data.mc.mc4_based.mc4version.protocol.major; 
        s_eo_themotcon.sharedcan.discoverytarget.protocolversion.minor = servcfg->data.mc.mc4_based.mc4version.protocol.minor;
        s_eo_themotcon.sharedcan.discoverytarget.firmwareversion.major = servcfg->data.mc.mc4_based.mc4version.firmware.major; 
        s_eo_themotcon.sharedcan.discoverytarget.firmwareversion.minor = servcfg->data.mc.mc4_based.mc4version.firmware.minor;   
        
        uint8_t i = 0;
        for(i=0; i<12; i++)
        {            
            const eOmn_serv_canlocation_t *canloc = &servcfg->data.mc.mc4_based.mc4joints[i];
            eo_common_hlfword_bitset(&s_eo_themotcon.sharedcan.discoverytarget.canmap[canloc->port], canloc->addr);         
        }
                       
        s_eo_themotcon.sharedcan.ondiscoverystop.function = s_eo_motioncontrol_onstop_search4mc4s;
        s_eo_themotcon.sharedcan.ondiscoverystop.parameter = (void*)servcfg;
             
        // 3. at first i verify the mais. then, function s_eo_motioncontrol_onendofverify_mais() shall either issue a mais error or start discovery of mc4 boards      
        eo_mais_Verify(eo_mais_GetHandle(), &s_eo_themotcon.mcmc4.servconfigmais, s_eo_motioncontrol_onendofverify_mais, eobool_true);
      
    }        
    
    
    
    return(eores_OK);   
}


extern eOresult_t eo_motioncontrol_Deactivate(EOtheMotionController *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_themotcon.service.active)
    {
        return(eores_OK);        
    } 
    
    
    // at first we stop the service
    if(eobool_true == s_eo_themotcon.service.running)
    {
        eo_motioncontrol_Stop(p);   
    }        
    
    // then we deconfig things
    if(eomn_serv_MC_foc == s_eo_themotcon.service.servconfig.type)
    {
        
        // for foc-based, we must ... deconfig mc foc boards, unload them, set num of entities = 0, clear status, deactivate encoder 
        
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, s_eo_themotcon.sharedcan.entitydescriptor); 
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, s_eo_themotcon.sharedcan.entitydescriptor); 
        

        eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_themotcon.sharedcan.boardproperties); 
        
        eo_encoderreader_Deactivate(p->mcfoc.theencoderreader);
        
        // ems controller ... unfortunately cannot be deinitted
        ///// to be done a deinit to the ems controller
        
        // now i reset 
        eo_vector_Clear(s_eo_themotcon.sharedcan.entitydescriptor);
        eo_vector_Clear(s_eo_themotcon.sharedcan.boardproperties);               
    
    }
    else if(eomn_serv_MC_mc4 == s_eo_themotcon.service.servconfig.type)
    {
        
        // for mc4-based, we must ... deconfig mc4 foc boards, unload them, set num of entities = 0, clear status, deactivate mais 
        
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, s_eo_themotcon.sharedcan.entitydescriptor); 
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, s_eo_themotcon.sharedcan.entitydescriptor); 
        

        eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_themotcon.sharedcan.boardproperties); 
               
        // now i reset 
        eo_vector_Clear(s_eo_themotcon.sharedcan.entitydescriptor);
        eo_vector_Clear(s_eo_themotcon.sharedcan.boardproperties);
        
        eo_mais_Deactivate(p->mcmc4.themais);        
        memset(&s_eo_themotcon.mcmc4.servconfigmais, 0, sizeof(s_eo_themotcon.mcmc4.servconfigmais));        

    }    
    
    
    s_eo_themotcon.numofjomos = 0;
    eo_entities_SetNumOfJoints(eo_entities_GetHandle(), 0);
    eo_entities_SetNumOfMotors(eo_entities_GetHandle(), 0);
    
    memset(&s_eo_themotcon.service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    s_eo_themotcon.service.servconfig.type = eo_motcon_mode_NONE;
    memset(&s_eo_themotcon.sharedcan.discoverytarget, 0, sizeof(eOcandiscovery_target_t));
    
    s_eo_themotcon.service.onverify = NULL;
    s_eo_themotcon.service.activateafterverify = eobool_false;
    s_eo_themotcon.sharedcan.ondiscoverystop.function = NULL;
    s_eo_themotcon.sharedcan.ondiscoverystop.parameter = NULL;
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_themotcon.diagnostics.reportTimer);  
    
    s_eo_themotcon.service.active = eobool_false;    
    
    return(eores_OK);
}


extern eOresult_t eo_motioncontrol_Activate(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }  

    if((eo_motcon_mode_foc != servcfg->type) && (eo_motcon_mode_mc4 != servcfg->type))
    {
        return(eores_NOK_generic);
    }    
    
    if(eobool_true == s_eo_themotcon.service.active)
    {
        eo_motioncontrol_Deactivate(p);        
    }   

    if(eomn_serv_MC_foc == servcfg->type)
    {
        
        EOconstarray* carray = eo_constarray_Load((EOarray*)&servcfg->data.mc.foc_based.arrayofjomodescriptors);
        
        uint8_t numofjomos = eo_constarray_Size(carray);
        
        eo_entities_SetNumOfJoints(eo_entities_GetHandle(), numofjomos);
        eo_entities_SetNumOfMotors(eo_entities_GetHandle(), numofjomos);
        

        if(0 == eo_entities_NumOfJoints(eo_entities_GetHandle()))
        {
            s_eo_themotcon.service.active = eobool_false;
            return(eores_NOK_generic);
        }
        else
        {                
            memcpy(&s_eo_themotcon.service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
          
            s_eo_themotcon.numofjomos = numofjomos;
            
            // now... use the servcfg
            uint8_t i = 0;
            
            EOconstarray* carray = eo_constarray_Load((EOarray*)&servcfg->data.mc.foc_based.arrayofjomodescriptors);

            
            // load the can mapping 
            for(i=0; i<numofjomos; i++)
            {
                const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
                
                eOcanmap_board_properties_t prop = {0};
                
                prop.type = eobrd_cantype_foc;
                prop.location.port = jomodes->actuator.foc.canloc.port;
                prop.location.addr = jomodes->actuator.foc.canloc.addr;
                prop.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
                prop.requiredprotocol.major = servcfg->data.mc.foc_based.version.protocol.major;
                prop.requiredprotocol.minor = servcfg->data.mc.foc_based.version.protocol.minor;
                
                eo_vector_PushBack(s_eo_themotcon.sharedcan.boardproperties, &prop);            
            }
            eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_themotcon.sharedcan.boardproperties); 
            
            // load the entity mapping.
            for(i=0; i<numofjomos; i++)
            {
                const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
                
                eOcanmap_entitydescriptor_t des = {0};
                
                des.location.port = jomodes->actuator.foc.canloc.port;
                des.location.addr = jomodes->actuator.foc.canloc.addr;
                des.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
                des.index = (eOcanmap_entityindex_t)i;

                eo_vector_PushBack(s_eo_themotcon.sharedcan.entitydescriptor, &des);            
            }        
            eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, s_eo_themotcon.sharedcan.entitydescriptor); 
            eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, s_eo_themotcon.sharedcan.entitydescriptor);        

            // init the encoders
            
            eo_encoderreader_Activate(p->mcfoc.theencoderreader, &servcfg->data.mc.foc_based.arrayofjomodescriptors);

            
            // init the emscontroller.
            #warning TODO: change the emsController. see comments below
            //                the emscontroller is not a singleton which can be initted and deinitted. 
            // it should have a _Initialise(), a _GetHandle(), a _Config(cfg) and a _Deconfig().
            if(NULL == s_eo_themotcon.mcfoc.thecontroller)
            {
                s_eo_themotcon.mcfoc.thecontroller = eo_emsController_Init((eOemscontroller_board_t)servcfg->data.mc.foc_based.boardtype4mccontroller, emscontroller_actuation_2FOC, numofjomos);   
            }
            
            s_eo_themotcon.service.active = eobool_true;        
        }
    
    }
    else if(eomn_serv_MC_mc4 == servcfg->type)
    {
        
        const uint8_t numofjomos = 12;
        
        eo_entities_SetNumOfJoints(eo_entities_GetHandle(), numofjomos);
        eo_entities_SetNumOfMotors(eo_entities_GetHandle(), numofjomos);
        

        if(0 == eo_entities_NumOfJoints(eo_entities_GetHandle()))
        {
            s_eo_themotcon.service.active = eobool_false;
            return(eores_NOK_generic);
        }
        else
        {                
            memcpy(&s_eo_themotcon.service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
          
            s_eo_themotcon.numofjomos = numofjomos;
            
            // now... use the servcfg
            uint8_t i = 0;
          
            eOcanmap_board_properties_t prop = {0};
            const eOmn_serv_canlocation_t *canloc = NULL;
            
            // load the can mapping for the 12 boards ... (only mc4 boards as teh mais was added bt eo_mais_Activate()
            
            // push the 12 mc4 boards
            for(i=0; i<numofjomos; i++)
            {
                canloc = &servcfg->data.mc.mc4_based.mc4joints[i];
                               
                prop.type = eobrd_cantype_mc4;
                prop.location.port = canloc->port;
                prop.location.addr = canloc->addr;
                prop.location.insideindex = canloc->insideindex;
                prop.requiredprotocol.major = servcfg->data.mc.mc4_based.mc4version.protocol.major;
                prop.requiredprotocol.minor = servcfg->data.mc.mc4_based.mc4version.protocol.minor;
                
                eo_vector_PushBack(s_eo_themotcon.sharedcan.boardproperties, &prop);            
            }
           

            // load the 12 of them
            eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_themotcon.sharedcan.boardproperties); 
            
            // load the entity mapping.
            eOcanmap_entitydescriptor_t des = {0};
            for(i=0; i<numofjomos; i++)
            {               
                canloc = &servcfg->data.mc.mc4_based.mc4joints[i];
                                              
                des.location.port = canloc->port;
                des.location.addr = canloc->addr;
                des.location.insideindex = canloc->insideindex;
                des.index = (eOcanmap_entityindex_t)i;

                eo_vector_PushBack(s_eo_themotcon.sharedcan.entitydescriptor, &des);            
            }        
            eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, s_eo_themotcon.sharedcan.entitydescriptor); 
            eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, s_eo_themotcon.sharedcan.entitydescriptor);        

            // init the mais
            
            eo_mais_Activate(p->mcmc4.themais, &s_eo_themotcon.mcmc4.servconfigmais);
            eo_mais_Start(p->mcmc4.themais);
            
            // do something with the mc4s....
            
            s_eo_themotcon.mcmc4.themc4boards = eo_mc4boards_Initialise(NULL);            
            eo_mc4boards_Config(eo_mc4boards_GetHandle());
            
            // init others
            eo_virtualstrain_Initialise();
            eo_motiondone_Initialise();
                    
            s_eo_themotcon.service.active = eobool_true;         
        }            
        
    }

    
    return(eores_OK);   
}



extern eOresult_t eo_motioncontrol_Start(EOtheMotionController *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == s_eo_themotcon.service.active)
    {   // nothing to do
        return(eores_OK);
    } 
        
    if(eobool_true == s_eo_themotcon.service.running)
    {
        return(eores_OK);
    }
    
    // mc4based: enable broadcast etc
    // focbased: just init a read of the encoder
    if(eomn_serv_MC_foc == p->service.servconfig.type)
    {   
        // just start a reading of encoders        
        eo_encoderreader_StartReading(p->mcfoc.theencoderreader);

    }
    else if(eomn_serv_MC_mc4 == p->service.servconfig.type)
    {
        eo_mc4boards_BroadcastStart(eo_mc4boards_GetHandle());
    }
    
    
    s_eo_themotcon.service.running = eobool_true;
    
    return(eores_OK);    
}


extern eOresult_t eo_motioncontrol_Tick(EOtheMotionController *p)
{
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == s_eo_themotcon.service.active)
    {   // nothing to do 
        return(eores_OK);
    } 
    
    if(eobool_false == s_eo_themotcon.service.running)
    {
        return(eores_OK);
    }
    
    
    if(eomn_serv_MC_foc == p->service.servconfig.type)
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
            if(eobool_true == eo_encoderreader_IsReadingAvailable(p->mcfoc.theencoderreader))
            {
                break;
            }
            else
            {
                hal_sys_delay(5*hal_RELTIME_1microsec);
            }
        }        
        
        // read the encoders
        
        if(eobool_true == eo_encoderreader_IsReadingAvailable(p->mcfoc.theencoderreader))
        {    
            for(i=0; i<p->numofjomos; i++)
            {
                uint32_t extra = 0;
                res = eo_encoderreader_Read(p->mcfoc.theencoderreader, i, &(encvalue[i]), &extra, &(encflags[i]));
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
        eo_encoderreader_StartReading(p->mcfoc.theencoderreader);
            
        eo_emsController_AcquireAbsEncoders((int32_t*)encvalue, error_mask);
        
        eo_emsController_CheckFaults();
            
        /* 2) pid calc */
        eo_emsController_PWM(pwm);

        /* 3) prepare and punt in rx queue new setpoint */
        s_eo_motioncontrol_SetCurrentSetpoint(p, pwm, 4); 
     
        /* 4) update joint status */
        s_eo_motioncontrol_UpdateJointStatus(p);
  
    }
    else if(eomn_serv_MC_mc4 == p->service.servconfig.type)
    {
        // motion done
        eo_motiondone_Tick(eo_motiondone_GetHandle());
    
        // virtual strain
        eo_virtualstrain_Tick(eo_virtualstrain_GetHandle());        
        
    }

    return(eores_OK);    
}


extern eOresult_t eo_motioncontrol_Stop(EOtheMotionController *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == s_eo_themotcon.service.active)
    {   // nothing to do 
        return(eores_OK);
    } 
    
    if(eobool_false == s_eo_themotcon.service.running)
    {
        return(eores_OK);
    }
    
    if(eomn_serv_MC_foc == p->service.servconfig.type)
    {   
        // just put controller in idle mode        
        eo_emsMotorController_GoIdle();
    }      
    else if(eomn_serv_MC_mc4 == p->service.servconfig.type)
    {
        // just stop broadcast of the mc4 boards
        eo_mc4boards_BroadcastStop(eo_mc4boards_GetHandle()); 
    }    
      
    s_eo_themotcon.service.running = eobool_false;
    
    return(eores_OK);    
}


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
        .maxvelocityofjoint =        0,
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


static void s_eo_motioncontrol_send_periodic_error_report(void *p)
{
    s_eo_themotcon.diagnostics.errorDescriptor.par64 ++;
    eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
    
    if(EOK_int08dummy != s_eo_themotcon.diagnostics.errorCallbackCount)
    {
        s_eo_themotcon.diagnostics.errorCallbackCount--;
    }
    if(0 == s_eo_themotcon.diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(s_eo_themotcon.diagnostics.reportTimer);
    }
}


static eOresult_t s_eo_motioncontrol_onstop_search4focs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_themotcon.service.activateafterverify))
    {
        const eOmn_serv_configuration_t * mcserv = (const eOmn_serv_configuration_t *)par;
        eo_motioncontrol_Activate(&s_eo_themotcon, mcserv);
    }
    
    s_eo_themotcon.diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    s_eo_themotcon.diagnostics.errorDescriptor.sourceaddress    = 0;
    s_eo_themotcon.diagnostics.errorDescriptor.par16            = 0;
    s_eo_themotcon.diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    
    if(eobool_true == searchisok)
    {        
        s_eo_themotcon.diagnostics.errorType = eo_errortype_debug;
        s_eo_themotcon.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_ok);
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
        
        if((0 != s_eo_themotcon.diagnostics.repetitionOKcase) && (0 != s_eo_themotcon.diagnostics.reportPeriod))
        {
            s_eo_themotcon.diagnostics.errorCallbackCount = s_eo_themotcon.diagnostics.repetitionOKcase;        
            eo_timer_Start(s_eo_themotcon.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themotcon.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    } 

    if(eobool_false == searchisok)
    {
        s_eo_themotcon.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_candiscovery_of_foc);
        s_eo_themotcon.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
        
        if(0 != s_eo_themotcon.diagnostics.reportPeriod)
        {
            s_eo_themotcon.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_themotcon.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themotcon.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }     
           
    if(NULL != s_eo_themotcon.service.onverify)
    {
        s_eo_themotcon.service.onverify(&s_eo_themotcon, searchisok); 
    }    
    
    return(eores_OK);   
}

static eOresult_t s_eo_motioncontrol_onendofverify_encoder(EOaService* s, eObool_t operationisok)
{    
    if(eobool_true == operationisok)
    {
        eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_themotcon.sharedcan.discoverytarget, &s_eo_themotcon.sharedcan.ondiscoverystop);        
    }    
    else
    {
        // the encoder reader fails. we dont even start the discovery of the foc boards. we just issue an error report and call onverify() w/ false argument
        
        // prepare things
        s_eo_themotcon.diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
        s_eo_themotcon.diagnostics.errorDescriptor.sourceaddress    = 0;
        s_eo_themotcon.diagnostics.errorDescriptor.par16            = 0;
        s_eo_themotcon.diagnostics.errorDescriptor.par64            = 0;    
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
        

        // fill error description. and transmit it
        s_eo_themotcon.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_encoders_verify);
        s_eo_themotcon.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
        
        if(0 != s_eo_themotcon.diagnostics.reportPeriod)
        {
            s_eo_themotcon.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_themotcon.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themotcon.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
  
        // call onverify
        if(NULL != s_eo_themotcon.service.onverify)
        {
            s_eo_themotcon.service.onverify(&s_eo_themotcon, eobool_false); 
        }            
                
    }  
    
    return(eores_OK);    
}



static eOresult_t s_eo_motioncontrol_onendofverify_mais(EOaService* s, eObool_t operationisok)
{
    if(eobool_true == operationisok)
    {
        eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_themotcon.sharedcan.discoverytarget, &s_eo_themotcon.sharedcan.ondiscoverystop);        
    } 
    else
    {
        // the mais fails. we dont even start the discovery of the mc4 boards. we just issue an error report and call onverify() w/ false argument
        
        // prepare things        
        s_eo_themotcon.diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
        s_eo_themotcon.diagnostics.errorDescriptor.sourceaddress    = 0;
        s_eo_themotcon.diagnostics.errorDescriptor.par16            = 0;
        s_eo_themotcon.diagnostics.errorDescriptor.par64            = 0;    
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
               
        // fill error description. and transmit it
        s_eo_themotcon.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_mc4_failed_mais_verify);
        s_eo_themotcon.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
        
        if(0 != s_eo_themotcon.diagnostics.reportPeriod)
        {
            s_eo_themotcon.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_themotcon.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themotcon.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
 
        // call onverify
        if(NULL != s_eo_themotcon.service.onverify)
        {
            s_eo_themotcon.service.onverify(&s_eo_themotcon, eobool_false); 
        }            
        
    }
 
    return(eores_OK);    
}


static eOresult_t s_eo_motioncontrol_onstop_search4mc4s(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_themotcon.service.activateafterverify))
    {
        const eOmn_serv_configuration_t * mcserv = (const eOmn_serv_configuration_t *)par;
        eo_motioncontrol_Activate(&s_eo_themotcon, mcserv);
    }

    s_eo_themotcon.diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    s_eo_themotcon.diagnostics.errorDescriptor.sourceaddress    = 0;
    s_eo_themotcon.diagnostics.errorDescriptor.par16            = 0;
    s_eo_themotcon.diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    
    if(eobool_true == searchisok)
    {        
        s_eo_themotcon.diagnostics.errorType = eo_errortype_debug;
        s_eo_themotcon.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_mc4_ok);
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
        
        if((0 != s_eo_themotcon.diagnostics.repetitionOKcase) && (0 != s_eo_themotcon.diagnostics.reportPeriod))
        {
            s_eo_themotcon.diagnostics.errorCallbackCount = s_eo_themotcon.diagnostics.repetitionOKcase;        
            eo_timer_Start(s_eo_themotcon.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themotcon.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    } 
    
    if(eobool_false == searchisok)
    {
        s_eo_themotcon.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_mc4_failed_candiscovery_of_mc4);
        s_eo_themotcon.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themotcon.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themotcon.diagnostics.errorDescriptor);
        
        if(0 != s_eo_themotcon.diagnostics.reportPeriod)
        {
            s_eo_themotcon.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_themotcon.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themotcon.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    
    if(NULL != s_eo_themotcon.service.onverify)
    {
        s_eo_themotcon.service.onverify(&s_eo_themotcon, searchisok); 
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



