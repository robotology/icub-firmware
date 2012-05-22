/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _EOUTILITIES_H_
#define _EOUTILITIES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoUtilities.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_c2e2rd3f3egfvr45 Configuation of  vfef ve e
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"



// - public #define  --------------------------------------------------------------------------------------------------

//#define EOUTIL_CHAMELEON_DESCRIPTOR_NONE    0xFF
#define EOUTIL_CHAMELEON_DESCRIPTOR_SIZE_NONE    3
#define EOUTIL_CHAMELEON_DESCRIPTOR_IDEN_NONE    0


// - declaration of public user-defined types ------------------------------------------------------------------------- 


//typedef uint8_t eOutil_chameleon_id_t;      // invalid chameleon is 0xff
typedef struct
{
    uint8_t     size        : 2;            /**< 0 for 1 byte, 1 for 2 bytes, 2 for 4 bytes, 3 for none. */
    uint8_t     identifier  : 6;            /**< from 0 to 63 */
} eOutil_chameleon_descr_t;                 EO_VERIFYsizeof(eOutil_chameleon_descr_t, 1);  


/** @typedef    typedef struct eOutil_debug_values_t
    @brief      eOutil_debug_values_t contains some debug data whcih can be configured by the user 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 1+1+2+4+0 = 8
{
    uint8_t                     debug08var00;
    uint8_t                     debug08var01;
    uint16_t                    debug16var00;
    uint32_t                    debug32var00;     
} eOutil_debug_values_t;        


/** @typedef    typedef struct eOutil_2chameleons32_t
    @brief      eOutil_2chameleons32_t contains up to two chameleons of 32 bits each. 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct              // size is 12 bytes
{
    uint8_t                 numberof;           /**< tells how many chameleons are used */ 
    uint8_t                 identifier[2];      /**< the identifiers of each chameleon */
    uint8_t                 filler01[1];        /**< a filler */
    int32_t                 chameleon[2];       /**< the two chameleons */
} eOutil_2chameleons32_t;   EO_VERIFYsizeof(eOutil_2chameleons32_t, 12);


/** @typedef    typedef struct eOutil_3chameleons32_t
    @brief      eOutil_3chameleons32_t contains up to three chameleons of 32 bits each. 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct              // size is 16 bytes
{
    uint8_t                 numberof;           /**< tells how many chameleons are used */ 
    uint8_t                 identifier[3];      /**< the identifiers of each chameleon */
    int32_t                 chameleon[3];       /**< the three chameleons */
} eOutil_3chameleons32_t;   EO_VERIFYsizeof(eOutil_3chameleons32_t, 16);


/** @typedef    typedef struct eOutil_canframe_t
    @brief      eOutil_canframe_t contains a can frame as defined in hal_can.h. 
 **/
typedef struct              // size is 16 bytes
{
    uint32_t                id;             /**< can frame id    */
    uint8_t                 id_type;        /**< can frame id format */
    uint8_t                 frame_type;     /**< frame type */
    uint8_t                 size;           /**< data size */
    uint8_t                 unused;         /**< filler */
    uint8_t                 data[8];        /**< the data (payload) */    
} eOutil_canframe_t;        EO_VERIFYsizeof(eOutil_canframe_t, 16);


/** @typedef    typedef enum  eOutil_canport_t
    @brief      eOutil_canframe_t contains a can port as defined in hal_can.h. 
 **/
typedef enum              
{
    eOutil_canport1   = 0,              /**< CAN1        */
    eOutil_canport2    = 1              /**< CAN2        */
} eOutil_canport_t;    

 enum { eOutil_canports_num = 2 };  
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_c2e2rd3f3egfvr45  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
