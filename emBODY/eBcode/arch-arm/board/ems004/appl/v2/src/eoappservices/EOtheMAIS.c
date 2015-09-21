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
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS_hid.h"


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

static void s_eo_mais_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMAIS s_eo_themais = 
{
    .initted            = eobool_false,
    .thereismais        = eobool_false,
    .protindex          = 0,
    .id32               = eo_prot_ID32dummy,
    .command            = {0}
};

//static const char s_eobj_ownname[] = "EOtheMAIS";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMAIS* eo_mais_Initialise(void)
{
    if(eobool_true == s_eo_themais.initted)
    {
        return(&s_eo_themais);
    }
    

    if(0 == eo_entities_NumOfMaises(eo_entities_GetHandle()))
    {
        s_eo_themais.thereismais = eobool_false;
    }
    else
    {
        s_eo_themais.thereismais = eobool_true;
        s_eo_themais.protindex = 0;
        s_eo_themais.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, s_eo_themais.protindex, eoprot_tag_none);
    }

    s_eo_themais.initted = eobool_true;
    
    return(&s_eo_themais);   
}


extern EOtheMAIS* eo_mais_GetHandle(void)
{
    return(eo_mais_Initialise());
}


extern eOresult_t eo_mais_Start(EOtheMAIS *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.thereismais)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }
    
       
    // now, i do things. 
    eOas_mais_config_t *cfg = eo_entities_GetMaisConfig(eo_entities_GetHandle(), s_eo_themais.protindex);

    if(NULL == cfg)
    {   // we dont have mais
        return(eores_OK);
    }

//    s_eo_themais.datarate = cfg->datarate;      // it must be 10
//    s_eo_themais.mode = cfg->mode;              // it must be eoas_maismode_txdatacontinuously

    s_eo_themais.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    
    // set txmode
    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_themais.command.value = &cfg->mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);
    
    // set datarate
    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_themais.command.value = &cfg->datarate;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);    
    
    return(eores_OK);       
}


extern eOresult_t eo_mais_Config(EOtheMAIS *p, eOas_mais_config_t* maiscfg)
{
    if((NULL == p) || (NULL == maiscfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.thereismais)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  

    // ok, now we do something. 
    
    eOresult_t res = eores_OK;
    
    if(eores_OK != (res = eo_mais_ConfigMode(p, (eOas_maismode_t)maiscfg->mode)))
    {
        return(res);
    }
    
    if(eores_OK != (res = eo_mais_ConfigDataRate(p, maiscfg->datarate)))
    {
        return(res);
    }   
    
    if(eores_OK != (res = eo_mais_ConfigResolution(p, (eOas_maisresolution_t)maiscfg->resolution)))
    {
        return(res);
    }
       
    return(eores_OK);     
}


extern eOresult_t eo_mais_ConfigMode(EOtheMAIS *p, eOas_maismode_t mode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.thereismais)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  

    // ok, now we do something.     
 
    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;

    s_eo_themais.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_themais.command.value = &mode;
    
    // if pc104 tell me to enable mais tx, before to send cmd verify if i'm in RUN state:
    // if yes ==> ok no problem
    // if no ==> i'll send cmd when go to RUN state
    if(eoas_maismode_txdatacontinuously == mode)
    {
        // only if the appl is in RUN state enable mais tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);
        }
    }
    else
    {
         eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);
    }
    
    return(eores_OK);
}


extern eOresult_t eo_mais_ConfigDataRate(EOtheMAIS *p, uint8_t datarate)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.thereismais)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  

    // ok, now we do something.     
 

    s_eo_themais.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_themais.command.value = &datarate;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);    
    
    return(eores_OK);  
}

extern eOresult_t eo_mais_ConfigResolution(EOtheMAIS *p, eOas_maisresolution_t resolution)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.thereismais)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  

    // ok, now we do something.     
 
    s_eo_themais.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    s_eo_themais.command.value = &resolution;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);    
    
    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), s_eo_themais.protindex);
    if(NULL == status)
    {
        return(eores_NOK_generic); //error
    }     

    s_eo_mais_process_mais_resolution(resolution, status);  
    
    return(eores_OK);          
}


