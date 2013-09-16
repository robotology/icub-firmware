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

/* @file       shalINFO.c
    @brief      This header file implements the shalINFO library.
    @author     marco.accame@iit.it
    @date       03/11/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



#include "string.h"

#include "shalBASE.h"
#include "shalPART.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "shalINFO.h"


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



typedef struct              // 80B
{
    shalbaseDataHeader_t    head;                   // 16B
    eEboardInfo_t           data;                   // 64B
} infoBoardInfoStorage_t;   EECOMMON_VERIFYsizeof(infoBoardInfoStorage_t, 80);



typedef struct              // 272B
{
    shalbaseDataHeader_t    head;                   // 16B
    shalinfo_deviceinfo_t   data;                   // 256B
} infoDeviceInfoStorage_t;  EECOMMON_VERIFYsizeof(infoDeviceInfoStorage_t, 272);




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// always in static mode
#define SHALINFO_MODE_STATICLIBRARY


#define SHALINFO_ROMADDR            (EENV_MEMMAP_SHALINFO_ROMADDR)
#define SHALINFO_ROMSIZE            (EENV_MEMMAP_SHALINFO_ROMSIZE)

#define SHALINFO_RAMADDR            (EENV_MEMMAP_SHALINFO_RAMADDR)
#define SHALINFO_RAMSIZE            (EENV_MEMMAP_SHALINFO_RAMSIZE)

#define SHALINFO_STGTYPE            (ee_strg_eeprom)
#define SHALINFO_STGADDR            (EENV_MEMMAP_SHALINFO_STGADDR)
#define SHALINFO_STGSIZE            (EENV_MEMMAP_SHALINFO_STGSIZE)

// the ram size to be used in scatter-file and the one used by the program for static ram
#define SHALINFO_RAMFOR_RWDATA      (EENV_MEMMAP_SHALINFO_RAMFOR_RWDATA) 

// the ram size to be used with __attribute__((at(SHALINFO_RAMADDR)))
#define SHALINFO_RAMFOR_ZIDATA      (EENV_MEMMAP_SHALINFO_RAMFOR_ZIDATA)


typedef int dummy1[(sizeof(infoBoardInfoStorage_t)+sizeof(infoDeviceInfoStorage_t))     <= (SHALINFO_RAMSIZE) ? 1 : -1];
typedef int dummy2[SHALINFO_RAMFOR_ZIDATA <= ((SHALINFO_RAMSIZE-SHALINFO_RAMFOR_RWDATA)) ? 1 : -1];





// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_shalinfo_permanent_boardinfo_init(void);
static infoBoardInfoStorage_t* s_shalinfo_permanent_boardinfo_get(void);
static void s_shalinfo_permanent_boardinfo_set(infoBoardInfoStorage_t *infoboardinfo);
static void s_shalinfo_permanent_boardinfo_cache_invalidate(void);
static void s_shalinfo_permanent_boardinfo_reset(infoBoardInfoStorage_t* infoboardinfo);


static void s_shalinfo_permanent_deviceinfo_init(void);
static infoDeviceInfoStorage_t* s_shalinfo_permanent_deviceinfo_get(void);
static void s_shalinfo_permanent_deviceinfo_set(infoDeviceInfoStorage_t *infodeviceinfo);
static void s_shalinfo_permanent_deviceinfo_cache_invalidate(void);

static eEresult_t s_shalinfo_deviceinfo_ipnetwork_clr(void);

static eEresult_t s_shalinfo_deviceinfo_can1network_clr(void);
static eEresult_t s_shalinfo_deviceinfo_can2network_clr(void);

static void s_shalinfo_ipnetwork_default_set(eEipnetwork_t* ntw);
static void s_shalinfo_can1network_default_set(eEcannetwork_t* ntw);
static void s_shalinfo_can2network_default_set(eEcannetwork_t* ntw);
static void s_shalinfo_pages_default_set(void *startofpages);


static eEboolval_t s_shalinfo_storage_is_valid(void* storage);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


const eEstorage_t s_shalinfo_strg_boardinfo =
{
    .type   = SHALINFO_STGTYPE,
    .size   = sizeof(infoBoardInfoStorage_t),
    .addr   = SHALINFO_STGADDR    
};

const eEstorage_t s_shalinfo_strg_deviceinfo =
{
    .type   = SHALINFO_STGTYPE,
    .size   = sizeof(infoDeviceInfoStorage_t),
    .addr   = SHALINFO_STGADDR + sizeof(infoBoardInfoStorage_t)    
};


// - default values ---------------------------------------------------------------------------------------------------


static const eEboardInfo_t s_shalinfo_boardinfo_default =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = 0x11,
            .version    = 
            { 
                .major = 0, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2000,
                .month = 1,
                .day   = 1,
                .hour  = 0,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_none,
        .name           = "dummy"
    },
    .uniqueid       = 0,
    .extra          = {0}
};



// - volatile values --------------------------------------------------------------------------------------------------

// used for board-info services 
static volatile infoBoardInfoStorage_t s_shalinfo_boardinfoStored  __attribute__((at(SHALINFO_RAMADDR)));

// used for device-info services
static volatile infoDeviceInfoStorage_t s_shalinfo_deviceinfoStored  __attribute__((at(SHALINFO_RAMADDR+sizeof(infoBoardInfoStorage_t))));


// - module info ------------------------------------------------------------------------------------------------------

#if     defined(SHALINFO_MODE_STATICLIBRARY)
    #define SHALINFO_MODULEINFO_PLACED_AT
    #define SHALINFO_ENTITY_TYPE                ee_entity_statlib
#else
    #define SHALINFO_MODULEINFO_PLACED_AT       __attribute__((at(SHALINFO_ROMADDR+EENV_MODULEINFO_OFFSET)))
    #define SHALINFO_ENTITY_TYPE                ee_entity_sharlib /* or ee_entity_statlib ?*/
