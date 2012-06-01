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

/* @file       eOcfg_nvsEP_mc_torso_usr.c
    @brief      This file keeps the user-defined local ...
    @author     marco.accame@iit.it
    @date       09/06/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "eOcfg_nvsEP_mc_hid.h"
#include "eOcfg_nvsEP_mc_any_con_bodypart.h"
#include "eOcfg_nvsEP_mc_any_con_bodypart_hid.h"
#include "eOcfg_nvsEP_mc_torso_con.h"

#include "EOnv.h"
#include "EOconstvector_hid.h"
#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"






// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_torso_usr.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_nvsEP_mc_torso_usr_hid.h"


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

// the generic declaration
//extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT__x1x2x3(const EOnv* nv);
//extern void eo_cfg_nvsEP_mc_torso_usr_hid_update__x1x2x3(const EOnv* nv, const eOabstime_t time, const uint32_t sign);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#define JXXMACRO_INIT        torso_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        torso_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _torso
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j00
#define JXXMACRO_JNUM        0
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"


#define JXXMACRO_INIT        torso_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        torso_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _torso
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j01
#define JXXMACRO_JNUM        1
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        torso_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        torso_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _torso
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j02
#define JXXMACRO_JNUM        2
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        torso_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        torso_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _torso
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j03
#define JXXMACRO_JNUM        3
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"


#define MXXMACRO_INIT        torso_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        torso_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _torso
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m00
#define MXXMACRO_MNUM        0
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        torso_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        torso_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _torso
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m01
#define MXXMACRO_MNUM        1
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        torso_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        torso_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _torso
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m02
#define MXXMACRO_MNUM        2
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        torso_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        torso_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _torso
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m03
#define MXXMACRO_MNUM        3
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define CXXMACRO_INIT        torso_usr_hid_INIT_Cxx
#define CXXMACRO_UPDT        torso_usr_hid_UPDT_Cxx
#define CXXMACRO_PART        _torso
#define CXXMACRO_BOARD       _ebx
#define CXXMACRO_CSTR        _c00
#define CXXMACRO_CNUM        0
#include "eOcfg_nvsEP_mc_any_usr_cxxmacro.c"



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


static const EOnv_usr_t s_eo_cfg_nvsEP_mc_torso_usr_array_of_EOnv_usr[] =
{
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__pidposition,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__pidvelocity,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__pidtorque,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__impedance,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__minpositionofjoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__maxpositionofjoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__velocitysetpointtimeout,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__controlmode,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__motionmonitormode,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__des02FORjstatuschamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__holder01FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__holder01FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__holder02FFU03,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jconfig__holder02FFU04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jstatus__basic,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jstatus__ofpid,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jstatus__chamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jinputs__externallymeasuredtorque,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jinputs__holder02FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jinputs__holder04FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jcmmnds__calibration,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jcmmnds__setpoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jcmmnds__stoptrajectory,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jcmmnds__holder01FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jcmmnds__holder01FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j00_ebx_jcmmnds__holder01FFU03,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__pidposition,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__pidvelocity,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__pidtorque,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__impedance,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__minpositionofjoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__maxpositionofjoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__velocitysetpointtimeout,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__controlmode,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__motionmonitormode,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__des02FORjstatuschamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__holder01FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__holder01FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__holder02FFU03,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jconfig__holder02FFU04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jstatus__basic,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jstatus__ofpid,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jstatus__chamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jinputs__externallymeasuredtorque,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jinputs__holder02FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jinputs__holder04FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jcmmnds__calibration,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jcmmnds__setpoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jcmmnds__stoptrajectory,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jcmmnds__holder01FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jcmmnds__holder01FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j01_ebx_jcmmnds__holder01FFU03,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__pidposition,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__pidvelocity,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__pidtorque,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__impedance,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__minpositionofjoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__maxpositionofjoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__velocitysetpointtimeout,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__controlmode,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__motionmonitormode,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__des02FORjstatuschamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__holder01FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__holder01FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__holder02FFU03,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jconfig__holder02FFU04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jstatus__basic,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jstatus__ofpid,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jstatus__chamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jinputs__externallymeasuredtorque,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jinputs__holder02FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jinputs__holder04FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jcmmnds__calibration,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jcmmnds__setpoint,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jcmmnds__stoptrajectory,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jcmmnds__holder01FFU01,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jcmmnds__holder01FFU02,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_j02_ebx_jcmmnds__holder01FFU03,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 



    // and in here come the 3 motors     
    
    
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mconfig__pidcurrent,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mconfig__maxvelocityofmotor,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mconfig__maxcurrentofmotor,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mconfig__des02FORmstatuschamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mstatus__basic,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m00_ebx_mstatus__chamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    
    
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mconfig__pidcurrent,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mconfig__maxvelocityofmotor,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mconfig__maxcurrentofmotor,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mconfig__des02FORmstatuschamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mstatus__basic,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m01_ebx_mstatus__chamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    
    
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mconfig__pidcurrent,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mconfig__maxvelocityofmotor,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mconfig__maxcurrentofmotor,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mconfig__des02FORmstatuschamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mstatus__basic,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_m02_ebx_mstatus__chamaleon04,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    
    // and in here the controller
    
    {   // c00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_c00_ebx_cconfig,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // c00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_c00_ebx_cconfig__durationofctrlloop,
        EONV_ONROPRECEPTION_IS_NULL                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // c00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_c00_ebx_cstatus,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // c00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_c00_ebx_cstatus__alljomoinitted,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // c00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_torso_c00_ebx_ccmmnds__go2stateofcontroller,
        EONV_ONROPRECEPTION_IS_NULL                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    } 
    
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_mc_torso_usr_array_of_EOnv_usr, sizeof(EOnv_usr_t)*(varsMCtorso_TOTALnumber)); 


const EOconstvector  s_eo_cfg_nvsEP_mc_torso_usr_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_mc_torso_usr_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_mc_torso_usr_array_of_EOnv_usr
};


const EOconstvector* const eo_cfg_nvsEP_mc_torso_usr_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_mc_torso_usr_constvector_of_EOnv_usr;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_mc_torso_usr_initialise(eOnvEP_t ep, void* loc, void* rem)
{ 
//    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
//    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_torso_t* ul = NULL;
    uint8_t i;
   
    // copy default values
    if(NULL != loc)
    {
        ul = (eo_cfg_nvsEP_mc_torso_t*) loc;
        for(i=0; i<jointTorso_TOTALnumber; i++)
        {
            memcpy(&ul->joints[i], eo_cfg_nvsEP_mc_torso_joint_defaultvalue, sizeof(eOmc_joint_t));
        }
        for(i=0; i<motorTorso_TOTALnumber; i++)
        {
            memcpy(&ul->motors[i], eo_cfg_nvsEP_mc_torso_motor_defaultvalue, sizeof(eOmc_motor_t));
        }
        memcpy(&ul->thecontroller, eo_cfg_nvsEP_mc_torso_controller_defaultvalue, sizeof(eOmc_controller_t));
    }    
    if(NULL != rem)
    {   
        ul = (eo_cfg_nvsEP_mc_torso_t*) rem;
        for(i=0; i<jointTorso_TOTALnumber; i++)
        {
            memcpy(&ul->joints[i], eo_cfg_nvsEP_mc_torso_joint_defaultvalue, sizeof(eOmc_joint_t));
        }
        for(i=0; i<motorTorso_TOTALnumber; i++)
        {
            memcpy(&ul->motors[i], eo_cfg_nvsEP_mc_torso_motor_defaultvalue, sizeof(eOmc_motor_t));
        }
        memcpy(&ul->thecontroller, eo_cfg_nvsEP_mc_torso_controller_defaultvalue, sizeof(eOmc_controller_t));
    }    
    
    // launch a specialised initialisation
    eo_cfg_nvsEP_mc_torso_usr_hid_INITIALISE(ep, loc, rem);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INITIALISE(ep, loc, rem);
}


// jxx-init:
__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__pidposition(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidposition((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__pidvelocity(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidvelocity((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__pidtorque(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidtorque((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__impedance(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__impedance((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__minpositionofjoint(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__minpositionofjoint((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__maxpositionofjoint(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__maxpositionofjoint((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__velocitysetpointtimeout((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__controlmode(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__controlmode((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__motionmonitormode(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__motionmonitormode((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__holder01FFU01(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU01((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__holder01FFU02(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU02((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__holder02FFU03(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU03((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jconfig__holder02FFU04(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU04((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jstatus(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jstatus__basic(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__basic((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jstatus__ofpid(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__ofpid((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jstatus__chamaleon04(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__chamaleon04((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jinputs__externallymeasuredtorque(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__externallymeasuredtorque((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jinputs__holder02FFU01(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder02FFU01((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jinputs__holder04FFU02(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder04FFU02((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jcmmnds__calibration(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__calibration((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jcmmnds__setpoint(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__setpoint((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jcmmnds__stoptrajectory(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__stoptrajectory((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jcmmnds__holder01FFU01(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU01((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jcmmnds__holder01FFU02(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU02((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Jxx_jcmmnds__holder01FFU03(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU03((eOcfg_nvsEP_mc_jointNumber_t)xx, nv);
}


// jxx-updt:

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__pidposition(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__pidvelocity(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidvelocity((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__pidtorque(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__impedance(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__minpositionofjoint(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__maxpositionofjoint(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__controlmode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__controlmode((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__motionmonitormode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__holder01FFU01(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU01((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__holder01FFU02(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU02((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);    
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__holder02FFU03(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU03((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jconfig__holder02FFU04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU04((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jstatus__basic(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jstatus__ofpid(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__ofpid((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jstatus__chamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__chamaleon04((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__externallymeasuredtorque((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jinputs__holder02FFU01(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder02FFU01((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jinputs__holder04FFU02(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder04FFU02((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jcmmnds__calibration(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jcmmnds__setpoint(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jcmmnds__stoptrajectory(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU01(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU01((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU02(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU02((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU03(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU03((eOcfg_nvsEP_mc_jointNumber_t)xx, nv, time, sign);
}


// mxx-init:

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mconfig(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mconfig__pidcurrent(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__pidcurrent((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mconfig__maxvelocityofmotor(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxvelocityofmotor((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mconfig__maxcurrentofmotor(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxcurrentofmotor((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mstatus(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mstatus__basic(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__basic((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Mxx_mstatus__chamaleon04(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__chamaleon04((eOcfg_nvsEP_mc_motorNumber_t)xx, nv);
}

// mxx-updt:


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mconfig__pidcurrent(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}


__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mstatus__basic(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Mxx_mstatus__chamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__chamaleon04((eOcfg_nvsEP_mc_motorNumber_t)xx, nv, time, sign); 
}


// c00-init:    xx is 0
__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Cxx_cconfig(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig__durationofctrlloop((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Cxx_cstatus(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus__alljomoinitted((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_INIT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_INIT_Cxx_ccmmnds__go2stateofcontroller((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv);
}


// c00-updt:    xx is 0
extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Cxx_cconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig__durationofctrlloop((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Cxx_cstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus__alljomoinitted((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv, time, sign);
}

__weak extern void eo_cfg_nvsEP_mc_torso_usr_hid_UPDT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    theOwnershipIsLocal = theOwnershipIsLocal;
    eo_cfg_nvsEP_mc_hid_UPDT_Cxx_ccmmnds__go2stateofcontroller((eOcfg_nvsEP_mc_controllerNumber_t)xx, nv, time, sign);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------









// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



