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

/* @file       shalBASE.c
    @brief      This header file implements the shalBASE library.
    @author     marco.accame@iit.it
    @date       05/07/2010
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#ifdef DONTOPTIMISEBASE 
#pragma O0
#endif

#include "hal.h"
#include "string.h"
#include "eEmemorymap.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "shalBASE.h"


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



typedef struct                                      // 24B
{
    uint8_t                 gotoflag;
    eEprocess_t             gotoval;
    uint8_t                 free2useflag;
    uint8_t                 free2usesize;
    uint8_t                 free2usedata[shalbase_ipc_userdefdata_maxsize];
} baseIPCdata_t;            EECOMMON_VERIFYsizeof(baseIPCdata_t, 24);

typedef struct                                      // 40B
{
    shalbaseDataHeader_t    head;                   // 16B
    baseIPCdata_t           ipcdata;                // 24B
} baseIPCdataStorage_t;     EECOMMON_VERIFYsizeof(baseIPCdataStorage_t, 40);


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// always in static mode
#define SHALBASE_MODE_STATICLIBRARY



#if     defined(SHALBASE_MODE_STATICLIBRARY)

#define SHALBASE_ROMADDR            (0)
#define SHALBASE_ROMSIZE            (0)

#define SHALBASE_RAMADDR            (EENV_MEMMAP_SHARSERV_BASE_RAMADDR)
#define SHALBASE_RAMSIZE            (EENV_MEMMAP_SHARSERV_BASE_RAMSIZE)

#define SHALBASE_STGTYPE            (ee_strg_none)
#define SHALBASE_STGADDR            (0)
#define SHALBASE_STGSIZE            (0)

#else   // shared lib 

    #error --> must use only SHALBASE_MODE_STATICLIBRARY

#endif   // shared lib 

// - flags ------------------------------------------------------------------------------------------------------------
#define FLAG_OK                         0x01
#define FLAG_KO                         0x00




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_shalbase_storage_init(const eEstorageType_t strgtype);
static void s_shalbase_jump_to(uint32_t appaddr);

static eEboolval_t s_shalbase_is_initted(void);

static eEboolval_t s_shalbase_ipc_ram_is_valid(void);
static void s_shalbase_ipc_ram_set_valid(void);
static void s_shalbase_ipc_ram_set_invalid(void);
static void s_shalbase_ipc_ram_initialise(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

typedef uint8_t verify_size_t[(SHALBASE_RAMSIZE > sizeof(baseIPCdataStorage_t)) ? (1) : (-1)];

// this variable is placed in NZI section. it is used for ipc
static volatile baseIPCdataStorage_t s_shalbase_IPCdataStored      __attribute__((at(SHALBASE_RAMADDR)));


// - module info ------------------------------------------------------------------------------------------------------


static const eEmoduleInfo_t s_shalbase_moduleinfo =
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_statlib,
            .signature  = ee_shalSharServ | SHALBASE_SIGN,
            .version    = 
            { 
                .major = SHALBASE_VER_MAJOR, 
                .minor = SHALBASE_VER_MINOR
            },  
            .builddate  = 
            {
                .year  = SHALBASE_BUILDDATE_YEAR,
                .month = SHALBASE_BUILDDATE_MONTH,
                .day   = SHALBASE_BUILDDATE_DAY,
                .hour  = SHALBASE_BUILDDATE_HOUR,
                .min   = SHALBASE_BUILDDATE_MIN
            }
        },
        .rom        = 
        {   
            .addr   = SHALBASE_ROMADDR,
            .size   = SHALBASE_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = SHALBASE_RAMADDR,
            .size   = SHALBASE_RAMSIZE
        },
        .storage    = 
        {
            .type   = SHALBASE_STGTYPE,
            .size   = SHALBASE_STGSIZE,
            .addr   = SHALBASE_STGADDR
        },
        .communication  = ee_commtype_none,
        .name           = SHALBASE_NAME
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


extern const eEmoduleInfo_t * shalbase_moduleinfo_get(void)
{
    return((const eEmoduleInfo_t*)&s_shalbase_moduleinfo);
}

extern const eEentity_t * shalbase_moduleinfo_entity_get(void)
{
    return((const eEentity_t*)&s_shalbase_moduleinfo.info.entity);
}

extern eEresult_t shalbase_isvalid(void)
{
    return(ee_res_OK);
}


extern eEresult_t shalbase_init(shalbase_initmode_t initmode)
{
    if(ee_true == s_shalbase_is_initted())
    {   // the volatile ram is ok, thus some e-process has already initted the shalBASE
        if(shalbase_initmode_dontforce == initmode)
        {   // if we have force storage init then we dont care that the shalBASE was already initted and we do it again
            return(ee_res_OK);
        }
    }
    
    // initialise the eeprom storage
    s_shalbase_storage_init(ee_strg_eeprom);
       
    // initialise the ipc ram
    s_shalbase_ipc_ram_initialise();       
    
    // ok, return
    return(ee_res_OK);
}

extern eEresult_t shalbase_deinit(void)
{
    // deinit the storage

    // finally, ... invalidate the ram
    s_shalbase_ipc_ram_set_invalid();
    
    return(ee_res_OK);
}



extern eEresult_t shalbase_ipc_gotoproc_get(eEprocess_t *pr)
{
    if(ee_true != s_shalbase_ipc_ram_is_valid())
    {
        return(ee_res_NOK_generic); 
    }
    
    if(FLAG_OK != s_shalbase_IPCdataStored.ipcdata.gotoflag)
    {
        return(ee_res_NOK_generic);
    }
    else
    {
        *pr = (s_shalbase_IPCdataStored.ipcdata.gotoval);
        return(ee_res_OK);
    }

}

extern eEresult_t shalbase_ipc_gotoproc_set(eEprocess_t pr)
{
    s_shalbase_ipc_ram_set_valid();
    s_shalbase_IPCdataStored.ipcdata.gotoflag  = FLAG_OK;
    s_shalbase_IPCdataStored.ipcdata.gotoval   = pr;
    
    return(ee_res_OK);
}


extern eEresult_t shalbase_ipc_gotoproc_clr(void)
{
    s_shalbase_ipc_ram_set_valid();
    s_shalbase_IPCdataStored.ipcdata.gotoflag  = FLAG_KO;
    s_shalbase_IPCdataStored.ipcdata.gotoval   = ee_procNone;
    
    return(ee_res_OK);
}


extern eEresult_t shalbase_ipc_userdefdata_get(uint8_t *data, uint8_t *size, const uint8_t maxsize)
{

    if(ee_true != s_shalbase_ipc_ram_is_valid())
    {
        return(ee_res_NOK_generic); 
    }
    
    if(FLAG_OK != s_shalbase_IPCdataStored.ipcdata.free2useflag)
    {
        return(ee_res_NOK_generic);
    }
    else
    {
        *size = (maxsize > s_shalbase_IPCdataStored.ipcdata.free2usesize) ? (s_shalbase_IPCdataStored.ipcdata.free2usesize) : (maxsize);
        memcpy(data, (uint8_t *)s_shalbase_IPCdataStored.ipcdata.free2usedata, *size);
        return(ee_res_OK);
    }
}	

	
extern eEresult_t shalbase_ipc_userdefdata_set(uint8_t *data, uint8_t size)
{
    s_shalbase_ipc_ram_set_valid();
    s_shalbase_IPCdataStored.ipcdata.free2useflag         = FLAG_OK;
    s_shalbase_IPCdataStored.ipcdata.free2usesize         = (size < shalbase_ipc_userdefdata_maxsize) ? (size) : (shalbase_ipc_userdefdata_maxsize);
    memcpy((void*)s_shalbase_IPCdataStored.ipcdata.free2usedata, data, s_shalbase_IPCdataStored.ipcdata.free2usesize); 
    
    return(ee_res_OK);
}


extern eEresult_t shalbase_ipc_userdefdata_clr(void)
{
    s_shalbase_ipc_ram_set_valid();
    s_shalbase_IPCdataStored.ipcdata.free2useflag  = FLAG_KO;
    s_shalbase_IPCdataStored.ipcdata.free2usesize  = 0;
    memset((void*)s_shalbase_IPCdataStored.ipcdata.free2usedata, 0, shalbase_ipc_userdefdata_maxsize);
    
    return(ee_res_OK);
}


extern eEresult_t shalbase_system_canjump(uint32_t addr)
{
    return((hal_res_OK == hal_sys_canexecuteataddress(addr)) ? (ee_res_OK) : (ee_res_NOK_generic));
}

extern eEresult_t shalbase_system_canjump_to_proc(uint32_t addr, eEmoduleInfo_t *procinfo)
{
    // test if the proc described by procinfo is at address addr
    volatile eEmoduleInfo_t *mi = (volatile eEmoduleInfo_t*)(addr+EENV_MODULEINFO_OFFSET);
//    volatile eEentity_t *en = (eEentity_t*)mi;
    
    if(ee_res_NOK_generic == shalbase_system_canjump(addr))
    {
        return(ee_res_NOK_generic);
    }
    
    if(NULL == procinfo)
    {   // verify only if the module info placed in rom is coherent for an eprocess 
        if((ee_entity_process != mi->info.entity.type) || (addr != mi->info.rom.addr))
        {
            return(ee_res_NOK_generic);
        }
    }
    else if(0 != memcmp((void*)procinfo, (void*)mi, sizeof(eEmoduleInfo_t)))
    {   // we have a procinfo, thus we verify the full integrity
        return(ee_res_NOK_generic); 
    }  

    return(ee_res_OK);    
}

extern eEresult_t shalbase_system_jumpnow(uint32_t addr)
{

    if(ee_res_NOK_generic == shalbase_system_canjump(addr))
    {
        return(ee_res_NOK_generic);
    }
    
    // jump
    s_shalbase_jump_to(addr);
    
    // it never returns
    return(ee_res_NOK_generic);    
}

extern eEresult_t shalbase_system_restart(void)
{
    hal_sys_irq_disable();
    hal_sys_systemreset();

    // it never returns
    return(ee_res_NOK_generic);  
}


extern eEresult_t shalbase_storage_get(const eEstorage_t *strg, void *data, uint32_t size)
{
    volatile hal_result_t res = hal_res_NOK_generic;

    if(ee_true != s_shalbase_is_initted())
    {
        return(ee_res_NOK_generic); 
    }

    if(NULL == strg)
    {
        return(ee_res_NOK_generic);
    }
    
    if((size > strg->size) || (ee_strg_none == strg->type))
    {
        return(ee_res_NOK_generic);
    }
    
    
    if(ee_strg_eflash == strg->type)
    {
        return(ee_res_NOK_generic);
    }
    else if(ee_strg_eeprom == strg->type)
    {
        res = hal_eeprom_read(hal_eeprom_i2c_01, strg->addr, size, data);
        res =  res;
    }

    return(ee_res_OK);  
}


extern eEresult_t shalbase_storage_set(const eEstorage_t *strg, const void *data, uint32_t size)
{
    volatile hal_result_t res = hal_res_NOK_generic;

    if(ee_true != s_shalbase_is_initted())
    {
        return(ee_res_NOK_generic); 
    }
    
    
    if(NULL == strg)
    {
        return(ee_res_NOK_generic);
    }
    
    if((size > strg->size) || (ee_strg_none == strg->type))
    {
        return(ee_res_NOK_generic);
    }

    if(ee_strg_eflash == strg->type)
    {
        res = hal_res_NOK_generic;
    }
    else if(ee_strg_eeprom == strg->type)
    {
        res = hal_eeprom_write(hal_eeprom_i2c_01, strg->addr, size, (void*)data);
        res =  res;
    } 
    
    return(ee_res_OK);       
}

extern eEresult_t shalbase_storage_clr(const eEstorage_t *strg, const uint32_t size)
{
    volatile hal_result_t res = hal_res_NOK_generic;

    if(ee_true != s_shalbase_is_initted())
    {
        return(ee_res_NOK_generic); 
    }

    if(NULL == strg)
    {
        return(ee_res_NOK_generic);
    }
    
    if((size > strg->size) || (ee_strg_none == strg->type))
    {
        return(ee_res_NOK_generic);
    }

    if(ee_strg_eflash == strg->type)
    {
        res = hal_res_NOK_generic;
    }
    else if(ee_strg_eeprom == strg->type)
    {
        //#warning add hal_eeprom_clear
        res = hal_eeprom_erase(hal_eeprom_i2c_01, strg->addr, size);
        res =  res;
    }    

    return(ee_res_OK); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern eEboolval_t shalbase_hid_storage_is_valid(void* storage, const eEentity_t* refentity)
{   
    // to verify that the data is consistent we use the shalbaseDataHeader_t placed
    // at beginning of data.
    // so far we just compare the DATAhead->entity vs the reference entity (and only: type and signature)
    // later on we migth use the crc32 to do the same thing.
    
    // if consistent then we check version.major code.
    
    
    shalbaseDataHeader_t* DATAhead = (shalbaseDataHeader_t*)storage;
    eEentity_t* ramentity = (eEentity_t*)&DATAhead->entity;
    if(0 != memcmp(ramentity, refentity, 2)) 
    {
        return(ee_false);
    }
    else
    {   // ok, now we verify version, and ... builddate
//         if(DATAhead->entity.version.major != refentity->version.major)
//         {
//             return(ee_false);
//         }
        // any minor and any buildate are ok.
        return(ee_true);
    }
}

extern void shalbase_hid_storage_set_valid(void* storage, const eEentity_t* refentity)
{
    // we set the volatile ram as valid by copying some known data in its inside. 
    // this known data is the entire s_shalbase_moduleinfo.info.entity
    shalbaseDataHeader_t* DATAhead = (shalbaseDataHeader_t*)storage;   
    eEentity_t* ramentity = (eEentity_t*)&DATAhead->entity;
    memset(&DATAhead->datactrl, 0, sizeof(shalbaseDataCtrl_t));
    memcpy(ramentity, refentity, sizeof(eEentity_t));    
}

extern void shalbase_hid_storage_set_invalid(void* storage, const eEentity_t* refentity)
{
    // we set the volatile ram as invalid by assigning 0 to its data header.
    shalbaseDataHeader_t* DATAhead = (shalbaseDataHeader_t*)storage;  
    memset(&DATAhead->datactrl, 0, sizeof(shalbaseDataCtrl_t));
    memset(DATAhead, 0, sizeof(shalbaseDataHeader_t)); 
}


extern void shalbase_entrypoint(void)
{
    shalbase_init(shalbase_initmode_dontforce);
    shalbase_deinit();

    shalbase_ipc_gotoproc_get(NULL);			
    shalbase_ipc_gotoproc_set((eEprocess_t)0);
    shalbase_ipc_gotoproc_clr();

    shalbase_ipc_userdefdata_get(NULL, NULL, 0);		
    shalbase_ipc_userdefdata_set(NULL, 0);
    shalbase_ipc_userdefdata_clr();

    shalbase_system_canjump(0);
    shalbase_system_canjump_to_proc(0, NULL);
    shalbase_system_jumpnow(0);
    shalbase_system_restart();


//    shalbase_storage_init((eEstorageType_t)0);
//    shalbase_storage_deinit((eEstorageType_t)0);

    shalbase_storage_get(NULL, NULL, 0);
    shalbase_storage_set(NULL, NULL, 0);
    shalbase_storage_clr(NULL, 0);

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_shalbase_jump_to(uint32_t appaddr)
{
    hal_sys_executenowataddress(appaddr);
}


static void s_shalbase_storage_init(const eEstorageType_t strgtype)
{
    volatile hal_result_t res = hal_res_NOK_generic;
    
    
    
//  acemor or 5 july 2013: removed the irq disable and enable between eeprom init because:
//  1. it is not necessary 
//  2. sometimes it makes the program hang when jtag is used. it may be due to the presence of rtos running    
// removed:   hal_sys_irq_disable();
    
    if(ee_strg_eflash == strgtype)
    {
        res = hal_res_NOK_generic;
    }
    else if(ee_strg_eeprom == strgtype)
    {
        res = hal_eeprom_init(hal_eeprom_i2c_01, NULL);
        res =  res;
    }
    
// removed:   hal_sys_irq_enable();
}

static eEboolval_t s_shalbase_is_initted(void)
{
    return(s_shalbase_ipc_ram_is_valid());
}

static eEboolval_t s_shalbase_ipc_ram_is_valid(void)
{   
#if 0    
    // we verify that the eEentity_t fields in head and in moduleinfo are consistent. 
    // however ... in versions before 1.1 we compared the whole entity. now we compares only the first two bytes: type, signature
    // and about version.major
    eEentity_t* ramentity = (eEentity_t*)&s_shalbase_IPCdataStored.head.entity;
    const eEentity_t* ROMentity = &s_shalbase_moduleinfo.info.entity;
    if(0 != memcmp(ramentity, ROMentity, 2)) 
    {
        return(ee_false);
    }
    else
    {   // ok, now we verify version, and ... builddate
        if(ramentity->version.major != ROMentity->version.major)
        {
            return(ee_false);
        }
        // any minor and any buildate are ok.
        return(ee_true);
    }
#else
    
    const eEentity_t* ROMentity = &s_shalbase_moduleinfo.info.entity;
    
    if(ee_true == shalbase_hid_storage_is_valid((void*)&s_shalbase_IPCdataStored, ROMentity))
    {   // ok. check vs major number
        eEentity_t* ramentity = (eEentity_t*)&s_shalbase_IPCdataStored.head.entity;
        if(ramentity->version.major == ROMentity->version.major)
        {   // only the same major number is ok
            return(ee_true);
        }
        
    }
    
    // ... unlucky !
    return(ee_false);
    
#endif
}


static void s_shalbase_ipc_ram_set_valid(void)
{
#if 0    
    // we set the volatile ram as valid by copying some known data in its inside. 
    // this known data is the entire s_shalbase_moduleinfo.info.entity
    eEentity_t* ramentity = (eEentity_t*)&s_shalbase_IPCdataStored.head.entity;
    const eEentity_t* ROMentity = &s_shalbase_moduleinfo.info.entity;
    memcpy(ramentity, ROMentity, sizeof(eEentity_t));
#else
    const eEentity_t* ROMentity = &s_shalbase_moduleinfo.info.entity;
    shalbase_hid_storage_set_valid((void*)&s_shalbase_IPCdataStored, ROMentity);
#endif    
}

static void s_shalbase_ipc_ram_set_invalid(void)
{
#if 0    
    // we set the volatile ram as invalid by assigning 0 to it.
//    eEentity_t* ramentity = (eEentity_t*)&s_shalbase_IPCdataStored.head.entity;
//    memset(ramentity, 0, sizeof(eEentity_t));
    memset((void*)&s_shalbase_IPCdataStored.head, 0, sizeof(shalbaseDataHeader_t));
#else
    const eEentity_t* ROMentity = &s_shalbase_moduleinfo.info.entity;
    shalbase_hid_storage_set_invalid((void*)&s_shalbase_IPCdataStored, ROMentity);
#endif    
}

static void s_shalbase_ipc_ram_initialise(void)
{
    // validate the volatile ram
    s_shalbase_ipc_ram_set_valid();
  
    // set the remaining of the s_shalbase_IPCdataStored.head
    s_shalbase_IPCdataStored.head.datactrl.cached         = 0;
  
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