#endif

static const eEmoduleInfo_t s_shalinfo_moduleinfo   SHALINFO_MODULEINFO_PLACED_AT =
{
    .info           =
    {
        .entity     =
        {
            .type       = SHALINFO_ENTITY_TYPE,
            .signature  = ee_shalSharServ | SHALINFO_SIGN,
            .version    = 
            { 
                .major = SHALINFO_VER_MAJOR, 
                .minor = SHALINFO_VER_MINOR
            },  
            .builddate  = 
            {
                .year  = SHALINFO_BUILDDATE_YEAR,
                .month = SHALINFO_BUILDDATE_MONTH,
                .day   = SHALINFO_BUILDDATE_DAY,
                .hour  = SHALINFO_BUILDDATE_HOUR,
                .min   = SHALINFO_BUILDDATE_MIN
            }
        },
        .rom        = 
        {   
            .addr   = SHALINFO_ROMADDR,
            .size   = SHALINFO_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = SHALINFO_RAMADDR,
            .size   = SHALINFO_RAMSIZE
        },
        .storage    = 
        {
            .type   = SHALINFO_STGTYPE,
            .size   = SHALINFO_STGSIZE,
            .addr   = SHALINFO_STGADDR
        },
        .communication  = ee_commtype_none,
        .name           = SHALINFO_NAME
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 0},
        .can1protversion = { .major = 0, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}                
};
                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if     defined(SHALINFO_MODE_STATICLIBRARY)

extern const eEmoduleInfo_t * shalinfo_moduleinfo_get(void)
{
    return((const eEmoduleInfo_t*)&s_shalinfo_moduleinfo);
}

extern const eEentity_t * shalinfo_moduleinfo_entity_get(void)
{
    return((const eEentity_t*)&s_shalinfo_moduleinfo.info.entity);
}

extern eEresult_t shalinfo_isvalid(void)
{
    return(ee_res_OK);
}

#else
    // using inline functions
#endif


extern eEresult_t shalinfo_init(void)
{
    // in the beginning ... the base

    if(ee_res_NOK_generic == shalbase_isvalid())
    {
        return(ee_res_NOK_generic);
    }

    shalbase_init(shalbase_initmode_dontforce);
    
    
    // then the rest ... 


    // initialise the management of boardinfo
    
    s_shalinfo_permanent_boardinfo_init();
    s_shalinfo_permanent_boardinfo_cache_invalidate();
    // we assure that boardinfo is correct
    infoBoardInfoStorage_t* boardinfo = s_shalinfo_permanent_boardinfo_get();
    if(ee_false == s_shalinfo_storage_is_valid(boardinfo))
    {
        // first time we run ... or we have read an old boardinfo. i reset it. then someone shall impose the correct value
        s_shalinfo_permanent_boardinfo_reset(boardinfo);
    }
    

    // initialise the management of deviceinfo
    
    s_shalinfo_permanent_deviceinfo_init();
    s_shalinfo_permanent_deviceinfo_cache_invalidate();
    // we assure that deviceinfo is correct
    infoDeviceInfoStorage_t* deviceinfo = s_shalinfo_permanent_deviceinfo_get();
    if(ee_false == s_shalinfo_storage_is_valid(deviceinfo))
    {
        // first time we run ... or we have read an old device. i reset it. then someone shall impose the correct value
        shalinfo_deviceinfo_clr();
    }    


    return(ee_res_OK);

}


