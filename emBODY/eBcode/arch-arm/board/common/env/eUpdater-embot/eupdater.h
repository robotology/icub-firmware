/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EUPDATER_H_
#define _EUPDATER_H_



#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOsocketDatagram.h"

// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern void eupdater_init(void);
    
extern eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec);



// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section
 
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


