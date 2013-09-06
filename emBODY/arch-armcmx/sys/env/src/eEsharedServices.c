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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eEsharedServices.h"


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
#define SHARSERV_ROMADDR    EENV_MEMMAP_SHALSYSTEM_ROMADDR 
#define SHARSERV_ROMSIZE    EENV_MEMMAP_SHALSYSTEM_ROMSIZE

#define SHARSERV_RAMADDR    EENV_MEMMAP_SHALSYSTEM_RAMADDR
#define SHARSERV_RAMSIZE    EENV_MEMMAP_SHALSYSTEM_RAMSIZE

#define SHARSERV_STGADDR    EENV_MEMMAP_SHALSYSTEM_STGADDR
#define SHARSERV_STGSIZE    EENV_MEMMAP_SHALSYSTEM_STGSIZE
#define SHARSERV_STGTYPE    (ee_strg_eeprom)


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#if     defined(SHARSERV_MODE_SHALIB)
    #define SHARSERV_MODULEINFO_PLACED_AT       __attribute__((at(SHARSERV_ROMADDR+EENV_MODULEINFO_OFFSET)))
#else
    #define SHARSERV_MODULEINFO_PLACED_AT    
#endif

static const eEmoduleInfo_t s_sharserv_moduleinfo   SHARSERV_MODULEINFO_PLACED_AT =
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_sharlib,
            .signature  = ee_shalSharServ,
            .version    = 
            { 
                .major = SHARSERV_VER_MAJOR, 
                .minor = SHARSERV_VER_MINOR
            },  
            .builddate  = 
            {
                .year  = SHARSERV_BUILDDATE_YEAR,
                .month = SHARSERV_BUILDDATE_MONTH,
                .day   = SHARSERV_BUILDDATE_DAY,
                .hour  = SHARSERV_BUILDDATE_HOUR,
                .min   = SHARSERV_BUILDDATE_MIN
            }
        },
        .rom        = 
        {   
            .addr   = SHARSERV_ROMADDR,
            .size   = SHARSERV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = SHARSERV_RAMADDR,
            .size   = SHARSERV_RAMSIZE
        },
        .storage    = 
        {
            .type   = SHARSERV_STGTYPE,
            .size   = SHARSERV_STGSIZE,
            .addr   = SHARSERV_STGADDR
        },
        .communication  = ee_commtype_none,
        .name           = SHARSERV_NAME
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


#if     defined(SHARSERV_MODE_SHALIB)
    // using inline functions
#else
    
extern const eEmoduleInfo_t * ee_sharserv_moduleinfo_get(void)
{
    return((const eEmoduleInfo_t*)&s_sharserv_moduleinfo);
}

extern const eEentity_t * ee_sharserv_moduleinfo_entity_get(void)
{
    return((const eEentity_t*)&s_sharserv_moduleinfo.info.entity);
}

extern eEresult_t ee_sharserv_isvalid(void)
{
    return(ee_res_OK);
}

#endif


extern eEresult_t ee_sharserv_init(uint8_t forcestorageinit)
{
    //#warning --> do ee_sharserv_init() so that it calls shalBASE, shalPART and shalINFO
    
    return(ee_res_OK);
}


extern eEresult_t ee_sharserv_deinit(void)
{
    //#warning --> do ee_sharserv_deinit() so that it calls shalBASE, shalPART and shalINFO
    
    return(ee_res_OK);
}



// - base

extern eEresult_t ee_sharserv_base_init(uint8_t forcestorageinit)
{
    return(shalbase_init(forcestorageinit));
}

extern eEresult_t ee_sharserv_base_deinit(void)
{
    return(shalbase_deinit());
}

extern eEresult_t ee_sharserv_base_boardinfo_synchronise(const eEboardInfo_t* boardinfo)
{
    return(shalbase_boardinfo_synchronise(boardinfo));
}

extern eEresult_t ee_sharserv_base_boardinfo_get(const eEboardInfo_t** boardinfo)
{
    return(shalbase_boardinfo_get(boardinfo));
}

