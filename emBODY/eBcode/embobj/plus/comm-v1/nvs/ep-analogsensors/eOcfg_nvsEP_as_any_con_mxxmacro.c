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

/* @file       eOcfg_nvsEP_mc_any_con_mxxmacro.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       04/06/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoSensors.h"
#include "eOcfg_nvsEP_as_any_con_mxx.h"    
#include "eOcfg_nvsEP_as_any_con_mxxdefault.h"
      




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_jpragma_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// the first xxname() does not works.
// see why at http://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
//#define MMACRO_NAMEOFVARIABLE(pstr, jstr, var)            eo_cfg_nvsEP_as ## pstr ## jstr ## var 

#define yy0nameofvariable(pstr, jstr, var)                  eo_cfg_nvsEP_as ## pstr ## jstr ## var
#define MMACRO_NAMEOFVARIABLE(pstr, jstr, var)              yy0nameofvariable(pstr, jstr, var)

#define yy0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define MMACRO_VERIFYSIZEOF(id, sname, ssize)               yy0verifysizeof(id, sname, ssize)

#define yy0getnvid(extprefix, postfix, j)                   extprefix##postfix(j)
#define MMACRO_GETNVID(extprefix, postfix, j)               yy0getnvid(extprefix, postfix, j)



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
#define MOFFSETof_mconfig                                               (MMACRO_OFF)
#define MCAPACITY_mconfig                                               sizeof(eOsnsr_mais_config_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, _mconfig) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY_mconfig,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.mconfig,
    EO_INIT(.offset)    MOFFSETof_mconfig,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig)
};
#define MOFFSETafter_mconfig                                           (MOFFSETof_mconfig+MCAPACITY_mconfig)

#define MOFFSETof_mconfig__mode                                        (MOFFSETof_mconfig)
#define MCAPACITY_mconfig__mode                                        sizeof(eOenum08_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, _mconfig__mode) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__mode, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY_mconfig__mode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.mconfig.mode,
    EO_INIT(.offset)    MOFFSETof_mconfig__mode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__mode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__mode)
};
#define MOFFSETafter_mconfig__mode                                     (MOFFSETof_mconfig__mode+MCAPACITY_mconfig__mode)

#define MOFFSETof_mconfig__datarate                                        (MOFFSETafter_mconfig__mode)
#define MCAPACITY_mconfig__datarate                                        sizeof(uint8_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, _mconfig__datarate) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__datarate, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY_mconfig__datarate,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.mconfig.datarate,
    EO_INIT(.offset)    MOFFSETof_mconfig__datarate,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__datarate),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__datarate)
};
#define MOFFSETafter_mconfig__datarate                                     (MOFFSETof_mconfig__datarate+MCAPACITY_mconfig__datarate)

#define MOFFSETof_mconfig__resolution                                        (MOFFSETafter_mconfig__datarate)
#define MCAPACITY_mconfig__resolution                                        sizeof(eOenum08_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, _mconfig__resolution) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__resolution, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY_mconfig__resolution,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.mconfig.resolution,
    EO_INIT(.offset)    MOFFSETof_mconfig__resolution,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__resolution),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mconfig__resolution)
};
#define MOFFSETafter_mconfig__resolution                                     (MOFFSETof_mconfig__resolution+MCAPACITY_mconfig__resolution)


#define MOFFSETof_sconfig_filler01                                       (MOFFSETafter_mconfig__resolution)
#define MCAPACITY_sconfig_filler01                                       (1)
//
#define MOFFSETafter_sconfig_filler01                                     (MOFFSETof_sconfig_filler01+MCAPACITY_sconfig_filler01)

#define MOFFSETof_minputs_filler04                                       (MOFFSETafter_mconfig)//(MOFFSETafter_sconfig_filler01)
#define MCAPACITY_minputs_filler04                                       (4)
//
#define MOFFSETafter_minputs_filler04                                     (MOFFSETof_minputs_filler04+MCAPACITY_minputs_filler04)


#define MOFFSETof_mstatus                                                   (MOFFSETafter_minputs_filler04) //(MOFFSETafter_mconfig)
#define MCAPACITY_mstatus                                                   sizeof(eOsnsr_mais_status_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, _mstatus) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mstatus, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY_mstatus,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.mstatus,
    EO_INIT(.offset)    MOFFSETof_mstatus,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mstatus),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mstatus)
};
#define MOFFSETafter_mstatus                                                (MOFFSETof_mstatus+MCAPACITY_mstatus)


#define MOFFSETof_mstatus__the15values                                        (MOFFSETafter_minputs_filler04)
#define MCAPACITY_mstatus__the15values                                        sizeof(eOsnsr_arrayofupto36bytes_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, _mstatus__the15values) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mstatus__the15values, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY_mstatus__the15values,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.mstatus.the15values,
    EO_INIT(.offset)    MOFFSETof_mstatus__the15values,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mstatus__the15values),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP_mstatus__the15values)
};
#define MOFFSETafter_mstatus__the15values                                     (MOFFSETof_mstatus__the15values+MCAPACITY_mstatus__the15values)



// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
MMACRO_VERIFYSIZEOF(MMACRO_NUM, eOsnsr_mais_t, MOFFSETafter_mstatus__the15values-MMACRO_OFF);
//MMACRO_VERIFYSIZEOF(MMACRO_NUM, eOsnsr_mais_t, 48);



#undef MMACRO_PSTR 
#undef MMACRO_STR 
#undef MMACRO_PNUM
#undef MMACRO_NUM  
#undef MMACRO_OFF

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
