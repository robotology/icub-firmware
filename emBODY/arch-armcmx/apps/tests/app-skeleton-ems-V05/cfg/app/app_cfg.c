// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"

#include "ipal.h"
#include "hal.h"

#include "Eocommon.h"
#include "EOethBaseModule.h"
#include "EOtheBOARDtransceiver.h"

//endpoints
//#include "eOcfg_EPs_eb1.h"
//#include "eOcfg_EPs_eb2.h"
//#include "eOcfg_EPs_eb3.h"
#include "eOcfg_EPs_eb4.h"
//#include "eOcfg_EPs_eb5.h"
//#include "eOcfg_EPs_eb6.h"
//#include "eOcfg_EPs_eb7.h"
//#include "eOcfg_EPs_eb8.h"
//#include "eOcfg_EPs_eb9.h"

//in order to get ep number
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"
#include "eOcfg_nvsEP_sk.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "app_cfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



extern EOethBaseModule_cfg_t    ethmod_cfg = 
{
    EO_INIT(.dtagramQueue_itemNum)      DGRAMPAYLOAD_NUM,
    EO_INIT(.dtagramQueue_itemSize)     DGRAMPAYLOAD_SIZE,
    EO_INIT(.remaddr)                   IPAL_ipv4addr(REM_IP0, REM_IP1, REM_IP2, REM_IP3),
    EO_INIT(.remport)                   REMPORT,
    EO_INIT(.localport)                 LOCALPORT_ONRX,
    EO_INIT(.action_onRec)              NULL,
    EO_INIT(.periodTx)                  0   //Not periodic!!!
};

extern EOethBaseModule_cfg_t    *ethmod_cfg_ptr = &ethmod_cfg; 



extern uint16_t     connectedEncodersMask_cfg =( (ENC1_ENABLE << 0)| (ENC2_ENABLE << 1) | (ENC3_ENABLE << 2) |
                                               (ENC4_ENABLE << 3) | (ENC5_ENABLE << 4) |(ENC6_ENABLE << 5) |
                                               (ENC7_ENABLE << 6) |(ENC8_ENABLE << 7) | (ENC9_ENABLE << 8)
                                               ); 

//extern const EOconstvector const * vectorof_endpoint_cfg =  eo_cfg_EPs_vectorof_eb4;
//extern eOuint16_fp_uint16_t     hashfunction_ep2index = eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
//


extern eOnvEP_t    mc_endpoint = endpoint_mc_rightlowerarm;  /**<  motion control endopoint managed by the application    */
extern eOnvEP_t    as_endpoint = 0; //currently not used  /**<  analog sensor endopoint managed by the application    */
extern eOnvEP_t    sk_endpoint = endpoint_sk_emsboard_rightlowerarm;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