extern eEresult_t shalinfo_deinit(void)
{
    // so far we dont need to undo anything. whenever we support eeprom (on i2c) we need to ....
    return(ee_res_OK);
}

extern eEresult_t shalinfo_erase(void)
{
    volatile eEresult_t res;
    res = shalbase_storage_clr(&s_shalinfo_strg_boardinfo, sizeof(infoBoardInfoStorage_t));
    res = shalbase_storage_clr(&s_shalinfo_strg_deviceinfo, sizeof(infoDeviceInfoStorage_t));
    res =  res;
    return(ee_res_OK);
}



extern eEresult_t shalinfo_boardinfo_synchronise(const eEboardInfo_t* boardinfo)
{
    infoBoardInfoStorage_t* infoboardinfo = s_shalinfo_permanent_boardinfo_get();
    uint8_t change = 0;
 
// we dont need to verify that is we called shalinfo_init()    
//     if(ee_false == s_shalinfo_storage_is_valid(infoboardinfo))
//     {   // we change if the storage content is invalid 
//         change = 1;
//     }

    if(0 != memcmp(&infoboardinfo->data, boardinfo, sizeof(eEboardInfo_t)))
    {   // we change if the storage content is not what is expected 
        change = 1;
    }
    
    if(1 == change)
    {
        // copy data
        memcpy(&infoboardinfo->data, boardinfo, sizeof(eEboardInfo_t));        
        // set it valid
        const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
        shalbase_hid_storage_set_valid((void*)infoboardinfo, ROMentity);
        // send it to storage
        s_shalinfo_permanent_boardinfo_set(infoboardinfo); 

        // it sets the mac address, the ip, the netmask according to the boardinfo.identifier
        // and also initialises the other device info parts
        shalinfo_deviceinfo_clr();
    }


    // retrieve the eEboardInfo_t stored in storage. compare it w/ boardinfo. if different, then write boardinfo in storage
    return(ee_res_OK);
}


extern eEresult_t shalinfo_boardinfo_get(const eEboardInfo_t** boardinfo)
{
    eEresult_t res = ee_res_OK;
    infoBoardInfoStorage_t* infoboardinfo = s_shalinfo_permanent_boardinfo_get();

    *boardinfo = &infoboardinfo->data;

    if(ee_true == s_shalinfo_storage_is_valid(infoboardinfo))
    {
        res = ee_res_OK;
    }
    else
    {
        res = ee_res_NOK_generic;
    }
    return(res);
}



extern eEresult_t shalinfo_deviceinfo_clr(void)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();

    // change data    
    s_shalinfo_ipnetwork_default_set(&infodeviceinfo->data.ipnetwork);
    s_shalinfo_can1network_default_set(&infodeviceinfo->data.can1network);
    s_shalinfo_can2network_default_set(&infodeviceinfo->data.can2network);
    s_shalinfo_pages_default_set(&infodeviceinfo->data.page08[0]);

    // set it valid
    const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
    shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
    // send it to storage
    s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 

    return(res);
}

extern eEresult_t shalinfo_deviceinfo_get(const shalinfo_deviceinfo_t** deviceinfo)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();

    *deviceinfo = &infodeviceinfo->data;

    if(ee_true == s_shalinfo_storage_is_valid(infodeviceinfo))
    {
        res = ee_res_OK;
    }
    else
    {
        res = ee_res_NOK_generic;
    }
    return(res);
}


extern eEresult_t shalinfo_deviceinfo_set(const shalinfo_deviceinfo_t* deviceinfo)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
    uint8_t change = 0;

