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

/* @file       eOcfg_nvsEP_mc_any_usr_mxxmacro.c
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
// MXXMACRO_INIT        my_INIT
// MXXMACRO_UPDT        my_UPDT
// MXXMACRO_PART        _leftleg etc
// MXXMACRO_BOARD       _ebx etc
// MXXMACRO_MSTR        _m00 etc
// MXXMACRO_MNUM        0 etc

#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mconfig

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"




#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mconfig__pidcurrent

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mconfig__maxvelocityofmotor

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mconfig__maxcurrentofmotor

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mconfig__des02FORmstatuschamaleon04

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mstatus

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mstatus__basic

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    MXXMACRO_INIT
#define NVMACRO_FUN_UPDT    MXXMACRO_UPDT

#define NVMACRO_PSTR        MXXMACRO_PART
#define NVMACRO_BSTR        MXXMACRO_BOARD
#define NVMACRO_NVSTR1      MXXMACRO_MSTR
#define NVMACRO_NVNUM1      MXXMACRO_MNUM
#define NVMACRO_NVSTR2      _mstatus__chamaleon04

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#undef MXXMACRO_INIT        
#undef MXXMACRO_UPDT        
#undef MXXMACRO_PART        
#undef MXXMACRO_BOARD       
#undef MXXMACRO_MSTR        
#undef MXXMACRO_MNUM        




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
