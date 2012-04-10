
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOSKIN_H_
#define _EOSKIN_H_




/** @file       EoSkin.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_cevcwervcrev2345 Configuation of the vrfverver
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMeasures.h"
#include "EoUtilities.h"
#include "EOarray.h"

// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef struct              // size is 4+10*16+0 = 164
{
    eOarray_head_t          head;
    uint8_t                 data[10*sizeof(eOutil_canframe_t)];
} EOarray_of_10canframes;   EO_VERIFYsizeof(EOarray_of_10canframes, 164);


/** @typedef    typedef struct eOskin_array_data_t
    @brief      eOskin_array_data_t contains up to 10 can frames as received by hal placed in an EOarray  object
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 164+0 = 164 
{
    EOarray_of_10canframes  array;
} eOskin_array_data_t;      EO_VERIFYsizeof(eOskin_array_data_t, 164);




    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group eo_cevcwervcrev2345  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

