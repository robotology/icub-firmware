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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EESHAREDSERVICES_H_
#define _EESHAREDSERVICES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEsharedServices.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEmemorymap.h"


// - public #define  --------------------------------------------------------------------------------------------------


#if defined(SHARSERV_MODE_SHALIB)

#if !defined(SHALS_MODE_STATIC)
#define SHALBASE_MODE_STATICLIBRARY
#define SHALPART_MODE_STATICLIBRARY
#define SHALINFO_MODE_STATICLIBRARY  
#endif

#endif



#define SHARSERV_NAME                   "sharSERV"          

#define SHARSERV_VER_MAJOR              0x01                // change of APIs
#define SHARSERV_VER_MINOR              0x02                // change of internals

#define SHARSERV_BUILDDATE_YEAR         2013
#define SHARSERV_BUILDDATE_MONTH        9
#define SHARSERV_BUILDDATE_DAY          4
#define SHARSERV_BUILDDATE_HOUR         15
#define SHARSERV_BUILDDATE_MIN          0



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum 
{
    sharserv_part_reset_default     = 0, 
    sharserv_part_reset_rawvals     = 1
} ee_sharserv_part_reset_mode_t;

typedef enum
{
    sharserv_info_ipnet             = 0,
    sharserv_info_can1net           = 1,
    sharserv_info_can2net           = 2,
    sharserv_info_page08            = 3,
    sharserv_info_page32            = 4,
    sharserv_info_page64            = 5,
    sharserv_info_page128           = 6
} ee_sharserv_info_deviceinfo_part_t;

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

#if     defined(SHARSERV_MODE_SHALIB)

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
    if((en->type==ee_entity_sharlib) && (en->signature==ee_shalINFO) && 
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

#else

extern const eEmoduleInfo_t * ee_sharserv_moduleinfo_get(void);
extern const eEentity_t * ee_sharserv_moduleinfo_entity_get(void);
extern eEresult_t ee_sharserv_isvalid(void);

#endif	

extern eEresult_t ee_sharserv_init(uint8_t forcestorageinit);
extern eEresult_t ee_sharserv_deinit(void);

// so far it is just a dummy wrapper. in the future we may use just ... ee_sharserv_xxxx calls
#include "shalBASE.h"
#include "shalPART.h"
#include "shalINFO.h"


// - base

extern eEresult_t ee_sharserv_base_init(uint8_t forcestorageinit);
extern eEresult_t ee_sharserv_base_deinit(void);

// only the eLoader calls ee_sharserv_base_boardinfo_synchronise()
extern eEresult_t ee_sharserv_base_boardinfo_synchronise(const eEboardInfo_t* boardinfo);
extern eEresult_t ee_sharserv_base_boardinfo_get(const eEboardInfo_t** boardinfo);

extern eEresult_t ee_sharserv_base_ipc_gotoproc_get(eEprocess_t *pr);			
extern eEresult_t ee_sharserv_base_ipc_gotoproc_set(eEprocess_t pr);
extern eEresult_t ee_sharserv_base_ipc_gotoproc_clr(void);

extern eEresult_t ee_sharserv_base_ipc_volatiledata_get(uint8_t *data, uint8_t *size, const uint8_t maxsize);		
extern eEresult_t ee_sharserv_base_ipc_volatiledata_set(uint8_t *data, uint8_t size);
extern eEresult_t ee_sharserv_base_ipc_volatiledata_clr(void);

extern eEresult_t ee_sharserv_base_system_canjump(uint32_t addr);
extern eEresult_t ee_sharserv_base_system_canjump_to_proc(uint32_t addr, eEmoduleInfo_t *procinfo);
extern eEresult_t ee_sharserv_base_system_jumpnow(uint32_t addr);
extern eEresult_t ee_sharserv_base_system_restart(void);

extern eEresult_t ee_sharserv_base_storage_get(const eEstorage_t *strg, void *data, uint32_t size);
extern eEresult_t ee_sharserv_base_storage_set(const eEstorage_t *strg, const void *data, uint32_t size);
extern eEresult_t ee_sharserv_base_storage_clr(const eEstorage_t *strg, const uint32_t size);


// - part

extern eEresult_t ee_sharserv_part_init(void);
extern eEresult_t ee_sharserv_part_reset(ee_sharserv_part_reset_mode_t rm);
extern eEresult_t ee_sharserv_part_deinit(void);

extern eEresult_t ee_sharserv_part_proc_synchronise(eEprocess_t proc, const eEmoduleInfo_t *moduleinfo);
extern eEresult_t ee_sharserv_part_proc_def2run_get(eEprocess_t *proc);
extern eEresult_t ee_sharserv_part_proc_def2run_set(eEprocess_t proc);
extern eEresult_t ee_sharserv_part_proc_runaddress_get(eEprocess_t proc, uint32_t *addr);

extern eEresult_t ee_sharserv_part_proc_startuptimeinupdater_get(uint32_t *startuptimeinupdater);
extern eEresult_t ee_sharserv_part_proc_startuptimeinupdater_set(uint32_t startuptimeinupdater);

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
                                                
                                                

// - info

extern eEresult_t ee_sharserv_info_init(void);
extern eEresult_t ee_sharserv_info_deinit(void);
extern eEresult_t ee_sharserv_info_erase(void);

// only the loader calls it with its own board-info.
extern eEresult_t ee_sharserv_info_boardinfo_synchronise(const eEboardInfo_t* boardinfo);
// other eprocesses or shared libraries just get it.
extern eEresult_t ee_sharserv_info_boardinfo_get(const eEboardInfo_t** boardinfo);

extern eEresult_t ee_sharserv_info_deviceinfo_clr(void);
extern eEresult_t ee_sharserv_info_deviceinfo_get(const ee_sharserv_info_deviceinfo_t** deviceinfo);
extern eEresult_t ee_sharserv_info_deviceinfo_set(const ee_sharserv_info_deviceinfo_t* deviceinfo);

extern eEresult_t ee_sharserv_info_deviceinfo_part_clr(ee_sharserv_info_deviceinfo_part_t part);
extern eEresult_t ee_sharserv_info_deviceinfo_part_get(ee_sharserv_info_deviceinfo_part_t part, const void** data);
extern eEresult_t ee_sharserv_info_deviceinfo_part_set(ee_sharserv_info_deviceinfo_part_t part, const void* data);


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



