/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOTHESKIN_HID_H_
#define _EOTHESKIN_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EOtheServices_hid.h"
#include "EOvector.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheSKIN.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheSKIN_hid
{
    eOservice_core_t                        service;
    eOservice_diagnostics_t                 diagnostics;
    eOservice_cantools_t                    sharedcan;
    
    eObool_t                                patchisrunning[eomn_serv_skin_maxpatches];
    uint8_t                                 numofskinpatches;    
    uint8_t                                 numofmtbs;       
    
    EOvector*                               rxdata[eomn_serv_skin_maxpatches]; // of eOsk_candata_t     
    eOsk_skin_t*                            skinpatches[eomn_serv_skin_maxpatches];
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



