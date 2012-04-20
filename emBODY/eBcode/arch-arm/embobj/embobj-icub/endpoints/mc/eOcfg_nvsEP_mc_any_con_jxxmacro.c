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

/* @file       eOcfg_nvsEP_mc_any_con_jxxmacro.c
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
#include "eOcfg_nvsEP_mc_any_con_jxx.h"    
#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
      




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mc_any_con_jxxmacro.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// the first xxname() does not works.
// see why at http://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan
//#define MACRO_NAMEOFVARIABLE(pstr, jstr, var)            eo_cfg_nvsEP_mc ## pstr ## jstr ## var 

#define xx0nameofvariable(pstr, jstr, var)                  eo_cfg_nvsEP_mc ## pstr ## jstr ## var
#define MACRO_NAMEOFVARIABLE(pstr, jstr, var)               xx0nameofvariable(pstr, jstr, var)

#define xx0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define MACRO_VERIFYSIZEOF(id, sname, ssize)                xx0verifysizeof(id, sname, ssize)

#define xx0getnvid(extprefix, postfix, j)                   extprefix##postfix(j)
#define MACRO_GETNVID(extprefix, postfix, j)                xx0getnvid(extprefix, postfix, j)



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



#define OFFSETof_jconfig                                        (JMACRO_JOFF)
#define CAPACITY_jconfig                                        sizeof(eOmc_joint_config_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY_jconfig,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig,
    EO_INIT(.offset)    OFFSETof_jconfig,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig)
};
#define OFFSETafter_jconfig                                     (OFFSETof_jconfig+CAPACITY_jconfig)


#define OFFSETof_jconfig__pidposition                           (OFFSETof_jconfig)
#define CAPACITY_jconfig__pidposition                           sizeof(eOmc_PID_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__pidposition) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__pidposition, JMACRO_JNUM), 
    EO_INIT(.capacity)  CAPACITY_jconfig__pidposition,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.pidposition,
    EO_INIT(.offset)    OFFSETof_jconfig__pidposition,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidposition)
};
#define OFFSETafter_jconfig__pidposition                        (OFFSETof_jconfig__pidposition+CAPACITY_jconfig__pidposition)


#define OFFSETof_jconfig__pidvelocity                           (OFFSETafter_jconfig__pidposition)
#define CAPACITY_jconfig__pidvelocity                           sizeof(eOmc_PID_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__pidvelocity) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__pidvelocity, JMACRO_JNUM),       
    EO_INIT(.capacity)  CAPACITY_jconfig__pidvelocity,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.pidvelocity,
    EO_INIT(.offset)    OFFSETof_jconfig__pidvelocity,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidvelocity)
};
#define OFFSETafter_jconfig__pidvelocity                        (OFFSETof_jconfig__pidvelocity+CAPACITY_jconfig__pidvelocity)


#define OFFSETof_jconfig__pidtorque                             (OFFSETafter_jconfig__pidvelocity)
#define CAPACITY_jconfig__pidtorque                             sizeof(eOmc_PID_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__pidtorque) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__pidtorque, JMACRO_JNUM),    
    EO_INIT(.capacity)  CAPACITY_jconfig__pidtorque,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.pidtorque,
    EO_INIT(.offset)    OFFSETof_jconfig__pidtorque,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__pidtorque)
};
#define OFFSETafter_jconfig__pidtorque                          (OFFSETof_jconfig__pidtorque+CAPACITY_jconfig__pidtorque)


#define OFFSETof_jconfig__minpositionofjoint                    (OFFSETafter_jconfig__pidtorque)
#define CAPACITY_jconfig__minpositionofjoint                    sizeof(eOmeas_position_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__minpositionofjoint) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__minpositionofjoint, JMACRO_JNUM),    
    EO_INIT(.capacity)  CAPACITY_jconfig__minpositionofjoint,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.minpositionofjoint,
    EO_INIT(.offset)    OFFSETof_jconfig__minpositionofjoint,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__minpositionofjoint)
};
#define OFFSETafter_jconfig__minpositionofjoint                (OFFSETof_jconfig__minpositionofjoint+CAPACITY_jconfig__minpositionofjoint)


#define OFFSETof_jconfig__maxpositionofjoint                    (OFFSETafter_jconfig__minpositionofjoint)
#define CAPACITY_jconfig__maxpositionofjoint                    sizeof(eOmeas_position_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__maxpositionofjoint) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__maxpositionofjoint, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY_jconfig__maxpositionofjoint,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.maxpositionofjoint,
    EO_INIT(.offset)    OFFSETof_jconfig__maxpositionofjoint,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__maxpositionofjoint)
};
#define OFFSETafter_jconfig__maxpositionofjoint                (OFFSETof_jconfig__maxpositionofjoint+CAPACITY_jconfig__maxpositionofjoint)


#define OFFSETof_jconfig__velocitysetpointtimeout              (OFFSETafter_jconfig__maxpositionofjoint)
#define CAPACITY_jconfig__velocitysetpointtimeout              sizeof(eOmeas_time_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__velocitysetpointtimeout) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__velocitysetpointtimeout, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY_jconfig__velocitysetpointtimeout,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.velocitysetpointtimeout,
    EO_INIT(.offset)    OFFSETof_jconfig__velocitysetpointtimeout,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__velocitysetpointtimeout)
};
#define OFFSETafter_jconfig__velocitysetpointtimeout            (OFFSETof_jconfig__velocitysetpointtimeout+CAPACITY_jconfig__velocitysetpointtimeout)


#define OFFSETof_jconfig__upto02descrforchameleon02            (OFFSETafter_jconfig__velocitysetpointtimeout)
#define CAPACITY_jconfig__upto02descrforchameleon02            (2*sizeof(eOutil_chameleon_descriptor_t))
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, _jconfig__upto02descrforchameleon02) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, _jconfig__upto02descrforchameleon02, JMACRO_JNUM),    
    EO_INIT(.capacity)  CAPACITY_jconfig__upto02descrforchameleon02,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig.upto02descrforchameleon02,
    EO_INIT(.offset)    OFFSETof_jconfig__upto02descrforchameleon02,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP_jconfig__upto02descrforchameleon02)
};
#define OFFSETafter_jconfig__upto02descrforchameleon02          (OFFSETof_jconfig__upto02descrforchameleon02+CAPACITY_jconfig__upto02descrforchameleon02)


#define OFFSETof_jconfig__filler04                              (OFFSETafter_jconfig__upto02descrforchameleon02)
#define CAPACITY_jconfig__filler04                              (4)
//
#define OFFSETafter_jconfig__filler04                           OFFSETof_jconfig__filler04+CAPACITY_jconfig__filler04


// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
MACRO_VERIFYSIZEOF(JMACRO_JNUM, eOmc_joint_config_t, OFFSETafter_jconfig__filler04-JMACRO_JOFF);


#define OFFSETof__jstatus                                        (OFFSETafter_jconfig)
#define CAPACITY__jstatus                                        sizeof(eOmc_joint_status_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __jstatus) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __jstatus, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__jstatus,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jstatus,
    EO_INIT(.offset)    OFFSETof__jstatus,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__jstatus),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__jstatus)
};
#define OFFSETafter__jstatus                                     (OFFSETof__jstatus+CAPACITY__jstatus)

// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
//MACRO_VERIFYSIZEOF(JMACRO_JNUM, eOmc_joint_status_t, OFFSETafter_jstatus-OFFSETafter_jconfig__filler04-JMACRO_JOFF);


#define OFFSETof__calibrator                                        (OFFSETafter__jstatus)
#define CAPACITY__calibrator                                        sizeof(eOmc_calibrator_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __calibrator) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __calibrator, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__calibrator,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.calibrator,
    EO_INIT(.offset)    OFFSETof__calibrator,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__calibrator),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__calibrator)
};
#define OFFSETafter__calibrator                                     (OFFSETof__calibrator+CAPACITY__calibrator)


#define OFFSETof__setpoint                                        (OFFSETafter__calibrator)
#define CAPACITY__setpoint                                        sizeof(eOmc_setpoint_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __setpoint) =
{   
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __setpoint, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__setpoint,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.setpoint,
    EO_INIT(.offset)    OFFSETof__setpoint,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__setpoint),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__setpoint)
};
#define OFFSETafter__setpoint                                     (OFFSETof__setpoint+CAPACITY__setpoint)


#define OFFSETof__controlmode                                    (OFFSETafter__setpoint)
#define CAPACITY__controlmode                                     sizeof(eOenum08_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __controlmode) =
{       
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __controlmode, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__controlmode,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.controlmode,
    EO_INIT(.offset)    OFFSETof__controlmode,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__controlmode),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__controlmode)
};
#define OFFSETafter__controlmode                                     (OFFSETof__controlmode+CAPACITY__controlmode)


#define OFFSETof__signalwhenmotionisdone                         (OFFSETafter__controlmode)
#define CAPACITY__signalwhenmotionisdone                         sizeof(eObool_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __signalwhenmotionisdone) =
{    
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __signalwhenmotionisdone, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__signalwhenmotionisdone,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.signalwhenmotionisdone,
    EO_INIT(.offset)    OFFSETof__signalwhenmotionisdone,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__signalwhenmotionisdone),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__signalwhenmotionisdone)
};
#define OFFSETafter__signalwhenmotionisdone                                     (OFFSETof__signalwhenmotionisdone+CAPACITY__signalwhenmotionisdone)


#define OFFSETof__motionisdone                                          (OFFSETafter__signalwhenmotionisdone)
#define CAPACITY__motionisdone                                          sizeof(eObool_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __motionisdone) =
{    
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __motionisdone, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__motionisdone,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.motionisdone,
    EO_INIT(.offset)    OFFSETof__motionisdone,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__motionisdone),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__motionisdone)
};
#define OFFSETafter__motionisdone                                       (OFFSETof__motionisdone+CAPACITY__motionisdone)



#define OFFSETof_filler01                                               (OFFSETafter__motionisdone)
#define CAPACITY_filler01                                               (1)
//
#define OFFSETafter_filler01                                            (OFFSETof_filler01+CAPACITY_filler01)


#define OFFSETof__externalvalueoftorque                                 (OFFSETafter_filler01)
#define CAPACITY__externalvalueoftorque                                 sizeof(eOmeas_torque_t)
extern EOnv_con_t MACRO_NAMEOFVARIABLE(JMACRO_PSTR, JMACRO_JSTR, __externalvalueoftorque) =
{    
    EO_INIT(.id)        MACRO_GETNVID(JMACRO_EXTERNALPREFIX_GETID, __externalvalueoftorque, JMACRO_JNUM),
    EO_INIT(.capacity)  CAPACITY__externalvalueoftorque,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.externalvalueoftorque,
    EO_INIT(.offset)    OFFSETof__externalvalueoftorque,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__externalvalueoftorque),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_jxx_NVFUNTYP__externalvalueoftorque)
};
#define OFFSETafter__externalvalueoftorque                              (OFFSETof__externalvalueoftorque+CAPACITY__externalvalueoftorque)



#define OFFSETof_filler06                                           (OFFSETafter__externalvalueoftorque)
#define CAPACITY_filler06                                           (6)
//
#define OFFSETafter_filler06                                        (OFFSETof_filler06+CAPACITY_filler06)

// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
MACRO_VERIFYSIZEOF(JMACRO_JNUM, eOmc_joint_t, OFFSETafter_filler06-JMACRO_JOFF);



#undef JMACRO_PSTR 
#undef JMACRO_JSTR 
#undef JMACRO_PNUM
#undef JMACRO_JNUM  
#undef JMACRO_JOFF

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
