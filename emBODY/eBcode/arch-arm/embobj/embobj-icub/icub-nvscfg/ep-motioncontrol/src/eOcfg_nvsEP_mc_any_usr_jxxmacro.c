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

/* @file       eOcfg_nvsEP_mc_any_usr_jxxmacro.c
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
// JXXMACRO_INIT        my_INIT
// JXXMACRO_UPDT        my_UPDT
// JXXMACRO_PART        _leftleg etc
// JXXMACRO_BOARD       _ebx etc
// JXXMACRO_JSTR        _j00 etc
// JXXMACRO_JNUM        0 etc

#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__pidposition

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__pidvelocity

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__pidtorque

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__impedance

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"




#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__minpositionofjoint

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__maxpositionofjoint

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__velocitysetpointtimeout

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__controlmode

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__motionmonitormode

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__des02FORjstatuschamaleon04

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__holder01FFU01

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__holder01FFU02

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__holder02FFU03

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jconfig__holder02FFU04

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"




#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jstatus

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jstatus__basic

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jstatus__ofpid

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jstatus__chamaleon04

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"




#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jinputs__externallymeasuredtorque

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jinputs__holder02FFU01

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jinputs__holder04FFU02

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"







#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jcmmnds__calibration

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jcmmnds__setpoint

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jcmmnds__stoptrajectory

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jcmmnds__holder01FFU01

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jcmmnds__holder01FFU02

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"


#define NVMACRO_FUN_INIT    JXXMACRO_INIT
#define NVMACRO_FUN_UPDT    JXXMACRO_UPDT

#define NVMACRO_PSTR        JXXMACRO_PART
#define NVMACRO_BSTR        JXXMACRO_BOARD
#define NVMACRO_NVSTR1      JXXMACRO_JSTR
#define NVMACRO_NVNUM1      JXXMACRO_JNUM
#define NVMACRO_NVSTR2      _jcmmnds__holder01FFU03

#define NVMACRO_USE_INIT    1
#define NVMACRO_USE_UPDT    1

#include "eOcfg_nvsEP_mc_any_usr_nvmacro.c"



#undef JXXMACRO_INIT        
#undef JXXMACRO_UPDT        
#undef JXXMACRO_PART        
#undef JXXMACRO_BOARD       
#undef JXXMACRO_JSTR        
#undef JXXMACRO_JNUM        







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