extern eEresult_t ee_sharserv_base_ipc_gotoproc_get(eEprocess_t *pr)
{
    return(shalbase_ipc_gotoproc_get(pr));
}
			
extern eEresult_t ee_sharserv_base_ipc_gotoproc_set(eEprocess_t pr)
{
    return(shalbase_ipc_gotoproc_set(pr));
}

extern eEresult_t ee_sharserv_base_ipc_gotoproc_clr(void)
{
    return(shalbase_ipc_gotoproc_clr());
}

extern eEresult_t ee_sharserv_base_ipc_volatiledata_get(uint8_t *data, uint8_t *size, const uint8_t maxsize)
{
    return(shalbase_ipc_volatiledata_get(data, size, maxsize));
}
		
extern eEresult_t ee_sharserv_base_ipc_volatiledata_set(uint8_t *data, uint8_t size)
{
    return(shalbase_ipc_volatiledata_set(data, size));
}

extern eEresult_t ee_sharserv_base_ipc_volatiledata_clr(void)
{
    return(shalbase_ipc_volatiledata_clr());
}


extern eEresult_t ee_sharserv_base_system_canjump(uint32_t addr)
{
    return(shalbase_system_canjump(addr));
}

extern eEresult_t ee_sharserv_base_system_canjump_to_proc(uint32_t addr, eEmoduleInfo_t *procinfo)
{
    return(shalbase_system_canjump_to_proc(addr, procinfo));
}

extern eEresult_t ee_sharserv_base_system_jumpnow(uint32_t addr)
{
    return(shalbase_system_jumpnow(addr));
}

extern eEresult_t ee_sharserv_base_system_restart(void)
{
    return(shalbase_system_restart());
}

extern eEresult_t ee_sharserv_base_storage_get(const eEstorage_t *strg, void *data, uint32_t size)
{
    return(shalbase_storage_get(strg, data, size));
}

extern eEresult_t ee_sharserv_base_storage_set(const eEstorage_t *strg, const void *data, uint32_t size)
{
    return(shalbase_storage_set(strg, data, size));
}

extern eEresult_t ee_sharserv_base_storage_clr(const eEstorage_t *strg, const uint32_t size)
{
    return(shalbase_storage_clr(strg, size));
}


// - part

extern eEresult_t ee_sharserv_part_init(void)
{
    return(shalpart_init());
}

extern eEresult_t ee_sharserv_part_reset(ee_sharserv_part_reset_mode_t rm)
{
    return(shalpart_reset((shalpart_reset_mode_t)rm));
}

extern eEresult_t ee_sharserv_part_deinit(void)
{
    return(shalpart_deinit());
}

extern eEresult_t ee_sharserv_part_proc_synchronise(eEprocess_t proc, const eEmoduleInfo_t *moduleinfo)
{
    return(shalpart_proc_synchronise(proc, moduleinfo));
}

extern eEresult_t ee_sharserv_part_proc_def2run_get(eEprocess_t *proc)
{
    return(shalpart_proc_def2run_get(proc));
}

extern eEresult_t ee_sharserv_part_proc_def2run_set(eEprocess_t proc)
{
    return(shalpart_proc_def2run_set(proc));
}

extern eEresult_t ee_sharserv_part_proc_runaddress_get(eEprocess_t proc, uint32_t *addr)
{
    return(shalpart_proc_runaddress_get(proc, addr));
}


extern eEresult_t ee_sharserv_part_proc_startuptimeinupdater_get(uint32_t *startuptimeinupdater)
{
    return(shalpart_proc_startuptimeinupdater_get(startuptimeinupdater));
}

extern eEresult_t ee_sharserv_part_proc_startuptimeinupdater_set(uint32_t startuptimeinupdater)
{
    return(shalpart_proc_startuptimeinupdater_set(startuptimeinupdater));
}


extern eEresult_t ee_sharserv_part_proc_allavailable_get(const eEprocess_t **table, uint8_t *size)
{
    return(shalpart_proc_allavailable_get(table, size));
}

extern eEresult_t ee_sharserv_part_proc_add(eEprocess_t proc, eEmoduleInfo_t *moduleinfo)
{
    return(shalpart_proc_add(proc, moduleinfo));
}

