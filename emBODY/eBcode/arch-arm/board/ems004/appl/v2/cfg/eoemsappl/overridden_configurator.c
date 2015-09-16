
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
#include "string.h"
#include "EoCommon.h"
#include "EOMtask.h"

#include "EOMtheEMSappl.h"
#include "EOtheEMSApplBody.h"
#include "EOMtheEMSapplCfg.h"


#include "EoError.h"
#include "EOtheErrorManager.h"

#include "EOtheCANservice.h"

#include "EOtheMAIS.h"

#include "EOtheCANdiscovery2.h"

#include "EOtheMC4boards.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

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
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// marco.accame on Nov 26 2014: the user-defined emsconfigurator_evt_userdef is triggered by the EOtheEMSapplBody to
// tell the EOMtheEMSconfigurator that has received a CAN frame.

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef00Event(EOMtheEMSconfigurator* p)
{
    // in here we want to read all can frames that we have on can1 and also on can2
    
    uint8_t numofrxframes = 0;
    
    if(0 != (numofrxframes = eo_canserv_NumberOfFramesInRXqueue(eo_canserv_GetHandle(), eOcanport1)))
    {
        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, numofrxframes, NULL);        
    }
    
    if(0 != (numofrxframes = eo_canserv_NumberOfFramesInRXqueue(eo_canserv_GetHandle(), eOcanport2)))
    {
        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport2, numofrxframes, NULL);        
    }
       
}


// marco.accame on 15 sept 15:  user-defined emsconfigurator_evt_userdef01 is triggered by a timer inside the EOtheCANdiscovery2 so that we can tick it.

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef01Event(EOMtheEMSconfigurator* p)
{
    
    eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
    
//    static uint32_t count_times = 0;
//    count_times++;

//    //  davide on 25 feb 2015:
//    //  it seems that the problem of the 2foc is due to the high frequency of the messages sent via CAN.
//    //  Now the messages are sent with a frequency of 4HZ (timer countdown = 250ms), and the problem should be fixed without
//    //  a fixed delay of 5 seconds from the beginning of the application
//    
//    // The first time I only send the request...from that point on, I continue to check if the boards are ready, and if
//    // not I re-send the request for the firmware version
//    if(count_times == 1)
//    {
//        eo_candiscovery_ResetAndStartProcedure(eo_candiscovery_GetHandle());
//        return;
//    }
//          
//    // verifico che le board mc4, 1foc gli analog sensors siano ready, ovvero che abbiano mandato la loro fw version
//    eo_candiscovery_EvaluateDiscoveredResources(eo_candiscovery_GetHandle());
//    
//    // true only if both MC and MAIS are ready
//    if ((eo_candiscovery_isMCReady(eo_candiscovery_GetHandle()) == eobool_true) && (eo_candiscovery_isMAISReady(eo_candiscovery_GetHandle()) == eobool_true))
//    {
//        count_times = 0;
//        //stoppo il timer canBoardsReady_timer da 10 milli e segnalo le schede trovate coi loro protocolli
//        eo_candiscovery_Stop(eo_candiscovery_GetHandle());
//        // poi, nel caso mc4:  mando la configurazione alle board e abilito MAIS e BCastPolicy
//        if(eobool_true == eo_mc4boards_AreThere(eo_mc4boards_GetHandle()))
//        {
//            eo_mc4boards_Config(eo_mc4boards_GetHandle());
//            eo_mais_Start(eo_mais_GetHandle());          
//        }
//    }
//    else
//    {  
//        // I check again if the can boards are ready. however, i dont check the boards already ok
//        eo_candiscovery_CheckRemainingCanBoards(eo_candiscovery_GetHandle());
//    }

}

// marco.accame on Nov 26 2014: this function is triggered if function eom_emssocket_Transmit() fails
// to transmit a udp packet.
extern void eom_emsconfigurator_hid_userdef_onemstransceivererror(EOMtheEMStransceiver* p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_configurator_udptxfailure);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSconfigurator", &errdes); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




