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

/* @file       eOcfg_nvsEP_mc_any_usr_cxxmacro.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       04/06/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoCommon.h"    
#include "EOnv_hid.h"

      




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// use 
// CXXMACRO_INIT        my_INIT
// CXXMACRO_UPDT        my_UPDT
// CXXMACRO_PART        _leftleg etc
// CXXMACRO_BOARD       _ebx etc
// CXXMACRO_MSTR        _c00 etc
// CXXMACRO_MNUM        0 etc

#define NVMACRO_FUN_INIT    CXXMACRO_INIT
#define NVMACRO_FUN_UPDT    CXXMACRO_UPDT

#define NVMACRO_PSTR        CXXMACRO_PART
#define NVMACRO_BSTR        CXXMACRO_BOARD
#define NVMACRO_NVSTR1      CXXMACRO_CSTR
#define NVMACRO_NVNUM1      CXXMACRO_CNUM
#define NVMACRO_NVSTR2      _cconfig

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"




#define NVMACRO_FUN_INIT    CXXMACRO_INIT
#define NVMACRO_FUN_UPDT    CXXMACRO_UPDT

#define NVMACRO_PSTR        CXXMACRO_PART
#define NVMACRO_BSTR        CXXMACRO_BOARD
#define NVMACRO_NVSTR1      CXXMACRO_CSTR
#define NVMACRO_NVNUM1      CXXMACRO_CNUM
#define NVMACRO_NVSTR2      _cconfig__durationofctrlloop

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    CXXMACRO_INIT
#define NVMACRO_FUN_UPDT    CXXMACRO_UPDT

#define NVMACRO_PSTR        CXXMACRO_PART
#define NVMACRO_BSTR        CXXMACRO_BOARD
#define NVMACRO_NVSTR1      CXXMACRO_CSTR
#define NVMACRO_NVNUM1      CXXMACRO_CNUM
#define NVMACRO_NVSTR2      _cstatus

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    CXXMACRO_INIT
#define NVMACRO_FUN_UPDT    CXXMACRO_UPDT

#define NVMACRO_PSTR        CXXMACRO_PART
#define NVMACRO_BSTR        CXXMACRO_BOARD
#define NVMACRO_NVSTR1      CXXMACRO_CSTR
#define NVMACRO_NVNUM1      CXXMACRO_CNUM
#define NVMACRO_NVSTR2      _cstatus__alljomoinitted

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    CXXMACRO_INIT
#define NVMACRO_FUN_UPDT    CXXMACRO_UPDT

#define NVMACRO_PSTR        CXXMACRO_PART
#define NVMACRO_BSTR        CXXMACRO_BOARD
#define NVMACRO_NVSTR1      CXXMACRO_CSTR
#define NVMACRO_NVNUM1      CXXMACRO_CNUM
#define NVMACRO_NVSTR2      _ccmmnds__go2stateofcontroller

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#undef CXXMACRO_INIT        
#undef CXXMACRO_UPDT        
#undef CXXMACRO_PART        
#undef CXXMACRO_BOARD       
#undef CXXMACRO_CSTR        
#undef CXXMACRO_CNUM        




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
