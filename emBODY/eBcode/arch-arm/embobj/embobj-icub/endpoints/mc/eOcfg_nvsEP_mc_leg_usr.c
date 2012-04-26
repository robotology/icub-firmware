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

/* @file       eOcfg_nvsEP_mc_leg_usr.c
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
#include "eOcfg_nvsEP_mc_any_con_bodypart.h"
#include "eOcfg_nvsEP_mc_leg_con.h"

#include "EOnv.h"
#include "EOconstvector_hid.h"
#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_leg_usr.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_nvsEP_mc_leg_usr_hid.h"


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
//extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT__x1x2x3(const EOnv* nv);
//extern void eo_cfg_nvsEP_mc_leg_usr_hid_update__x1x2x3(const EOnv* nv, const eOabstime_t time, const uint32_t sign);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#define JXXMACRO_INIT        leg_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        leg_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _leg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j00
#define JXXMACRO_JNUM        0
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"


#define JXXMACRO_INIT        leg_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        leg_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _leg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j01
#define JXXMACRO_JNUM        1
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        leg_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        leg_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _leg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j02
#define JXXMACRO_JNUM        2
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        leg_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        leg_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _leg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j03
#define JXXMACRO_JNUM        3
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        leg_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        leg_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _leg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j04
#define JXXMACRO_JNUM        4
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        leg_usr_hid_INIT_Jxx
#define JXXMACRO_UPDT        leg_usr_hid_UPDT_Jxx
#define JXXMACRO_PART        _leg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j05
#define JXXMACRO_JNUM        5
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define MXXMACRO_INIT        leg_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        leg_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _leg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m00
#define MXXMACRO_MNUM        0
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        leg_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        leg_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _leg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m01
#define MXXMACRO_MNUM        1
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        leg_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        leg_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _leg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m02
#define MXXMACRO_MNUM        2
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        leg_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        leg_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _leg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m03
#define MXXMACRO_MNUM        3
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        leg_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        leg_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _leg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m04
#define MXXMACRO_MNUM        4
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        leg_usr_hid_INIT_Mxx
#define MXXMACRO_UPDT        leg_usr_hid_UPDT_Mxx
#define MXXMACRO_PART        _leg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m05
#define MXXMACRO_MNUM        5
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


static const EOnv_usr_t s_eo_cfg_nvsEP_mc_leg_usr_array_of_EOnv_usr[] =
{
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__impedance,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__motionmonitormode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__des02FORjstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__holder02FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jconfig__holder02FFU04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jstatus__ofpid,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jinputs__externallymeasuredtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jinputs__holder02FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jinputs__holder04FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jcmmnds__calibration,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jcmmnds__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jcmmnds__stoptrajectory,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jcmmnds__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jcmmnds__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j00_ebx_jcmmnds__holder01FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__impedance,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__motionmonitormode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__des02FORjstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__holder02FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jconfig__holder02FFU04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jstatus__ofpid,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jinputs__externallymeasuredtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jinputs__holder02FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jinputs__holder04FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jcmmnds__calibration,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jcmmnds__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jcmmnds__stoptrajectory,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jcmmnds__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jcmmnds__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j01_ebx_jcmmnds__holder01FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__impedance,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__motionmonitormode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__des02FORjstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__holder02FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jconfig__holder02FFU04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jstatus__ofpid,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jinputs__externallymeasuredtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jinputs__holder02FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jinputs__holder04FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jcmmnds__calibration,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jcmmnds__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jcmmnds__stoptrajectory,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jcmmnds__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jcmmnds__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j02_ebx_jcmmnds__holder01FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 

    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__impedance,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__motionmonitormode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__des02FORjstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__holder02FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jconfig__holder02FFU04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jstatus__ofpid,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jinputs__externallymeasuredtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jinputs__holder02FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jinputs__holder04FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jcmmnds__calibration,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jcmmnds__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jcmmnds__stoptrajectory,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jcmmnds__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jcmmnds__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j03_ebx_jcmmnds__holder01FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 

    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__impedance,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__motionmonitormode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__des02FORjstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__holder02FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jconfig__holder02FFU04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jstatus__ofpid,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jinputs__externallymeasuredtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jinputs__holder02FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jinputs__holder04FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jcmmnds__calibration,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jcmmnds__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jcmmnds__stoptrajectory,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jcmmnds__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jcmmnds__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j04_ebx_jcmmnds__holder01FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 

    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__impedance,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__motionmonitormode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__des02FORjstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__holder02FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jconfig__holder02FFU04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jstatus__ofpid,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jinputs__externallymeasuredtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jinputs__holder02FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jinputs__holder04FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jcmmnds__calibration,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jcmmnds__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jcmmnds__stoptrajectory,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jcmmnds__holder01FFU01,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jcmmnds__holder01FFU02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_j05_ebx_jcmmnds__holder01FFU03,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    
    // and in here come the 6 motors     
    
    
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mconfig__des02FORmstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m00_ebx_mstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    
    
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mconfig__des02FORmstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m01_ebx_mstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    
    
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mconfig__des02FORmstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m02_ebx_mstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mconfig__des02FORmstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m03_ebx_mstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mconfig__des02FORmstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m04_ebx_mstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mconfig__des02FORmstatuschamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mstatus__basic,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leg_m05_ebx_mstatus__chamaleon04,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }    
    
};  EO_VERIFYsizeof(s_eo_cfg_nvsEP_mc_leg_usr_array_of_EOnv_usr, sizeof(EOnv_usr_t)*(varsMCleg_TOTALnumber)); 


const EOconstvector  s_eo_cfg_nvsEP_mc_leg_usr_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_mc_leg_usr_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_mc_leg_usr_array_of_EOnv_usr
};


extern const EOconstvector* const nvsEP_mc_leg_usr_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_mc_leg_usr_constvector_of_EOnv_usr;

extern eo_cfg_nvsEP_mc_leg_t* nvsEP_mc_leg_usr_mem_local    = NULL;
extern eo_cfg_nvsEP_mc_leg_t* nvsEP_mc_leg_usr_mem_remote   = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_mc_leg_usr_initialise(eOnvEP_t ep, void* loc, void* rem)
{ 
    // init the extern variable which can be directly used by the application 
    nvsEP_mc_leg_usr_mem_local      = (eo_cfg_nvsEP_mc_leg_t*)loc;
    nvsEP_mc_leg_usr_mem_remote     = (eo_cfg_nvsEP_mc_leg_t*)rem;
    
    // launch a specialised initialisation
    eo_cfg_nvsEP_mc_leg_usr_hid_INITIALISE(ep, nvsEP_mc_leg_usr_mem_local, nvsEP_mc_leg_usr_mem_remote);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{

}


// jxx-init:
__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__impedance(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__controlmode(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__motionmonitormode(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__holder01FFU01(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__holder01FFU02(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__holder02FFU03(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__holder02FFU04(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jstatus(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jstatus__basic(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jstatus__ofpid(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jstatus__chamaleon04(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jinputs__externallymeasuredtorque(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jinputs__holder02FFU01(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jinputs__holder04FFU02(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jcmmnds__calibration(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jcmmnds__setpoint(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jcmmnds__stoptrajectory(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jcmmnds__holder01FFU01(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jcmmnds__holder01FFU02(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jcmmnds__holder01FFU03(uint16_t n, const EOnv* nv)
{
    
}


// jxx-updt:

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__impedance(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__controlmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__motionmonitormode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__holder01FFU01(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__holder01FFU02(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__holder02FFU03(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__holder02FFU04(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jstatus__basic(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jstatus__ofpid(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jstatus__chamaleon04(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jinputs__holder02FFU01(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jinputs__holder04FFU02(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jcmmnds__calibration(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jcmmnds__setpoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jcmmnds__stoptrajectory(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU01(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU02(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU03(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


// mxx-init:

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(uint16_t n, const EOnv* nv)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mstatus(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mstatus__basic(uint16_t n, const EOnv* nv)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mstatus__chamaleon04(uint16_t n, const EOnv* nv)
{
    
}

// mxx-updt:


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mstatus__basic(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

__weak extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mstatus__chamaleon04(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}











// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



