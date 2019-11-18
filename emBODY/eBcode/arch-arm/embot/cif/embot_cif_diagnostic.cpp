
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_cif_diagnostic.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#warning TODO: add embot sys etc so that we get time w/ embot::sys::now()
#include "EOVtheSystem.h"

#include "embot_common.h"
#include "embot_utils.h"
#include "embot_eprot_rop.h"
#include "embot_eprot_diagnostics.h"


#include "EOMtheEMSDiagnostic.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

static void s_manage_trace(const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes);
static void s_manage_fatal(const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes);


extern void embot_cif_diagnostic_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes)
{
    if(eo_errortype_trace == errtype)
    {   
        // trace is not transmitted and is managed in here         
        s_manage_trace(info, caller, errdes);
        return;
    }
    
    #warning TODO: manage extended info (add methods + test them)
    #warning DECIDE: if we want to communicate the caller object via diagnostics [for now we dont do it]
    
    embot::eprot::diagnostics::TYP typ = static_cast<embot::eprot::diagnostics::TYP>(errtype-1);
    embot::eprot::diagnostics::SRC src = static_cast<embot::eprot::diagnostics::SRC>(errdes->sourcedevice);
    embot::eprot::diagnostics::ADR adr = static_cast<embot::eprot::diagnostics::ADR>(errdes->sourceaddress);
    embot::eprot::diagnostics::EXT ext = embot::eprot::diagnostics::EXT::none;
    embot::eprot::diagnostics::FFU ffu = embot::eprot::diagnostics::FFU::none;
    
    embot::eprot::diagnostics::InfoBasic ib {
        eov_sys_LifeTimeGet(eov_sys_GetHandle()),
        errdes->code,
        {typ, src, adr, ext, ffu},
        errdes->par16,
        errdes->par64,        
    };
    
    EOMtheEMSDiagnostic::instance().send(ib,true);
    
    
    if(errtype <= eo_errortype_error)
    {
        return;
    }
    
    // else: manage the fatal error
    
    s_manage_fatal(info, caller, errdes);  
}
       
static void s_manage_trace(const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes)
{
    #warning TODO: manage print of trace on debug port    
}

static void s_manage_fatal(const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes)
{
    #warning TODO: manage fatal error (go in error state, start periodic tx of error status)  
    // see: s_eom_emsappl_OnError() for details
}


// --------------------------------------------------------------------------------------------------------------------
// - test unit
// --------------------------------------------------------------------------------------------------------------------
// TBD

    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

