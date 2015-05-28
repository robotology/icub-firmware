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

#include "EOtheEMSapplBody.h"

#include "EoProtocolSK.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSKIN.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSKIN_hid.h"


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

static EOtheSKIN s_eo_theskin = 
{
    .initted            = eobool_false,
    .thereisskin        = eobool_false,
    .numofskins         = 0
};

//static const char s_eobj_ownname[] = "EOtheSKIN";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSKIN* eo_skin_Initialise(void)
{
    if(eobool_true == s_eo_theskin.initted)
    {
        return(&s_eo_theskin);
    }
    
    s_eo_theskin.numofskins = eo_entities_NumOfSkins(eo_entities_GetHandle());

    if(0 == s_eo_theskin.numofskins)
    {
        s_eo_theskin.thereisskin = eobool_false;
    }
    else
    {
        s_eo_theskin.thereisskin = eobool_true;
    }

    s_eo_theskin.initted = eobool_true;
    
    return(&s_eo_theskin);   
}


extern EOtheSKIN* eo_skin_GetHandle(void)
{
    return(eo_skin_Initialise());
}


extern eOresult_t eo_skin_DisableTX(EOtheSKIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theskin.thereisskin)
    {   // nothing to do because we dont have a skin board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    eOcanprot_command_t command = {0};
    icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
    command.class = eocanprot_msgclass_pollingSkin;
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;       
    command.value = &sigmode;

    uint8_t i=0;
    eOsk_skin_t *skin = NULL;
    for(i=0; i<s_eo_theskin.numofskins; i++)
    {
        // i stop this skin only if it was started before
        if(NULL == (skin = eo_entities_GetSkin(eo_entities_GetHandle(), i)))
        {   
            continue;   // i dont have this skin ... i go the next one
        }
            
        if(eosk_sigmode_dontsignal == skin->config.sigmode)
        {   
            // if the skin was not initted by robot-interface, then i dont deinit it. the reason is twofold:
            // 1. if the skin boards dont transmit, there is no need to silence them,
            // 2. in case the .ini file of robotinterface has disable the skin because the skin is not mounted, i dont want to tx on a disconnected can bus.
            continue;   // i dont need to silence this skin ... i go the next one
        }
            
        // i set the skin as not transmitting as soon as possible. because in such a way, can messages being received
        // are not pushed back in skin->status.arrayofcandata and its does not overflow.
        skin->config.sigmode = eosk_sigmode_dontsignal;
              
        // then i stop transmission of each skin can board
       
        // i get the addresses of the can boards of the i-th skin.
        // the simplification we use is that they all are on the same CAN bus and all have consecutive addresses.
        // we send the same command to all of them
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, i, 0);
        eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, id32);    
    }
    
    return(eores_OK);

}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the skin ... better in here rather than elsewhere.

// marco.accame: we start with a silent skin and with an empty status_arrayofcandata.
// then we put skin in sigmode eosk_sigmode_signal only if robotInterface orders it to the board.


extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv)
{
    eOsk_sigmode_t *sigmode = (eOsk_sigmode_t*)eo_nv_RAM(nv);
    
    *sigmode = eosk_sigmode_dontsignal; 
}


extern void eoprot_fun_INIT_sk_skin_status_arrayofcandata(const EOnv* nv)
{
    EOarray_of_skincandata_t *tmp = (EOarray_of_skincandata_t*)eo_nv_RAM(nv);
    tmp = tmp;
    // marco.accame: items of array are eOsk_candata_t. its capacity is:
    uint16_t capacity = sizeof(tmp->data) / sizeof(eOsk_candata_t);    
    // eo_array_New() initialises capacity and itemsize and also sets size to 0 
    EOarray *array = eo_array_New(capacity, sizeof(eOsk_candata_t), eo_nv_RAM(nv));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



