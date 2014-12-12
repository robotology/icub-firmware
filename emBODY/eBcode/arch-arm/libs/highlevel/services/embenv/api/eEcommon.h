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
#ifndef _EECOMMON_H_
#define _EECOMMON_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEcommon.h
    @brief      This header file contains common data structures used in the embEenv.
    @author     marco.accame@iit.it
    @date       11/20/2013
**/

/** @defgroup embenv embEenv environment
    The embEnv allows ...... 
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------


#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------

// it holds a c-preprocessor mode to verify that the size of a type is exactly size bytes.
#define EECOMMON_VERIFYsizeof(type, size)     typedef uint8_t EEGUARD_##type[ ( size == sizeof(type) ) ? (1) : (-1)];

// it builds an ip address in embEnv format starting from the dot decimal value ip1.ip2.ip3.ip4
#define EECOMMON_ipaddr_from(ip1, ip2, ip3, ip4)    ( (((uint32_t)(ip4) << 24)&0xff000000) | \
                                                      (((uint32_t)(ip3) << 16)&0x00ff0000) | \
                                                      (((uint32_t)(ip2) << 8)&0x0000ff00)  | \
                                                      (((uint32_t)(ip1) << 0)&0x000000ff) )
// the used mac oui                                                     
#define EECOMMON_mac_oui_iit                        (0x0000000000000002)//(0x0000000000332211)

// the default bytes of the address
#define EECOMMON_ipaddr_def1                        10
#define EECOMMON_ipaddr_def2                        0
#define EECOMMON_ipaddr_def3                        1
#define EECOMMON_ipaddr_def4                        99

// the base ip address: it is 
#define EECOMMON_ipaddr_base_iit                    EECOMMON_ipaddr_from(EECOMMON_ipaddr_def1, EECOMMON_ipaddr_def2, EECOMMON_ipaddr_def3, 0)

// the defaul ip address
#define EECOMMON_ipaddr_default                     EECOMMON_ipaddr_from(EECOMMON_ipaddr_def1, EECOMMON_ipaddr_def2, EECOMMON_ipaddr_def3, EECOMMON_ipaddr_def4)

// the ip mask
#define EECOMMON_ipmask_default_iit                 EECOMMON_ipaddr_from(255, 255, 255, 0)


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef int8_t eEenum08_t
    @brief      eEenum08_t contains a generic enum which must be contained in exactly 8 bit. It is a int8_t
 **/
typedef int8_t eEenum08_t;


/** @typedef    typedef uint32_t eEipaddr_t
    @brief      contains the ip address
 **/
typedef uint32_t eEipaddr_t;


/** @typedef    typedef uint64_t eEmacaddr_t
    @brief      contains the mac address in the 6 LSBs
 **/
typedef uint64_t eEmacaddr_t;


/** @typedef    typedef uint32_t eEcanaddr_t
    @brief      contains the can address
 **/
typedef uint32_t eEcanaddr_t;


/** @typedef    typedef enum eEresult_t
    @brief      eEresult_t is keeps results of functions. It holds values which are compatible
                with other %&^result_t types (OK is 0, generic NOK is -1).
 **/  
typedef enum               
{
    ee_res_OK               = 0,        
    ee_res_NOK_generic      = -1 
} eEresult_t; 


/** @typedef    typedef enum eEboolval_t 
    @brief      eEboolval_t contains values for bool type (eEbool_t).
 **/ 
typedef enum
{
    ee_false = 0,      /**< false */
    ee_true  = 1       /**< true */
} eEboolval_t;


/** @typedef    typedef enum eEbool_t 
    @brief      eEbool_t is the bool type.
 **/ 
typedef uint8_t     eEbool_t;


/** @typedef    typedef uint32_t eEreltime_t
    @brief      eEreltime_t contains the relative time expressed in micro-seconds. It is used for relative timing
                operations because its maximum value is about 4294 seconds. 
 **/
typedef uint32_t    eEreltime_t; 

typedef enum 
{
    ee_Jan      = 1,
    ee_Feb      = 2,
    ee_Mar      = 3,
    ee_Apr      = 4,
    ee_May      = 5,
    ee_Jun      = 6,
    ee_Jul      = 7,
    ee_Aug      = 8,
    ee_Sep      = 9,
    ee_Oct      = 10,
    ee_Nov      = 11,
    ee_Dec      = 12
} eEmonth_t;

