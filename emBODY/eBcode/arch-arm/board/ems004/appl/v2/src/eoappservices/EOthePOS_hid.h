
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOTHEPOS_HID_H_
#define _EOTHEPOS_HID_H_


#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"
#include "EOwatchdog.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOthePOS.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { pos_maxRegulars = 1 }; // there cannot be more than 1 pos, and typically not more than 1 signalled variable.

struct EOthePOS_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eObool_t                                configured;
    
    eOas_pos_config_t                       posconfig;
    
    eOprotID32_t                            id32;  
    eOas_pos_t*                             pos;
    EOarray*                                id32ofregulars;
    uint8_t                                 numberofowners;
    
    
    uint16_t                                not_heardof_target[2];
    uint16_t                                not_heardof_status[2];
    uint32_t                                not_heardof_counter;
    uint32_t                                not_heardof_checkrate;
    eObool_t                                transmissionisactive;
    eObool_t                                isalive;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



