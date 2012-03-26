
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EESHAREDINFO_H_
#define _EESHAREDINFO_H_


/** @file       eEsharedInfo.h
    @brief      This header file implements public interface to the shared info in the micro.
    @author     marco.accame@iit.it
    @date       12/12/2011
**/

/** @defgroup ee_shared_info Library shared INFO
    The shared INFO library allows ...... 
 
    @todo acemor-facenda: do documentation.
    
    @{        
 **/

 
// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "eEcommon.h"
#include "eEpermdata.h"


// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    hal_eeprom_t            haleeprom;
    eEcheckmode_t           checkmode;
    hal_crc_t               halcrc;
} eEsharinfo_cfg_t;


// used as a simple partition table when the shalPART is not used.
typedef struct              // 7*8+2*8 = 72 BYTES
{
    eEstorage_t             strg_shro_procinfo_loader;  /**< storage which keeps the procinfo of the loader */
    eEstorage_t             strg_shro_boardinfo;        /**< storage which keeps the info about the board */
    eEstorage_t             strg_shro_partable;         /**< storage which keeps the partition table */
    eEstorage_t             strg_shrw_ipcdata;          /**< storage which keeps the inter-process communication data */
    eEstorage_t             strg_shrw_procinfo_applic;  /**< storage which keeps the procinfo of the application */
    eEstorage_t             strg_shrw_deviceinfo;       /**< storage which keeps the info about the device (and shared by the loader and application) */
    eEstorage_t             strg_priv_application;      /**< storage used only by the application */
    eEsysmemory_t           proc_loader;                /**< mapping of the loader process */
    eEsysmemory_t           proc_application;           /**< mapping of the application process */    
} eEsharinfoPartable_t;     EECOMMON_VERIFYsizeof(eEsharinfoPartable_t, 72);

typedef enum
{
    devinfo_can1    = 0,
    devinfo_page04  = 1,
    devinfo_page08  = 2,
    devinfo_page16  = 3,
    devinfo_page32  = 4   
} eEsharinfoDeviceInfoItem_t;


typedef struct              // 64 BYTES              
{
    eEcannetwork_t          can1network;    /*004B*/
    uint8_t                 page04[4];      /*004B*/
    uint8_t                 page08[8];      /*008B*/
    uint8_t                 page16[16];     /*016B*/
    uint8_t                 page32[32];     /*032B*/
} eEsharinfoDeviceInfo_t;   EECOMMON_VERIFYsizeof(eEsharinfoDeviceInfo_t, 64);


typedef struct              // 4 bytes
{
    uint8_t                 size;
    eEprocess_t             from;
    eEprocess_t             to;
    uint8_t                 opc;        // i.e., eraseflash sent to the eloader from the eapplication
} eEsharinfoIPChead_t;      EECOMMON_VERIFYsizeof(eEsharinfoIPChead_t, 4);

typedef struct              // 16 BYTES
{
    eEsharinfoIPChead_t     head;        
    uint8_t                 pars[12];         // used as a cast to the
} eEsharinfoIPCdata_t;      EECOMMON_VERIFYsizeof(eEsharinfoIPCdata_t, 16);

typedef enum
{
    ipcopc_erase_applic             = 1,     /**< the receiver must erase the whole flash reserved to application */
    ipcopc_erase_sharinfo           = 2,     /**< the receiver must erase the whole storage part (eeprom) reserved to shared info */
    ipcopc_erase_flash              = 3      /**< the receiver must erase the specified continuous portion of flash */
} eEsharinfoIPCopc_t;

typedef struct              // 04 BYTES
{
    eEsharinfoIPChead_t     head;        
} eEsharinfoIPCerase_applic_t;      EECOMMON_VERIFYsizeof(eEsharinfoIPCerase_applic_t, 4);

typedef struct              // 04 BYTES
{
    eEsharinfoIPChead_t     head;        
} eEsharinfoIPCerase_sharinfo_t;    EECOMMON_VERIFYsizeof(eEsharinfoIPCerase_sharinfo_t, 4);    

typedef struct              // 12 BYTES
{
    eEsharinfoIPChead_t     head;
    uint32_t                addr;
    uint32_t                size;        
} eEsharinfoIPCerase_flash_t;       EECOMMON_VERIFYsizeof(eEsharinfoIPCerase_flash_t, 12);


// for instance: the eloader processes: delete_appl and delete_appl_keeping_its_strg



// - declaration of extern public functions ---------------------------------------------------------------------------

extern eEresult_t ee_sharinfo_init(const eEsharinfo_cfg_t *cfg);

extern eEbool_t ee_sharinfo_overlapped_is(const eEstorage_t *stg);

extern eEresult_t ee_sharinfo_erase(void);

extern eEresult_t ee_sharinfo_partable_sync(void);
extern eEresult_t ee_sharinfo_partable_get(const eEsharinfoPartable_t **ptab);

extern eEresult_t ee_sharinfo_boardinfo_sync(const eEboardInfo_t *brdinfo);
extern eEresult_t ee_sharinfo_boardinfo_get(const eEboardInfo_t **brdinfo);

extern eEresult_t ee_sharinfo_deviceinfo_clr(void);
extern eEresult_t ee_sharinfo_deviceinfo_set(const eEsharinfoDeviceInfo_t *devinfo);
extern eEresult_t ee_sharinfo_deviceinfo_get(const eEsharinfoDeviceInfo_t **devinfo);
extern eEresult_t ee_sharinfo_deviceinfo_item_set(const void *data, eEsharinfoDeviceInfoItem_t item);
extern eEresult_t ee_sharinfo_deviceinfo_item_get(const void **data, eEsharinfoDeviceInfoItem_t item );

extern eEresult_t ee_sharinfo_procinfo_clr(eEprocess_t proc);
extern eEresult_t ee_sharinfo_procinfo_sync(const eEmoduleInfo_t *info, eEprocess_t proc);
extern eEresult_t ee_sharinfo_procinfo_get(const eEmoduleInfo_t **info, eEprocess_t proc);

extern eEresult_t ee_sharinfo_ipcdata_clr(void);
extern eEresult_t ee_sharinfo_ipcdata_set(const eEsharinfoIPCdata_t* ipcdata);
extern eEresult_t ee_sharinfo_ipcdata_get(const eEsharinfoIPCdata_t** ipcdata);


/** @}            
    end of group ee_shared_info 
 **/
 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



