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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EESHAREDSERVICES_H_
#define _EESHAREDSERVICES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEsharedServices.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       11/20/2013
**/


// it is possible using shared-services in two modes:
// - shared library mode by defining SHARSERV_MODE_SHALIB
// - static library mode by undefining  SHARSERV_MODE_SHALIB
// the static library mode is the recommended one


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"

#if defined(SHARSERV_MODE_SHALIB)
// it is required the memory map of shared services to know: EENV_MEMMAP_SHARSERV_ROMADDR, EENV_MODULEINFO_OFFSET
#include "eEmemorymap.h"    
#endif

// - public #define  --------------------------------------------------------------------------------------------------


#define SHARSERV_NAME                   "sharSERV"          

#define SHARSERV_VER_MAJOR              0x03                // change of APIs
#define SHARSERV_VER_MINOR              0x00                // change of internals

#define SHARSERV_BUILDDATE_YEAR         2013
#define SHARSERV_BUILDDATE_MONTH        11
#define SHARSERV_BUILDDATE_DAY          22
#define SHARSERV_BUILDDATE_HOUR         16
#define SHARSERV_BUILDDATE_MIN          0



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef     void     (*sharserv_void_fp_void_t)                       (void);

typedef enum
{
    sharserv_base_initmode_dontforcestorageinit     = 0,
    sharserv_base_initmode_forcestorageinit         = 1   
} sharserv_base_initmode_t;

typedef struct
{
    sharserv_void_fp_void_t     onerror;
    sharserv_base_initmode_t    initmode;   
} sharserv_mode_t;

enum { sharserv_base_ipc_userdefdata_maxsize = 20 }; // max size of user-defined data that is possible to pass between e-processes


typedef enum
{
    sharserv_info_ipnet             = 0,
    sharserv_info_can1net           = 1,
    sharserv_info_can2net           = 2,
    sharserv_info_page08            = 3,
    sharserv_info_page32            = 4,
    sharserv_info_page64            = 5,
    sharserv_info_page128           = 6
} ee_sharserv_info_deviceinfo_item_t;


typedef struct                      // 256B              
{
    eEipnetwork_t                   ipnetwork;      //016B 
    eEcannetwork_t                  can1network;    //004B
    eEcannetwork_t                  can2network;    //004B
    uint8_t                         page08[8];      //008B
    uint8_t                         page32[32];     //032B
    uint8_t                         page64[64];     //064B
    uint8_t                         page128[128];   //128B
} ee_sharserv_info_deviceinfo_t;    EECOMMON_VERIFYsizeof(ee_sharserv_info_deviceinfo_t, 256);


// - declaration of extern public functions ---------------------------------------------------------------------------

#if     !defined(SHARSERV_MODE_SHALIB)

extern const eEmoduleInfo_t * ee_sharserv_moduleinfo_get(void);
extern const eEentity_t * ee_sharserv_moduleinfo_entity_get(void);
extern eEresult_t ee_sharserv_isvalid(void);

#else

// inline and with reference only to rom addresses to make it independent from presence of shalib in rom
// in such a way the functions are executed with secure code and not with a jump to a location where the
// shalib may not be present

EO_extern_inline const eEmoduleInfo_t * ee_sharserv_moduleinfo_get(void)
{ 
    return((const eEmoduleInfo_t*)(EENV_MEMMAP_SHARSERV_ROMADDR+EENV_MODULEINFO_OFFSET)); 
}

EO_extern_inline const eEentity_t * ee_sharserv_moduleinfo_entity_get(void)
{ 
    return((const eEentity_t*)(EENV_MEMMAP_SHARSERV_ROMADDR+EENV_MODULEINFO_OFFSET)); 
}
	
EO_extern_inline eEresult_t ee_sharserv_isvalid(void) 
{ 
    const eEentity_t *en = ee_sharserv_moduleinfo_entity_get();
    if((en->type==ee_entity_sharlib) && (en->signature==ee_shalSharServ) && 
       (en->version.major==SHARSERV_VER_MAJOR) && (en->version.minor==SHARSERV_VER_MINOR) &&
       (en->builddate.year==SHARSERV_BUILDDATE_YEAR) && (en->builddate.month==SHARSERV_BUILDDATE_MONTH) &&
       (en->builddate.day==SHARSERV_BUILDDATE_DAY) && (en->builddate.hour==SHARSERV_BUILDDATE_HOUR) &&
       (en->builddate.min==SHARSERV_BUILDDATE_MIN)) 
       { 
            return(ee_res_OK); 
       } 
       else 
       { 
            return(ee_res_NOK_generic); 
       }
}

#endif	


extern eEresult_t ee_sharserv_init(const sharserv_mode_t* mode);
extern eEresult_t ee_sharserv_selfregister(void);
extern eEresult_t ee_sharserv_deinit(void);