// we dont need to verify that is we called shalinfo_init()        
//     if(ee_false == s_shalinfo_storage_is_valid(infodeviceinfo))
//     {   // we change if the storage content is invalid 
//         change = 1;
//     }
        
    if(0 != memcmp(&(infodeviceinfo->data), deviceinfo, sizeof(shalinfo_deviceinfo_t)))
    {   // we change if the content is different from what we want
        change = 1;
    }
    
    if(1 == change)
    {        
        // copy data
        memcpy((void*)&(infodeviceinfo->data), deviceinfo, sizeof(shalinfo_deviceinfo_t));
        // set it valid
        const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
        shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
        // send it to storage
        s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 
    }

    return(res);
}


extern eEresult_t shalinfo_deviceinfo_part_get(shalinfo_deviceinfo_part_t part, const void** data)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();

    switch(part)
    {
        case shalinfo_ipnet:    
        {
            *data = &infodeviceinfo->data.ipnetwork;
        } break;
        case shalinfo_can1net:    
        {
            *data = &infodeviceinfo->data.can1network;
        } break;
        case shalinfo_can2net:    
        {
            *data = &infodeviceinfo->data.can2network;
        } break;
        case shalinfo_page08:    
        {
            *data = &infodeviceinfo->data.page08[0];
        } break;
        case shalinfo_page32:    
        {
            *data = &infodeviceinfo->data.page32[0];
        } break;
        case shalinfo_page64:    
        {
            *data = &infodeviceinfo->data.page64[0];
        } break;
        case shalinfo_page128:    
        {
            *data = &infodeviceinfo->data.page128[0];
        } break;
        default:
        {
            *data = NULL;
            res =  ee_res_NOK_generic;
        } break;
    };

    if(NULL == *data)
    {
        return(res);
    }

    if(ee_true == s_shalinfo_storage_is_valid(infodeviceinfo))
    {
        res = ee_res_OK;
    }
    else
    {
        res = ee_res_NOK_generic;
    }
    return(res);
}


extern eEresult_t shalinfo_deviceinfo_part_set(shalinfo_deviceinfo_part_t part, const void* data)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
    void *datapart =  NULL;
    uint16_t sizepart = 0;
    
// we dont need to verify that is we called shalinfo_init()        
//     if(ee_false == s_shalinfo_storage_is_valid(infodeviceinfo))
//     {   // if the storage content is invalid then we must reset the whole
//         shalinfo_deviceinfo_clr();
//         // and retrieve the infodeviceinfo again
//         infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
//     }    

    switch(part)
    {
        case shalinfo_ipnet:    
        { 
            datapart = &infodeviceinfo->data.ipnetwork;
            sizepart = sizeof(eEipnetwork_t);
        } break;
        case shalinfo_can1net:    
        {
            datapart = &infodeviceinfo->data.can1network;
            sizepart = sizeof(eEcannetwork_t);
        } break;
        case shalinfo_can2net:    
        {
            datapart = &infodeviceinfo->data.can2network;
            sizepart = sizeof(eEcannetwork_t);
        } break;
        case shalinfo_page08:    
        {
            datapart = &infodeviceinfo->data.page08[0];
            sizepart = sizeof(infodeviceinfo->data.page08);
        } break;
        case shalinfo_page32:    
        {
            datapart = &infodeviceinfo->data.page32[0];
            sizepart = sizeof(infodeviceinfo->data.page32);
        } break;
        case shalinfo_page64:    
        {
            datapart = &infodeviceinfo->data.page64[0];
            sizepart = sizeof(infodeviceinfo->data.page64);
        } break;
        case shalinfo_page128:    
        {
            datapart = &infodeviceinfo->data.page128[0];
            sizepart = sizeof(infodeviceinfo->data.page128);
        } break;
        default:
        {
            datapart = NULL;
            sizepart = 0;
            res = ee_res_NOK_generic;
        } break;
    };

    if(NULL == datapart)
    {
        return(res);
    }
    

    if(0 != memcmp(datapart, data, sizepart))
    {  
        // copy data
        memcpy(datapart, data, sizepart);
        // set it valid
        const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
        shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
        // send it to storage        
        s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 
    }

    return(res);
}

extern eEresult_t shalinfo_deviceinfo_part_clr(shalinfo_deviceinfo_part_t part)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
    void *datapart =  NULL;
    uint16_t sizepart = 0;

