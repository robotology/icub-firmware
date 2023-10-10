
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __SERVICETESTER_H_
#define __SERVICETESTER_H_

#ifdef __cplusplus
extern "C" {
#endif



// - declaration of extern public functions ---------------------------------------------------------------------------

// this must be called inside eo_services_Initialise()
extern void servicetester_init(void);

// this must be called inside eom_emsconfigurator_hid_userdef_ProcessUserdef03Event()
extern void servicetester_cfgtick(void);
    
// this must be called inside eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception() 
extern void servicetester_runtick(void);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


