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
#ifndef _SHALBASE_H_
#define _SHALBASE_H_


/** @file       shalBASE.h
    @brief      This header file implements public interface to the shalBASE shared library.
    @author     marco.accame@iit.it
    @date       03/10/2011
**/

/** @defgroup shal_base Library shalBASE
    The shalBASE library allows ...... 
 
    @todo acemor-facenda: do documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "eEcommon.h"


// - public #define  --------------------------------------------------------------------------------------------------


// here is information used for versioning of the library
// a process uses this info to recognise validity of the permanent or volatile data managed by the library
// so far, data is recognised valid if the read value has the same SHALxxx_SIGN and SHALxxx_VER_MAJOR values.
// if not recognised as valid the data is reset to the default values

#define SHALBASE_NAME                   "shalBASE"          // name ... 

#define SHALBASE_SIGN                   0x10                // must have least significant nibble at 0

#define SHALBASE_VER_MAJOR              0x02                // change of binary form of data
#define SHALBASE_VER_MINOR              0x00                // change of implementation

#define SHALBASE_BUILDDATE_YEAR         2013
#define SHALBASE_BUILDDATE_MONTH        9
#define SHALBASE_BUILDDATE_DAY          11
#define SHALBASE_BUILDDATE_HOUR         16
#define SHALBASE_BUILDDATE_MIN          0


// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { shalbase_ipc_userdefdata_maxsize = 20 }; // max size of user-defined data that is possible to pass between e-processes
 
typedef enum
{
    shalbase_initmode_dontforce         = 0,
    shalbase_initmode_force             = 1   
} shalbase_initmode_t;


// - declaration of extern public functions ---------------------------------------------------------------------------

// - info about the module

extern const eEmoduleInfo_t * shalbase_moduleinfo_get(void);
extern const eEentity_t * shalbase_moduleinfo_entity_get(void);
extern eEresult_t shalbase_isvalid(void);


// init the base shared library.
extern eEresult_t shalbase_init(shalbase_initmode_t initmode);
extern eEresult_t shalbase_deinit(void);


// offers ipc services: (1) for the gotoproc message, (2) for user-defined data
extern eEresult_t shalbase_ipc_gotoproc_get(eEprocess_t *pr);			
extern eEresult_t shalbase_ipc_gotoproc_set(eEprocess_t pr);
extern eEresult_t shalbase_ipc_gotoproc_clr(void);

extern eEresult_t shalbase_ipc_userdefdata_get(uint8_t *data, uint8_t *size, const uint8_t maxsize);		
extern eEresult_t shalbase_ipc_userdefdata_set(uint8_t *data, uint8_t size);
extern eEresult_t shalbase_ipc_userdefdata_clr(void);

// offers some system services (through hal)
extern eEresult_t shalbase_system_canjump(uint32_t addr);
extern eEresult_t shalbase_system_canjump_to_proc(uint32_t addr, eEmoduleInfo_t *procinfo);
extern eEresult_t shalbase_system_jumpnow(uint32_t addr);
extern eEresult_t shalbase_system_restart(void);

// offers some basic storage services used by other modules (shalPART and shalINFO)
extern eEresult_t shalbase_storage_get(const eEstorage_t *strg, void *data, uint32_t size);
extern eEresult_t shalbase_storage_set(const eEstorage_t *strg, const void *data, uint32_t size);
extern eEresult_t shalbase_storage_clr(const eEstorage_t *strg, const uint32_t size);
 


// - declaration of extern hidden interface ---------------------------------------------------------------------------

typedef struct 
{
    uint8_t                 cached;                 // 1B
    uint8_t                 dummy;
    uint16_t                datalen;                // 2B. counted after the datacrc32. if 0, crc32 is not used
    uint32_t                datacrc32;              // 4B
} shalbaseDataCtrl_t;       EECOMMON_VERIFYsizeof(shalbaseDataCtrl_t, 8);

typedef struct                                      // 16B
{
    shalbaseDataCtrl_t      datactrl;
    eEentity_t              entity;                 // 8B
} shalbaseDataHeader_t;     EECOMMON_VERIFYsizeof(shalbaseDataHeader_t, 16); 

extern eEboolval_t shalbase_hid_storage_is_valid(void* storage, const eEentity_t* refentity);

extern void shalbase_hid_storage_set_valid(void* storage, const eEentity_t* refentity);

extern void shalbase_hid_storage_set_invalid(void* storage, const eEentity_t* refentity);


/** @}            
    end of group shal_base 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