extern eOresult_t eo_mais_OnDiscoveryStop(EOtheMAIS *p, EOtheCANdiscovery2 *discovery2, eObool_t searchisok)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // wip part. we should: 
    // 1. send up a ok result about the strain ...
    // 2. call eo_mais_Initialise()
    // 3. return OK
    
    
    if(NULL == p)
    {
        eo_mais_Initialise();
    }

    return(eores_OK); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to MAIS are in a unique place


extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv)
{
    eOas_mais_config_t* maiscfg = eo_nv_RAM(nv);
    
    maiscfg->datarate = 10;
    maiscfg->mode = eoas_maismode_txdatacontinuously;
    maiscfg->resolution = eoas_maisresolution_08;
}


extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv)
{
    eOas_mais_status_t *status = eo_nv_RAM(nv);  
    
    // marco.accame: i init as for eoas_maisresolution_08 
    //               the array the15values can be initted for size 0 or 15 as i now use teh proper eo_array_Assign() method
    uint8_t capacity    = 15;
    uint8_t itemsize    = 1;
    uint8_t size        = 15;
    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_mais_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status)
{
    uint8_t size = 0;
    uint8_t itemsize = 1;
    uint8_t capacity = 0;
    
    if(eoas_maisresolution_08 == resolution)
    {
        capacity    = 15;
        itemsize    = 1;
        size        = 15;
        //#warning acemor-> nella mais nel caso di risoluzione a 8 bit perche' la capacity di the15values e' 16 e non 15?
        // penso sia un errore. metto a 15. anche perche' nel parser dei pacchetti can la size viene messa a 15
        //status->the15values.head.capacity = 16;
        //status->the15values.head.itemsize = 1;
        //status->the15values.head.size = 16;
    }
    else if(eoas_maisresolution_16 == resolution)
    {
        capacity    = 15;
        itemsize    = 2;
        size        = 15;
        //#warning acemor-> nella mais ho messo la capacity di the15values a 15 anche nel caso di risoluzione a 16 bit
        //status->the15values.head.capacity = 16;
        //status->the15values.head.itemsize = 2;
        //status->the15values.head.size = 16;
    } 

    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);      
}


// -- oldies

//extern eOresult_t eo_mais_Config(EOtheMAIS *p, eOas_mais_config_t* maiscfg)
//{
//    if((NULL == p) || (NULL == maiscfg))
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    if(eobool_false == s_eo_themais.thereismais)
//    {   // nothing to do because we dont have a mais board
//        return(eores_OK);
//    }  

//    // ok, now we do something. 

//    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;

//    s_eo_themais.command.class = eocanprot_msgclass_pollingAnalogSensor;
//    
//    // if robotInterface orders to enable mais tx, before to send the command over can i must verify if i'm in the RUN state:
//    // if yes -> i send the command
//    // if no  -> i'll send the command at entry in the RUN state
//    if(eoas_maismode_txdatacontinuously == maiscfg->mode)
//    {
//        // only if the appl is in RUN state we enable mais tx
//        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
//        if(eo_sm_emsappl_STrun == currentstate)
//        {
//            s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//            s_eo_themais.command.value = &(maiscfg->mode);
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);
//        }
//    }    
//    
//    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//    s_eo_themais.command.value = &(maiscfg->datarate);
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);

//    s_eo_themais.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
//    s_eo_themais.command.value = &(maiscfg->resolution);
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.command, s_eo_themais.id32);    
//    
//    
//    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), s_eo_themais.protindex);
//    if(NULL == status)
//    {
//        return(eores_NOK_generic); //error
//    }     

//    s_eo_mais_process_mais_resolution((eOas_maisresolution_t)maiscfg->resolution, status);    


//    return(eores_OK);     
//}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



