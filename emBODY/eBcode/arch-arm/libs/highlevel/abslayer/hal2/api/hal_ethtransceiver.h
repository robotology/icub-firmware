/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_ETHTRANSCEIVER_H_
#define _HAL_ETHTRANSCEIVER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_ethtransceiver.h
    @brief      This header file implements public interface to the hal ethtransceiver module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/18/2011
**/

/** @defgroup arm_hal_ethtransceiver HAL ETHTRANSCEIVER

    The HAL ETHTRANSCEIVER ....
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_ethtransceiver_t 
    @brief      contains the id of the can transceiver
 **/
typedef enum  
{ 
    hal_ethtransceiver1 = 0            /**< for hal_eth1        */
} hal_ethtransceiver_t; 

enum { hal_ethtransceivers_number = 1 };


/** @typedef    typedef enum hal_ethtransceiver_phy_t 
    @brief      contains the id of the eth phy transceiver
 **/
typedef enum  
{ 
    hal_ethtransceiver_phy0 = 0,            /**<  */
    hal_ethtransceiver_phy1 = 1,            /**<  */
    hal_ethtransceiver_phy2 = 2,            /**<  */
} hal_ethtransceiver_phy_t; 

enum { hal_ethtransceiver_phys_number = 3 };



/** @typedef    typedef struct hal_ethtransceiver_cfg_t;
    @brief      contains configuration data of ethtransceiver.
 **/
typedef struct
{
    uint8_t dummy;          /**< dummy...     */
} hal_ethtransceiver_cfg_t;


typedef enum
{
    hal_ethtransceiver_phymode_auto                 = 0,
    hal_ethtransceiver_phymode_halfduplex10mbps     = 1,
    hal_ethtransceiver_phymode_halfduplex100mbps    = 2,
    hal_ethtransceiver_phymode_fullduplex10mbps     = 3,
    hal_ethtransceiver_phymode_fullduplex100mbps    = 4,
    hal_ethtransceiver_phymode_none                 = 255
} hal_ethtransceiver_phymode_t;


// added for diagnostics support for ethernet
typedef struct
{
    uint32_t linkisup                       :1;     // not used
    uint32_t autoNeg_done                   :1;     // 1 -> done,   0 -> not done
    uint32_t linkisgood                     :1;     // 1 -> good,   0 -> bad
    uint32_t linkspeed                      :1;     // 1 -> 100Mb,  0 -> 10Mb
    uint32_t linkduplex                     :1;     // 1 -> full,   0 -> half

    uint32_t mdixisused                     :1;     // 1 -> MDI-X, 0 -> MDI
    uint32_t autonegotiationdone            :1;     // 1 -> Auto-negotiation completed
    uint32_t linkgood                       :1;     // 1 -> Link good, 0 -> Link not good    
    uint32_t partnerflowcontrolcapable      :1;     // 1 -> Link partner flow control (pause) capable
    uint32_t partner100FDcapable            :1;     // 1 -> Link partner 100BT full duplex capable
    uint32_t partner100HDcapable            :1;     // 1 -> Link partner 100BT half duplex capable
    uint32_t partner010FDcapable            :1;     // 1 -> Link partner 10BT full duplex capable
    uint32_t partner010HDcapable            :1;     // 1 -> Link partner 10BT half duplex capable
    
    uint32_t mdixHPisused                   :1;     // 1 -> HP Auto MDI/MDI-X mode, 0 -> Micrel Auto MDI/MDI-X mode
    uint32_t polarityreversed               :1;     // 1 -> Polarity is reversed    
    uint32_t transmitflowcontrolactive      :1;     // 1 -> Transmit flow control feature is active
    uint32_t receiveflowcontrolactive       :1;     // 1 -> Receive flow control feature is active
    uint32_t operationspeed100mbps          :1;     // 1 -> Link speed is , 0 -> Link speed is 10Mbps
    uint32_t operationduplexFULL            :1;     // 1 -> Link duplex is full, 0 -> Link duplex is half
   
    uint32_t dummy                          :13;
} hal_ethtransceiver_phystatus_t;


typedef enum
{
    hal_ethtransceiver_phyerror_rxCrc           = 0x07,
    hal_ethtransceiver_phyerror_rxUnicast       = 0x0D,
    hal_ethtransceiver_phyerror_rx64Octets      = 0x0E,
    hal_ethtransceiver_phyerror_txUnicast       = 0x1A
} hal_ethtransceiver_phyerror_t;

typedef struct
{
    uint32_t value;
    uint32_t counteroverflow    :1;
    uint32_t validvalue         :1;
    uint32_t dummy              :30;
} hal_ethtransceiver_phyerrorinfo_t;



 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_ethtransceiver_cfg_t hal_ethtransceiver_cfg_default;   // = { .dummy = 0};


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hal_result_t hal_ethtransceiver_init(const hal_ethtransceiver_cfg_t *cfg)
    @brief  	This function initializes the ethtransceiver attached to the MPU
    @param  	cfg 	        The configuration of the ethtransceiver. It can be NULL.
    @return 	hal_res_NOK_generic in case the ethtransceiver cannot be configured, else hal_res_OK
    @warning    the transceiver can be initted only once. The second call of hal_ethtransceiver_init(cfg) will do nothing.
                The return value will be hal_res_OK if the parameter cfg is the same as one used the first time,
                otherwise it will be hal_res_NOK_generic to express the fact that the device is not initted as wanted.
  */
extern hal_result_t hal_ethtransceiver_init(const hal_ethtransceiver_cfg_t *cfg);


extern hal_result_t hal_ethtransceiver_start(hal_ethtransceiver_phymode_t *usedmiiphymode);


/** @fn			extern hal_bool_t hal_ethtransceiver_initted_is(void)
    @brief  	This function tells if the ethtransceiver is already initialised.
    @return 	hal_true or hal_false.
  */
extern hal_bool_t hal_ethtransceiver_initted_is(void);



extern hal_result_t hal_ethtransceiver_phy_numberof(uint8_t *num);


/** @fn         extern hal_result_t hal_ethtransceiver_phy_linkupmask(uint8_t *mask)
    @brief      checks if physical links are up. In output mask indicates each link status: 
                if bit in pos x values 1 then phy num x is up, else is down. phy num starts to count from 0.
    @return     if mask is NULL it returns hal_res_NOK_nullpointer else hal_res_OK. If the feature is
                not supported it returns hal_res_NOK_generic.
 **/
extern hal_result_t hal_ethtransceiver_phy_linkupmask(uint8_t *mask);



extern hal_result_t hal_ethtransceiver_phy_status(hal_ethtransceiver_phystatus_t *array, uint8_t sizeofarray);



/** @fn         extern hal_result_t hal_ethtransceiver_phy_errorinfo(uint8_t phynum, hal_ethtransceiver_phyerror_t error, hal_ethtransceiver_phyerrorinfo_t *result)
    @brief      in output @result contains info about error of type @errortype on physical link @phynum
    @return     hal_res_OK or hal_res_NOK_...
 **/
extern hal_result_t hal_ethtransceiver_phy_errorinfo(uint8_t phynum, hal_ethtransceiver_phyerror_t error, hal_ethtransceiver_phyerrorinfo_t *result);

/** @}            
    end of group arm_hal_ethtransceiver  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