// we dont need to verify that is we called shalinfo_init()        
//     if(ee_false == s_shalinfo_storage_is_valid(infodeviceinfo))
//     {   // if the storage content is invalid then we must reset the whole
//         shalinfo_deviceinfo_clr();
//         // and retrieve the infodeviceinfo again
//         infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
//     }     
    
    switch(part)
    {
        case shalinfo_ipnet:    
        { 
            res = s_shalinfo_deviceinfo_ipnetwork_clr();
            datapart = NULL;
            sizepart = 0;
        } break;
        case shalinfo_can1net:    
        {
            res = s_shalinfo_deviceinfo_can1network_clr();
            datapart = NULL;
            sizepart = 0;
        } break;
        case shalinfo_can2net:    
        {
            res = s_shalinfo_deviceinfo_can2network_clr();
            datapart = NULL;
            sizepart = 0;
        } break;
        case shalinfo_page08:    
        {
            datapart = &infodeviceinfo->data.page08[0];
            sizepart = sizeof(infodeviceinfo->data.page08);
        } break;
        case shalinfo_page32:    
        {
            datapart = &infodeviceinfo->data.page32[0];
            sizepart = sizeof(infodeviceinfo->data.page32);
        } break;
        case shalinfo_page64:    
        {
            datapart = &infodeviceinfo->data.page64[0];
            sizepart = sizeof(infodeviceinfo->data.page64);
        } break;
        case shalinfo_page128:    
        {
            datapart = &infodeviceinfo->data.page128[0];
            sizepart = sizeof(infodeviceinfo->data.page128);
        } break;
        default:
        {
            datapart = NULL;
            sizepart = 0;
            res = ee_res_NOK_generic;
        } break;
    };

    if(NULL == datapart)
    {
        return(res);
    }


    // set data
    memset(datapart, 0xFF, sizepart);
    // set it valid
    const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
    shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
    // send it to storage    
    s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 

    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// -- board info routines

static void s_shalinfo_permanent_boardinfo_init(void)
{
    memset((void*)&s_shalinfo_boardinfoStored, 0, sizeof(s_shalinfo_boardinfoStored));
}

static infoBoardInfoStorage_t* s_shalinfo_permanent_boardinfo_get(void)
{
    if(0 == s_shalinfo_boardinfoStored.head.datactrl.cached)
    {
        shalbase_storage_get(&s_shalinfo_strg_boardinfo, (void*)&s_shalinfo_boardinfoStored, sizeof(infoBoardInfoStorage_t));
    }

    s_shalinfo_boardinfoStored.head.datactrl.cached = 1;

    return((infoBoardInfoStorage_t*)&s_shalinfo_boardinfoStored);
}

static void s_shalinfo_permanent_boardinfo_set(infoBoardInfoStorage_t *infoboardinfo)
{
    infoboardinfo->head.datactrl.cached = 0;
    shalbase_storage_set(&s_shalinfo_strg_boardinfo, infoboardinfo, sizeof(infoBoardInfoStorage_t));
}

static void s_shalinfo_permanent_boardinfo_cache_invalidate(void)
{
    s_shalinfo_boardinfoStored.head.datactrl.cached = 0;
}

static void s_shalinfo_permanent_boardinfo_reset(infoBoardInfoStorage_t* infoboardinfo)
{  
    // impose data        
    memcpy(&infoboardinfo->data, &s_shalinfo_boardinfo_default, sizeof(eEboardInfo_t));
    // set it valid
    const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
    shalbase_hid_storage_set_valid((void*)infoboardinfo, ROMentity);        
    // send it to storage        
    s_shalinfo_permanent_boardinfo_set(infoboardinfo);
}


// -- device info routines

static void s_shalinfo_permanent_deviceinfo_init(void)
{
    memset((void*)&s_shalinfo_deviceinfoStored, 0, sizeof(s_shalinfo_deviceinfoStored));
}

static infoDeviceInfoStorage_t* s_shalinfo_permanent_deviceinfo_get(void)
{
    if(0 == s_shalinfo_deviceinfoStored.head.datactrl.cached)
    {
        shalbase_storage_get(&s_shalinfo_strg_deviceinfo, (void*)&s_shalinfo_deviceinfoStored, sizeof(infoDeviceInfoStorage_t));
    }

    s_shalinfo_deviceinfoStored.head.datactrl.cached = 1;

    return((infoDeviceInfoStorage_t*)&s_shalinfo_deviceinfoStored);
}

