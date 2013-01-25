/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#include <stdint.h>
#include "string.h"
#include "hal_base.h"
#include "hal_eeprom.h"
#include "hal_crc.h"
#include "eEsharedStorage.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eEprivateStorage.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    ee_privatestorage_cfg_t         config;
    uint32_t                        eeprom_address;
    uint16_t                        dataplussig_sizeof;
    uint16_t                        dataplussig_sigoff;
    void*                           dataplussig_ramfor;
    uint16_t                        tmpsigvalue;
} ee_privstrg_internals_t;



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EE_PRIVSTRG_CRCSIZE 2


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static uint16_t s_ee_privatestorage_compute_signature(void);
static void s_ee_privatestorage_set_signature(uint16_t sig);
static eEbool_t s_ee_privatestorage_isvalid(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static ee_privstrg_internals_t s_ee_privatestorage_internals =
{
    .config =
    {
        .size               = 0,
        .defval             = NULL
    },
    .eeprom_address         = 0,
    .dataplussig_sizeof     = 0,
    .dataplussig_sigoff     = 0,
    .dataplussig_ramfor     = NULL,
    .tmpsigvalue            = 0
};

                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// boardinfo and defaultcanaddr passed only by the ee_procLoader.

extern eEresult_t ee_privatestorage_init(ee_privatestorage_cfg_t* cfg, void** pdata)
{
    eEresult_t res = ee_res_NOK_generic;
    const eEsharinfoPartable_t *ptab;
    hal_result_t halres = hal_res_NOK_generic;
    uint16_t tmp16 = 0;

    // some controls upon validity of the params
    if((NULL == cfg) || (0 == cfg->size))
    {
        return(ee_res_NOK_generic);
    }
    
    // controls upon a previous call, so that we dont call it twice
    if(0 != s_ee_privatestorage_internals.config.size)
    {
        return(ee_res_NOK_generic);
    }
    
    
    if(NULL != pdata)
    {
        *pdata = NULL;
    }   
 
    // ok, params are ok. and it is first call. let's go on
    
    memcpy(&s_ee_privatestorage_internals.config, cfg, sizeof(ee_privatestorage_cfg_t));    
    ee_sharedstorage_partiontable_get(&ptab);   
    s_ee_privatestorage_internals.eeprom_address        = ptab->strg_priv_application.addr;   
    // for data i use 4-bytes alignment
    tmp16 = (s_ee_privatestorage_internals.config.size + 3)/4;
    tmp16 *= 4;
    s_ee_privatestorage_internals.dataplussig_sigoff    = tmp16; 
    s_ee_privatestorage_internals.dataplussig_sizeof    = s_ee_privatestorage_internals.dataplussig_sigoff + 4;  // add 4 bytes for the sig at the end.
    s_ee_privatestorage_internals.tmpsigvalue           = 0;   
    
    s_ee_privatestorage_internals.dataplussig_ramfor    = calloc(s_ee_privatestorage_internals.dataplussig_sizeof, 1);
    
    if(NULL == s_ee_privatestorage_internals.dataplussig_ramfor)
    {
        return(ee_res_NOK_generic);
    }
    
    // read data plus sig
    halres = hal_eeprom_read(hal_eeprom_emulatedflash, s_ee_privatestorage_internals.eeprom_address, 
                             s_ee_privatestorage_internals.dataplussig_sizeof, s_ee_privatestorage_internals.dataplussig_ramfor);
    if(hal_res_OK != halres)
    {
        return(ee_res_NOK_generic);    
    }
    
    // check if data is valid
    if(ee_false == s_ee_privatestorage_isvalid())
    { 
        // reset data
        if(NULL != s_ee_privatestorage_internals.config.defval)
        {
            memcpy(s_ee_privatestorage_internals.dataplussig_ramfor, s_ee_privatestorage_internals.config.defval, s_ee_privatestorage_internals.config.size);
        }
        else
        {
            memset(s_ee_privatestorage_internals.dataplussig_ramfor, 0, s_ee_privatestorage_internals.config.size);            
        }
        
        // now compute the signature and set it
        tmp16 = s_ee_privatestorage_compute_signature();
        s_ee_privatestorage_set_signature(tmp16);

        
        // now i write in eeprom
        halres = hal_eeprom_write(hal_eeprom_emulatedflash, s_ee_privatestorage_internals.eeprom_address, 
                                  s_ee_privatestorage_internals.dataplussig_sizeof, s_ee_privatestorage_internals.dataplussig_ramfor);
                                  
        if(hal_res_OK != res)
        {
            return(ee_res_NOK_generic);    
        }
    }
    
    if(NULL != pdata)
    {
        *pdata = s_ee_privatestorage_internals.dataplussig_ramfor;
    }
    
    return(ee_res_OK);    
}


extern eEresult_t ee_privatestorage_get(void** pdata, eEbool_t* itiscorrect)
{
    eEresult_t res = ee_res_NOK_generic;
    hal_result_t halres = hal_res_NOK_generic;
    
    // controls upon initialisation. if not initted yet, then we return error
    if(0 == s_ee_privatestorage_internals.config.size)
    {
        return(ee_res_NOK_generic);
    }
    
    halres = hal_eeprom_read(hal_eeprom_emulatedflash, s_ee_privatestorage_internals.eeprom_address, 
                             s_ee_privatestorage_internals.dataplussig_sizeof, s_ee_privatestorage_internals.dataplussig_ramfor);
                             
    if(hal_res_OK != halres)
    {
        return(ee_res_NOK_generic);    
    }
    
    if(NULL != pdata)
    {
        *pdata = s_ee_privatestorage_internals.dataplussig_ramfor;
    }  
    
    if(NULL != itiscorrect)
    {
        *itiscorrect = s_ee_privatestorage_isvalid();       
    }
    
    return(ee_res_OK);  
}


extern eEresult_t ee_privatestorage_set(const void* data)
{
    eEresult_t res = ee_res_NOK_generic;
    hal_result_t halres = hal_res_NOK_generic;
    uint16_t tmp16 = 0;
    
    // controls upon initialisation. if not initted yet, then we return error
    if(0 == s_ee_privatestorage_internals.config.size)
    {
        return(ee_res_NOK_generic);
    }
    
    if((NULL != data) && (data != s_ee_privatestorage_internals.dataplussig_ramfor))
    {
        memcpy(s_ee_privatestorage_internals.dataplussig_ramfor, data, s_ee_privatestorage_internals.config.size);
    }
    
    // now compute its signature and set it ..
    tmp16 = s_ee_privatestorage_compute_signature();
    s_ee_privatestorage_set_signature(tmp16);
    
    // now i write in eeprom
    halres = hal_eeprom_write(hal_eeprom_emulatedflash, s_ee_privatestorage_internals.eeprom_address, 
                              s_ee_privatestorage_internals.dataplussig_sizeof, s_ee_privatestorage_internals.dataplussig_ramfor);

    if(hal_res_OK != halres)
    {
        return(ee_res_NOK_generic);    
    }
        
    return(ee_res_OK);  
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_ee_privatestorage_compute_signature(void)
{
    return(0xdead);
}

static void s_ee_privatestorage_set_signature(uint16_t sig)
{
    ((uint16_t*)s_ee_privatestorage_internals.dataplussig_ramfor)[s_ee_privatestorage_internals.dataplussig_sigoff] = sig;
}


static eEbool_t s_ee_privatestorage_isvalid(void)
{
    uint16_t readsignature = 0;
    uint16_t compsignature = 0;
    
    readsignature = ((uint16_t*)s_ee_privatestorage_internals.dataplussig_ramfor)[s_ee_privatestorage_internals.dataplussig_sigoff];
    
    compsignature = s_ee_privatestorage_compute_signature();
    
    return((compsignature == readsignature) ? (ee_true) : (ee_false));
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



