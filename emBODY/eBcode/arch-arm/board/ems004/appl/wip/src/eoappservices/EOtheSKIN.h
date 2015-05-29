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
#ifndef _EOTHESKIN_H_
#define _EOTHESKIN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheSKIN.h
    @brief      this object implements what is required for managing the SKIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheSKIN Object EOtheSKIN
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheSKIN_hid EOtheSKIN;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheSKIN* eo_skin_Initialise(void);

extern EOtheSKIN* eo_skin_GetHandle(void);

// it disables the tx mode ..
extern eOresult_t eo_skin_DisableTX(EOtheSKIN *p);


extern eOresult_t eo_skin_ConfigTXMode(EOtheSKIN *p, uint8_t skinindex, eOsk_sigmode_t mode);

extern eOresult_t eo_skin_ConfigBoards(EOtheSKIN *p, uint8_t skinindex, eOsk_cmd_boardsCfg_t *brdcfg);

extern eOresult_t eo_skin_ConfigTriangles(EOtheSKIN *p, uint8_t skinindex, eOsk_cmd_trianglesCfg_t *trgcfg);

extern eOresult_t eo_skin_ProcessRXframe(EOtheSKIN *p, eOcanframe_t *frame, eOcanport_t port);




/** @}            
    end of group eo_EOtheSKIN
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


