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
#ifndef _EOTHESTRAIN_H_
#define _EOTHESTRAIN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheSTRAIN.h
    @brief      this object implements what is required for managing the STRAIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheSTRAIN Object EOtheSTRAIN
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheSTRAIN_hid EOtheSTRAIN;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheSTRAIN* eo_strain_Initialise(void);

extern EOtheSTRAIN* eo_strain_GetHandle(void);

// it sets the strain tx mode according to the mode is kept in ram
extern eOresult_t eo_strain_SendTXmode(EOtheSTRAIN *p);

// it disables the tx mode ..
extern eOresult_t eo_strain_DisableTX(EOtheSTRAIN *p);

extern eOresult_t eo_strain_Config(EOtheSTRAIN *p, eOas_strain_config_t *cfg);

extern eOresult_t eo_strain_ConfigMode(EOtheSTRAIN *p, eOas_strainmode_t mode);

extern eOresult_t eo_strain_ConfigDataRate(EOtheSTRAIN *p, uint8_t datarate);

extern eOresult_t eo_strain_FullScale_StartRequest(EOtheSTRAIN *p);


/** @}            
    end of group eo_EOtheSTRAIN
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


