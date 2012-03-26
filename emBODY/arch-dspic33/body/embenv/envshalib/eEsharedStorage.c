


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include "string.h"
#include "hal_base.h"
#include "hal_eeprom.h"
#include "hal_crc.h"

//#include "board_func.h"
//#include "sharinfo_loader.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eEsharedStorage.h"


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
static uint8_t s_ee_sharedstorage_memory_is_overlapped_to_eproc(eEprocess_t eproc, uint32_t addr, uint32_t size);
static eEbool_t s_ee_sharedstorage_memory_is_overlapped_to_sharinfo(uint32_t addr, uint32_t size);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint32_t                 s_canaddress_default        = 1;
static eEprocess_t              s_eproc                     = ee_procNone;

// initted from storage
const eEsharinfoPartable_t      *s_ptable                   = NULL;
const eEsharinfoDeviceInfo_t    *s_devinfo                  = NULL;

// initted from the user
const eEboardInfo_t             *s_boardinfo                = NULL;
const eEmoduleInfo_t            *s_loaderinfo               = NULL;
const eEmoduleInfo_t            *s_applicinfo               = NULL;

                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// boardinfo and defaultcanaddr passed only by the ee_procLoader.
extern eEresult_t ee_sharedstorage_init(eEprocess_t caller, const eEmoduleInfo_t *eprocinfo, const eEboardInfo_t *boardinfo, const uint32_t defaultcanaddr)
{
    eEresult_t res = ee_res_NOK_generic;

    // some controls upun validity of the params
    if(NULL == eprocinfo)
    {
        return(res);
    }
    
   
    
    if((ee_procApplication != caller) && (ee_procLoader != caller))
    {
        return(res);
    }    
    
  
    if((ee_procLoader == caller) && ((NULL == boardinfo) || (NULL == eprocinfo)))
    {
        return(res);
    }
    

    // ok, params are ok. now i init some variables
  
    s_eproc = caller;
    
    if(ee_procLoader == s_eproc)
    {
        s_boardinfo             = boardinfo;
        s_loaderinfo            = eprocinfo;
        s_canaddress_default    = defaultcanaddr;    
    }
    else //if(ee_procApplication == s_eproc)
    {   // application
        s_applicinfo            = eprocinfo;    
    }

    
    // init the sharinfo library
    if(ee_res_OK != ee_sharinfo_init(NULL))
    {
        return(res);
    }    

    // the loader syncs the partition table
    if(ee_procLoader == s_eproc)
    {
        if(ee_res_OK != ee_sharinfo_partable_sync())
        {
            return(res);
        }
    }   
    
    // both get the partition table
    if(ee_res_OK != ee_sharinfo_partable_get(&s_ptable))
    {
        return(res);
    } 

    // the loader syncs the boardinfo
    if(ee_procLoader == s_eproc)
    {
        if(ee_res_OK != ee_sharinfo_boardinfo_sync(boardinfo))
        {
            return(res);
        }
    }

    // both eprocesses get the boardinfo
    if(ee_res_OK != ee_sharinfo_boardinfo_get(&s_boardinfo))
    {
          return(res);  
    }
     

    // the calling e-process syncs its eprocinfo
    if(ee_res_OK != ee_sharinfo_procinfo_sync(eprocinfo, s_eproc))
    {
        return(res);
    }
    
    // and retrieve the proper loader info and application info. in case of call from loader, the application info may be null.
    
    if(ee_res_OK != ee_sharinfo_procinfo_get(&s_loaderinfo, ee_procLoader))
    {    
        return(res);
    }
    
    if(ee_res_OK != ee_sharinfo_procinfo_get(&s_applicinfo, ee_procApplication))
    {    
        if(ee_procApplication == s_eproc)
        {
            return(res);
        }
        else
        {
            // dont return error because it is the case of a loader call when no application has yet synch-ed its info 
        }
    } 

    
    // now the loader works on the device info. in case there is not a valid device info, it imposes the default one
    
    if(ee_procLoader == s_eproc)
    {
        if(ee_res_OK != ee_sharinfo_deviceinfo_get(&s_devinfo))
        {
            // at first clear it
            if(ee_res_OK != ee_sharinfo_deviceinfo_clr())
            {
                return(res);
            }

            // then write the default can1
            if(ee_res_OK != ee_sharinfo_deviceinfo_item_set(&s_canaddress_default, devinfo_can1))
            {
                return(res);
            }    
        }
        else if((0x0 == s_devinfo->can1network.idcan) || (s_devinfo->can1network.idcan >= 0xf))
        {
            // be sure the address is between 0x1 and 0xe
            if(ee_res_OK != ee_sharinfo_deviceinfo_item_set(&s_canaddress_default, devinfo_can1))
            {
                return(res);
            }   
        }        
    }
    
    // both eprocesses get the device info 
    if(ee_res_OK != ee_sharinfo_deviceinfo_get(&s_devinfo))
    {
          return(res);  
    }    
     
    
    
    // ipc must not be initted ...


    return(ee_res_OK); 

}

