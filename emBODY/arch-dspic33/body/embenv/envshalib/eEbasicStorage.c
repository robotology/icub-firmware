


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "string.h"
#include "p33fxxxx.h"
#include <libpic30.h>

#include "eEmemorymap_dspic33.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eEbasicStorage.h"


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

// same as eEsharinfoDeviceInfo_t + check_t
typedef struct              // size is 4+60+4+4 = 72         
{
    uint32_t                canadr;
    uint8_t                 filler60deviceinfo[60];
    uint8_t                 filler04check[4];
    uint32_t                crc;
} s_deviceinfo_in_flash_t;

typedef struct
{
    uint8_t                 type;           // must be 2 (process)
    uint8_t                 id;             // must be 0 (loader)
    uint8_t                 major;
    uint8_t                 minor;
    uint32_t                year    : 12;
    uint32_t                month   : 4;
    uint32_t                day     : 5;
    uint32_t                hour    : 5;
    uint32_t                min     : 6;
} s_beginningofeprocinfo_in_flash_t;

typedef enum
{
    status_notinitted   = 0,
    status_initial      = 1,
    status_cached       = 2,
    status_invalid      = 3
} status_t;



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define ADDR_OF_DEVICEINFO  EE_MEMMAP_STRG_SHRW_DEVINFO_ADDR

#define ADDR_OF_LOADERINFO  EE_MEMMAP_STRG_SHRO_LOADERINFO_ADDR




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static s_deviceinfo_in_flash_t s_devinfo = {1, {0}, {0}, 0};
static s_beginningofeprocinfo_in_flash_t s_beginningofloaderinfo = {0};
static uint8_t s_status = status_notinitted;    //

                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern eEbasicstrg_res_t ee_basicstrg_init(void)
{
   
    _memcpy_p2d16((void*)&s_beginningofloaderinfo, ADDR_OF_LOADERINFO, (uint16_t)sizeof(s_beginningofeprocinfo_in_flash_t));
    
    if((2 != s_beginningofloaderinfo.type) || (0 != s_beginningofloaderinfo.id))
    {
        s_status = status_invalid;
        return(ee_basicstrg_res_ok_generic);
    }
    
    s_status = status_initial;
    s_devinfo.canadr = 1;
    return(ee_basicstrg_res_ok);
}

extern eEbasicstrg_res_t ee_basicstrg_canaddress_get(uint32_t *canaddr)
{

    if(status_notinitted == s_status)
    {
        eEbasicstrg_res_t res = ee_basicstrg_init();
        if(ee_basicstrg_res_ok_generic == res)
        {
            return(ee_basicstrg_res_ok_generic);
        }
    }

    if(status_invalid == s_status)
    {
        return(ee_basicstrg_res_ok_generic);
    }    

    if(NULL == canaddr)
    {
        return(ee_basicstrg_res_ok_generic);
    }
    
    if(status_cached == s_status)
    {
        *canaddr = s_devinfo.canadr;
        return(ee_basicstrg_res_ok);
    }
    
    // else .... read from flash
    
    
    _memcpy_p2d16((void*)&s_devinfo, ADDR_OF_DEVICEINFO, (uint16_t)sizeof(s_deviceinfo_in_flash_t)); 
    
    // now verify the crc ... use a very primitive check ...
    if(0xffffffff == s_devinfo.crc)
    {
        memset(&s_devinfo, 0, sizeof(s_deviceinfo_in_flash_t));
        s_devinfo.canadr = 1;
        return(ee_basicstrg_res_ok_generic);
    }
    
    // else ...
    
    s_status = status_cached;
    
    *canaddr = s_devinfo.canadr;
    return(ee_basicstrg_res_ok);

}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



