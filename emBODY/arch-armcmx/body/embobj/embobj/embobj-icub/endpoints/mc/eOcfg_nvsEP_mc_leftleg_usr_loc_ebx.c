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

/* @file       eOcfg_nvsEP_mc_leftleg_usr_loc_ebx.c
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
#include "eOcfg_nvsEP_mc_leftleg_con.h"

#include "EOnv.h"
#include "EOconstvector_hid.h"
#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_leftleg_usr_loc_ebx.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
//static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT__x1x2x3(const EOnv* nv);
//static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_update__x1x2x3(const EOnv* nv, const eOabstime_t time, const uint32_t sign);


// jxx-init:
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__upto02descrforchameleon02(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__jstatus(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__calibrator(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__setpoint(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__controlmode(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__signalwhenmotionisdone(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__motionisdone(uint16_t n, const EOnv* nv);
// jxx-updt: all the nvs
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__upto02descrforchameleon02(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__jstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__calibrator(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__setpoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__controlmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__signalwhenmotionisdone(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__motionisdone(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

// mxx-init:
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__upto02descrforchameleon06(uint16_t n, const EOnv* nv);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx__mstatus(uint16_t n, const EOnv* nv);
// mxx-updt: all the nvs
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__upto02descrforchameleon06(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx__mstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// the generic entry
//static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_peripheralinterface__x1x2x3 =
//{
//    EO_INIT(.init)      s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT__x1x2x3,
//    EO_INIT(.update)    s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT__x1x2x3
//};

#define JXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx
#define JXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx
#define JXXMACRO_PART        _leftleg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j00
#define JXXMACRO_JNUM        0
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"


#define JXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx
#define JXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx
#define JXXMACRO_PART        _leftleg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j01
#define JXXMACRO_JNUM        1
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx
#define JXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx
#define JXXMACRO_PART        _leftleg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j02
#define JXXMACRO_JNUM        2
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx
#define JXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx
#define JXXMACRO_PART        _leftleg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j03
#define JXXMACRO_JNUM        3
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx
#define JXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx
#define JXXMACRO_PART        _leftleg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j04
#define JXXMACRO_JNUM        4
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define JXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx
#define JXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx
#define JXXMACRO_PART        _leftleg
#define JXXMACRO_BOARD       _ebx
#define JXXMACRO_JSTR        _j05
#define JXXMACRO_JNUM        5
#include "eOcfg_nvsEP_mc_any_usr_jxxmacro.c"

#define MXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx
#define MXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx
#define MXXMACRO_PART        _leftleg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m00
#define MXXMACRO_MNUM        0
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx
#define MXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx
#define MXXMACRO_PART        _leftleg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m01
#define MXXMACRO_MNUM        1
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


#define MXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx
#define MXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx
#define MXXMACRO_PART        _leftleg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m02
#define MXXMACRO_MNUM        2
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx
#define MXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx
#define MXXMACRO_PART        _leftleg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m03
#define MXXMACRO_MNUM        3
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx
#define MXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx
#define MXXMACRO_PART        _leftleg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m04
#define MXXMACRO_MNUM        4
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"

#define MXXMACRO_INIT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx
#define MXXMACRO_UPDT        eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx
#define MXXMACRO_PART        _leftleg
#define MXXMACRO_BOARD       _ebx
#define MXXMACRO_MSTR        _m05
#define MXXMACRO_MNUM        5
#include "eOcfg_nvsEP_mc_any_usr_mxxmacro.c"


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

// the generic entry
//    {   // 00 __x1x2x3
//        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_peripheralinterface__x1x2x3,
//        EO_INIT(.on_rop_reception)      NULL,                 // we dont do anything on rop reception before or after its processing
//        EO_INIT(.stg_address)           EOK_uint32dummy       // we dont give any storage address.
//    },

static const EOnv_usr_t s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_array_of_EOnv_usr[varsMCleftleg_TOTALnumber] =
{
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_jconfig__upto02descrforchameleon02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx__jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx__calibrator,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx__signalwhenmotionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j00_ebx__motionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     

    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx_jconfig__upto02descrforchameleon02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx__jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx__calibrator,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx__signalwhenmotionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j01_ebx__motionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   

    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx_jconfig__upto02descrforchameleon02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx__jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx__calibrator,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },    
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx__signalwhenmotionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j02_ebx__motionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   

    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx_jconfig__upto02descrforchameleon02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx__jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx__calibrator,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx__signalwhenmotionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j03_ebx__motionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   

    
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx_jconfig__upto02descrforchameleon02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx__jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx__calibrator,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx__signalwhenmotionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j04_ebx__motionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    

    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__pidposition,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__pidvelocity,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__pidtorque,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__minpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__maxpositionofjoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__velocitysetpointtimeout,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx_jconfig__upto02descrforchameleon02,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },      
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx__jstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },   
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx__calibrator,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx__setpoint,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx__controlmode,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx__signalwhenmotionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // j05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_j05_ebx__motionisdone,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },       

    
    // and in here come the 6 motors     
    
    
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m00_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m00_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m00_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m00_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m00_ebx_mconfig__upto02descrforchameleon06,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m00 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m00_ebx__mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  


    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m01_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m01_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m01_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m01_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m01_ebx_mconfig__upto02descrforchameleon06,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m01 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m01_ebx__mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  

    
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m02_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m02_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m02_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m02_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m02_ebx_mconfig__upto02descrforchameleon06,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m02 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m02_ebx__mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  


    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m03_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m03_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m03_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m03_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m03_ebx_mconfig__upto02descrforchameleon06,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m03 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m03_ebx__mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  


    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m04_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m04_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m04_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m04_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m04_ebx_mconfig__upto02descrforchameleon06,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m04 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m04_ebx__mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  


    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m05_ebx_mconfig,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m05_ebx_mconfig__pidcurrent,
        EO_INIT(.on_rop_reception)      NULL,                 
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m05_ebx_mconfig__maxvelocityofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },     
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m05_ebx_mconfig__maxcurrentofmotor,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }, 
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m05_ebx_mconfig__upto02descrforchameleon06,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    },  
    {   // m05 
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_mc_leftleg_m05_ebx__mstatus,
        EO_INIT(.on_rop_reception)      NULL,                
        EO_INIT(.stg_address)           EOK_uint32dummy       
    }  
    
};


const EOconstvector  s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_array_of_EOnv_usr
};


extern const EOconstvector* const nvsEP_mc_leftleg_usr_loc_ebx_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_constvector_of_EOnv_usr;

extern eo_cfg_nvsEP_mc_leftleg_t* nvsEP_mc_leftleg_usr_loc_ebx_mem_local  = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_initialise(void* loc, void* rem)
{ 
    uint8_t j, m;
    eo_cfg_nvsEP_mc_leftleg_t *lloc = (eo_cfg_nvsEP_mc_leftleg_t*)loc;
    eo_cfg_nvsEP_mc_leftleg_t *rrem = (eo_cfg_nvsEP_mc_leftleg_t*)rem;    // it is NULL if we are in a local ownership
    
    rrem =rrem;

    // 0. init the extern variable whcih can be directly used by the application 
    nvsEP_mc_leftleg_usr_loc_ebx_mem_local = lloc;
    
    
    
    // in here we initailise the ram allocated by the EOnvscfg object: 
    // 1. at least put it at its default value. 
    // 2. if you need to initialise other peripherals or objects linked to the values do it.
    //    HOWEVER: initialisation of NVs is done on specific functions 
    
    
    // 1. assign default values
    //memcpy(mngmnt_loc, &eo_cfg_nvsEP_mc_leftleg_default, sizeof(eo_cfg_nvsEP_mc_leftleg_t));
    // as there is not a global default value ...
    for(j=0; j<jointLeftLeg_TOTALnumber; j++)
    {
        memcpy(&lloc->joints[j], &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue, sizeof(eOmc_joint_t)); 
    }
    for(m=0; m<motorLeftLeg_TOTALnumber; m++)
    {
        memcpy(&lloc->motors[m], &eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue, sizeof(eOmc_motor_t)); 
    }    
    
    
    // 2. init other peripherals ...
    // i dont do it
    

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// jxx-init:
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig(uint16_t n, const EOnv* nv)
{
    
}

static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv)
{
    
}

static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx_jconfig__upto02descrforchameleon02(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__jstatus(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__calibrator(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__setpoint(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__controlmode(uint16_t n, const EOnv* nv)
{
    
}

static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__signalwhenmotionisdone(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Jxx__motionisdone(uint16_t n, const EOnv* nv)
{
    
}


// jxx-updt: all the nvs
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx_jconfig__upto02descrforchameleon02(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__jstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__calibrator(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__setpoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__controlmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__signalwhenmotionisdone(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}

static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Jxx__motionisdone(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig(uint16_t n, const EOnv* nv)
{
    
}

static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx_mconfig__upto02descrforchameleon06(uint16_t n, const EOnv* nv)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_Mxx__mstatus(uint16_t n, const EOnv* nv)
{
    
}


// mxx-updt: all the nvs
static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx_mconfig__upto02descrforchameleon06(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}


static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_Mxx__mstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    
}








// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



