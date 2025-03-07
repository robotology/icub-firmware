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

#include "EoCommon.h"

#include "EOtheErrorManager.h"
#include "EoError.h"


#include "EOtheCANservice.h"
#include "EOtheInertials3.h"
#include "EOtheTemperatures.h"
#include "EOtheCANdiscovery2.h"
#include "EOtheMotionController.h"
#include "EOtheSKIN.h"
#include "EOtheMais.h"
#include "EOthePSC.h"
#include "EOthePOS.h"
#include "EOtheETHmonitor.h"
//#include "EOtheMAIS.h"
#include "EOtheSTRAIN.h"

#include "embot_app_eth_theFTservice.h"
#include "embot_app_eth_theBATservice.h"

#if defined(TESTRTC_IS_ACTIVE)
#include "testRTC.h"
#elif defined(enableTHESERVICETESTER)
#include "servicetester.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// if defined, the CAN flush is done at end of the DO phase, else at the beginning of the TX phase
#define CANflushMODE_DO_phase


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void flushCANtransmission();
static void waitforCANisflushed(const eOreltime_t timeout);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    // tick of currents watchdog  
//    eo_currents_watchdog_Tick(eo_currents_watchdog_GetHandle());
}


extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    // i tick the can-discovery. 
    // this function does something only if a discovery is active and if the search timer period has expired.
    eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
    
    // i manage the can-bus reception. i parse everything. 
    // it will be the can parser functions which will call the relevant objects which will do what they must.
    // as an example, the broadcast skin can frames are always parsed and are given to EOtheSKIN which will decide what to do with them
    eo_canserv_ParseAll(eo_canserv_GetHandle());    
    
#if defined(TESTRTC_IS_ACTIVE)     
    testRTC_RUN_tick();
#elif defined(enableTHESERVICETESTER)
    servicetester_runtick();
#endif    
}





extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    // so far we tick only the motion control.
    eo_motioncontrol_Tick(eo_motioncontrol_GetHandle());
    
    eo_mais_Tick(eo_mais_GetHandle());
    eo_strain_Tick(eo_strain_GetHandle());
    
    // ft service
    embot::app::eth::theFTservice::getInstance().Tick();
    
    // bat service
    embot::app::eth::theBATservice::getInstance().Tick();    
    
    eo_psc_Tick(eo_psc_GetHandle());
    eo_pos_Tick(eo_pos_GetHandle());
    
    // however, we could also tick others ....
    // TODO: see if i can move all the _Tick() in the do phase.
    
    // eo_ethmonitor_Tick(eo_ethmonitor_GetHandle()); // if we do it in here, then in eb2/eb4 it warns about execution time overflow
    
#if defined(CANflushMODE_DO_phase)
    flushCANtransmission();
#else    
    // we flush elsewhere
#endif       
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
#if defined(CANflushMODE_DO_phase)
    // we flush elsewhere
#else    
    flushCANtransmission();
#endif    
}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{  
    eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
    
    // ticks some services ... 
    // marco.accame: i put them in here just after tx phase. however, we can move it even in eom_emsrunner_hid_userdef_taskDO_activity() 
    // because eom_emsrunner_CycleHasJustTransmittedRegulars() keeps memory of previous tx cycle.
    eo_skin_Tick(eo_skin_GetHandle(), prevTXhadRegulars); 
     
    eo_inertials3_Tick(eo_inertials3_GetHandle(), prevTXhadRegulars); 
    
    eo_temperatures_Tick(eo_temperatures_GetHandle(), prevTXhadRegulars);
    
    eo_ethmonitor_Tick(eo_ethmonitor_GetHandle());


    
    // ABSOLUTELY KEEP IT LAST: wait until can tx started by flushCANtransmission() is all done
    static const eOreltime_t timeout = 3*EOK_reltime1ms;
    waitforCANisflushed(timeout);   

    return; 
}



extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_udptxfailure);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes); 
}


extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_transceivererror);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void flushCANtransmission()
{
    uint8_t txcan1frames = 0;
    uint8_t txcan2frames = 0;        
    eo_canserv_TXstartAll(eo_canserv_GetHandle(), &txcan1frames, &txcan2frames);    
    eom_emsrunner_Set_TXcanframes(eom_emsrunner_GetHandle(), txcan1frames, txcan2frames);        
}

static void waitforCANisflushed(const eOreltime_t timeout)
{
    // it lock the thread TX until all CAN frame has exited the board
//    const eOreltime_t timeout = 3*EOK_reltime1ms;
    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), timeout);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




