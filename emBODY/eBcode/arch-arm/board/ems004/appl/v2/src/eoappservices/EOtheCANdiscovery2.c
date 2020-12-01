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
#include "EoError.h"
#include "EOtheErrorManager.h"


#include "EOtheCANservice.h"
#include "EOtheCANprotocol.h"

#include "EOaction.h"

#include "EOMtheEMSconfigurator.h"

#include "EOtheEntities.h"

#include "EOMtheEMSappl.h"

#include "EOVtheCallbackManager.h"

#include "EOVtheSystem.h"

#include "hal_trace.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANdiscovery2.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANdiscovery2_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const eOcandiscovery_cfg_t eo_candiscovery_default_cfg = 
{ 
    .period     = 100*1000, 
    .timeout    = 3*1000*1000 
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define EOCANDISCOVERY2_TRACE
#define EOCANDISCOVERY2_DIAGNOSTICS_SENDUPOKRESULT
#define EOCANDISCOVERY2_DIAGNOSTICS_SENDUPDETECTEDBOARDS


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_candiscovery2_start(void);

static void s_eo_candiscovery2_resetTarget(void);

static void s_eo_candiscovery2_resetDetection(void);

static void s_eo_candiscovery2_sendDiagnosticsToHost(eObool_t allboardsfound, eObool_t allboardsareok);

static void s_eo_candiscovery2_resetSearchStatus(void);

static eObool_t s_eo_candiscovery2_AllBoardsAreFound(EOtheCANdiscovery2 *p);

static eObool_t s_eo_candiscovery2_IsDetectionOK(EOtheCANdiscovery2 *p, eObrd_canlocation_t loc, eObool_t match, eObrd_info_t *detected);

static void s_eo_candiscovery2_on_timer_expiry(void *arg);

static eObool_t s_eo_candiscovery2_search(void);

static eObool_t s_eo_isFirmwareVersionToBeVerified(const eObrd_firmwareversion_t* target);

static eObool_t s_eo_isProtocolVersionToBeVerified(const eObrd_protocolversion_t* target);

static eOresult_t s_eo_candiscovery2_getFWversion(uint8_t boardtype, eObrd_canlocation_t location, eObrd_protocolversion_t requiredprotocolversion);

static eObool_t s_eo_isFirmwareVersionCompatible(const eObrd_firmwareversion_t* target, const eObrd_firmwareversion_t* detected);

static eObool_t s_eo_isProtocolVersionCompatible(const eObrd_protocolversion_t* target, const eObrd_protocolversion_t* detected);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheCANdiscovery2 s_eo_thecandiscovery2 = 
{
    EO_INIT(.initted)                           eobool_false,
    EO_INIT(.config)                            {0},
    EO_INIT(.discoverytimer)                    NULL,
    EO_INIT(.discoverymaxretries)               10,    
    EO_INIT(.searchstatus)                      {0},
    EO_INIT(.arrayoftargets)                    NULL,
    EO_INIT(.canmapofoverlappedtargets)         {0},
    EO_INIT(.onstop)                            {0},
    EO_INIT(.detection)                         {0}
};

static const char s_eobj_ownname[] = "EOtheCANdiscovery2";

#if defined(EOCANDISCOVERY2_TRACE) 
static char s_trace_string[128] = {0};
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheCANdiscovery2* eo_candiscovery2_Initialise(const eOcandiscovery_cfg_t* cfg)
{
    if(eobool_true == s_eo_thecandiscovery2.initted)
    {
        return(&s_eo_thecandiscovery2);
    }
    
    if(NULL == cfg)
    {
        cfg = &eo_candiscovery_default_cfg;
    }
    
    memcpy(&s_eo_thecandiscovery2.config, cfg, sizeof(eOcandiscovery_cfg_t));
    
    if(eok_reltimeINFINITE == s_eo_thecandiscovery2.config.timeout)
    {
        s_eo_thecandiscovery2.discoverymaxretries = eok_reltimeINFINITE;
    }
    else
    {
        s_eo_thecandiscovery2.discoverymaxretries = s_eo_thecandiscovery2.config.timeout / s_eo_thecandiscovery2.config.period;
    }

    s_eo_thecandiscovery2.discoverytimer = eo_timer_New();

    s_eo_thecandiscovery2.arrayoftargets = eo_array_New(eo_candiscovery_maxtargets, sizeof(eOcandiscovery_target_t), NULL);
    
    s_eo_candiscovery2_resetTarget();
    s_eo_candiscovery2_resetDetection();
    s_eo_candiscovery2_resetSearchStatus();


    s_eo_thecandiscovery2.initted = eobool_true;
    
    return(&s_eo_thecandiscovery2);   
}


extern EOtheCANdiscovery2* eo_candiscovery2_GetHandle(void)
{
    if(s_eo_thecandiscovery2.initted == eobool_false)
    {
        return NULL;
    }
    
    return(&s_eo_thecandiscovery2);
}


extern eOresult_t eo_candiscovery2_Start(EOtheCANdiscovery2 *p, const eOcandiscovery_target_t *target, eOcandiscovery_onstop_t* onstop)
{    
    // prepare array of capacity 1 and copy target inside it. no need to check vs target being non NULL. 
    uint8_t memory[sizeof(eOarray_head_t)+sizeof(eOcandiscovery_target_t)] = {0};   
    EOarray *targets = eo_array_New(1, sizeof(eOcandiscovery_target_t), memory);
    eo_array_PushBack(targets, target);
       
    // perform the new call
    return(eo_candiscovery2_Start2(p, targets, onstop));        
}

 
extern eOresult_t eo_candiscovery2_Start2(EOtheCANdiscovery2 *p, EOarray *targets, eOcandiscovery_onstop_t* onstop)
{
    if((NULL == p) || (NULL == targets))
    {
        return(eores_NOK_nullpointer);
    }  
        
    uint8_t ntargets = eo_array_Size(targets);
    if(0 == ntargets)
    {
        return(eores_NOK_generic);
    }
        
    // if a search is active, i must call the stop
    if(eobool_true == s_eo_thecandiscovery2.searchstatus.searching)
    {
        eo_candiscovery2_Stop(p);
    }
        
    // reset target of previous search
    s_eo_candiscovery2_resetTarget();
    s_eo_candiscovery2_resetDetection();
    
    // reset search status
    s_eo_candiscovery2_resetSearchStatus();
    
        
    // copy the new target of boards, at most ... eo_candiscovery_maxtargets items from targets into arrayoftargets
    if(ntargets > eo_candiscovery_maxtargets)
    {
        ntargets = eo_candiscovery_maxtargets;
    }
         
    for(uint8_t i=0; i<ntargets; i++)
    {
        eo_array_PushBack(s_eo_thecandiscovery2.arrayoftargets, eo_array_At(targets, i));
    }
    
    // copy onstop
    memcpy(&s_eo_thecandiscovery2.onstop, onstop, sizeof(eOcandiscovery_onstop_t));
    
    // now start the procedure
    s_eo_thecandiscovery2.searchstatus.searching = eobool_true;
    s_eo_thecandiscovery2.searchstatus.tickingenabled = eobool_false;
    s_eo_thecandiscovery2.searchstatus.forcestop = eobool_false;
    s_eo_thecandiscovery2.detection.atleastonereplyisincompatible = eobool_false;
    s_eo_thecandiscovery2.detection.allhavereplied = eobool_false;
    s_eo_thecandiscovery2.searchstatus.timeofstart = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
#if defined(EOCANDISCOVERY2_TRACE)
    uint64_t start = s_eo_thecandiscovery2.searchstatus.timeofstart/1000;
    snprintf(s_trace_string, sizeof(s_trace_string), "EOtheCANdiscovery2: starts @ lifetime %d ms", (uint32_t)start);
    hal_trace_puts(s_trace_string);
#endif


    s_eo_candiscovery2_start();
    
    if(eobool_false == s_eo_thecandiscovery2.detection.allhavereplied)
    {
    
        EOaction_strg astg = {0};
        EOaction *action = (EOaction*)&astg;
           
        // the action depends on the state of the application. 
        // if we are in config mode, then we send an event to the config task.  
        // else if we are in run mode we just set an enable flag for teh control-loop to exec _Tick().
        
        eo_action_SetCallback(action, s_eo_candiscovery2_on_timer_expiry, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        eo_timer_Start(s_eo_thecandiscovery2.discoverytimer, eok_abstimeNOW, s_eo_thecandiscovery2.config.period, eo_tmrmode_FOREVER, action);         
    }
    
    return(eores_OK);    
    
}

extern eOresult_t eo_candiscovery2_Tick(EOtheCANdiscovery2 *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }  
    
    // i execute only if the tick is enabled    
    if(eobool_false == s_eo_thecandiscovery2.searchstatus.tickingenabled)
    {
        return(eores_OK);
    }

    // i execute only if the search is enabled    
    if(eobool_false == s_eo_thecandiscovery2.searchstatus.searching)
    {
        return(eores_OK);
    } 

#if defined(EOCANDISCOVERY2_TRACE)    
    uint64_t now = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    uint64_t delta = (now - s_eo_thecandiscovery2.searchstatus.timeofstart) / 1000;
    snprintf(s_trace_string, sizeof(s_trace_string), "EOtheCANdiscovery2: tick() after %d ms from start", (uint32_t)delta);
    hal_trace_puts(s_trace_string);
#endif
    
    // i check if there is a forced stop, which means that we have reached the max number of attempts
    if(eobool_true == s_eo_thecandiscovery2.searchstatus.forcestop)
    {   //  ... stop everything. the stop operation also sends up diagnostics or ack/nak
        return(eo_candiscovery2_Stop(p));
    }
    
    s_eo_candiscovery2_search();
          
    return(eores_OK);
}



extern eOresult_t eo_candiscovery2_OneBoardIsFound(EOtheCANdiscovery2 *p, eObrd_canlocation_t loc, eObool_t match, eObrd_info_t *detected)
{   
    if((NULL == p) || (NULL == detected))
    {
        return(eores_NOK_nullpointer);
    }
    
    // use the information inside loc to mark that a can board has replied. 
    eo_common_hlfword_bitset(&s_eo_thecandiscovery2.detection.replies[loc.port], loc.addr);
    // put inside detected what the board has told
    memcpy(&s_eo_thecandiscovery2.detection.boards[loc.port][loc.addr].info, detected, sizeof(eObrd_info_t)); 
    // set the rx time
    s_eo_thecandiscovery2.detection.boards[loc.port][loc.addr].time = (eov_sys_LifeTimeGet(eov_sys_GetHandle()) - s_eo_thecandiscovery2.searchstatus.timeofstart)/1000;
    // now mark success or failure of the query. the success depends on what we want to do with board-type, prot-version, appl-version.
    // so far we use the strict rule: 
    // - the board must be the same, 
    // - the prot-version must be the same (if the expected is not 0.0), 
    // - the appl-version must be the same (if the expected is not 0.0).
    
    eObool_t itisok = s_eo_candiscovery2_IsDetectionOK(p, loc, match, detected);

    
    // mark a failure
    if(eobool_false == itisok)
    {
        s_eo_thecandiscovery2.detection.atleastonereplyisincompatible = eobool_true;
        eo_common_hlfword_bitset(&s_eo_thecandiscovery2.detection.incompatibilities[loc.port], loc.addr);
    }
    
    // now i verify if every board i was looking for has replied. if so, i can stop procedure.    
    s_eo_thecandiscovery2.detection.allhavereplied = s_eo_candiscovery2_AllBoardsAreFound(p);
    
#if defined(EOCANDISCOVERY2_TRACE)     
    snprintf(s_trace_string, sizeof(s_trace_string), "EOtheCANdiscovery2: FOUND board at CAN%d ID%d after %d ms from start", loc.port+1, loc.addr, s_eo_thecandiscovery2.detection.boards[loc.port][loc.addr].time);
    hal_trace_puts(s_trace_string);
#endif
    
    if(eobool_true == s_eo_thecandiscovery2.detection.allhavereplied)
    {
        eo_candiscovery2_Stop(p);
    }

    return(eores_OK);
}


extern eObool_t eo_candiscovery2_IsSearchOK(EOtheCANdiscovery2 *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }   

    uint16_t incompatibilitiesInBothCAN = s_eo_thecandiscovery2.detection.incompatibilities[0] | s_eo_thecandiscovery2.detection.incompatibilities[1];
    eObool_t allboardsareok = (0x0000 == incompatibilitiesInBothCAN) ? (eobool_true) : (eobool_false);
    
    if((eobool_true == s_eo_thecandiscovery2.detection.allhavereplied) && (eobool_true == allboardsareok))
    {   
        return(eobool_true);
    }
    return(eobool_false);
}