/** @typedef    typedef struct eEdate_t
    @brief      eEdate_t keeps the date of when a module is built or a board is made
    @warning    this type must be of a given fixed size: 4B
 **/ 
typedef struct                     
{
    uint32_t            year  : 12;    /**< the year a.d. upto 2047 */
    uint32_t            month : 4;     /**< the month, where jan is 1, dec is 12. use eEmonth_t */
    uint32_t            day   : 5;     /**< the day from 1 to 31 */
    uint32_t            hour  : 5;     /**< the hour from 0 to 23 */
    uint32_t            min   : 6;     /**< the minute from 0 to 59 */
} eEdate_t;             EECOMMON_VERIFYsizeof(eEdate_t, 4);


/** @typedef    typedef struct eEversion_t
    @brief      eEversion_t is used to keep the version of an eEmodule. 
    @warning    this type must be of a given fixed size: 2B
 **/  
typedef struct      
{
    uint8_t             major;          /**< major number   */ 
    uint8_t             minor;          /**< minor number  */  
} eEversion_t;          EECOMMON_VERIFYsizeof(eEversion_t, 2);


/** @typedef    typedef enum eEmoduleType_t
    @brief      eEmoduleType_t keeps the kinds of module. 
 **/  
typedef enum               
{
    ee_none                 = 0,    /**< module of no type */
    ee_process              = 1,    /**< the module is an eProcess: it contains executable code **/        
    ee_sharlib              = 2,    /**< the module is a shared library: it contains a library mapped at a fixed address and available to all eProcesses) */
    ee_storage              = 3     /**< the module is used for storage of data */    
} eEmoduleType_t;


/** @typedef    typedef struct eEsysmemory_t
    @brief      eEsysmemory_t keeps information on memory. It can be used for instance to tell 
                which FLASH / RAM / EEPROM is used by an eProcess.
    @warning    this type must be of a given fixed size: 8B
  **/
typedef struct    
{
    uint32_t            addr;               /**< the address of memory */
    uint32_t            size;               /**< the size of memory */
} eEsysmemory_t;        EECOMMON_VERIFYsizeof(eEsysmemory_t, 8);


/** @typedef    typedef enum eEstorageType_t
    @brief      eEstorageType_t specifies the kind of storage in use 
 **/  
typedef enum
{
    ee_strg_none            = 0,                    /**< no storage */
    ee_strg_eflash          = 1,                    /**< embedded flash */
    ee_strg_emuleeprom      = 2,                    /**< EEPROM emulated on flash */
    ee_strg_eeprom          = 3                     /**< EEPROM */
} eEstorageType_t;


/** @typedef    typedef struct eEstorage_t
    @brief      eEstorage_t keeps information on storage. 
    @warning    this type must be of a given fixed size: 8B
  **/
typedef struct          
{
    uint32_t            type   : 2;         /**< the type of storage medium: use eEstorageType_t */
    uint32_t            size   : 30;        /**< the size of storage medium */
    uint32_t            addr   : 32;        /**< the starting address of storage medium */
} eEstorage_t;          EECOMMON_VERIFYsizeof(eEstorage_t, 8);


/** @typedef    typedef enum eEcommunicationType_t
    @brief      keeps information on communication type 
  **/
typedef enum
{
    ee_commtype_none    = 0,
    ee_commtype_eth     = 1 << 0,
    ee_commtype_can1    = 1 << 1,
    ee_commtype_can2    = 1 << 2,
    ee_commtype_gtw     = 1 << 3,
    ee_commtype_exp4    = 1 << 4,
    ee_commtype_exp5    = 1 << 5,
    ee_commtype_exp6    = 1 << 6,
    ee_commtype_exp7    = 1 << 7
} eEcommunicationType_t;


/** @typedef    typedef struct eEprotocolInfo_t
    @brief      eEprotocolInfo_t keeps information on protocol. 
    @warning    this type must be of a given fixed size: 8B
  **/
