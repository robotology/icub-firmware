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

#include "EoProtocolAS.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSTRAIN.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSTRAIN_hid.h"


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

static EOtheSTRAIN s_eo_thestrain = 
{
    .initted            = eobool_false,
    .thereisstrain      = eobool_false,
    .datarate           = 0,
    .mode               = 0
};

//static const char s_eobj_ownname[] = "EOtheSTRAIN";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSTRAIN* eo_strain_Initialise(void)
{
    if(eobool_true == s_eo_thestrain.initted)
    {
        return(&s_eo_thestrain);
    }
    

    if(0 == eo_entities_NumOfStrains(eo_entities_GetHandle()))
    {
        s_eo_thestrain.thereisstrain = eobool_false;
    }
    else
    {
        s_eo_thestrain.thereisstrain = eobool_true;
    }

    s_eo_thestrain.initted = eobool_true;
    
    return(&s_eo_thestrain);   
}


extern EOtheSTRAIN* eo_strain_GetHandle(void)
{
    return(eo_strain_Initialise());
}


extern eOresult_t eo_strain_SendTXmode(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    const uint8_t number = 0; 
    eOas_strain_config_t *cfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), number);

    if(NULL == cfg)
    {   // we dont have strain
        return(eores_OK);
    }

    s_eo_thestrain.mode = cfg->mode;       
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, number, 0);
    // set txmode
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingAnalogSensor;    
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = &s_eo_thestrain.mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    return(eores_OK);
}

extern eOresult_t eo_strain_DisableTX(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    const uint8_t number = 0; 
    eOas_strain_config_t *cfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), number);

    if(NULL == cfg)
    {   // we dont have strain
        return(eores_OK);
    }

    eOenum08_t mode = eoas_strainmode_acquirebutdonttx;       
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, number, 0);
    // set txmode
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingAnalogSensor;    
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    return(eores_OK);      

}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the strain ... better in here rather than elsewhere.


extern void eoprot_fun_INIT_as_strain_config(const EOnv* nv)
{
    eOas_strain_config_t* straincfg = eo_nv_RAM(nv);
    
    straincfg->datarate = 10;
    straincfg->mode = eoas_strainmode_acquirebutdonttx;
    straincfg->signaloncefullscale = eobool_false;
}


extern void eoprot_fun_INIT_as_strain_status(const EOnv* nv)
{
    eOas_strain_status_t *status = eo_nv_RAM(nv);
    uint8_t capacity = 0;
    uint8_t itemsize = 0;
    uint8_t size = 0;
    EOarray* array = NULL;
    
    // marco.accame: cannot understand why in here size is zero and in others it was not zero. see why. 
    //               probably fullscale, being of itemsize = 2 is treated without the proper eo_array_ methods
    capacity    = 6;
    itemsize    = 2;
    size        = 0; 
    array = eo_array_New(capacity, itemsize, &status->fullscale);
    eo_array_Resize(array, size);


    // marco.accame: size can be 0 or 6. now i use teh proper eo_array_Assign method for calibratedvalues/uncalibratedvalues 
    //
    capacity    = 6;
    itemsize    = 2;
    size        = 6;
    array = eo_array_New(capacity, itemsize, &status->calibratedvalues);
    eo_array_Resize(array, size);   

    capacity    = 6;
    itemsize    = 2;
    size        = 6;
    array = eo_array_New(capacity, itemsize, &status->uncalibratedvalues);
    eo_array_Resize(array, size); 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



