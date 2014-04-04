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

#ifndef _HL_ETHTRANS_H_
#define _HL_ETHTRANS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_ethtrans.h
    @brief      This header file implements public interface to the hl ethtrans module.
    @author     marco.accame@iit.it
    @date       11/11/2013
**/

/** @defgroup arm_hl_ethtrans HL ETHTRANS

    The HL ETHTRANSCEIVER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_eth.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    hl_bool_t           supported;      /**< hl_true if supported */
} hl_ethtrans_mapping_t;


/** @typedef    typedef struct hl_ethtrans_cfg_t;
    @brief      contains configuration data of ethtrans.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hl_ethtrans_cfg_t;


typedef enum
{
    hl_ethtrans_phymode_auto                = 0,
    hl_ethtrans_phymode_halfduplex10mbps    = 1,
    hl_ethtrans_phymode_halfduplex100mbps   = 2,
    hl_ethtrans_phymode_fullduplex10mbps    = 3,
    hl_ethtrans_phymode_fullduplex100mbps   = 4,
    hl_ethtrans_phymode_none                = 255
} hl_ethtrans_phymode_t;


typedef struct
{
    uint32_t linkisup           :1;
    uint32_t autoNeg_done       :1;
    uint32_t linkisgood         :1;
    uint32_t linkspeed          :1;     // 1 -> 100Mb, 0 -> 10Mb
    uint32_t linkduplex         :1;     // 1 -> full,  0 -> half
    uint32_t dummy              :27;
} hl_ethtrans_phystatus_t;

typedef enum
{
    hl_ethtrans_phyerror_rxCrc          = 0x07,
    hl_ethtrans_phyerror_rxUnicast      = 0x0D,
    hl_ethtrans_phyerror_rx64Octets     = 0x0E,
    hl_ethtrans_phyerror_txUnicast      = 0x1A
} hl_ethtrans_phyerror_t;

typedef struct
{
    uint32_t value;
    uint32_t counteroverflow    :1;
    uint32_t validvalue         :1;
    uint32_t dummy              :30;
} hl_ethtrans_phyerrorinfo_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_ethtrans_cfg_t hl_ethtrans_cfg_default;   // = { .dummy = 0};

// it must be externally declared.
extern const hl_ethtrans_mapping_t* hl_ethtrans_map;


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hl_result_t hl_ethtrans_init(const hl_ethtrans_cfg_t *cfg)
    @brief  	This function initializes the ethtrans attached to the MPU
    @param  	cfg 	        The configuration of the ethtrans. It can be NULL.
    @return 	hl_res_NOK_generic in case the ethtrans cannot be configured, else hl_res_OK
    @warning    The initialisation may temporarily stop the ethtrans, thus multiple calls to this function
                should be avoided as they can corrupt network traffic.
  */
extern hl_result_t hl_ethtrans_init(const hl_ethtrans_cfg_t *cfg);


// usedmiiphymode can be NULL
extern hl_result_t hl_ethtrans_start(hl_ethtrans_phymode_t *usedmiiphymode);

extern hl_result_t hl_ethtrans_getmiiphymode(hl_ethtrans_phymode_t *usedphymode);

/** @fn			extern hl_bool_t hl_ethtrans_initted_is(void)
    @brief  	This function tells if the ethtrans is already initialised.
    @return 	hl_true or hl_false.
  */
extern hl_bool_t hl_ethtrans_initted_is(void);


// must be defined externally
extern void* hl_ethtrans_chip_init_param;

// weakly defined.
extern hl_result_t hl_ethtrans_chip_init(void* param);
extern hl_result_t hl_ethtrans_chip_start(hl_ethtrans_phymode_t *usedmiiphymode);
extern hl_result_t hl_ethtrans_chip_getmiiphymode(hl_ethtrans_phymode_t *usedmiiphymode);

/** @}            
    end of group arm_hl_ethtrans  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



