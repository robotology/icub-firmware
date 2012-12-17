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
#ifndef _EOAPCANSERVICESPROVIDER_HID_H_
#define _EOAPCANSERVICESPROVIDER_HID_H_


/*  @file       EOappCanServicesProvider_hid.h
    @brief      this file implemnts the hidden interfacto to EOappCanServicesProvider obj.
    @author     valentina.gaggero@iit.it
    @date       02/14/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "osal.h"
#include "hal.h"

#include "EoCommon.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOappCanServicesProvider.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eOboolvalues_t      waitenable;
    uint8_t             numoftxframe2send;
    osal_semaphore_t*   semaphore;
} eOappCanSP_waitTxIsDone_datastruct_t;



/** @struct     EOconstvector_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOappCanSP_hid 
{
    eOappCanSP_cfg_t                        cfg;
    EOicubCanProto                          *icubCanProto_ptr;
    eo_appCanSP_runMode_t                   runmode;
    eOappCanSP_waitTxIsDone_datastruct_t    waittxdata[hal_can_ports_num];
};

// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



