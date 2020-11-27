
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHEPOS_H_
#define _EOTHEPOS_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------


/** @defgroup eo_EOthePOS Object EOthePOS
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EOtheServices.h"

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheCANdiscovery2.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOthePOS_hid EOthePOS;

enum { eo_pos_maxnumberofCANboards = 1 }; 

#warning TBD: but we may need less than 3

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOthePOS* eo_pos_Initialise(void);

extern EOthePOS* eo_pos_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_pos_GetServiceState(EOthePOS *p);
extern eOresult_t eo_pos_SendReport(EOthePOS *p);


extern eOresult_t eo_pos_Verify(EOthePOS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_pos_Activate(EOthePOS *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_pos_Deactivate(EOthePOS *p);

extern eOresult_t eo_pos_Start(EOthePOS *p);

extern eOresult_t eo_pos_SetRegulars(EOthePOS *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

extern eOresult_t eo_pos_Tick(EOthePOS *p);

extern eOresult_t eo_pos_Stop(EOthePOS *p);

extern uint8_t eo_pos_GetNumberOfOwners(EOthePOS *p); // Start() increments owners, Stop or Deactivate() decrements them.

// it enables/disables transmission of the pos boards. _Start() just starts the service, not the transmission
extern eOresult_t eo_pos_Transmission(EOthePOS *p, eObool_t on);


// we can call them if _Activate() was called. they are used by the callbacks of eth protocol
extern eOresult_t eo_pos_Config(EOthePOS *p, eOas_pos_config_t* config);
extern eOresult_t eo_pos_AcceptCANframe(EOthePOS *p, eOcanframe_t *frame, eOcanport_t port);

extern eObool_t eo_pos_isAlive(EOthePOS *p);



/** @}            
    end of group eo_EOthePOS
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