extern eEresult_t ee_sharserv_part_proc_rem(eEprocess_t proc)
{
    return(shalpart_proc_rem(proc));
}

extern eEresult_t ee_sharserv_part_proc_set(eEprocess_t proc, eEmoduleInfo_t *moduleinfo)
{
    return(shalpart_proc_set(proc, moduleinfo));
}

extern eEresult_t ee_sharserv_part_proc_get(eEprocess_t proc, const eEmoduleInfo_t **moduleinfo)
{
    return(shalpart_proc_get(proc, moduleinfo));
}

extern eEresult_t ee_sharserv_part_shal_synchronise(eEsharlib_t shal, const eEmoduleInfo_t *moduleinfo)
{
    return(shalpart_shal_synchronise(shal, moduleinfo));
}

extern eEresult_t ee_sharserv_part_shal_allavailable_get(const eEsharlib_t **table, uint8_t *size)
{
    return(shalpart_shal_allavailable_get(table, size));
}

extern eEresult_t ee_sharserv_part_shal_add(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo)
{
    return(shalpart_shal_add(shal, moduleinfo));
}

extern eEresult_t ee_sharserv_part_shal_rem(eEsharlib_t shal)
{
    return(shalpart_shal_rem(shal));
}

extern eEresult_t ee_sharserv_part_shal_set(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo)
{
    return(shalpart_shal_set(shal, moduleinfo));
}

extern eEresult_t ee_sharserv_part_shal_get(eEsharlib_t shal, const eEmoduleInfo_t **moduleinfo)
{
    return(shalpart_shal_get(shal, moduleinfo));
}


// - info

extern eEresult_t ee_sharserv_info_init(void)
{
    return(shalinfo_init());
}

extern eEresult_t ee_sharserv_info_deinit(void)
{
    return(shalinfo_deinit());
}

extern eEresult_t ee_sharserv_info_erase(void)
{
    return(shalinfo_erase());
}

extern eEresult_t ee_sharserv_info_boardinfo_synchronise(const eEboardInfo_t* boardinfo)
{
    return(shalinfo_boardinfo_synchronise(boardinfo));
}

// other eprocesses or shared libraries just get it.
extern eEresult_t ee_sharserv_info_boardinfo_get(const eEboardInfo_t** boardinfo)
{
    return(shalinfo_boardinfo_get(boardinfo));
}

extern eEresult_t ee_sharserv_info_deviceinfo_clr(void)
{
    return(shalinfo_deviceinfo_clr());
}

extern eEresult_t ee_sharserv_info_deviceinfo_get(const ee_sharserv_info_deviceinfo_t** deviceinfo)
{
    return(shalinfo_deviceinfo_get((const shalinfo_deviceinfo_t**)deviceinfo));
}

extern eEresult_t ee_sharserv_info_deviceinfo_set(const ee_sharserv_info_deviceinfo_t* deviceinfo)
{
    return(shalinfo_deviceinfo_set((const shalinfo_deviceinfo_t*)deviceinfo));
}

extern eEresult_t ee_sharserv_info_deviceinfo_part_clr(ee_sharserv_info_deviceinfo_part_t part)
{
    return(shalinfo_deviceinfo_part_clr((shalinfo_deviceinfo_part_t)part));
}

extern eEresult_t ee_sharserv_info_deviceinfo_part_get(ee_sharserv_info_deviceinfo_part_t part, const void** data)
{
    return(shalinfo_deviceinfo_part_get((shalinfo_deviceinfo_part_t)part, data));
}

extern eEresult_t ee_sharserv_info_deviceinfo_part_set(ee_sharserv_info_deviceinfo_part_t part, const void* data)
{
    return(shalinfo_deviceinfo_part_set((shalinfo_deviceinfo_part_t)part, data));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
extern void shalbase_entrypoint(void);

extern void shalpart_hid_entrypoint(void);

extern void sharserv_entrypoint(void)
{
    shalbase_entrypoint();
    shalinfo_init();
    shalpart_hid_entrypoint();  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



