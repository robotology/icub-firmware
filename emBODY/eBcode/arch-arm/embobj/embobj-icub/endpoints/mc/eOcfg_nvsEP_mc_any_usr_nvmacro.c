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

/* @file       eOcfg_nvsEP_mc_any_usr_nvmacro.c
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


#define xx0fun_init(pstr, bstr, nvstr, var)                  s_eo_cfg_nvsEP_mc_INIT ## pstr ## bstr ## nvstr ## var
#define MACRO_fun_INIT(pstr, bstr, nvstr, var)               xx0fun_init(pstr, bstr, nvstr, var)


#define xx0fun_exinit(funinit, var, nvnum, a1)                  s_ ## funinit ## var (nvnum, a1)
#define MACRO_fun_external_INIT(funinit, var, nvnum, a1)        xx0fun_exinit(funinit, var, nvnum, a1)

//#define xx0fun_updt(pstr, bstr, nvstr, var)                  s_eo_cfg_nvsEP_mc ## pstr _usr_loc ## bstr _action_UPDT ## nvstr ## var
#define xx0fun_updt(pstr, bstr, nvstr, var)                  s_eo_cfg_nvsEP_mc_UPDT ## pstr ## bstr ## nvstr ## var
#define MACRO_fun_UPDT(pstr, bstr, nvstr, var)               xx0fun_updt(pstr, bstr, nvstr, var)


#define xx0fun_exupdt(funupdt, var, nvnum, a1, a2, a3)                  s_ ## funupdt ## var (nvnum, a1, a2, a3)
#define MACRO_fun_external_UDPT(funupdt, var, nvnum, a1, a2, a3)        xx0fun_exupdt(funupdt, var, nvnum, a1, a2, a3)


#define xx0var(pstr, bstr, nvstr, var)                  s_eo_cfg_nvsEP_mc ## pstr ## bstr ## nvstr ## var
#define MACRO_PERIPH_INTERFACE(pstr, bstr, nvstr, var)               xx0var(pstr, bstr, nvstr, var)


#if(1 == NVMACRO_USE_INIT)
//static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_INIT_j00NVMACRO_NVSTR2(const EOnv* nv)
static void MACRO_fun_INIT(NVMACRO_PSTR, NVMACRO_NVSTR1, NVMACRO_BSTR, NVMACRO_NVSTR2)(const EOnv* nv)
{   
    MACRO_fun_external_INIT(NVMACRO_FUN_INIT, NVMACRO_NVSTR2, NVMACRO_NVNUM1, nv);
}
#endif

#if(1 == NVMACRO_USE_UPDT)
//static void s_eo_cfg_nvsEP_mc_leftleg_usr_loc_ebx_action_UPDT_j00NVMACRO_NVSTR2(const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void MACRO_fun_UPDT(NVMACRO_PSTR, NVMACRO_NVSTR1, NVMACRO_BSTR, NVMACRO_NVSTR2)(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    MACRO_fun_external_UDPT(NVMACRO_FUN_UPDT, NVMACRO_NVSTR2, NVMACRO_NVNUM1, nv, time, sign);
}
#endif

// s_eo_cfg_nvsEP_mc_leftleg_j00_ebx_xxxxx
static const eOnv_fn_peripheral_t MACRO_PERIPH_INTERFACE(NVMACRO_PSTR, NVMACRO_NVSTR1, NVMACRO_BSTR, NVMACRO_NVSTR2) =
{
#if(1 == NVMACRO_USE_INIT)
    EO_INIT(.init)      MACRO_fun_INIT(NVMACRO_PSTR, NVMACRO_NVSTR1, NVMACRO_BSTR, NVMACRO_NVSTR2),
#else
    EO_INIT(.init)      NULL,
#endif
#if(1 == NVMACRO_USE_UPDT)
    EO_INIT(.update)    MACRO_fun_UPDT(NVMACRO_PSTR, NVMACRO_NVSTR1, NVMACRO_BSTR, NVMACRO_NVSTR2)
#else
    EO_INIT(.update)    NULL
#endif
};


#undef NVMACRO_FUN_INIT    
#undef NVMACRO_FUN_UPDT    

#undef NVMACRO_PSTR        
#undef NVMACRO_BSTR        
#undef NVMACRO_NVSTR1      
#undef NVMACRO_NVNUM1      
#undef NVMACRO_NVSTR2      

#undef NVMACRO_USE_INIT    
#undef NVMACRO_USE_UPDT    



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