static void s_shalinfo_permanent_deviceinfo_set(infoDeviceInfoStorage_t *infodeviceinfo)
{
    infodeviceinfo->head.datactrl.cached = 0;
    shalbase_storage_set(&s_shalinfo_strg_deviceinfo, infodeviceinfo, sizeof(infoDeviceInfoStorage_t));
}

static void s_shalinfo_permanent_deviceinfo_cache_invalidate(void)
{
    s_shalinfo_deviceinfoStored.head.datactrl.cached = 0;
}



// - various

static eEresult_t s_shalinfo_deviceinfo_ipnetwork_clr(void)
{
    eEresult_t res = ee_res_OK;
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
    
    eEipnetwork_t ntw;
    s_shalinfo_ipnetwork_default_set(&ntw);


    if(0 != memcmp(&(infodeviceinfo->data.ipnetwork), &ntw, sizeof(eEipnetwork_t)))
    {
        // copy data
        memcpy((void*)&(infodeviceinfo->data.ipnetwork), &ntw, sizeof(eEipnetwork_t));
        // set it valid
        const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
        shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
        // send it to storage        
        s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 
    }

    return(res);
}


static eEresult_t s_shalinfo_deviceinfo_can1network_clr(void)
{
    eEresult_t res = ee_res_OK;
    infoBoardInfoStorage_t* infoboardinfo = s_shalinfo_permanent_boardinfo_get();
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
    
    eEcannetwork_t ntw;
    ntw.idcan  = 1;


    if(0 != memcmp(&(infodeviceinfo->data.can1network), &ntw, sizeof(eEcannetwork_t)))
    {
        // copy data
        memcpy((void*)&(infodeviceinfo->data.can1network), &ntw, sizeof(eEcannetwork_t));
        // set it valid
        const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
        shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
        // send it to storage        
        s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 
    }

    return(res);
}

static eEresult_t s_shalinfo_deviceinfo_can2network_clr(void)
{
    eEresult_t res = ee_res_OK;
    infoBoardInfoStorage_t* infoboardinfo = s_shalinfo_permanent_boardinfo_get();
    infoDeviceInfoStorage_t* infodeviceinfo = s_shalinfo_permanent_deviceinfo_get();
    
    eEcannetwork_t ntw;
    ntw.idcan  = 2;


    if(0 != memcmp(&(infodeviceinfo->data.can2network), &ntw, sizeof(eEcannetwork_t)))
    {
        // copy data
        memcpy((void*)&(infodeviceinfo->data.can2network), &ntw, sizeof(eEcannetwork_t));
        // set it valid
        const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
        shalbase_hid_storage_set_valid((void*)infodeviceinfo, ROMentity);        
        // send it to storage        
        s_shalinfo_permanent_deviceinfo_set(infodeviceinfo); 
    }

    return(res);
}


static void s_shalinfo_ipnetwork_default_set(eEipnetwork_t* ntw)
{
    infoBoardInfoStorage_t* infoboardinfo = s_shalinfo_permanent_boardinfo_get();

    ee_common_ipnetwork_clr(ntw, infoboardinfo->data.uniqueid);  
}

static void s_shalinfo_can1network_default_set(eEcannetwork_t* ntw)
{
    ntw->idcan  = 0;
}

static void s_shalinfo_can2network_default_set(eEcannetwork_t* ntw)
{
    ntw->idcan  = 0;
}

static void s_shalinfo_pages_default_set(void *startofpages)
{
     memset(startofpages, 0xFF, 8+32+64+128);
}
   

static eEboolval_t s_shalinfo_storage_is_valid(void* storage)
{   
    const eEentity_t* ROMentity = &s_shalinfo_moduleinfo.info.entity;
    
    if(ee_true == shalbase_hid_storage_is_valid(storage, ROMentity))
    {   // ok. check vs major number
        shalbaseDataHeader_t* DATAhead = (shalbaseDataHeader_t*)storage;
        if(DATAhead->entity.version.major == ROMentity->version.major)
        {   // only the same major number is ok
            return(ee_true);
        }
        
    }
    
    // ... unlucky !
    return(ee_false);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



