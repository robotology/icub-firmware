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
#ifndef _SHALPART_H_
#define _SHALPART_H_


/** @file       shalPART.h
    @brief      This header file implements public interface to the shalPART shared library.
    @author     marco.accame@iit.it
    @date       03/07/2011
**/

/** @defgroup shal_part  shalPART
    The shalPART shared library allows ...... 
 
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

#define SHALPART_NAME                   "shalPART"          // name ... 

#define SHALPART_SIGN                   0x20                // must have least significant nibble at 0

#define SHALPART_VER_MAJOR              0x02                // change of binary form of data
#define SHALPART_VER_MINOR              0x00                // change of implementation

#define SHALPART_BUILDDATE_YEAR         2013
#define SHALPART_BUILDDATE_MONTH        9
#define SHALPART_BUILDDATE_DAY          11
#define SHALPART_BUILDDATE_HOUR         17
#define SHALPART_BUILDDATE_MIN          0

#define SHALPART_DEFAULTSTARTUPTIME     (5*1000*1000)



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum 
{
    shalpart_reset_default  = 0, 
    shalpart_reset_rawvals = 1
} shalpart_reset_mode_t;

// - declaration of extern public functions ---------------------------------------------------------------------------


// - info about the module

extern const eEmoduleInfo_t * shalpart_moduleinfo_get(void);
extern const eEentity_t * shalpart_moduleinfo_entity_get(void);
extern eEresult_t shalpart_isvalid(void);


// - generic services

extern eEresult_t shalpart_init(void);
extern eEresult_t shalpart_reset(shalpart_reset_mode_t rm);
extern eEresult_t shalpart_deinit(void);

// - management of process

extern eEresult_t shalpart_proc_synchronise(eEprocess_t proc, const eEmoduleInfo_t *moduleinfo);
extern eEresult_t shalpart_proc_allavailable_get(const eEprocess_t **table, uint8_t *size);

extern eEresult_t shalpart_proc_add(eEprocess_t proc, eEmoduleInfo_t *moduleinfo);
extern eEresult_t shalpart_proc_rem(eEprocess_t proc);
extern eEresult_t shalpart_proc_set(eEprocess_t proc, eEmoduleInfo_t *moduleinfo);
extern eEresult_t shalpart_proc_get(eEprocess_t proc, const eEmoduleInfo_t **moduleinfo);


// - management of shared libraries

extern eEresult_t shalpart_shal_synchronise(eEsharlib_t shal, const eEmoduleInfo_t *moduleinfo);
extern eEresult_t shalpart_shal_allavailable_get(const eEsharlib_t **table, uint8_t *size);

extern eEresult_t shalpart_shal_add(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo);
extern eEresult_t shalpart_shal_rem(eEsharlib_t shal);
extern eEresult_t shalpart_shal_set(eEsharlib_t shal, eEmoduleInfo_t *moduleinfo);
extern eEresult_t shalpart_shal_get(eEsharlib_t shal, const eEmoduleInfo_t **moduleinfo);
    
    
// management of bootstrap info

// retrieve the address of the execution specified process
extern eEresult_t shalpart_proc_runaddress_get(eEprocess_t proc, uint32_t *addr);


// retrieves / sets the process which must run as startup (loaded by the eLoader)
extern eEresult_t shalpart_proc_startup_get(eEprocess_t *proc);
extern eEresult_t shalpart_proc_startup_set(eEprocess_t proc);
 

// retrieves / sets the time that the system spends in the startup process
extern eEresult_t shalpart_proc_startuptime_get(uint32_t *startuptime);
extern eEresult_t shalpart_proc_startuptime_set(uint32_t startuptime);

 
// retrieves / sets the process which must eventually run after the startup
extern eEresult_t shalpart_proc_def2run_get(eEprocess_t *proc);
extern eEresult_t shalpart_proc_def2run_set(eEprocess_t proc);
 
 

/** @}            
    end of group shal_part 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