//extern const eOcandiscovery_target_t* eo_candiscovery2_GetTarget(EOtheCANdiscovery2 *p)
//{
//    if(NULL == p)
//    {
//        return(NULL);
//    }   
//    
//    #warning TODO: remove it
//    
//    return(eo_array_At(s_eo_thecandiscovery2.arrayoftargets, 0));    
//}

extern const EOarray* eo_candiscovery2_GetTargets(EOtheCANdiscovery2 *p)
{
    if(NULL == p)
    {
        return(NULL);
    }   

    return(s_eo_thecandiscovery2.arrayoftargets);    
}

extern const eOcandiscovery_detection_t* eo_candiscovery2_GetDetection(EOtheCANdiscovery2 *p)
{
    if(NULL == p)
    {
        return(NULL);
    }   

    return(&s_eo_thecandiscovery2.detection);    
}


extern eOresult_t eo_candiscovery2_Stop(EOtheCANdiscovery2 *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // there are four possible situations:
    // 1. all boards are found and they all have correct fw version: OK
    //    if we have this situation we are not in timeout expiry. 
    //    i must: stop the timer, send up a confirmation OK, reset status. 
    // 2. all boards are found but at least one has an incorrect fw version: NOK_allfound_fwerror
    //    if we have this situation we are not in timeout expiry. 
    //    i must: stop the timer, send up a confirmation NOK_allfound_fwerror, send up diagnostics with list of error in fw version, reset status.
    // 3. at least one board is not found. the ones found have correct fw version: NOK_notfound_fwok
    //    if we have this situation we are in timeout expiry.
    //    i must: stop the timer, send up a confirmation NOK_notfound_fwok, send up diagnostics with list of missing boards, reset status.
    // 4. at least one board is not found. at least one found have wrong fw version: NOK_notfound_fwko
    //    if we have this situation we are in timeout expiry.
    //    i must: stop the timer, send up a confirmation NOK_notfound_fwko, send up diagnostics with list of missing boards, send up diagnostics with list of error in fw version, reset status.    

    uint64_t endtime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    uint64_t delta = endtime - s_eo_thecandiscovery2.searchstatus.timeofstart;  
    delta /= 1000;
    s_eo_thecandiscovery2.detection.duration = (uint16_t)delta;    

#if defined(EOCANDISCOVERY2_TRACE)     
snprintf(s_trace_string, sizeof(s_trace_string), "EOtheCANdiscovery2: stop() after %d ms from start", (uint32_t)delta);
    hal_trace_puts(s_trace_string);
#endif    
    
    eObool_t allboardsareok = (eobool_false == s_eo_thecandiscovery2.detection.atleastonereplyisincompatible) ? (eobool_true) : (eobool_false);
    
    if((eobool_true == s_eo_thecandiscovery2.detection.allhavereplied) && (eobool_true == allboardsareok))
    {
        eo_timer_Stop(s_eo_thecandiscovery2.discoverytimer);
        s_eo_candiscovery2_sendDiagnosticsToHost(eobool_true, eobool_true); 
        s_eo_candiscovery2_resetSearchStatus();
    }
    else
    {
        eo_timer_Stop(s_eo_thecandiscovery2.discoverytimer);
        s_eo_candiscovery2_sendDiagnosticsToHost(s_eo_thecandiscovery2.detection.allhavereplied, allboardsareok);
        s_eo_candiscovery2_resetSearchStatus();        
    }
    
    // put it in heres, so that we can call a _Start() inside the callback ....
    if(NULL != s_eo_thecandiscovery2.onstop.function)
    {  
        eObool_t allisok = s_eo_thecandiscovery2.detection.allhavereplied && allboardsareok;
        s_eo_thecandiscovery2.onstop.function(s_eo_thecandiscovery2.onstop.parameter, &s_eo_thecandiscovery2, allisok);
    }
        
    return(eores_OK); 
}


