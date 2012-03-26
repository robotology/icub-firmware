
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EESHAREDSTORAGE_H_
#define _EESHAREDSTORAGE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEsharedStorage.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEsharedInfo.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section




// - declaration of public user-defined types ------------------------------------------------------------------------- 


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eEresult_t ee_sharedstorage_init(eEprocess_t eproc, const eEboardInfo_t *boardinfo, const eEmoduleInfo_t *loaderinfo, const uint32_t defaultcanaddr)
    @brief      init the storage manager for the a given process (loader or application) on the dspic33.
    @return     ee_res_OK or ee_res_NOK_generic upon failure
    @warning    hal_eeprom and hal-crc must be properly initted beforehand
 **/
extern eEresult_t ee_sharedstorage_init(eEprocess_t eproc, const eEmoduleInfo_t *eprocinfo, const eEboardInfo_t *boardinfo, const uint32_t defaultcanaddr);

extern eEresult_t ee_sharedstorage_partiontable_get(const eEsharinfoPartable_t **ptab); // cannot set

extern eEresult_t ee_sharedstorage_boardinfo_get(const eEboardInfo_t **boardinfo); // cannot set.

extern eEresult_t ee_sharedstorage_deviceinfo_get(const eEsharinfoDeviceInfo_t **devinfo); 

extern eEresult_t ee_sharedstorage_deviceinfo_canaddress_get(uint32_t *canaddr);

extern eEresult_t ee_sharedstorage_deviceinfo_canaddress_set(uint32_t canaddr);


// used just for test ......
extern eEresult_t ee_sharedstorage_ipc_debug_sendcommand_eraseflash(eEprocess_t from, eEprocess_t to, uint32_t addr, uint32_t size);

/** @fn         extern eEresult_t sharinfo_loader_ipc_process(void)
    @brief      process an ipc command.
    @return     ee_res_OK or ee_res_NOK_generic upon failure
 **/
extern eEresult_t ee_sharedstorage_ipc_loader_process(void);




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



