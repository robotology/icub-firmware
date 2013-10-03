/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _IPAL_IPIIT_STORAGE_H_
#define _IPAL_IPIIT_STORAGE_H_


/** @file       ipal_ipiit_storage.h
	@brief      This header file implements public interface to the ipiit storage 
	@author     valentina.gaggero@iit.it
    @date       01/10/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "ipal.h"



// - public #define  --------------------------------------------------------------------------------------------------


// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


extern ipal_result_t ipal_ipiit_storage_arp_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_dhcp_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_eth_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_ftp_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_igmp_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_sys_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_tcp_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_telnet_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_tftp_init(const ipal_cfg_t *cfg);

extern ipal_result_t ipal_ipiit_storage_udp_init(const ipal_cfg_t *cfg);

 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

