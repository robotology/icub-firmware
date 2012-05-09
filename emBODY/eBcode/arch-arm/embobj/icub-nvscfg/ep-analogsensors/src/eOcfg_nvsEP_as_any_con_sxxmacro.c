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
#define SMACRO_NAMEOFVARIABLE(pstr, jstr, var)              xx0nameofvariable(pstr, jstr, var)

#define xx0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define SMACRO_VERIFYSIZEOF(id, sname, ssize)                xx0verifysizeof(id, sname, ssize)

#define xx0getnvid(extprefix, postfix, j)                   extprefix##postfix(j)
#define SMACRO_GETNVID(extprefix, postfix, j)               xx0getnvid(extprefix, postfix, j)



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



#define SOFFSETof_sconfig__mode                                        (SMACRO_OFF)
#define SCAPACITY_sconfig__mode                                        sizeof(eOenum08_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, _sconfig__mode) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, _sconfig__mode, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY_sconfig__mode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.sconfig.mode,
    EO_INIT(.offset)    SOFFSETof_sconfig__mode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__mode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__mode)
};
#define SOFFSETafter_sconfig__mode                                     (SOFFSETof_sconfig__mode+SCAPACITY_sconfig__mode)

#define SOFFSETof_sconfig__datarate                                        (SOFFSETafter_sconfig__mode)
#define SCAPACITY_sconfig__datarate                                        sizeof(uint8_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, _sconfig__datarate) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, _sconfig__datarate, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY_sconfig__datarate,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.sconfig.datarate,
    EO_INIT(.offset)    SOFFSETof_sconfig__datarate,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__datarate),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__datarate)
};
#define SOFFSETafter_sconfig__datarate                                     (SOFFSETof_sconfig__datarate+SCAPACITY_sconfig__datarate)

#define SOFFSETof_sconfig__signaloncefullscale                                        (SOFFSETafter_sconfig__datarate)
#define SCAPACITY_sconfig__signaloncefullscale                                        sizeof(eObool_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, _sconfig__signaloncefullscale) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, _sconfig__signaloncefullscale, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY_sconfig__signaloncefullscale,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.sconfig.signaloncefullscale,
    EO_INIT(.offset)    SOFFSETof_sconfig__signaloncefullscale,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__signaloncefullscale),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sconfig__signaloncefullscale)
};
#define SOFFSETafter_sconfig__signaloncefullscale                                     (SOFFSETof_sconfig__signaloncefullscale+SCAPACITY_sconfig__signaloncefullscale)


#define SOFFSETof_sconfig_filler01                                       (SOFFSETafter_sconfig__signaloncefullscale)
#define SCAPACITY_sconfig_filler01                                       (1)
//
#define SOFFSETafter_sconfig_filler01                                    (SOFFSETof_sconfig_filler01+SCAPACITY_sconfig_filler01)

#define SOFFSETof_sinputs_filler04                                       (SOFFSETafter_sconfig_filler01)
#define SCAPACITY_sinputs_filler04                                       (4)
//
#define SOFFSETafter_sinputs_filler04                                    (SOFFSETof_sinputs_filler04+SCAPACITY_sinputs_filler04)


#define SOFFSETof_sstatus__fullscale                                        (SOFFSETafter_sinputs_filler04)
#define SCAPACITY_sstatus__fullscale                                        sizeof(eOsnsr_arrayofupto12bytes_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, _sstatus__fullscale) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, _sstatus__fullscale, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY_sstatus__fullscale,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.sstatus.fullscale,
    EO_INIT(.offset)    SOFFSETof_sstatus__fullscale,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__fullscale),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__fullscale)
};
#define SOFFSETafter_sstatus__fullscale                                     (SOFFSETof_sstatus__fullscale+SCAPACITY_sstatus__fullscale)


#define SOFFSETof_sstatus__calibratedvalues                                        (SOFFSETafter_sstatus__fullscale)
#define SCAPACITY_sstatus__calibratedvalues                                        sizeof(eOsnsr_arrayofupto12bytes_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, _sstatus__calibratedvalues) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, _sstatus__calibratedvalues, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY_sstatus__calibratedvalues,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.sstatus.calibratedvalues,
    EO_INIT(.offset)    SOFFSETof_sstatus__calibratedvalues,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__calibratedvalues),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__calibratedvalues)
};
#define SOFFSETafter_sstatus__calibratedvalues                                     (SOFFSETof_sstatus__calibratedvalues+SCAPACITY_sstatus__calibratedvalues)

#define SOFFSETof_sstatus__uncalibratedvalues                                        (SOFFSETafter_sstatus__calibratedvalues)
#define SCAPACITY_sstatus__uncalibratedvalues                                        sizeof(eOsnsr_arrayofupto12bytes_t)
extern EOnv_con_t SMACRO_NAMEOFVARIABLE(SMACRO_PSTR, SMACRO_STR, _sstatus__uncalibratedvalues) =
{   
 
    EO_INIT(.id)        SMACRO_GETNVID(SMACRO_EXTERNALPREFIX_GETID, _sstatus__uncalibratedvalues, SMACRO_NUM),
    EO_INIT(.capacity)  SCAPACITY_sstatus__uncalibratedvalues,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue.sstatus.uncalibratedvalues,
    EO_INIT(.offset)    SOFFSETof_sstatus__uncalibratedvalues,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__uncalibratedvalues),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_as_any_con_sxx_NVFUNTYP_sstatus__uncalibratedvalues)
};
#define SOFFSETafter_sstatus__uncalibratedvalues                                     (SOFFSETof_sstatus__uncalibratedvalues+SCAPACITY_sstatus__uncalibratedvalues)



// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
SMACRO_VERIFYSIZEOF(SMACRO_NUM, eOsnsr_strain_t, SOFFSETafter_sstatus__uncalibratedvalues-SMACRO_OFF);



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