extern eOresult_t eo_candiscovery2_SendLatestSearchResults(EOtheCANdiscovery2 *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eObool_t allboardsareok = (eobool_false == s_eo_thecandiscovery2.detection.atleastonereplyisincompatible) ? (eobool_true) : (eobool_false);
    s_eo_candiscovery2_sendDiagnosticsToHost(s_eo_thecandiscovery2.detection.allhavereplied, allboardsareok);
    
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_candiscovery2_sendDiagnosticsToHost(eObool_t allboardsfound, eObool_t allboardsareok)
{
    eOerrmanDescriptor_t errdes = {0};
    uint8_t i = 0;
    uint8_t n = 0;
    
    // 1. at first we send info about what we have discovered for all targets
    for(i=eOcanport1; i<eOcanports_number; i++)
    {
        uint16_t differentboardtype = s_eo_thecandiscovery2.detection.differentboardtype[i] & s_eo_thecandiscovery2.canmapofoverlappedtargets[i];
        uint16_t differentfirmwareversion = s_eo_thecandiscovery2.detection.differentfirmwareversion[i] & s_eo_thecandiscovery2.canmapofoverlappedtargets[i];
        uint16_t differentprotocolversion = s_eo_thecandiscovery2.detection.differentprotocolversion[i] & s_eo_thecandiscovery2.canmapofoverlappedtargets[i];
        
        uint16_t maskofdifferences = differentboardtype | differentfirmwareversion | differentprotocolversion;
        
        for(n=0; n<15; n++)
        {
            if(eobool_true == eo_common_hlfword_bitcheck(maskofdifferences, n))
            {   // we have differences
                errdes.code             = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_candiscovery_detectedboard);
                errdes.sourcedevice     = (eOcanport1 == i) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                errdes.sourceaddress    = 0;
                errdes.par16            = (n & 0x000f);
                errdes.par64            = ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.firmware.build) |
                                          ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.firmware.minor << 8) | ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.firmware.major << 16) |
                                          ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.protocol.minor << 24) | ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.protocol.major << 32) |
                                          ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.type << 40) |
                                          (((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].time) << 48);                
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, s_eobj_ownname, &errdes);                    
            } 
