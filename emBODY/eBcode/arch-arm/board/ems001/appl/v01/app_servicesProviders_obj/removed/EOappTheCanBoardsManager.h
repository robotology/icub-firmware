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
#ifndef _EOAPPTHECANBOARDSMANAGER_H_
#define _EOAPPTHECANBOARDSMANAGER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappCanBoardsManager.h
    @brief     This file provides interface to encodersReader.
    @author    valentina.gaggero@iit.it
    @date       02/17/2012
**/
/** @defgroup appTheCanBrdsMng Object EOappTheCanBrdsMng
    .........     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "eOcommon.h"
#include "eOBoards.h"
#include "EoManagement.h"

#include "EOappCanServicesProvider.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappTheCanBrdsMng_hid  EOappTheCanBrdsMng;


typedef struct
{
    EOappCanSP              *appCanSP_ptr;  /**< application CAN services provider*/
} eOappTheCanBrdsMng_cfg_t;

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOappTheCanBrdsMng* eo_appTheCanBrdsMng_Initialise(eOappTheCanBrdsMng_cfg_t *cfg);

extern EOappTheCanBrdsMng* eo_appTheCanBrdsMng_GetHandle(void);

extern eOresult_t eo_appTheCanBrdsMng_ConfigAllBoards(EOappTheCanBrdsMng *p);

// extern eOresult_t eo_appTheCanBrdsMng_StartAllBoards(EOappTheCanBrdsMng *p);

// extern eOresult_t eo_appTheCanBrdsMng_StopAllBoards(EOappTheCanBrdsMng *p);

extern eOresult_t eo_appTheCanBrdsMng_GetAppRunMode(EOappTheCanBrdsMng *p, eOmn_appl_runMode_t *apprunmode);


/** @}            
    end of group eo_app_encodersReader
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


