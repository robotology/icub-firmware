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

/* @file       eOcfg_nvsEP_as_any_usr_sxxmacro.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       04/06/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eOcommon.h"    
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
// SXXMACRO_INIT        my_INIT
// SXXMACRO_UPDT        my_UPDT
// SXXMACRO_PART        _leftleg etc
// SXXMACRO_BOARD       _ebx etc
// SXXMACRO_SSTR        _j00 etc
// SXXMACRO_SNUM        0 etc

#define NVMACRO_FUN_INIT    SXXMACRO_INIT
#define NVMACRO_FUN_UPDT    SXXMACRO_UPDT

#define NVMACRO_PSTR        SXXMACRO_PART
#define NVMACRO_BSTR        SXXMACRO_BOARD
#define NVMACRO_NVSTR1      SXXMACRO_SSTR
#define NVMACRO_NVNUM1      SXXMACRO_SNUM
#define NVMACRO_NVSTR2      _sconfig__mode

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_as_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    SXXMACRO_INIT
#define NVMACRO_FUN_UPDT    SXXMACRO_UPDT

#define NVMACRO_PSTR        SXXMACRO_PART
#define NVMACRO_BSTR        SXXMACRO_BOARD
#define NVMACRO_NVSTR1      SXXMACRO_SSTR
#define NVMACRO_NVNUM1      SXXMACRO_SNUM
#define NVMACRO_NVSTR2      _sconfig__datarate

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_as_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    SXXMACRO_INIT
#define NVMACRO_FUN_UPDT    SXXMACRO_UPDT

#define NVMACRO_PSTR        SXXMACRO_PART
#define NVMACRO_BSTR        SXXMACRO_BOARD
#define NVMACRO_NVSTR1      SXXMACRO_SSTR
#define NVMACRO_NVNUM1      SXXMACRO_SNUM
#define NVMACRO_NVSTR2      _sconfig__signaloncefullscale

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_as_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    SXXMACRO_INIT
#define NVMACRO_FUN_UPDT    SXXMACRO_UPDT

#define NVMACRO_PSTR        SXXMACRO_PART
#define NVMACRO_BSTR        SXXMACRO_BOARD
#define NVMACRO_NVSTR1      SXXMACRO_SSTR
#define NVMACRO_NVNUM1      SXXMACRO_SNUM
#define NVMACRO_NVSTR2      _sstatus__fullscale

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_as_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    SXXMACRO_INIT
#define NVMACRO_FUN_UPDT    SXXMACRO_UPDT

#define NVMACRO_PSTR        SXXMACRO_PART
#define NVMACRO_BSTR        SXXMACRO_BOARD
#define NVMACRO_NVSTR1      SXXMACRO_SSTR
#define NVMACRO_NVNUM1      SXXMACRO_SNUM
#define NVMACRO_NVSTR2      _sstatus__calibratedvalues

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_as_any_usr_nvmacro.c"




#define NVMACRO_FUN_INIT    SXXMACRO_INIT
#define NVMACRO_FUN_UPDT    SXXMACRO_UPDT

#define NVMACRO_PSTR        SXXMACRO_PART
#define NVMACRO_BSTR        SXXMACRO_BOARD
#define NVMACRO_NVSTR1      SXXMACRO_SSTR
#define NVMACRO_NVNUM1      SXXMACRO_SNUM
#define NVMACRO_NVSTR2      _sstatus__uncalibratedvalues

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_as_any_usr_nvmacro.c"



#undef SXXMACRO_INIT        
#undef SXXMACRO_UPDT        
#undef SXXMACRO_PART        
#undef SXXMACRO_BOARD       
#undef SXXMACRO_SSTR        
#undef SXXMACRO_SNUM        







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
