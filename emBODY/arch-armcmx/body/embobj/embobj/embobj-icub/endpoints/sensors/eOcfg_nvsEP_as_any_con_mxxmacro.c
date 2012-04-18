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
#define MMACRO_NAMEOFVARIABLE(pstr, jstr, var)               yy0nameofvariable(pstr, jstr, var)

#define yy0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define MMACRO_VERIFYSIZEOF(id, sname, ssize)                yy0verifysizeof(id, sname, ssize)

#define yy0getnvid(extprefix, postfix, j)                   extprefix##postfix(j)
#define MMACRO_GETNVID(extprefix, postfix, j)                yy0getnvid(extprefix, postfix, j)



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



#define MOFFSETof__txmode                                        (MMACRO_OFF)
#define MCAPACITY__txmode                                        sizeof(eOenum08_t)
extern EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, __txmode) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, __txmode, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY__txmode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.txmode,
    EO_INIT(.offset)    MOFFSETof__txmode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__txmode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__txmode)
};
#define MOFFSETafter__txmode                                     (MOFFSETof__txmode+MCAPACITY__txmode)

#define MOFFSETof__datarate                                        (MOFFSETafter__txmode)
#define MCAPACITY__datarate                                        sizeof(uint8_t)
extern EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, __datarate) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, __datarate, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY__datarate,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.datarate,
    EO_INIT(.offset)    MOFFSETof__datarate,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__datarate),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__datarate)
};
#define MOFFSETafter__datarate                                     (MOFFSETof__datarate+MCAPACITY__datarate)

#define MOFFSETof__resolutionmode                                        (MOFFSETafter__datarate)
#define MCAPACITY__resolutionmode                                        sizeof(eOenum08_t)
extern EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, __resolutionmode) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, __resolutionmode, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY__resolutionmode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.resolutionmode,
    EO_INIT(.offset)    MOFFSETof__resolutionmode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__resolutionmode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__resolutionmode)
};
#define MOFFSETafter__resolutionmode                                     (MOFFSETof__resolutionmode+MCAPACITY__resolutionmode)


#define MOFFSETof_filler01                                       (MOFFSETafter__resolutionmode)
#define MCAPACITY_filler01                                       (1)
//
#define MOFFSETafter_filler01                                     (MOFFSETof_filler01+MCAPACITY_filler01)

#define MOFFSETof_filler04                                       (MOFFSETafter_filler01)
#define MCAPACITY_filler04                                       (4)
//
#define MOFFSETafter_filler04                                     (MOFFSETof_filler04+MCAPACITY_filler04)


#define MOFFSETof__values                                        (MOFFSETafter_filler04)
#define MCAPACITY__values                                        sizeof(eOsnsr_arrayofupto12bytes_t)
extern EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_STR, __values) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, __values, MMACRO_NUM),
    EO_INIT(.capacity)  MCAPACITY__values,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue.values,
    EO_INIT(.offset)    MOFFSETof__values,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__values),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_mxx_NVFUNTYP__values)
};
#define MOFFSETafter__values                                     (MOFFSETof__values+MCAPACITY__values)



// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
MMACRO_VERIFYSIZEOF(MMACRO_NUM, eOsnsr_strain_t, MOFFSETafter__values-MMACRO_OFF);



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
