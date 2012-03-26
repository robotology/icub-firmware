
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _SHARINFO_APPLIC_H_
#define _SHARINFO_APPLIC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       sharinfo_loader.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEsharedInfo.h"
#include "eEpermdata.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section




// - declaration of public user-defined types ------------------------------------------------------------------------- 


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern eEresult_t sharinfo_loader_init(const eEboardInfo_t *boardinfo, const eEmoduleInfo_t *loaderinfo)
    @brief      init the shared info for the loader program on the dspic33.
    @return     ee_res_OK or ee_res_NOK_generic upon failure
    @warning    hal_eeprom and hal-crc must be properly initted beforehand
 **/
extern eEresult_t sharinfo_applic_init(const eEmoduleInfo_t *applicinfo);



//extern eEresult_t sharinfo_applic_ipc_process(void);

extern eEresult_t sharinfo_applic_ipc_sendcommand(eEprocess_t from, eEprocess_t to, eEsharinfoIPCopc_t opc);

extern eEresult_t sharinfo_applic_canaddress_get(uint32_t *canaddr);

extern eEresult_t sharinfo_applic_canaddress_set(uint32_t canaddr);

extern eEresult_t sharinfo_applic_deviceinfo_get(const eEsharinfoDeviceInfo_t **devinfo);

extern eEresult_t sharinfo_applic_partiontable_get(const eEsharinfoPartable_t **ptab);

extern eEresult_t sharinfo_applic_boardinfo_get(const eEboardInfo_t **bardinfo);

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