#if defined(EOCANDISCOVERY2_DIAGNOSTICS_SENDUPDETECTEDBOARDS)            
            else if(eobool_true == eo_common_hlfword_bitcheck(s_eo_thecandiscovery2.detection.replies[i], n))
            {   // we dont have differences. send an info but only if the board has replied 
                errdes.code             = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_candiscovery_detectedboard);
                errdes.sourcedevice     = (eOcanport1 == i) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                errdes.sourceaddress    = 0;
                errdes.par16            = (n & 0x000f);
                errdes.par64            = ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.firmware.build) |
                                          ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.firmware.minor << 8) | ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.firmware.major << 16) |
                                          ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.protocol.minor << 24) | ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.protocol.major << 32) |
                                          ((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].info.type << 40) |
                                          (((uint64_t)s_eo_thecandiscovery2.detection.boards[i][n].time) << 48);                      
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);                                 
            }
#endif            
        }
 
    }    
    
    // 2. then we send info about OK or KO
    
    if((eobool_true == allboardsfound) && (eobool_true == allboardsareok))
    {
        
#if defined(EOCANDISCOVERY2_DIAGNOSTICS_SENDUPOKRESULT)
                
        uint8_t ntargets = eo_array_Size(s_eo_thecandiscovery2.arrayoftargets);
        for(uint8_t t=0; t<ntargets; t++)
        { 
            eOcandiscovery_target_t* target = (eOcandiscovery_target_t*) eo_array_At(s_eo_thecandiscovery2.arrayoftargets, t);
            if(NULL == target)
            {
                continue;
            }
            
            uint8_t numofboardsintarget = eo_common_hlfword_bitsetcount(target->canmap[0]) + eo_common_hlfword_bitsetcount(target->canmap[1]);
            
            if(numofboardsintarget > 0)
            {
                eObool_t fakesearch = eo_common_byte_bitcheck(s_eo_thecandiscovery2.searchstatus.fakesearchmask, t);
                eOerrmanErrorType_t errtype = (eobool_true == fakesearch) ? (eo_errortype_warning) : (eo_errortype_info);
                
                errdes.code             = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_candiscovery_ok);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.par16            = (numofboardsintarget & 0x00ff) | ((uint16_t)fakesearch << 12);
                errdes.par64            = ((uint64_t)target->info.firmware.build) |
                                          ((uint64_t)target->info.firmware.minor << 8) | ((uint64_t)target->info.firmware.major << 16) |                                          
                                          ((uint64_t)target->info.protocol.minor << 24) | ((uint64_t)target->info.protocol.major << 32) |
                                          ((uint64_t)target->info.type << 40) |
                                          (((uint64_t)s_eo_thecandiscovery2.detection.duration) << 48);                      
                eo_errman_Error(eo_errman_GetHandle(), errtype, NULL, s_eobj_ownname, &errdes);            
            } 
        
        }
        

        