typedef struct          
{
    eEversion_t         udpprotversion;     /**< the protocol version of the udp communication (non-zero if exists) */
    eEversion_t         can1protversion;    /**< the protocol version of the can1 communication (non-zero if exists) */
    eEversion_t         can2protversion;    /**< the protocol version of the can2 communication (non-zero if exists) */
    eEversion_t         gtwprotversion;    /**< the protocol version of the gtw communication (non-zero if exists) */
} eEprotocolInfo_t;     EECOMMON_VERIFYsizeof(eEprotocolInfo_t, 8);


/** @typedef    typedef struct eEipnetwork_t
    @brief      eEipnetwork_t keeps information on the ipnetwork. 
    @warning    this type must be of a given fixed size: 16B
  **/
typedef struct          
{
    eEmacaddr_t         macaddress;         /**< the mac address */
    eEipaddr_t          ipaddress;          /**< the ip address */
    eEipaddr_t          ipnetmask;          /**< the netmask */
} eEipnetwork_t;        EECOMMON_VERIFYsizeof(eEipnetwork_t, 16);



/** @typedef    typedef struct eEcannetwork_t
    @brief      eEcannetwork_t keeps information on the can network. 
    @warning    this type must be of a given fixed size: 4B
  **/
typedef struct          // 4 BYTES
{
    eEcanaddr_t         idcan;
} eEcannetwork_t;       EECOMMON_VERIFYsizeof(eEcannetwork_t, 4);


/** @typedef    typedef enum eEtypeOfEntity_t
    @brief      specifies the entity. 
  **/
typedef enum
{
    ee_entity_none                  = 0,    /**< none */
    ee_entity_board                 = 1,    /**< a board. */
    ee_entity_process               = 2,    /**< an eProcess. */        
    ee_entity_sharlib               = 3,    /**< a shared library. */
    ee_entity_statlib               = 4,    /**< a static library. */     
    ee_entity_forfutureuse5         = 5,    /**< for future use */
    ee_entity_forfutureuse6         = 6,    /**< for future use */
    ee_entity_forfutureuse7         = 7     /**< for future use */
} eEtypeOfEntity_t;


/** @typedef    typedef struct eEentity_t
    @brief      Keeps basic information which specifies an entity. The entity can be one of what specified in eEtypeOfEntity_t.
                If for instance it is an eProcess, its type is ee_entity_process, its signature is the kind of eProcess 
                (e.g., the eLoader), its version is the software version, and buildate keeps the date of build.
    @warning    this type must be of a given fixed size: 8B
  **/
typedef struct          
{
    eEenum08_t          type;               /**< the type of entity: use enum type eEtypeOfEntity_t */
    eEenum08_t          signature;          /**< the signature of the entity. use enum values in eEprocess_t or eEsharlib_t or eEboard_t etc. */
    eEversion_t         version;            /**< the version of the entity w/ major + minor */
    eEdate_t            builddate;          /**< the build date of the entity */
} eEentity_t;           EECOMMON_VERIFYsizeof(eEentity_t, 8);


/** @typedef    typedef struct eEinfo_t
    @brief      Keeps extended information about an embedded entity: a board, a sw module, a shared library, etc. 
    @warning    this type must be of a given fixed size: 48B
  **/
typedef struct
{
    eEentity_t          entity;             /**< basic information about the entity */
    eEsysmemory_t       rom;                /**< the total size of rom which the entity has */
    eEsysmemory_t       ram;                /**< the total size of ram which the entity has */
    eEstorage_t         storage;            /**< the storage available to the entity */
    uint8_t             communication;      /**< the supported communication types: use enum eEcommunicationType_t in | combination */
    uint8_t             name[15];           /**< a string containing a descriptive name of the entity */
} eEinfo_t;             EECOMMON_VERIFYsizeof(eEinfo_t, 48);
        

/** @typedef    typedef struct eEinfoBoard_t
    @brief      eEinfoBoard_t keeps extended information about a HW board. 
    @warning    this type must be of a given fixed size: 64B
  **/
typedef struct                 
{
    eEinfo_t            info;               /**< the extended info for the generic module   */
    uint64_t            uniqueid;           /**< a unique id for the board                  */
    uint8_t             extra[8];           /**< extra space for other information          */
} eEboardInfo_t;        EECOMMON_VERIFYsizeof(eEboardInfo_t, 64);



