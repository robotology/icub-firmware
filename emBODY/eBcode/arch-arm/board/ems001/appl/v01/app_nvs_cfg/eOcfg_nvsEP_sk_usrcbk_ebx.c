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

/* @file       eOcfg_nvsEP_sk_usrcbk_ebx.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mc
    @author     valentina.gaggero@iit.it
    @date       05/04/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "hal.h"

#include "EoCommon.h"
#include "EOarray.h"
#include "EOnv_hid.h"

#include "EOSkin.h"
#include "eOcfg_nvsEP_sk.h"

//application
#include "EOtheEMSapplBody.h"
#include "EOicubCanProto_specifications.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mngmnt_usr_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//sk-init


//sk-update
extern void eo_cfg_nvsEP_sk_hid_UPDT_sconfig__sigmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    uint8_t                         i;
    eOappTheDB_SkinCanLocation_t    canLoc;
    eOsk_skinId_t                   skId = 0; //for DB the skin is unique entity
    eOsk_sigmode_t                  *sigmode = (eOsk_sigmode_t*)nv->loc;
    EOappCanSP                      *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    /* NOTE: corrently any skin board starts to send can frame when it finishes its initilisation
            and pc104 configures only signal mode and not skin boards. (how many triangulars, resolution bits, etc).
            so here i don't send commnad to start to skin boards (because they start automatically),
            but i sent default configurations
    */
    if(eosk_sigmode_signal == *sigmode)
    {
        eOicubCanProto_msgCommand_t msgCmd = 
        {
            EO_INIT(.class) 4,
            EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__TACT_SETUP
        };

//        hal_led_on(hal_led2);
        
        res = eo_appTheDB_GetSkinCanLocation(eo_appTheDB_GetHandle(), skId, &canLoc);
        if(eores_OK != res)
        {
            return;
        }
        for(i=8; i<15; i++)
        {
            eOicubCanProto_msgDestination_t msgdest;
            msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
            res = eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
            if(eores_OK != res)
            {
                return;
            }
        }

    }// end if
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



