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
#ifndef _SHALINFO_H_
#define _SHALINFO_H_


/** @file       shalINFO.h
    @brief      This header file implements public interface to the shalINFO library.
    @author     marco.accame@iit.it
    @date       03/11/2011
**/

/** @defgroup shal_info Library shalINFO
    The shalINFO library allows ...... 
 
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

#define SHALINFO_NAME                   "shalINFO"        

#define SHALINFO_SIGN                   0x30                // must have least significant nibble at 0

#define SHALINFO_VER_MAJOR              0x03                // change of binary form of data
#define SHALINFO_VER_MINOR              0x00                // change of implementation

#define SHALINFO_BUILDDATE_YEAR         2013
#define SHALINFO_BUILDDATE_MONTH        11
#define SHALINFO_BUILDDATE_DAY          22
#define SHALINFO_BUILDDATE_HOUR         16
#define SHALINFO_BUILDDATE_MIN          0




// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef enum
{
    shalinfo_ipnet          = 0,
    shalinfo_can1net        = 1,
    shalinfo_can2net        = 2,
    shalinfo_page08         = 3,
    shalinfo_page32         = 4,
    shalinfo_page64         = 5,
    shalinfo_page128        = 6
} shalinfo_deviceinfo_part_t;


typedef struct                  // 256B              
{
    eEipnetwork_t               ipnetwork;      //016B 
    eEcannetwork_t              can1network;    //004B
    eEcannetwork_t              can2network;    //004B
    uint8_t                     page08[8];      //008B
    uint8_t                     page32[32];     //032B
    uint8_t                     page64[64];     //064B
    uint8_t                     page128[128];   //128B
} shalinfo_deviceinfo_t;        EECOMMON_VERIFYsizeof(shalinfo_deviceinfo_t, 256);


// - declaration of extern public functions ---------------------------------------------------------------------------

// - info about the module

extern const eEmoduleInfo_t * shalinfo_moduleinfo_get(void);
extern const eEentity_t * shalinfo_moduleinfo_entity_get(void);
extern eEresult_t shalinfo_isvalid(void);


// - generic services

extern eEresult_t shalinfo_init(void);
extern eEresult_t shalinfo_deinit(void);
extern eEresult_t shalinfo_erase(void);

// - board-info services: it is about the hw printed-circuit-board.

// only the loader calls it with its own board-info.
extern eEresult_t shalinfo_boardinfo_synchronise(const eEboardInfo_t* boardinfo);
// other eprocesses or shared libraries just get it.
extern eEresult_t shalinfo_boardinfo_get(const eEboardInfo_t** boardinfo);


// - device-info services: it is about information used by all the e-processes. for instance mac address, ip address etc.

extern eEresult_t shalinfo_deviceinfo_clr(void);
extern eEresult_t shalinfo_deviceinfo_get(const shalinfo_deviceinfo_t** deviceinfo);
extern eEresult_t shalinfo_deviceinfo_set(const shalinfo_deviceinfo_t* deviceinfo);


extern eEresult_t shalinfo_deviceinfo_part_clr(shalinfo_deviceinfo_part_t part);
extern eEresult_t shalinfo_deviceinfo_part_get(shalinfo_deviceinfo_part_t part, const void** data);
extern eEresult_t shalinfo_deviceinfo_part_set(shalinfo_deviceinfo_part_t part, const void* data);


 
 

/** @}            
    end of group shal_info 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



