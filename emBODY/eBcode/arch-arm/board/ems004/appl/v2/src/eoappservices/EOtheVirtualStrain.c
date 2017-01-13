/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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
#include "EOtheErrorManager.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

//#include "EOtheEMSapplBody.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheVirtualStrain.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheVirtualStrain_hid.h"


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

#define COUNT_WATCHDOG_VIRTUALSTRAIN_MAX        10
#define COUNT_BETWEEN_TWO_UPDATES_MAx           200 /* equal to timeout in mc4 before mc4 considers useless strain torques
                                                       see macro "STRAIN_SAFE" in iCub\firmware\motorControllerDsp56f807\common_source_code\include\strain_board.h*/




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheVirtualStrain s_eo_thevirtualstrain = 
{
    EO_INIT(.initted)                   eobool_false,
    EO_INIT(.location)                  {0},
    EO_INIT(.torques)                   {0},
    EO_INIT(.thereisanewvalueoftorque)  eobool_false,
    EO_INIT(.countbetweentwoupdates)    0,
    EO_INIT(.countwatchdog)             0,
    EO_INIT(.command)                   {0}
};


//static const char s_eobj_ownname[] = "EOtheVirtualStrain";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheVirtualStrain* eo_virtualstrain_Initialise(void)
{
    if(eobool_true == s_eo_thevirtualstrain.initted)
    {
        return(&s_eo_thevirtualstrain);
    }
            
    s_eo_thevirtualstrain.location.port = eOcanport1;   // or one can read the location of the first joint ... as in next two lines
    //eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 0, eoprot_tag_none);
    //eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &s_eo_thevirtualstrain.location, NULL, NULL);    
    s_eo_thevirtualstrain.location.addr = eo_virtualstrain_canaddress;
    s_eo_thevirtualstrain.location.insideindex = eobrd_caninsideindex_none;
    
    
    memset(&s_eo_thevirtualstrain.torques, 0, sizeof(s_eo_thevirtualstrain.torques));
    s_eo_thevirtualstrain.thereisanewvalueoftorque = eobool_false;
    
    s_eo_thevirtualstrain.countbetweentwoupdates = 0;
    s_eo_thevirtualstrain.countwatchdog =0;
    
    s_eo_thevirtualstrain.command.clas = eocanprot_msgclass_periodicAnalogSensor;    
    s_eo_thevirtualstrain.command.type  = 0;
    s_eo_thevirtualstrain.command.value = NULL;
    
    s_eo_thevirtualstrain.initted = eobool_true;
    
    return(&s_eo_thevirtualstrain);   
}


extern EOtheVirtualStrain* eo_virtualstrain_GetHandle(void)
{
    return(eo_virtualstrain_Initialise());
}


extern eOresult_t eo_virtualstrain_SetTorque(EOtheVirtualStrain *p, eOprotIndex_t jindex, icubCanProto_torque_t value)
{
    // marco.accame on 28 may 2015: this remap is a little bit of a mistery. there is no comment on previous code about how we get these numbers.
    //                              moreover, it seems that we fill only three positions out of six in the torques of virtual strain. why is it so?
    static const uint8_t remap[3] = {4, 0, 1};  // if jindex is 0 (wrist pronosupination), then we use position 4
                                                // if jindex is 1 (wrist yaw), then we use position 0
                                                // if jindex is 2 (wrist pitch), then we use position 1
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

//    if(eobool_false == s_eo_thevirtualstrain.itismc4can)
//    {   // nothing to do because we dont have a mc4can board
//        return(eores_OK);
//    }
     
    if(jindex >= 3)
    {
        return(eores_OK); // marco.accame: i found that previous function returns ok even if nothing is updated. 
    }
    
    s_eo_thevirtualstrain.torques[remap[jindex]] = value;
    s_eo_thevirtualstrain.thereisanewvalueoftorque = eobool_true;
    
    return(eores_OK);
}

extern eOresult_t eo_virtualstrain_Tick(EOtheVirtualStrain *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

//    if(eobool_false == s_eo_thevirtualstrain.itismc4can)
//    {   // nothing to do because we dont have a mc4can board
//        return(eores_OK);
//    }
    
    
    // we impose not to tx the torques, unless ... see further
    eObool_t transmitnow = eobool_false;

    if(eobool_true == s_eo_thevirtualstrain.thereisanewvalueoftorque)
    {   // marco.accame: ok, i understand. i send if the torques are ok.
        transmitnow = eobool_true;        
        s_eo_thevirtualstrain.countbetweentwoupdates = 0;
        s_eo_thevirtualstrain.countwatchdog = 0;
    
        s_eo_thevirtualstrain.thereisanewvalueoftorque = eobool_false;
    }
    else
    {   // marco.accame: in here i dont quite understand. however i keep as it was.
        
        // marco.accame: the code in here forces a send of the virtual strain
        //               at least after COUNT_WATCHDOG_VIRTUALSTRAIN_MAX (10) ticks, thus after 10 ms.  
        //               so... why two counters? 
        s_eo_thevirtualstrain.countbetweentwoupdates ++;
        s_eo_thevirtualstrain.countwatchdog ++;
        
        if(s_eo_thevirtualstrain.countbetweentwoupdates <= COUNT_BETWEEN_TWO_UPDATES_MAx)
        {
            if(s_eo_thevirtualstrain.countwatchdog == COUNT_WATCHDOG_VIRTUALSTRAIN_MAX)
            {
                transmitnow = eobool_true;
                s_eo_thevirtualstrain.countwatchdog = 0;               
            }
            //else
            //{
            //    transmitnow = eobool_false;
            //}        
        }
        //else
        //{
        //    transmitnow = eobool_false;
        //}
    
    }
    
      
    if(eobool_true == transmitnow)
    {    
        s_eo_thevirtualstrain.command.type  = ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR;
        s_eo_thevirtualstrain.command.value = &s_eo_thevirtualstrain.torques[0];           
        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_thevirtualstrain.command, s_eo_thevirtualstrain.location);

        s_eo_thevirtualstrain.command.type  = ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR;
        s_eo_thevirtualstrain.command.value = &s_eo_thevirtualstrain.torques[3];
        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_thevirtualstrain.command, s_eo_thevirtualstrain.location);           
    }
    
    
    return(eores_OK); 
}


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