/** @typedef    typedef struct eEinfoModule_t
    @brief      eEinfoModule_t keeps extended information about a software module (process or shared lib).
    @warning    this type must be of a given fixed size: 64B    
  **/
typedef struct                   
{
    eEinfo_t            info;               /**< the extended info for the generic module                   */
    eEprotocolInfo_t    protocols;          /**< information on communication capabilities of the sw module */
    uint8_t             extra[8];           /**< extra space for other information.          */
} eEmoduleInfo_t;       EECOMMON_VERIFYsizeof(eEmoduleInfo_t, 64);


typedef struct                   
{
    eEmoduleInfo_t      moduleinfo;                 /**< normal moduleinfo. fill field extra[] with "EXT" to tell that it is a eEmoduleExtendedInfo_t object */
    char                compilationdatetime[24];    // it must keep macro expansion __DATE__ " " __TIME__ which is in format: Dec 01 2001 16:32:59
    uint8_t             userdefined[40];
} eEmoduleExtendedInfo_t; EECOMMON_VERIFYsizeof(eEmoduleExtendedInfo_t, 128);

/** @typedef    typedef enum eEprocess_t
    @brief      eEprocess_t keep the allowed eProcesses in embENV.
    @warning    this type must be of a given fixed size: 1B    
 **/ 
typedef uint8_t eEprocess_t;


/** @typedef    typedef enum eEprocessvalues_t
    @brief      eEprocess_t keep identifiers of the allowed eProcesses in embENV. 
 **/ 
typedef enum               
{
    ee_procNone                 = 255,  /**< none */
    ee_procLoader               = 0,    /**< the eLoader: the one executed just after reset which loads any other eProcess */
    ee_procUpdater              = 1,    /**< the eUpdater: the one responsible for performing FW update */
    ee_procApplication          = 2,    /**< the eApplication: the standard application */
    ee_procOther01              = 3,
    ee_procOther02              = 4,
    ee_procOther03              = 5,
    ee_procOther04              = 6,
    ee_procOther05              = 7
} eEprocessvalues_t;

enum { ee_procMaxNum = 8 };


/** @typedef    typedef enum eEsharlib_t
    @brief      eEsharlib_t keeps identifier of the allowed shared libraries in embENV. 
 **/ 
typedef enum               
{
    ee_shalNone                 = 255,  /**< none */
    ee_shalSharServ             = 0,    /**< the shared services when used as a shared library */
    ee_shalOther01              = 1,
    ee_shalOther02              = 2,
    ee_shalOther03              = 3
} eEsharlib_t; 

enum { ee_shalMaxNum = 4 }; 



/** @typedef    typedef struct eEbasicPartable_t
    @brief      eEbasicPartable_t keeps information about partition table in simpler systems (dspic33, not ARM).
    @warning    this type must be of a given fixed size: 40B    
  **/
typedef struct          
{
    eEsysmemory_t       proc_loader;        /**< the loader and the updater process */
    eEsysmemory_t       proc_applic;        /**< the application process */
    eEstorage_t         strg_ro_shar_brd;   /**< the storage is read only, shared, and keeps board info: board info */
    eEstorage_t         strg_rw_shar_brd;   /**< the storage is read write, shared, and its content depends on boards: device info */
    eEstorage_t         strg_rw_priv_app;   /**< the storage is read write, private, and its content depends on application: applic info */
} eEbasicPartable_t;    EECOMMON_VERIFYsizeof(eEbasicPartable_t, 40);


// - declaration of extern public functions ---------------------------------------------------------------------------

extern inline const eEentity_t * ee_common_moduleinfo_to_entity(const eEmoduleInfo_t *mi)
{ 
    return((const eEentity_t*)&(mi->info.entity)); 
}

extern const char* ee_common_get_month_string(eEdate_t date);

extern eEresult_t ee_is_extendemoduleinfo_valid(eEmoduleExtendedInfo_t* extmodinfo);

extern eEresult_t ee_common_ipnetwork_clr(eEipnetwork_t* ntw, uint64_t uniqueid);
 

/** @}            
    end of group embenv 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



