
/* @file       privateData.c
    @brief      This header file implements ....
    @author     valentina.gaggero@iit.it
    @date       27/01/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include "string.h"
#include "hal.h"
//#include "sharinfo_applic.h"
#include "eEsharedStorage.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "privateData.h"



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
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_ap_privatedata_isValid(ap_private_datastruct_t *privdata);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static ap_private_datastruct_t privateData_default =
{
    .dummy1 = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
    .dummy2 = {0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F},
    .dummy3 = {0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F},
    .dummy4 = {0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F}
};

                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t ap_privateData_init(void)
{
    const eEsharinfoPartable_t *ptab;
    hal_result_t res;
    ap_private_datastruct_t privdata;

    //sharinfo_applic_partiontable_get(&ptab);
	ee_sharedstorage_partiontable_get(&ptab);

    res = hal_eeprom_read(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                              sizeof(ap_private_datastruct_t), (void*)&privdata);
    if(hal_res_OK != res)
    {
        return(res);    
    }

    //check if data is valid
    if(!s_ap_privatedata_isValid(&privdata))
    { 
        res = hal_eeprom_write(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                              sizeof(ap_private_datastruct_t), (void*)&privateData_default);
        if(hal_res_OK != res)
        {
            return(res);    
        }
    }
    return(hal_res_OK);
}


extern hal_result_t ap_privateData_get(ap_private_datastruct_t *privdata)
{
    const eEsharinfoPartable_t *ptab;

    if(NULL == privdata)
    {
        return(hal_res_NOK_generic);
    }
    //sharinfo_applic_partiontable_get(&ptab);
	ee_sharedstorage_partiontable_get(&ptab);

    return( hal_eeprom_read(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                            sizeof(ap_private_datastruct_t), (void*)privdata));

}

extern hal_result_t ap_privateData_set(ap_private_datastruct_t *privdata)
{
    const eEsharinfoPartable_t *ptab;

    if(NULL == privdata)
    {
        return(hal_res_NOK_generic);
    }
    //sharinfo_applic_partiontable_get(&ptab);
	ee_sharedstorage_partiontable_get(&ptab);

    return( hal_eeprom_write(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                             sizeof(ap_private_datastruct_t), (void*)privdata) );

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_ap_privatedata_isValid(ap_private_datastruct_t *privdata)
{
    /* this is a stupid check...if you want it is possible use  ee_common_check_isvalid...*/
   if((0xFF == privdata->dummy1[0]) && (0xFF == privdata->dummy1[1]))
    {
        return(0);
    }
    else
    {
        return(1);
    }

}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



