
/* @file       sharinfo_applic.c
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include "string.h"
#include "hal_base.h"
#include "hal_eeprom.h"
#include "hal_crc.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "sharinfo_applic.h"


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
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// initted from storage
const eEsharinfoPartable_t    *s_ptable_ptr                = NULL;
const eEboardInfo_t           *s_boardinfo_ptr             = NULL;
const eEsharinfoDeviceInfo_t  *s_devinfo_ptr               = NULL;

// initted from the user
const eEmoduleInfo_t          *s_applicinfo_ptr             = NULL;

                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

        
extern eEresult_t sharinfo_applic_init(const eEmoduleInfo_t *applicinfo)
{
    eEresult_t res = ee_res_NOK_generic;

    // init the sharinfo library with default config. (NULL param)
    if(ee_res_OK != ee_sharinfo_init(NULL))
    {
        return(res);
    }
    
    if((NULL == applicinfo))
    {
        return(res);
    }

    s_applicinfo_ptr = applicinfo;

    // get the partition table
    if(ee_res_OK != ee_sharinfo_partable_get(&s_ptable_ptr))
    {
        return(res);
    } 

    // get the boardinfo
    if(ee_res_OK != ee_sharinfo_boardinfo_get(&s_boardinfo_ptr))
    {
        return(res);
    }
    
    if(ee_res_OK != ee_sharinfo_deviceinfo_get(&s_devinfo_ptr))
    {
          return(res);  
    }

    // sync the moduleinfo of the applic
    if(ee_res_OK != ee_sharinfo_procinfo_sync(applicinfo, ee_procApplication))
    {
        return(res);
    }

   
    // ipc must not be initted ..., but only used by get func

    return(ee_res_OK); 

}

// used to test sharinfo_loader_ipc_process() because the loader does not send commands to the application
extern eEresult_t sharinfo_applic_ipc_sendcommand(eEprocess_t from, eEprocess_t to, eEsharinfoIPCopc_t opc)
{

    eEsharinfoIPCdata_t ipcdata;

    ipcdata.head.size   = 4;
    ipcdata.head.from   = from;
    ipcdata.head.to     = to;
    ipcdata.head.opc    = opc;


    switch(opc)
    {
        case ipcopc_erase_applic:
        {
            
        } break; 

        case ipcopc_erase_sharinfo:
        {
        
        } break; 

        case ipcopc_erase_flash:
        {
            eEsharinfoIPCerase_flash_t *cmd = (eEsharinfoIPCerase_flash_t*)&ipcdata;
            cmd->head.size = 12;
            cmd->addr = 0x08000000UL + 80UL*1024UL;
            cmd->size = 8UL*1024UL;
        
        } break; 
    }

    return(ee_sharinfo_ipcdata_set(&ipcdata));   

}



extern eEresult_t sharinfo_applic_canaddress_get(uint32_t *canaddr)
{
    if((NULL == canaddr) || (NULL == s_devinfo_ptr))
    {
        return(ee_res_NOK_generic);
    }
    
    *canaddr = s_devinfo_ptr->can1network.idcan;
    return(ee_res_OK);
}

extern eEresult_t sharinfo_applic_canaddress_set(uint32_t canaddr)
{
    if(NULL == s_devinfo_ptr)
    {
        return(ee_res_NOK_generic);
    }

    return(ee_sharinfo_deviceinfo_item_set(&canaddr, devinfo_can1));
}

extern eEresult_t sharinfo_applic_deviceinfo_get(const eEsharinfoDeviceInfo_t **devinfo)
{
    if((NULL == devinfo) || (NULL == s_devinfo_ptr))
    {
        return(ee_res_NOK_generic);
    }
    
    *devinfo = s_devinfo_ptr;
    return(ee_res_OK);
}

extern eEresult_t sharinfo_applic_partiontable_get(const eEsharinfoPartable_t **ptab)
{
    if((NULL == ptab) || (NULL == s_ptable_ptr))
    {
        return(ee_res_NOK_generic);
    }
    
    *ptab = s_ptable_ptr;
    return(ee_res_OK);
}

extern eEresult_t sharinfo_applic_boardinfo_get(const eEboardInfo_t **bardinfo)
{
    if((NULL == bardinfo) || (NULL == s_boardinfo_ptr))
    {
        return(ee_res_NOK_generic);
    }
    
    *bardinfo = s_boardinfo_ptr;
    return(ee_res_OK);
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



