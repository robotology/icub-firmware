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
#ifndef _EOM_EMSAPPL_EBX_H_
#define _EOM_EMSAPPL_EBX_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eom_emsappl_ebx.h
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       05/21/2012
**/

/** @defgroup sdew12345fefc cedcew fcevw
    vcrvrev frv ef e rfc d
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "eom_emsappl_ebx_usethis.h" 

// - public #define  --------------------------------------------------------------------------------------------------

// some properties of the application: hasSKIN, hasMC4, has2FOC
#if     defined(USE_EB2) || defined(USE_EB4)
    #define EMSAPPL_hasSKIN
    #define EMSAPPL_hasMC4
    #undef  EMSAPPL_has2FOC
#elif   defined(USE_EB1) || defined(USE_EB3) || defined(USE_EB5) || defined(USE_EB6) || defined(USE_EB7) || defined(USE_EB8) || defined(USE_EB9)
    #undef  EMSAPPL_hasSKIN
    #undef  EMSAPPL_hasMC4
    #define EMSAPPL_has2FOC
#endif 


// the name of the application
#if     defined(USE_EB1)
    #define EMSAPPL_NAME "EOMemsApplEB1"
#elif   defined(USE_EB2)
    #define EMSAPPL_NAME "EOMemsApplEB2"
#elif   defined(USE_EB3)
    #define EMSAPPL_NAME "EOMemsApplEB3"
#elif   defined(USE_EB4)
    #define EMSAPPL_NAME "EOMemsApplEB4"
#elif   defined(USE_EB5)
    #define EMSAPPL_NAME "EOMemsApplEB5"
#elif   defined(USE_EB6)
    #define EMSAPPL_NAME "EOMemsApplEB6"
#elif   defined(USE_EB7)
    #define EMSAPPL_NAME "EOMemsApplEB7"    
#elif   defined(USE_EB8)
    #define EMSAPPL_NAME "EOMemsApplEB8"
#elif   defined(USE_EB9)
    #define EMSAPPL_NAME "EOMemsApplEB9"
#endif 



// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------




// - declaration of extern public functions ---------------------------------------------------------------------------



/** @}            
    end of group sdew12345fefc 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


