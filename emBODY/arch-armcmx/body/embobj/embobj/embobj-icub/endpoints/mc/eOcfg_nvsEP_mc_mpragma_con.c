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

/* @file       eOcfg_nvsEP_mc_jpragma_con.c
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

#include "EoMotionControl.h"
#include "eOcfg_nvsEP_mc_jxx_con.h"




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
//#define xxxname(pstr, jstr, var)            eo_cfg_nvsEP_mc ## pstr ## jstr ## var 
#define xx0name(pstr, jstr, var)            eo_cfg_nvsEP_mc ## pstr ## jstr ## var
#define xxxname(pstr, jstr, var)            xx0name(pstr, jstr, var)

#define xx0VERIFYsizeof(id, sname, ssize)   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define xxxVERIFYsizeof(id, sname, ssize)   xx0VERIFYsizeof(id, sname, ssize)




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



#define OFFSETof_jconfig                                        (JPRAGMA_JOFF)
#define CAPACITY_jconfig                                        sizeof(eOmc_joint_config_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_mxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig),
    EO_INIT(.capacity)  CAPACITY_jconfig,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig,
    EO_INIT(.offset)    OFFSETof_jconfig,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig)
};
#define OFFSETafter_jconfig                                     (OFFSETof_jconfig+CAPACITY_jconfig)


#define OFFSETof_jconfig__pidposition                           (OFFSETof_jconfig)
#define CAPACITY_jconfig__pidposition                           sizeof(eOmc_PID_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__pidposition) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__pidposition),
    EO_INIT(.capacity)  CAPACITY_jconfig__pidposition,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.pidposition,
    EO_INIT(.offset)    OFFSETof_jconfig__pidposition,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__pidposition),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__pidposition)
};
#define OFFSETafter_jconfig__pidposition                        (OFFSETof_jconfig__pidposition+CAPACITY_jconfig__pidposition)


#define OFFSETof_jconfig__pidvelocity                           (OFFSETafter_jconfig__pidposition)
#define CAPACITY_jconfig__pidvelocity                           sizeof(eOmc_PID_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__pidvelocity) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__pidvelocity),
    EO_INIT(.capacity)  CAPACITY_jconfig__pidvelocity,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.pidvelocity,
    EO_INIT(.offset)    OFFSETof_jconfig__pidvelocity,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__pidvelocity),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__pidvelocity)
};
#define OFFSETafter_jconfig__pidvelocity                        (OFFSETof_jconfig__pidvelocity+CAPACITY_jconfig__pidvelocity)


#define OFFSETof_jconfig__pidtorque                             (OFFSETafter_jconfig__pidvelocity)
#define CAPACITY_jconfig__pidtorque                             sizeof(eOmc_PID_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__pidtorque) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__pidtorque),
    EO_INIT(.capacity)  CAPACITY_jconfig__pidtorque,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.pidtorque,
    EO_INIT(.offset)    OFFSETof_jconfig__pidtorque,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__pidtorque),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__pidtorque)
};
#define OFFSETafter_jconfig__pidtorque                          (OFFSETof_jconfig__pidtorque+CAPACITY_jconfig__pidtorque)


#define OFFSETof_jconfig__minpositionofjoint                    (OFFSETafter_jconfig__pidtorque)
#define CAPACITY_jconfig__minpositionofjoint                    sizeof(eOmeas_position_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__minpositionofjoint) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__minpositionofjoint),
    EO_INIT(.capacity)  CAPACITY_jconfig__minpositionofjoint,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.minpositionofjoint,
    EO_INIT(.offset)    OFFSETof_jconfig__minpositionofjoint,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__minpositionofjoint),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__minpositionofjoint)
};
#define OFFSETafter_jconfig__minpositionofjoint                (OFFSETof_jconfig__minpositionofjoint+CAPACITY_jconfig__minpositionofjoint)


#define OFFSETof_jconfig__maxpositionofjoint                    (OFFSETafter_jconfig__minpositionofjoint)
#define CAPACITY_jconfig__maxpositionofjoint                    sizeof(eOmeas_position_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__maxpositionofjoint) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__maxpositionofjoint),
    EO_INIT(.capacity)  CAPACITY_jconfig__maxpositionofjoint,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.maxpositionofjoint,
    EO_INIT(.offset)    OFFSETof_jconfig__maxpositionofjoint,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__maxpositionofjoint),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__maxpositionofjoint)
};
#define OFFSETafter_jconfig__maxpositionofjoint                (OFFSETof_jconfig__maxpositionofjoint+CAPACITY_jconfig__maxpositionofjoint)


#define OFFSETof_jconfig__velocitysetpointtimeout              (OFFSETafter_jconfig__maxpositionofjoint)
#define CAPACITY_jconfig__velocitysetpointtimeout              sizeof(eOmeas_time_t)
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__velocitysetpointtimeout) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__velocitysetpointtimeout),
    EO_INIT(.capacity)  CAPACITY_jconfig__velocitysetpointtimeout,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.velocitysetpointtimeout,
    EO_INIT(.offset)    OFFSETof_jconfig__velocitysetpointtimeout,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout)
};
#define OFFSETafter_jconfig__velocitysetpointtimeout            (OFFSETof_jconfig__velocitysetpointtimeout+CAPACITY_jconfig__velocitysetpointtimeout)


#define OFFSETof_jconfig__upto02descrforchameleon02            (OFFSETafter_jconfig__velocitysetpointtimeout)
#define CAPACITY_jconfig__upto02descrforchameleon02            (2*sizeof(eOutil_chameleon_descriptor_t))
extern EOnv_con_t xxxname(JPRAGMA_PSTR, JPRAGMA_JSTR, _jconfig__upto02descrforchameleon02) =
{   
    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__upto02descrforchameleon02),
    EO_INIT(.capacity)  CAPACITY_jconfig__upto02descrforchameleon02,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.upto02descrforchameleon02,
    EO_INIT(.offset)    OFFSETof_jconfig__upto02descrforchameleon02,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02)
};
#define OFFSETafter_jconfig__upto02descrforchameleon02          (OFFSETof_jconfig__upto02descrforchameleon02+CAPACITY_jconfig__upto02descrforchameleon02)


#define OFFSETof_jconfig__filler04                              (OFFSETafter_jconfig__upto02descrforchameleon02)
#define CAPACITY_jconfig__filler04                              (4)
//extern EOnv_con_t eo_cfg_nvsEP_mc_##JPRAGMA_PSTR_j##JPRAGMA_JSTR_jconfig__filler04 =
//{   
//    EO_INIT(.id)        EOK_cfg_nvsEP_mc_jxx_NVID(JPRAGMA_PNUM, JPRAGMA_JNUM, EOK_cfg_nvsEP_mc_jxx_NVID_jconfig__filler04),
//    EO_INIT(.capacity)  CAPACITY_jconfig__filler04,
//    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_jxx_default.jconfig.filler04,
//    EO_INIT(.offset)    OFFSETof_jconfig__filler04,
//    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__filler04),
//    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_jxx_NVFUNTYP_jconfig__filler04)
//};
#define OFFSETafter_jconfig__filler04                       OFFSETof_jconfig__filler04+CAPACITY_jconfig__filler04



// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
xxxVERIFYsizeof(JPRAGMA_JNUM, eOmc_joint_config_t, OFFSETafter_jconfig__filler04-JPRAGMA_JOFF);


#undef JPRAGMA_PSTR 
#undef JPRAGMA_JSTR 
#undef JPRAGMA_PNUM
#undef JPRAGMA_JNUM  
#undef JPRAGMA_JOFF

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