#endif        
        
    }
    else
    {
       
        if(eobool_false == allboardsfound)
        {
            uint8_t ntargets = eo_array_Size(s_eo_thecandiscovery2.arrayoftargets);
            for(uint8_t t=0; t<ntargets; t++)
            { 
                eOcandiscovery_target_t* target = (eOcandiscovery_target_t*) eo_array_At(s_eo_thecandiscovery2.arrayoftargets, t);
                if(NULL == target)
                {
                    continue;
                }
                // "CFG: CANdiscovery cannot find some boards. In p16: board type in 0xff00 and number of missing in 0x00ff. In p64: mask of missing addresses in 0x000000000000ffff"           
                for(i=eOcanport1; i<eOcanports_number; i++)
                {
                    uint16_t maskofmissing = (~s_eo_thecandiscovery2.detection.replies[i]) & target->canmap[i];  
                    
                    if(0 != maskofmissing)
                    { 
                        uint8_t numofmissing = eo_common_hlfword_bitsetcount(maskofmissing);      
                        
                        errdes.code             = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_candiscovery_boardsmissing);
                        errdes.sourcedevice     = (eOcanport1 == i) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                        errdes.sourceaddress    = 0;
                        errdes.par16            = ((uint16_t)target->info.type << 8) | (numofmissing & 0x00ff);
                        errdes.par64            = (uint64_t)maskofmissing | (((uint64_t)s_eo_thecandiscovery2.detection.duration) << 48);
                        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
                    }
                }
            }
                
        }
        
        if(eobool_false == allboardsareok)
        {
            uint8_t ntargets = eo_array_Size(s_eo_thecandiscovery2.arrayoftargets);
            for(uint8_t t=0; t<ntargets; t++)
            { 
                eOcandiscovery_target_t* target = (eOcandiscovery_target_t*) eo_array_At(s_eo_thecandiscovery2.arrayoftargets, t);
                if(NULL == target)
                {
                    continue;
                }
                // "CFG: CANdiscovery detected invalid boards. In p16: target board type in 0xff00 and number of invalid in 0x00ff. In p64: each nibble contains 0x0 if ok, mask 0x1 if wrong type, mask 0x2 if wrong fw, mask 0x4 if wrong prot"            
                for(i=eOcanport1; i<eOcanports_number; i++)
                {
                    uint16_t maskofinvalid = (s_eo_thecandiscovery2.detection.incompatibilities[i]) & target->canmap[i];  
                         
                    if(0 != maskofinvalid)
                    { 
                        uint8_t numofinvalid = eo_common_hlfword_bitsetcount(maskofinvalid);     
                        uint64_t nibbles64 = 0;
                        for(n=0; n<15; n++)
                        {
                            if(eobool_true == eo_common_hlfword_bitcheck(maskofinvalid, n))
                            {
                                // ok, we have an invalid detected board. let us see what nibble to put
                                uint8_t nib = 0x0;
                                if(target->info.type != s_eo_thecandiscovery2.detection.boards[i][n].info.type)
                                {
                                    nib |= 0x1;
                                }
                                if(eobool_true == s_eo_isFirmwareVersionToBeVerified(&target->info.firmware))
                                {   // signal error only if fw version is to be verified
                                    if(eobool_false == s_eo_isFirmwareVersionCompatible(&target->info.firmware, &s_eo_thecandiscovery2.detection.boards[i][n].info.firmware))
                                    {
                                        nib |= 0x2;
                                    }
                                }
                                if(eobool_true == s_eo_isProtocolVersionToBeVerified(&target->info.protocol))
                                {   // signal error ony if prot version is be verified   
                                    if(eobool_false == s_eo_isProtocolVersionCompatible(&target->info.protocol, &s_eo_thecandiscovery2.detection.boards[i][n].info.protocol))                                
                                    {
                                        nib |= 0x4;
                                    }
                                }
                                nibbles64 |= ((uint64_t)nib << (4*n));
                            }
                        }
                        
                        errdes.code             = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_candiscovery_boardsinvalid);
                        errdes.sourcedevice     = (eOcanport1 == i) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                        errdes.sourceaddress    = 0;
                        errdes.par16            = ((uint16_t)target->info.type << 8) | (numofinvalid & 0x00ff);
                        errdes.par64            = nibbles64;
                        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
                    }
                }
            }
            
        }                
    }     
}