// - sys: offers basic system services

extern eEresult_t ee_sharserv_sys_canjump(uint32_t addr);
extern eEresult_t ee_sharserv_sys_canjump_to_proc(uint32_t addr, eEmoduleInfo_t *procinfo);
extern eEresult_t ee_sharserv_sys_jumpnow(uint32_t addr);
extern eEresult_t ee_sharserv_sys_restart(void);


// - ipc: offers inter-process communication services

extern eEresult_t ee_sharserv_ipc_gotoproc_get(eEprocess_t *pr);			
extern eEresult_t ee_sharserv_ipc_gotoproc_set(eEprocess_t pr);
extern eEresult_t ee_sharserv_ipc_gotoproc_clr(void);

extern eEresult_t ee_sharserv_ipc_userdefdata_get(uint8_t *data, uint8_t *size, const uint8_t maxsize);		
extern eEresult_t ee_sharserv_ipc_userdefdata_set(uint8_t *data, uint8_t size);
extern eEresult_t ee_sharserv_ipc_userdefdata_clr(void);

extern eEresult_t ee_sharserv_sys_storage_get(const eEstorage_t *strg, void *data, uint32_t size);
extern eEresult_t ee_sharserv_sys_storage_set(const eEstorage_t *strg, const void *data, uint32_t size);
extern eEresult_t ee_sharserv_sys_storage_clr(const eEstorage_t *strg, const uint32_t size);


// - part: offers partition table services


extern eEresult_t ee_sharserv_part_reset(void);

extern eEresult_t ee_sharserv_part_proc_synchronise(eEprocess_t proc, const eEmoduleInfo_t *moduleinfo);

extern eEresult_t ee_sharserv_part_proc_def2run_get(eEprocess_t *proc);
extern eEresult_t ee_sharserv_part_proc_def2run_set(eEprocess_t proc);

extern eEresult_t ee_sharserv_part_proc_startup_get(eEprocess_t *proc);
extern eEresult_t ee_sharserv_part_proc_startup_set(eEprocess_t proc);

extern eEresult_t ee_sharserv_part_proc_runaddress_get(eEprocess_t proc, uint32_t *addr);

extern eEresult_t ee_sharserv_part_proc_startuptime_get(eEreltime_t *startuptime);
extern eEresult_t ee_sharserv_part_proc_startuptime_set(eEreltime_t startuptime);

extern eEresult_t ee_sharserv_part_proc_allavailable_get(const eEprocess_t **table, uint8_t *size);

extern eEresult_t ee_sharserv_part_proc_add(eEprocess_t proc, eEmoduleInfo_t *moduleinfo);
extern eEresult_t ee_sharserv_part_proc_rem(eEprocess_t proc);
extern eEresult_t ee_sharserv_part_proc_set(eEprocess_t proc, eEmoduleInfo_t *moduleinfo);
extern eEresult_t ee_sharserv_part_proc_get(eEprocess_t proc, const eEmoduleInfo_t **moduleinfo);

extern eEresult_t ee_sharserv_part_shal_synchronise(eEsharlib_t shal, const eEmoduleInfo_t *moduleinfo);

extern eEresult_t ee_sharserv_part_shal_allavailable_get(const eEsharlib_t **table, uint8_t *size);

extern eEresult_t ee_sharserv_part_shal_add(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo);
extern eEresult_t ee_sharserv_part_shal_rem(eEsharlib_t shal);
extern eEresult_t ee_sharserv_part_shal_set(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo);
extern eEresult_t ee_sharserv_part_shal_get(eEsharlib_t shal, const eEmoduleInfo_t **moduleinfo);
                                                
                                                

// - info: offers info services 


extern eEresult_t ee_sharserv_info_reset(void);

// only the loader calls it with its own board-info.
extern eEresult_t ee_sharserv_info_boardinfo_synchronise(const eEboardInfo_t* boardinfo);
// others just get it.
extern eEresult_t ee_sharserv_info_boardinfo_get(const eEboardInfo_t** boardinfo);

extern eEresult_t ee_sharserv_info_deviceinfo_clr(void);
extern eEresult_t ee_sharserv_info_deviceinfo_get(const ee_sharserv_info_deviceinfo_t** deviceinfo);
extern eEresult_t ee_sharserv_info_deviceinfo_set(const ee_sharserv_info_deviceinfo_t* deviceinfo);

extern eEresult_t ee_sharserv_info_deviceinfo_item_clr(ee_sharserv_info_deviceinfo_item_t item);
extern eEresult_t ee_sharserv_info_deviceinfo_item_get(ee_sharserv_info_deviceinfo_item_t item, const void** data);
extern eEresult_t ee_sharserv_info_deviceinfo_item_set(ee_sharserv_info_deviceinfo_item_t item, const void* data);


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



