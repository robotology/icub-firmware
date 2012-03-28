/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _IPAL_FTP_HID_H_
#define _IPAL_FTP_HID_H_


/* @file       ipal_ftp_hid.h
	@brief      This file implements hidden interface of the ipal ftp module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "ipal.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------

#define ATTR_READ_ONLY      0x01
#define ATTR_HIDDEN         0x02
#define ATTR_SYSTEM         0x04
#define ATTR_VOLUME_ID      0x08
#define ATTR_DIRECTORY      0x10
#define ATTR_ARCHIVE        0x20
#define ATTR_LONG_NAME      (ATTR_READ_ONLY | ATTR_HIDDEN   | \
                             ATTR_SYSTEM    | ATTR_VOLUME_ID)



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @typedef    typedef struct ipal_ftp_info_search_t 
    @brief      ipal_ftp_info_search_t is used internally to ipal_ftp to cast parameter @e infosearch of
                function @e ffind_fn() of struct @ ipal_ftp_cfg_t. The user should use an equally
                organised type (such as the one from fsal).
 **/
typedef struct 
{                        
    uint8_t         name[256];                        /* Name                              */
    uint32_t        size;                             /* File size in bytes                */
    uint16_t        fileid;                           /* System Identification             */
    uint8_t         attrib;                           /* Attributes                        */
    struct tm       date;                             /* Create/Modify Time                */
} ipal_ftp_info_search_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern uint8_t ipal_ftp_hid_active; // = 0;

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t ipal_ftp_hid_getsize(const ipal_cfg_t *cfg);
extern ipal_result_t ipal_ftp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory);
extern ipal_result_t ipal_ftp_hid_init(ipal_cfg_t *cfg);
extern ipal_result_t ipal_ftp_hid_vars_init(const ipal_cfg_t *cfg);




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