static void s_eo_candiscovery2_resetTarget(void)
{  
    eo_array_Reset(s_eo_thecandiscovery2.arrayoftargets); 
    memset(s_eo_thecandiscovery2.canmapofoverlappedtargets, 0, sizeof(s_eo_thecandiscovery2.canmapofoverlappedtargets));

    memset(&s_eo_thecandiscovery2.onstop, 0, sizeof(eOcandiscovery_onstop_t));
}

static void s_eo_candiscovery2_resetDetection(void)
{
    // reset results of previous detections
    memset(&s_eo_thecandiscovery2.detection, 0, sizeof(s_eo_thecandiscovery2.detection));     
}

static void s_eo_candiscovery2_resetSearchStatus(void)
{
    memset(&s_eo_thecandiscovery2.searchstatus, 0, sizeof(s_eo_thecandiscovery2.searchstatus));
}



static eObool_t s_eo_candiscovery2_AllBoardsAreFound(EOtheCANdiscovery2 *p)
{
    // it is enough to compare the s_eo_thecandiscovery2.canmapofoverlappedtargets with the  s_eo_thecandiscovery2.detection.replies
    if((s_eo_thecandiscovery2.canmapofoverlappedtargets[0] == s_eo_thecandiscovery2.detection.replies[0]) && (s_eo_thecandiscovery2.canmapofoverlappedtargets[1] == s_eo_thecandiscovery2.detection.replies[1]))
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }      
}



