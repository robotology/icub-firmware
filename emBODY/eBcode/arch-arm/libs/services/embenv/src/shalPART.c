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

/* @file       shalPART.c
    @brief      This header file implements the shalPART shared library.
    @author     marco.accame@iit.it
    @date       07/05/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "eEmemorymap.h" 
#include "shalBASE.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "shalPART.h"


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



typedef struct              // numer of bytes depends on many things. it is 792 under following conditions:
{                           // ...
    uint8_t                 NprocMax;                   // constant and fixed to ...  as it is size of array inside
    uint8_t                 NshalMax;                   // constant and fixed to ...  as it is size of array inside 
    uint8_t                 Nproc;                      // can be changed in runtime if a new partition w/ a proc is added
    uint8_t                 Nshal;                      // can be changed in runtime if a new partition w/ a shal is added
    eEprocess_t             defProc2run;                // it is the default process the eLoader runs after the startup process
    eEprocess_t             startupProc;                // it is the process the eLoader runs at bootstrap      
    uint8_t                 ffu02[2];
    uint32_t                startupTime;                // it is the max waiting time of the startupProc (typically the updater)
    // 12 bytes fo far
    eEprocess_t             tableprocs[ee_procMaxNum];  // contains list of Nproc procs
    eEsharlib_t             tableshals[ee_shalMaxNum];  // contains list of Nshar shals 
    // 12 + 8 + 4 = 24 (multiple of 8 -> no holes)
    eEmoduleInfo_t          procInfo[ee_procMaxNum];    // the process info
    eEmoduleInfo_t          shalInfo[ee_shalMaxNum];    // the shallib info
    // 24 + 64*8 + 64*4 =   792
} partTable_t;              EECOMMON_VERIFYsizeof(partTable_t, 792);


typedef struct              // 808B 
{
    shalbaseDataHeader_t    head;   // 16B
    partTable_t             data;   // 792B
} partTableStorage_t;       EECOMMON_VERIFYsizeof(partTableStorage_t, 808);



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// always in static mode
#define SHALPART_MODE_STATICLIBRARY



#if     defined(SHALPART_MODE_STATICLIBRARY)


#define SHALPART_ROMADDR            (0)
#define SHALPART_ROMSIZE            (0)

#define SHALPART_RAMADDR            (0)
#define SHALPART_RAMSIZE            (0)

#define SHALPART_STGTYPE            (ee_strg_eeprom)
#define SHALPART_STGADDR            (EENV_MEMMAP_SHALPART_STGADDR)
#define SHALPART_STGSIZE            (EENV_MEMMAP_SHALPART_STGSIZE)

#else   // shared lib

#define SHALPART_ROMADDR            (EENV_MEMMAP_SHALPART_ROMADDR)
#define SHALPART_ROMSIZE            (EENV_MEMMAP_SHALPART_ROMSIZE)

#define SHALPART_RAMADDR            (EENV_MEMMAP_SHALPART_RAMADDR)
#define SHALPART_RAMSIZE            (EENV_MEMMAP_SHALPART_RAMSIZE)

#define SHALPART_STGTYPE            (ee_strg_eeprom)
#define SHALPART_STGADDR            (EENV_MEMMAP_SHALPART_STGADDR)
#define SHALPART_STGSIZE            (EENV_MEMMAP_SHALPART_STGSIZE)


// the ram size to be used in scatter-file and the one used by the program for static ram
#define SHALPART_RAMFOR_RWDATA      (EENV_MEMMAP_SHALPART_RAMFOR_RWDATA) 

// the ram size to be used with __attribute__((at(SHALPART_RAMADDR))), which is sizeof(partTableStorage_t)
#define SHALPART_RAMFOR_ZIDATA      (EENV_MEMMAP_SHALPART_RAMFOR_ZIDATA)


typedef int dummy1[sizeof(partTableStorage_t)     <= (SHALPART_RAMSIZE) ? 1 : -1];
typedef int dummy2[SHALPART_RAMFOR_ZIDATA <= ((SHALPART_RAMSIZE-SHALPART_RAMFOR_RWDATA)) ? 1 : -1];


#endif



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eEresult_t s_itisinside_tableprocs(partTableStorage_t *partinfo, eEprocess_t proc);
static void s_update_tableprocs(partTableStorage_t *partinfo);
static eEresult_t s_itisinside_tableshals(partTableStorage_t *partinfo, eEsharlib_t shal);
static void s_update_tableshals(partTableStorage_t *partinfo);

static void s_shalpart_permanent_partinfo_init(void);
static void s_shalpart_permanent_partinfo_reset(partTableStorage_t *partinfo);
static void s_shalpart_permanent_partinfo_cache_invalidate(void);
static partTableStorage_t* s_shalpart_permanent_partinfo_get(void);
static void s_shalpart_permanent_partinfo_set(partTableStorage_t *partinfo);

static eEboolval_t s_shalpart_storage_is_valid(void* storage);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#warning ENHANCE IT: by not mapping s_shalpart_tableStored at SHALPART_RAMADDR
static volatile partTableStorage_t s_shalpart_tableStored __attribute__((at(SHALPART_RAMADDR))); 

// - module info ------------------------------------------------------------------------------------------------------

#if     defined(SHALPART_MODE_STATICLIBRARY)
    #define SHALPART_MODULEINFO_PLACED_AT
    #define SHALPART_ENTITY_TYPE                ee_entity_statlib
#else
    #define SHALPART_MODULEINFO_PLACED_AT       __attribute__((at(SHALPART_ROMADDR+EENV_MODULEINFO_OFFSET)))
    #define SHALPART_ENTITY_TYPE                ee_entity_sharlib
#endif

static const eEmoduleInfo_t s_shalpart_moduleinfo   SHALPART_MODULEINFO_PLACED_AT =
{
    .info           =
    {
        .entity     =
        {
            .type       = SHALPART_ENTITY_TYPE,
            .signature  = ee_shalSharServ | SHALPART_SIGN,
            .version    = 
            { 
                .major = SHALPART_VER_MAJOR, 
                .minor = SHALPART_VER_MINOR
            },  
            .builddate  = 
            {
                .year  = SHALPART_BUILDDATE_YEAR,
                .month = SHALPART_BUILDDATE_MONTH,
                .day   = SHALPART_BUILDDATE_DAY,
                .hour  = SHALPART_BUILDDATE_HOUR,
                .min   = SHALPART_BUILDDATE_MIN
            }
        },
        .rom        = 
        {   
            .addr   = SHALPART_ROMADDR,
            .size   = SHALPART_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = SHALPART_RAMADDR,
            .size   = SHALPART_RAMSIZE
        },
        .storage    = 
        {
            .type   = SHALPART_STGTYPE,
            .size   = SHALPART_STGSIZE,
            .addr   = SHALPART_STGADDR
        },
        .communication  = ee_commtype_none,
        .name           = SHALPART_NAME
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


#if     defined(SHALPART_MODE_STATICLIBRARY)

extern const eEmoduleInfo_t * shalpart_moduleinfo_get(void)
{
    return((const eEmoduleInfo_t*)&s_shalpart_moduleinfo);
}

extern const eEentity_t * shalpart_moduleinfo_entity_get(void)
{
    return((const eEentity_t*)&s_shalpart_moduleinfo.info.entity);
}

extern eEresult_t shalpart_isvalid(void)
{
    return(ee_res_OK);
}

#else
    // using inline functions
#endif



extern eEresult_t shalpart_init(void)
{
    // in the beginning ... the base

    if(ee_res_NOK_generic == shalbase_isvalid())
    {
        return(ee_res_NOK_generic);
    }

    shalbase_init(shalbase_initmode_dontforce);
   
    // then the rest ... 
    
    
    // initialise the management of partinfo

    s_shalpart_permanent_partinfo_init();
    s_shalpart_permanent_partinfo_cache_invalidate();

    // before its use we verify that the partition is valid. if not ... reset it
    // if i verify in here then ... i dont need to check validity in other functions
    partTableStorage_t* partinfo = s_shalpart_permanent_partinfo_get();
    if(ee_false == s_shalpart_storage_is_valid(partinfo))
    {
        // first time we run ... or we have read an old partition table --> need to write the default data into permanent data
        s_shalpart_permanent_partinfo_reset(partinfo);
    }

    return(ee_res_OK);
}


extern eEresult_t shalpart_reset(shalpart_reset_mode_t rm)
{
    if(shalpart_reset_rawvals == rm)
    {
        memset((void*)&s_shalpart_tableStored, 0xff, sizeof(partTableStorage_t));
        shalbase_storage_set(&s_shalpart_moduleinfo.info.storage, (void*)&s_shalpart_tableStored, sizeof(partTableStorage_t));
    }
    else
    {  
        s_shalpart_permanent_partinfo_reset((partTableStorage_t*)&s_shalpart_tableStored);
    }

    return(ee_res_OK);
}

extern eEresult_t shalpart_deinit(void)
{
    // so far we dont need to undo anything. 
    return(ee_res_OK);
}



extern eEresult_t shalpart_proc_synchronise(eEprocess_t proc, const eEmoduleInfo_t *moduleinfo)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(ee_res_NOK_generic == s_itisinside_tableprocs(partinfo, proc))
    { 
        // add it
        shalpart_proc_add(proc, (eEmoduleInfo_t *)moduleinfo);
    }
    else
    {
        // verify if it is equal. if not copy  
        if(0 != memcmp(&(partinfo->data.procInfo[proc]), moduleinfo, sizeof(eEmoduleInfo_t)))
        {
            shalpart_proc_set(proc, (eEmoduleInfo_t *)moduleinfo);
        }          
    }
    
    return(ee_res_OK);    
}


extern eEresult_t shalpart_proc_def2run_get(eEprocess_t *proc)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    *proc = partinfo->data.defProc2run;

    if(ee_procNone == partinfo->data.defProc2run)
    {
        return(ee_res_NOK_generic);   
    }
    else
    {
   
        return(ee_res_OK);
    }
}


extern eEresult_t shalpart_proc_def2run_set(eEprocess_t proc)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(partinfo->data.defProc2run != proc)
    {
        partinfo->data.defProc2run = proc;
        s_shalpart_permanent_partinfo_set(partinfo); 
    }

    return(ee_res_OK);
}


extern eEresult_t shalpart_proc_startup_get(eEprocess_t *proc)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    *proc = partinfo->data.startupProc;

    if(ee_procNone == partinfo->data.startupProc)
    {
        return(ee_res_NOK_generic);   
    }
    else
    {
   
        return(ee_res_OK);
    }
}


extern eEresult_t shalpart_proc_startup_set(eEprocess_t proc)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(partinfo->data.startupProc != proc)
    {
        partinfo->data.startupProc = proc;
        s_shalpart_permanent_partinfo_set(partinfo); 
    }

    return(ee_res_OK);
}



extern eEresult_t shalpart_proc_startuptime_get(uint32_t *startuptime)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    *startuptime = partinfo->data.startupTime;

    if(0 == partinfo->data.startupTime)
    {
        return(ee_res_NOK_generic);   
    }
    else
    {
   
        return(ee_res_OK);
    }
}


extern eEresult_t shalpart_proc_startuptime_set(uint32_t startuptime)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(partinfo->data.startupTime != startuptime)
    {
        partinfo->data.startupTime = startuptime;
        s_shalpart_permanent_partinfo_set(partinfo); // aka: s_program_page();
    }

    return(ee_res_OK);
}


extern eEresult_t shalpart_proc_allavailable_get(const eEprocess_t **table, uint8_t *size)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    *table = partinfo->data.tableprocs;
    *size  = partinfo->data.Nproc;

    return(ee_res_OK);
}


extern eEresult_t shalpart_proc_add(eEprocess_t proc, eEmoduleInfo_t *moduleinfo) 
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();
 
    if((proc >= ee_procMaxNum) || (NULL == moduleinfo))
    {
        return(ee_res_NOK_generic);     
    }

    if((proc != moduleinfo->info.entity.signature) || (ee_entity_process != moduleinfo->info.entity.type))
    {
        return(ee_res_NOK_generic);
    }

    memcpy((void*)&(partinfo->data.procInfo[proc]), moduleinfo, sizeof(eEmoduleInfo_t));

    // 2. update the proc table
    s_update_tableprocs(partinfo);


    // 3. save in permanent memory
    s_shalpart_permanent_partinfo_set(partinfo); // aka s_program_page();


    return(ee_res_OK);

}


extern eEresult_t shalpart_proc_rem(eEprocess_t proc)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(ee_res_NOK_generic == s_itisinside_tableprocs(partinfo, proc))
    {
        return(ee_res_NOK_generic);
    }

    // copy a null entry into proper place
    memset((void*)&(partinfo->data.procInfo[proc]), 0, sizeof(eEmoduleInfo_t));

    // update the proc table
    s_update_tableprocs(partinfo);

    // if i remove the defproc2run ...
    if(proc == partinfo->data.defProc2run)
    {
        partinfo->data.defProc2run = ee_procNone;
    }
    
    // if i remove the startup proc ...
    if(proc == partinfo->data.startupProc)
    {
        partinfo->data.startupProc = ee_procNone;
    }


    // copy into permanent memory
    s_shalpart_permanent_partinfo_set(partinfo); 

    return(ee_res_OK);
}


extern eEresult_t shalpart_proc_set(eEprocess_t proc, eEmoduleInfo_t *moduleinfo) 
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if((ee_res_NOK_generic == s_itisinside_tableprocs(partinfo, proc)) || (NULL == moduleinfo))
    {
        return(ee_res_NOK_generic);
    }

    if((proc != moduleinfo->info.entity.signature) || (ee_entity_process != moduleinfo->info.entity.type))
    {
        return(ee_res_NOK_generic);
    } 
    
    // copy entry into proper place
    memcpy((void*)&(partinfo->data.procInfo[proc]), moduleinfo, sizeof(eEmoduleInfo_t));

    // 2. update the proc table: no need if it is already inside
    // s_update_tableprocs();

    // 3. copy into permenent memory
    s_shalpart_permanent_partinfo_set(partinfo); 


    return(ee_res_OK);

}


extern eEresult_t shalpart_proc_get(eEprocess_t proc, const eEmoduleInfo_t **moduleinfo)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(ee_res_NOK_generic == s_itisinside_tableprocs(partinfo, proc))
    {
        return(ee_res_NOK_generic);
    }


    *moduleinfo = &partinfo->data.procInfo[proc];

    return(ee_res_OK);
}


extern eEresult_t shalpart_proc_runaddress_get(eEprocess_t proc, uint32_t *addr)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(ee_res_NOK_generic == s_itisinside_tableprocs(partinfo, proc))
    {
        return(ee_res_NOK_generic);
    }

   
    *addr = partinfo->data.procInfo[proc].info.rom.addr;

    return(ee_res_OK);
}




extern eEresult_t shalpart_shal_synchronise(eEsharlib_t shal, const eEmoduleInfo_t *moduleinfo)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();
    

    if(ee_res_NOK_generic == s_itisinside_tableshals(partinfo, shal))
    { 
        // add it
        shalpart_shal_add(shal, (eEmoduleInfo_t *)moduleinfo);
    }
    else
    {
        // verify if it is equal. if not copy  
        if(0 != memcmp(&(partinfo->data.shalInfo[shal]), moduleinfo, sizeof(eEmoduleInfo_t)))
        {
            shalpart_shal_set(shal, (eEmoduleInfo_t *)moduleinfo);
        }          
    }
                            
    return(ee_res_OK);    
}




extern eEresult_t shalpart_shal_allavailable_get(const eEsharlib_t **table, uint8_t *size)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    *table = partinfo->data.tableshals;
    *size  = partinfo->data.Nshal;

    return(ee_res_OK);
}


extern eEresult_t shalpart_shal_add(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();
 
    if((shal >= ee_shalMaxNum) || (NULL == moduleinfo))
    {
        return(ee_res_NOK_generic);     
    }

    if((shal != moduleinfo->info.entity.signature) || (ee_entity_sharlib != moduleinfo->info.entity.type))
    {
        return(ee_res_NOK_generic);
    }   
    
    memcpy((void*)&(partinfo->data.shalInfo[shal]), moduleinfo, sizeof(eEmoduleInfo_t));

    // 2. update the shal table
    s_update_tableshals(partinfo);

    // 3. copy into permanent memory
    s_shalpart_permanent_partinfo_set(partinfo); 


    return(ee_res_OK);

}


extern eEresult_t shalpart_shal_rem(eEsharlib_t shal)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(ee_res_NOK_generic == s_itisinside_tableshals(partinfo, shal))
    {
        return(ee_res_NOK_generic);
    }

    // copy a null entry into proper place
    memset((void*)&(partinfo->data.shalInfo[shal]), 0, sizeof(eEmoduleInfo_t));

    // update the shal table
    s_update_tableshals(partinfo);

    // copy into permanent storage
    s_shalpart_permanent_partinfo_set(partinfo); 

    return(ee_res_OK);
}



extern eEresult_t shalpart_shal_set(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if((ee_res_NOK_generic == s_itisinside_tableshals(partinfo, shal)) || (NULL == moduleinfo))
    {
        return(ee_res_NOK_generic);
    }

    if((shal != moduleinfo->info.entity.signature) || (ee_entity_sharlib != moduleinfo->info.entity.type))
    {
        return(ee_res_NOK_generic);
    } 

    // copy entry into proper place
    memcpy((void*)&(partinfo->data.shalInfo[shal]), moduleinfo, sizeof(eEmoduleInfo_t));

    // 2. update the shal table: no need as we have verified that shal is already inside.
    // s_update_tableshals();

    // 4. copy into permanent storage
    s_shalpart_permanent_partinfo_set(partinfo);


    return(ee_res_OK);
}


extern eEresult_t shalpart_shal_get(eEsharlib_t shal, const eEmoduleInfo_t **moduleinfo)
{
    partTableStorage_t  * volatile partinfo = s_shalpart_permanent_partinfo_get();

    if(ee_res_NOK_generic == s_itisinside_tableshals(partinfo, shal))
    {
        return(ee_res_NOK_generic);
    }

    *moduleinfo = &partinfo->data.shalInfo[shal];

    return(ee_res_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void shalpart_hid_entrypoint(void)
{   
    shalpart_init();
    shalpart_reset((shalpart_reset_mode_t)0);
    shalpart_deinit();
    shalpart_proc_synchronise((eEprocess_t) 0, NULL);
    shalpart_proc_def2run_get(NULL);
    shalpart_proc_def2run_set((eEprocess_t) 0);
    shalpart_proc_runaddress_get((eEprocess_t)0, NULL);
    shalpart_proc_allavailable_get(NULL, NULL);
    shalpart_proc_add((eEprocess_t)0, NULL);
    shalpart_proc_rem((eEprocess_t)0);
    shalpart_proc_set((eEprocess_t)0, NULL);
    shalpart_proc_get((eEprocess_t)0, NULL);
    shalpart_shal_synchronise((eEsharlib_t)0, NULL);
    shalpart_shal_allavailable_get(NULL, NULL);
    shalpart_shal_add((eEsharlib_t)0, NULL);
    shalpart_shal_rem((eEsharlib_t)0);
    shalpart_shal_set((eEsharlib_t)0, NULL);
    shalpart_shal_get((eEsharlib_t)0, NULL);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static eEresult_t s_itisinside_tableprocs(partTableStorage_t *partinfo, eEprocess_t proc)
{   
    uint8_t i;

    if(proc >= ee_procMaxNum)
    {
        return(ee_res_NOK_generic);
    }

    for(i=0; i<partinfo->data.Nproc; i++)
    {
        if(partinfo->data.tableprocs[i] == proc)
        {
            return(ee_res_OK);
        }
    }

    return(ee_res_NOK_generic);
}


static void s_update_tableprocs(partTableStorage_t *partinfo)
{
    uint8_t i = 0;

    partinfo->data.Nproc = 0;

    for(i=0; i<ee_procMaxNum; i++)
    {
        partinfo->data.tableprocs[i] = ee_procNone;
    }

    for(i=0; i<ee_procMaxNum; i++)
    {
        if(ee_none != partinfo->data.procInfo[i].info.entity.type)
        {
            // found one
            partinfo->data.tableprocs[ partinfo->data.Nproc] = (eEprocess_t)i;
            partinfo->data.Nproc++; 
        }
    }
}


static eEresult_t s_itisinside_tableshals(partTableStorage_t *partinfo, eEsharlib_t shal)
{   
    uint8_t i;

    if(shal >= ee_shalMaxNum)
    {
        return(ee_res_NOK_generic);
    }

    for(i=0; i<partinfo->data.Nshal; i++)
    {
        if(partinfo->data.tableshals[i] == shal)
        {
            return(ee_res_OK);
        }
    }

    return(ee_res_NOK_generic);
}


static void s_update_tableshals(partTableStorage_t *partinfo)
{
    uint8_t i = 0;

    partinfo->data.Nshal = 0;

    for(i=0; i<ee_shalMaxNum; i++)
    {
        partinfo->data.tableshals[i] = ee_shalNone;
    }

    for(i=0; i<ee_shalMaxNum; i++)
    {
        if(ee_none != partinfo->data.shalInfo[i].info.entity.type)
        {
            // found one
            partinfo->data.tableshals[partinfo->data.Nshal] = (eEsharlib_t)i;
            partinfo->data.Nshal++; 
        }
    }
}


static partTableStorage_t* s_shalpart_permanent_partinfo_get(void)
{
    if(0 == s_shalpart_tableStored.head.datactrl.cached)
    {
        shalbase_storage_get(&s_shalpart_moduleinfo.info.storage, (void*)&s_shalpart_tableStored, sizeof(partTableStorage_t));
    }

    s_shalpart_tableStored.head.datactrl.cached = 1;

    return((partTableStorage_t*)&s_shalpart_tableStored);
}

static void s_shalpart_permanent_partinfo_init(void)
{
    memset((void*)&s_shalpart_tableStored, 0, sizeof(s_shalpart_tableStored));
}

static void s_shalpart_permanent_partinfo_cache_invalidate(void)
{
    s_shalpart_tableStored.head.datactrl.cached = 0;
}

static void s_shalpart_permanent_partinfo_reset(partTableStorage_t *partinfo)
{    
   
    // initialise data

    uint8_t i = 0;
    
    partinfo->data.NprocMax                 = ee_procMaxNum;
    partinfo->data.NshalMax                 = ee_shalMaxNum;
    partinfo->data.Nproc                    = 0;
    partinfo->data.Nshal                    = 0;
    partinfo->data.defProc2run              = ee_procNone;
    partinfo->data.startupProc              = ee_procNone;
    

    memset(partinfo->data.tableprocs, ee_procNone, sizeof(partinfo->data.tableprocs));
    memset(partinfo->data.tableshals, ee_procNone, sizeof(partinfo->data.tableshals));
    
    partinfo->data.startupTime = SHALPART_DEFAULTSTARTUPTIME;
    memset(partinfo->data.ffu02, 0, sizeof(partinfo->data.ffu02));

    for(i=0; i<ee_procMaxNum; i++)
    {
        memset(&partinfo->data.procInfo[i], 0, sizeof(eEmoduleInfo_t));
    }

    for(i=0; i<ee_shalMaxNum; i++)
    {
        memset(&partinfo->data.shalInfo[i], 0, sizeof(eEmoduleInfo_t));
    }
    
    // set it valid
    const eEentity_t* ROMentity = &s_shalpart_moduleinfo.info.entity;
    shalbase_hid_storage_set_valid((void*)partinfo, ROMentity);
        
    
    // copy into storage
    s_shalpart_permanent_partinfo_set(partinfo);
}


static void s_shalpart_permanent_partinfo_set(partTableStorage_t *partinfo)
{
    partinfo->head.datactrl.cached = 0;
    shalbase_storage_set(&s_shalpart_moduleinfo.info.storage, partinfo, sizeof(partTableStorage_t));
}


static eEboolval_t s_shalpart_storage_is_valid(void* storage)
{   
    const eEentity_t* ROMentity = &s_shalpart_moduleinfo.info.entity;
    
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

