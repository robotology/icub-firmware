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

/* @file       eOcfg_nvsEP_mc_any_con_sxxmacro.c
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
#include "eOcfg_nvsEP_as_any_con_sxx.h"    
#include "eOcfg_nvsEP_as_any_con_sxxdefault.h"
      




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
//#define SMACRO_NAMEOFVARIABLE(pstr, jstr, var)            eo_cfg_nvsEP_as ## pstr ## jstr ## var 

#define xx0nameofvariable(pstr, jstr, var)                  eo_cfg_nvsEP_as ## pstr ## jstr ## var
#define SMACRO_NAMEOFVARIABLE(pstr, jstr, var)               xx0nameofvariable(pstr, jstr, var)

#define xx0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define SMACRO_VERIFYSIZEOF(id, sname, ssize)                xx0verifysizeof(id, sname, ssize)

#define xx0getnvid(extprefix, postfix, j)                   extprefix##postfix(j)
#define SMACRO_GETNVID(extprefix, postfix, j)                xx0getnvid(extprefix, postfix, j)



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



#define SOFFSETof__txmode                                        (SMACRO_OFF)
#define SCAPACITY__txmode                                        sizeof(eOenum08_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, __txmode) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, __txmode, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY__txmode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.txmode,
    EO_INIT(.offset)    SOFFSETof__txmode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__txmode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__txmode)
};
#define SOFFSETafter__txmode                                     (SOFFSETof__txmode+SCAPACITY__txmode)

#define SOFFSETof__datarate                                        (SOFFSETafter__txmode)
#define SCAPACITY__datarate                                        sizeof(uint8_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, __datarate) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, __datarate, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY__datarate,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.datarate,
    EO_INIT(.offset)    SOFFSETof__datarate,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__datarate),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__datarate)
};
#define SOFFSETafter__datarate                                     (SOFFSETof__datarate+SCAPACITY__datarate)

#define SOFFSETof__signaloncefullscale                                        (SOFFSETafter__datarate)
#define SCAPACITY__signaloncefullscale                                        sizeof(eObool_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, __signaloncefullscale) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, __signaloncefullscale, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY__signaloncefullscale,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.signaloncefullscale,
    EO_INIT(.offset)    SOFFSETof__signaloncefullscale,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__signaloncefullscale),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__signaloncefullscale)
};
#define SOFFSETafter__signaloncefullscale                                     (SOFFSETof__signaloncefullscale+SCAPACITY__signaloncefullscale)


#define SOFFSETof_filler01                                       (SOFFSETafter__signaloncefullscale)
#define SCAPACITY_filler01                                       (1)
//
#define SOFFSETafter_filler01                                     (SOFFSETof_filler01+SCAPACITY_filler01)

#define SOFFSETof_filler04                                       (SOFFSETafter_filler01)
#define SCAPACITY_filler04                                       (4)
//
#define SOFFSETafter_filler04                                     (SOFFSETof_filler04+SCAPACITY_filler04)


#define SOFFSETof__values                                        (SOFFSETafter_filler04)
#define SCAPACITY__values                                        sizeof(eOsnsr_arrayofupto12bytes_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, __values) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, __values, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY__values,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.values,
    EO_INIT(.offset)    SOFFSETof__values,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__values),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP__values)
};
#define SOFFSETafter__values                                     (SOFFSETof__values+SCAPACITY__values)



// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
SMACRO_VERIFYSIZEOF(SMACRO_NUM, eOsnsr_strain_t, SOFFSETafter__values-SMACRO_OFF);



#undef SMACRO_PSTR 
#undef SMACRO_STR 
#undef SMACRO_PNUM
#undef SMACRO_NUM  
#undef SMACRO_OFF

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