extern eEresult_t ee_sharedstorage_partiontable_get(const eEsharinfoPartable_t **ptab)
{
    if((NULL == ptab) || (NULL == s_ptable))
    {
        return(ee_res_NOK_generic);
    }
    
    *ptab = s_ptable;
    return(ee_res_OK);
}

extern eEresult_t ee_sharedstorage_boardinfo_get(const eEboardInfo_t **boardinfo)
{
    if((NULL == boardinfo) || (NULL == s_boardinfo))
    {
        return(ee_res_NOK_generic);
    }
    
    *boardinfo = s_boardinfo;
    return(ee_res_OK);
}

extern eEresult_t ee_sharedstorage_deviceinfo_get(const eEsharinfoDeviceInfo_t **devinfo)
{
    if((NULL == devinfo) || (NULL == s_devinfo))
    {
        return(ee_res_NOK_generic);
    }
    
    *devinfo = s_devinfo;
    return(ee_res_OK);
}

extern eEresult_t ee_sharedstorage_deviceinfo_canaddress_get(uint32_t *canaddr)
{
    if((NULL == canaddr) || (NULL == s_devinfo))
    {
        return(ee_res_NOK_generic);
    }

    *canaddr = s_devinfo->can1network.idcan;
    return(ee_res_OK);
}

extern eEresult_t ee_sharedstorage_deviceinfo_canaddress_set(uint32_t canaddr)
{
    if(NULL == s_devinfo)
    {
        return(ee_res_NOK_generic);
    }

    return(ee_sharinfo_deviceinfo_item_set(&canaddr, devinfo_can1));
}


extern eEresult_t ee_sharedstorage_ipc_debug_sendcommand_eraseflash(eEprocess_t from, eEprocess_t to, uint32_t addr, uint32_t size)
{

    eEsharinfoIPCdata_t ipcdata;

    ipcdata.head.size   = 4;
    ipcdata.head.from   = from;
    ipcdata.head.to     = to;
    ipcdata.head.opc    = ipcopc_erase_flash;


    switch(ipcdata.head.opc)
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
            //cmd->addr = 0x08000000UL + 80UL*1024UL;
            //cmd->size = 8UL*1024UL;
            cmd->addr = addr;
            cmd->size = size;
        } break; 
    }

    return(ee_sharinfo_ipcdata_set(&ipcdata));   

}


