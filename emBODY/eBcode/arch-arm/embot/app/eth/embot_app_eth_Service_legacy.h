

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_SERVICE_LEGACY_H_
#define __EMBOT_APP_ETH_SERVICE_LEGACY_H_

//#include "embot_core.h"
//#include "embot_core_binary.h"

#include "EoCommon.h"
#include "EoManagement.h"
#include "EOvector.h"
#include "EOtimer.h"
#include "EOarray.h"
#include "EOtheCANdiscovery2.h"
#include "EOtheErrorManager.h"
#include "EOtheCANprotocol.h"

// legacy types defined in EOtheService{.h, _hid.h] and EOaService.h


struct eOservice_core_t
{
    // the following two are needed to use eOservice_onendofoperation_fun_t as it is in EOaService.h
    using EOaService = void;
    using onendofoperation_fun_t = void (*)(EOaService* p, eObool_t operationisok);
    
    eObool_t initted {eobool_false};
    eObool_t active {eobool_false};  
    eObool_t activateafterverify {eobool_false};
    eObool_t started {eobool_false};
    onendofoperation_fun_t onverify {nullptr};
    void *onverifyarg {nullptr};
    eOmn_serv_state_t state {eomn_serv_state_idle};
    const eOmn_serv_configuration_t* tmpcfg {nullptr};
    eOmn_serv_configuration_t servconfig {};
    constexpr eOservice_core_t() = default;
};    

struct eOservice_cantools_t
{
    EOvector* boardproperties {nullptr};    // of eObrd_canproperties_t
    EOvector* entitydescriptor {nullptr};   // of eOcanmap_entitydescriptor_t
    EOarray* discoverytargets {nullptr};    // of eOcandiscovery_target_t
    eOcandiscovery_onstop_t ondiscoverystop {nullptr}; 
    eOcanprot_command_t command {};   
    constexpr eOservice_cantools_t() = default;    
}; 

struct eOservice_diagnostics_t
{
    EOtimer* reportTimer {nullptr};
    eOreltime_t reportPeriod {5*1000*1000};  
    eOerrmanDescriptor_t errorDescriptor {};
    eOerrmanErrorType_t errorType {eo_errortype_info};
    uint8_t errorCallbackCount {0};
    uint8_t repetitionOKcase {0};
    constexpr eOservice_diagnostics_t() = default;
};


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
