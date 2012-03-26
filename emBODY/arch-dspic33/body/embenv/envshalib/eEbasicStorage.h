
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EEBASICSTORAGE_H_
#define _EEBASICSTORAGE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEbasicStorage.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section




// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
        
    ee_basicstrg_res_ok           = 0,        /**< as the ee_res_OK */
    ee_basicstrg_res_ok_generic   = -1        /**< as the ee_res_NOK_generic */
} eEbasicstrg_res_t;

// - declaration of extern public functions ---------------------------------------------------------------------------



extern eEbasicstrg_res_t ee_basicstrg_init(void);

extern eEbasicstrg_res_t ee_basicstrg_canaddress_get(uint32_t *canaddr);


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



