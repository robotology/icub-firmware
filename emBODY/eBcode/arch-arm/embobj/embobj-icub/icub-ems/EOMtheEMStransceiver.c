/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"

#include "EOtheBOARDtransceiver_hid.h"

#include "EOtransceiver.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMStransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMStransceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemstransceiver_cfg_t eom_emstransceiver_DefaultCfg = 
{
    EO_INIT(.hostipv4addr)              EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
    EO_INIT(.hostipv4port)              12345
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMStransceiver";

 
static EOMtheEMStransceiver s_emstransceiver_singleton = 
{
    EO_INIT(.transceiver)                   NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMStransceiver * eom_emstransceiver_Initialise(const eOemstransceiver_cfg_t *cfg)
{
    if(NULL != s_emstransceiver_singleton.transceiver)
    {
        return(&s_emstransceiver_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emstransceiver_DefaultCfg;
    }
    
 
    s_emstransceiver_singleton.transceiver = eo_boardtransceiver_Initialise(eom_emstransceiver_hid_userdef_get_cfg(cfg));
                                                   
    
    return(&s_emstransceiver_singleton);
}


extern EOMtheEMStransceiver* eom_emstransceiver_GetHandle(void) 
{
    if(NULL != s_emstransceiver_singleton.transceiver)
    {
        return(&s_emstransceiver_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOtransceiver* eom_emstransceiver_GetTransceiver(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emstransceiver_singleton.transceiver);
}

extern EOnvsCfg* eom_emstransceiver_GetNVScfg(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(eo_boardtransceiver_hid_GetNvsCfg());
}



extern eOresult_t eom_emstransceiver_Parse(EOMtheEMStransceiver* p, EOpacket* rxpkt, uint16_t *numberofrops, eOabstime_t* txtime)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == rxpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transceiver_Receive(s_emstransceiver_singleton.transceiver, rxpkt, numberofrops, txtime);
    
    return(res);
}


extern eOresult_t eom_emstransceiver_Form(EOMtheEMStransceiver* p, EOpacket** txpkt, uint16_t *numberofrops)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == txpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transceiver_Transmit(s_emstransceiver_singleton.transceiver, txpkt, numberofrops);
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


__weak extern eOboardtransceiver_cfg_t* eom_emstransceiver_hid_userdef_get_cfg(const eOemstransceiver_cfg_t *cfg)
{
#define USE_EB7

#if     defined(USE_EB1)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb1;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;  
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb1;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;
#elif   defined(USE_EB2)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb2;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb2;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
#elif   defined(USE_EB3)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb3;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb3;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
#elif   defined(USE_EB4)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb4;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb4;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
#elif   defined(USE_EB5)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb5;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb5;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
#elif   defined(USE_EB6)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb6;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb6;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
#elif   defined(USE_EB7)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb7;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb7;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;    
#elif   defined(USE_EB8)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb8;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb8;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
#elif   defined(USE_EB9)
    extern const EOconstvector* const eo_cfg_EPs_vectorof_eb9;
    extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
    const EOconstvector*        vectorof_endpoint_cfg   = eo_cfg_EPs_vectorof_eb9;
    eOuint16_fp_uint16_t        hashfunction_ep2index   = eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
#endif    

    static eOboardtransceiver_cfg_t boardtxrxcfg;

 
    boardtxrxcfg.vectorof_endpoint_cfg          = vectorof_endpoint_cfg;
    boardtxrxcfg.hashfunction_ep2index          = hashfunction_ep2index;
    boardtxrxcfg.remotehostipv4addr             = cfg->hostipv4addr;
    boardtxrxcfg.remotehostipv4port             = cfg->hostipv4port;
    boardtxrxcfg.tobedefined                    = 0;

    return(&boardtxrxcfg);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