// only the loader
extern eEresult_t ee_sharedstorage_ipc_loader_process(void)
{
    eEresult_t res = ee_res_NOK_generic;

    const eEsharinfoIPCdata_t* ipcdata = NULL;
    
    if(NULL == s_ptable)
    {
        return(res);
    }
       
    
    // now i get the ipc
    if(ee_res_NOK_generic == ee_sharinfo_ipcdata_get(&ipcdata))
    {
        ee_sharinfo_ipcdata_clr();
        res = ee_res_OK;
    }
    else if((NULL != ipcdata) && (0 != ipcdata->head.size))
    {
    
        if(ee_procLoader == ipcdata->head.to)
        {   // there is an ipc for the ee_procLoader
            
            switch(ipcdata->head.opc)
            {
                case ipcopc_erase_applic:
                {   
                    // erase the application
                    //#warning ipcopc_erase_applic: the appl is erase only staring from s_ptable->proc_application.addr + 0x400 
                    /* the erase operation operate with page, so we have to start from 0x400.
                        (not from 0x0 otherwise we erase the start addr of bootloader).
                        if we want erase from 0x0, we need to write the first 4pcunit with bootloader addr.
                        this is quite complicate...todo when have time.
                        Now if we reset the appl from 0x400 the bootloader try to jump to 0x200, 
                        probably executes some instruction, but after reset.
                        this is not safe, but works!
                    */
                    if(hal_res_OK == hal_flash_erase((s_ptable->proc_application.addr + 0x400), (s_ptable->proc_application.size-0x400)))
                    {
                        res = ee_res_OK;
                    }

                    // and its procinfo
                    ee_sharinfo_procinfo_clr(ee_procApplication);
                } break;

                case ipcopc_erase_sharinfo:
                {   
                    // erase the whole sharinfo ...
                    res = ee_sharinfo_erase();
                    
                    // a full re-init is required ...
                    res = ee_sharedstorage_init(ee_procLoader, s_loaderinfo, s_boardinfo, s_canaddress_default);
                } break;

                case ipcopc_erase_flash:
                {   
                    // erase a portion of flash ...
                    eEsharinfoIPCerase_flash_t *flash2erase = (eEsharinfoIPCerase_flash_t*)ipcdata;

                    // but only if it is not overlapped to the ee_procLoader. 
                    if(0 == s_ee_sharedstorage_memory_is_overlapped_to_eproc(ee_procLoader, flash2erase->addr, flash2erase->size))
                    {
                        // in case [addr, addr+size] overlaps the sharinfo
                        uint32_t addr = flash2erase->addr;
                        uint32_t size = flash2erase->size;                 
                    
                        if(hal_res_OK == hal_flash_erase(addr, size))
                        {
                            res = ee_res_OK;
                        }
                        
                        // if it was part of the application ...
                        if(1 == s_ee_sharedstorage_memory_is_overlapped_to_eproc(ee_procApplication, addr, size))
                        {
                            res = ee_sharinfo_procinfo_clr(ee_procApplication);
                        }
                       
                        // if it was part of the sharinfo ....
                        if(1 == s_ee_sharedstorage_memory_is_overlapped_to_sharinfo(addr, size))
                        {
                            res = ee_sharedstorage_init(ee_procLoader, s_loaderinfo, s_boardinfo, s_canaddress_default);
                        }                
                    }
                    
                    
                } break;


                default:
                {
                    res = ee_res_NOK_generic;
                } break;

            }

            ee_sharinfo_ipcdata_clr(); 
        }
        else
        {
            ee_sharinfo_ipcdata_clr();  
        }
    }


    return(res);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_ee_sharedstorage_memory_is_overlapped_to_eproc(eEprocess_t eproc, uint32_t addr, uint32_t size)
{
    const eEsysmemory_t* sysmem = NULL;
    
    switch(eproc)
    {
        case ee_procLoader:
        {
            sysmem = &s_ptable->proc_loader;
        } break;

        case ee_procApplication:
        {
            sysmem = &s_ptable->proc_application;
        } break;  
        
        default:
        { 
            sysmem = NULL;
        } break;
    }
    
    if(NULL == sysmem)
    {
        return(0);
    }
    
    if(sysmem->addr >= (addr+size))
    {   // the eproc is higher in memory space
        return(0);
    }
    else if((sysmem->addr+sysmem->size) <= (addr))
    {   // teh eproc is lower in memory space
        return(0);
    }
    else
    {   // there is an overlap 
        return(1);
    }

}

static eEbool_t s_ee_sharedstorage_memory_is_overlapped_to_sharinfo(uint32_t addr, uint32_t size)
{
    eEstorage_t stg;
    stg.type = ee_strg_emuleeprom;
    stg.addr = addr;
    stg.size = size;
    
    return(ee_sharinfo_overlapped_is(&stg));
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