static eObool_t s_eo_candiscovery2_IsDetectionOK(EOtheCANdiscovery2 *p, eObrd_canlocation_t loc, eObool_t match, eObrd_info_t *detected)  
{  
    eObool_t protocolVersionIsOK = eobool_true;
    eObool_t firmwareVersionIsOK = eobool_true;
    eObool_t boardtTypeIsOK = eobool_true;
    eObool_t targetFound = eobool_false;
    
    // marco.accame on 6 april 2018: we must compare vs all the targets inside arrayoftargets. 
    // at first we find the correct target by matching loc, then we do what we need
    
    uint8_t ntargets = eo_array_Size(s_eo_thecandiscovery2.arrayoftargets);
    eOcandiscovery_target_t *target = NULL;
    
    for(uint8_t t=0; t<ntargets; t++)
    { 
        target = (eOcandiscovery_target_t*) eo_array_At(s_eo_thecandiscovery2.arrayoftargets, t);
        if((NULL != target) && (eobool_true == eo_common_hlfword_bitcheck(target->canmap[loc.port], loc.addr)))
        {
            targetFound = eobool_true;
            break;
        }        
    }
    
    if(eobool_true == targetFound)
    {
        // in target we have the one
        
        if(target->info.type != detected->type)
        {   // the board must be of the same type
            eo_common_hlfword_bitset(&s_eo_thecandiscovery2.detection.differentboardtype[loc.port], loc.addr);
            boardtTypeIsOK = eobool_false;
        }

        if(eobool_false == s_eo_isProtocolVersionToBeVerified(&target->info.protocol))
        {   // if the protocol version is not to be verified, i don't care about the answer
            protocolVersionIsOK = eobool_true;
        }
        else
        {   // ok, i compare ...          
            // check differences between target and detected
            if(0 != memcmp(&detected->protocol, &target->info.protocol, sizeof(detected->protocol)))
            {   // in such a case i mark a difference in prot version. not necessarily we return false. because it may be that the minor 
                eo_common_hlfword_bitset(&s_eo_thecandiscovery2.detection.differentprotocolversion[loc.port], loc.addr);
            }
            
            // verify compatibility of protocol version
            if(eobool_false == s_eo_isProtocolVersionCompatible(&target->info.protocol, &detected->protocol))
            {   
                protocolVersionIsOK = eobool_false;
            }
        }
        
        if(eobool_false == s_eo_isFirmwareVersionToBeVerified(&target->info.firmware))
        {   // if the firmware version is not to be verified, i don't care about the answer
            firmwareVersionIsOK = eobool_true;
        }
        else
        {   // ok, i compare
            // check differences between target and detected
            if(0 != memcmp(&detected->firmware, &target->info.firmware, sizeof(target->info.firmware)))
            {   // in such a case i mark a difference in fw version.
                eo_common_hlfword_bitset(&s_eo_thecandiscovery2.detection.differentfirmwareversion[loc.port], loc.addr);
            } 
            
            // verify compatibility of firmware version
            if(eobool_false == s_eo_isFirmwareVersionCompatible(&target->info.firmware, &detected->firmware)) 
            {   
                firmwareVersionIsOK = eobool_false;
            }          
            
        }        
        
    }
     
    
    // return the logical AND of all tests
    return(targetFound && boardtTypeIsOK && firmwareVersionIsOK && protocolVersionIsOK);    
}


static void s_eo_candiscovery2_on_timer_expiry(void *arg)
{
    EOtheCANdiscovery2* p = (EOtheCANdiscovery2*)arg;
    
    p->searchstatus.tickingenabled = eobool_true;
    
    p->searchstatus.discoverynumretries ++;
    
    if(p->searchstatus.discoverynumretries >= p->discoverymaxretries)
    {   // make it stop ...
        p->searchstatus.forcestop = eobool_true;        
    }
        
    // ok, ... if we are in run i dont do anything else because i regularly call _Tick() every 1 ms.    
    // however, if in config mode i must alert the processing task to execute a _Tick().
    eOsmStatesEMSappl_t state = eo_sm_emsappl_STerr;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &state);
    if(eo_sm_emsappl_STcfg == state)
    {   // must send an event to the handling task
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_userdef01);
    }    
}



static eObool_t s_eo_candiscovery2_search(void)
{
    eObool_t allFound = eobool_true;
        
#if defined(EOCANDISCOVERY2_TRACE)     
    hal_trace_puts("EOtheCANdiscovery2: searching on CAN");
#endif
    
    uint8_t ntargets = eo_array_Size(s_eo_thecandiscovery2.arrayoftargets);
        
    for(uint8_t t=0; t<ntargets; t++)
    { 
        if(eobool_false == eo_common_byte_bitcheck(s_eo_thecandiscovery2.searchstatus.fakesearchmask, t))
        {
            // it is not a fake search, hence we must do action for this board
            eOcandiscovery_target_t *target = (eOcandiscovery_target_t*) eo_array_At(s_eo_thecandiscovery2.arrayoftargets, t);
            if(NULL == target)
            {
                continue;
            }
            
            for(uint8_t i=eOcanport1; i<eOcanports_number; i++)
            {
                for(uint8_t j=1; j<15; j++)
                {   // valid addresses are [1, 14]
                    if((eobool_true == eo_common_hlfword_bitcheck(target->canmap[i], j)) && (eobool_false == eo_common_hlfword_bitcheck(s_eo_thecandiscovery2.detection.replies[i], j)))
                    {
                        eObrd_canlocation_t location = {0};
                        location.port = i; location.addr = j; location.insideindex = eobrd_caninsideindex_none;
                        s_eo_candiscovery2_getFWversion(target->info.type, location, target->info.protocol);
                        allFound = eobool_false;
                    }        
                }
            }            
        }
        
        
    }    
    
    s_eo_thecandiscovery2.detection.allhavereplied = allFound;
        
    if(eobool_true == s_eo_thecandiscovery2.detection.allhavereplied)
    {
        eo_candiscovery2_Stop(&s_eo_thecandiscovery2);
    }

    return(s_eo_thecandiscovery2.detection.allhavereplied);
}



static eOresult_t s_eo_candiscovery2_getFWversion(uint8_t boardtype, eObrd_canlocation_t location, eObrd_protocolversion_t requiredprotocolversion)
{  
    eOcanprot_command_t command = {0};
    command.value = (void*)&requiredprotocolversion;
    
    eObool_t found = eobool_false;
    
    
    switch(boardtype)
    {
        case eobrd_cantype_mc4:
        case eobrd_cantype_foc:
        {
            found = eobool_true;
            command.clas = eocanprot_msgclass_pollingMotorControl;
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;            
        } break;
        
        case eobrd_cantype_mais:
        case eobrd_cantype_strain:
        case eobrd_cantype_mtb:
        case eobrd_cantype_strain2:
        case eobrd_cantype_mtb4:
        case eobrd_cantype_rfe:
        case eobrd_cantype_psc:
        case eobrd_cantype_pmc:
        case eobrd_cantype_sg3:
        {
            found = eobool_true;
            command.clas = eocanprot_msgclass_pollingAnalogSensor;
            command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION;         
        } break;
        
        default:
        {
            found = eobool_false;            
        } break;               
    }
    
    if(eobool_false == found)
    {
        return(eores_NOK_generic);        
    }
    
    
    return(eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location));     
}




static eObool_t s_eo_isFirmwareVersionCompatible(const eObrd_firmwareversion_t* target, const eObrd_firmwareversion_t* detected)
{
    //if((detected->major != target->major) || (detected->minor != target->minor)) 
    //if((detected->major != target->major) || (detected->minor < target->minor))    
    if((detected->major != target->major) || (detected->minor != target->minor) || (detected->build < target->build))
    {
        return(eobool_false);
    }
    
    return(eobool_true);
}


static eObool_t s_eo_isProtocolVersionCompatible(const eObrd_protocolversion_t* target, const eObrd_protocolversion_t* detected)
{
    if((detected->major != target->major) || (detected->minor != target->minor))    
    {
        return(eobool_false);
    }
    
    return(eobool_true);    
}


static eObool_t s_eo_isFirmwareVersionToBeVerified(const eObrd_firmwareversion_t* target)
{  
    if((0 == target->major) && (0 == target->minor) && (0 == target->build))
    {
        return(eobool_false);
    }
    
    return(eobool_true);
}


static eObool_t s_eo_isProtocolVersionToBeVerified(const eObrd_protocolversion_t* target)
{
    if((0 == target->major) && (0 == target->minor))    
    {
        return(eobool_false);
    }
    
    return(eobool_true);    
}



static void s_eo_candiscovery2_start(void)
{
    eOerrmanDescriptor_t errdes = {0};
    
    s_eo_thecandiscovery2.searchstatus.fakesearchmask = 0;  

    s_eo_thecandiscovery2.canmapofoverlappedtargets[0] = 0;
    s_eo_thecandiscovery2.canmapofoverlappedtargets[1] = 0;    
    
    
    uint8_t ntargets = eo_array_Size(s_eo_thecandiscovery2.arrayoftargets);
        
    for(uint8_t t=0; t<ntargets; t++)
    { 
        eOcandiscovery_target_t *target = (eOcandiscovery_target_t*) eo_array_At(s_eo_thecandiscovery2.arrayoftargets, t);
        
        errdes.code             = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_candiscovery_started);
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.par16            = target->canmap[1]; 
        errdes.par64            = ((uint64_t)target->info.firmware.build) |
                                  ((uint64_t)target->info.firmware.minor << 8) | ((uint64_t)target->info.firmware.major << 16) |                                          
                                  ((uint64_t)target->info.protocol.minor << 24) | ((uint64_t)target->info.protocol.major << 32) |
                                  ((uint64_t)target->info.type << 40) |
                                  ((uint64_t)target->canmap[0] << 48);            
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);
        
        if((eobool_false == s_eo_isFirmwareVersionToBeVerified(&target->info.firmware)) && (eobool_false == s_eo_isProtocolVersionToBeVerified(&target->info.protocol)))
        {
            eo_common_byte_bitset(&s_eo_thecandiscovery2.searchstatus.fakesearchmask, t);
        }
        
        s_eo_thecandiscovery2.canmapofoverlappedtargets[0] |= target->canmap[0];
        s_eo_thecandiscovery2.canmapofoverlappedtargets[1] |= target->canmap[1];
    }
    
    
    // 1. i send to all the can boards the first request. all subsequent requests are managed by the _Tick() function which is triggered by the timer
    //    however, if we dont need to send any request, this function sets s_eo_thecandiscovery2.detection.allhavereplied to true nd we dont neeed to
    //    activate the timer.
    
    s_eo_candiscovery2_search();    
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



