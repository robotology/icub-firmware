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
#include "eOcfg_nvsEP_mc_any_con_mxx.h"    
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"
      




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
//#define MMACRO_NAMEOFVARIABLE(pstr, mstr, var)            eo_cfg_nvsEP_mc ## pstr ## mstr ## var 

#define yy0nameofvariable(pstr, mstr, var)                  eo_cfg_nvsEP_mc ## pstr ## mstr ## var
#define MMACRO_NAMEOFVARIABLE(pstr, mstr, var)              yy0nameofvariable(pstr, mstr, var)

#define yy0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define MMACRO_VERIFYSIZEOF(id, sname, ssize)               yy0verifysizeof(id, sname, ssize)

#define yy0getnvid(extprefix, postfix, m)                   extprefix##postfix(m)
#define MMACRO_GETNVID(extprefix, postfix, m)               yy0getnvid(extprefix, postfix, m)



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



#define OFFSETof_mconfig                                        (MMACRO_MOFF)
#define CAPACITY_mconfig                                         sizeof(eOmc_motor_config_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mconfig) =
{   
 
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig, MMACRO_MNUM),
    EO_INIT(.capacity)  CAPACITY_mconfig,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mconfig,
    EO_INIT(.offset)    OFFSETof_mconfig,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig)
};
#define OFFSETafter_mconfig                                     (OFFSETof_mconfig+CAPACITY_mconfig)



#define OFFSETof_mconfig__pidcurrent                            (OFFSETof_mconfig)
#define CAPACITY_mconfig__pidcurrent                            sizeof(eOmc_PID_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mconfig__pidcurrent) =
{   
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__pidcurrent, MMACRO_MNUM), 
    EO_INIT(.capacity)  CAPACITY_mconfig__pidcurrent,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mconfig.pidcurrent,
    EO_INIT(.offset)    OFFSETof_mconfig__pidcurrent,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__pidcurrent),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__pidcurrent)
};
#define OFFSETafter_mconfig__pidcurrent                        (OFFSETof_mconfig__pidcurrent+CAPACITY_mconfig__pidcurrent)



#define OFFSETof_mconfig__maxvelocityofmotor                    (OFFSETafter_mconfig__pidcurrent)
#define CAPACITY_mconfig__maxvelocityofmotor                    sizeof(eOmeas_velocity_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mconfig__maxvelocityofmotor) =
{   
    //EO_INIT(.id)        EOK_cfg_nvsEP_mc_NVID_mconfig__maxvelocityofmotor(MMACRO_MNUM),  
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__maxvelocityofmotor, MMACRO_MNUM),    
    EO_INIT(.capacity)  CAPACITY_mconfig__maxvelocityofmotor,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mconfig.maxvelocityofmotor,
    EO_INIT(.offset)    OFFSETof_mconfig__maxvelocityofmotor,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxvelocityofmotor),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxvelocityofmotor)
};
#define OFFSETafter_mconfig__maxvelocityofmotor                (OFFSETof_mconfig__maxvelocityofmotor+CAPACITY_mconfig__maxvelocityofmotor)


#define OFFSETof_mconfig__maxcurrentofmotor                    (OFFSETafter_mconfig__maxvelocityofmotor)
#define CAPACITY_mconfig__maxcurrentofmotor                    sizeof(eOmeas_current_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mconfig__maxcurrentofmotor) =
{   
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__maxcurrentofmotor, MMACRO_MNUM),    
    EO_INIT(.capacity)  CAPACITY_mconfig__maxcurrentofmotor,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mconfig.maxcurrentofmotor,
    EO_INIT(.offset)    OFFSETof_mconfig__maxcurrentofmotor,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxcurrentofmotor),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__maxcurrentofmotor)
};
#define OFFSETafter_mconfig__maxcurrentofmotor                (OFFSETof_mconfig__maxcurrentofmotor+CAPACITY_mconfig__maxcurrentofmotor)


#define OFFSETof_mconfig__des02FORmstatuschamaleon04            (OFFSETafter_mconfig__maxcurrentofmotor)
#define CAPACITY_mconfig__des02FORmstatuschamaleon04            (2*sizeof(eOutil_chameleon_descr_t))
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mconfig__des02FORmstatuschamaleon04) =
{   
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mconfig__des02FORmstatuschamaleon04, MMACRO_MNUM),    
    EO_INIT(.capacity)  CAPACITY_mconfig__des02FORmstatuschamaleon04,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mconfig.des02FORmstatuschamaleon04,
    EO_INIT(.offset)    OFFSETof_mconfig__des02FORmstatuschamaleon04,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__des02FORmstatuschamaleon04),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mconfig__des02FORmstatuschamaleon04)
};
#define OFFSETafter_mconfig__des02FORmstatuschamaleon04          (OFFSETof_mconfig__des02FORmstatuschamaleon04+CAPACITY_mconfig__des02FORmstatuschamaleon04)


// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
MMACRO_VERIFYSIZEOF(MMACRO_MNUM, eOmc_motor_config_t, OFFSETafter_mconfig__des02FORmstatuschamaleon04-MMACRO_MOFF);


#define OFFSETof_mstatus                                        (OFFSETafter_mconfig)
#define CAPACITY_mstatus                                        sizeof(eOmc_motor_status_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mstatus) =
{   
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mstatus, MMACRO_MNUM),
    EO_INIT(.capacity)  CAPACITY_mstatus,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mstatus,
    EO_INIT(.offset)    OFFSETof_mstatus,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus)
};
#define OFFSETafter_mstatus                                     (OFFSETof_mstatus+CAPACITY_mstatus)


#define OFFSETof_mstatus__basic                                        (OFFSETafter_mconfig)
#define CAPACITY_mstatus__basic                                        sizeof(eOmc_motor_status_basic_t)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mstatus__basic) =
{   
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mstatus__basic, MMACRO_MNUM),
    EO_INIT(.capacity)  CAPACITY_mstatus__basic,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mstatus.basic,
    EO_INIT(.offset)    OFFSETof_mstatus__basic,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__basic),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__basic)
};
#define OFFSETafter_mstatus__basic                                     (OFFSETof_mstatus__basic+CAPACITY_mstatus__basic)


#define OFFSETof_mstatus__chamaleon04                                        (OFFSETafter_mstatus__basic)
#define CAPACITY_mstatus__chamaleon04                                        (4)
EOnv_con_t MMACRO_NAMEOFVARIABLE(MMACRO_PSTR, MMACRO_MSTR, _mstatus__chamaleon04) =
{   
    EO_INIT(.id)        MMACRO_GETNVID(MMACRO_EXTERNALPREFIX_GETID, _mstatus__chamaleon04, MMACRO_MNUM),
    EO_INIT(.capacity)  CAPACITY_mstatus__chamaleon04,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mstatus.chamaleon04,
    EO_INIT(.offset)    OFFSETof_mstatus__chamaleon04,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__chamaleon04),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_mxx_NVFUNTYP_mstatus__chamaleon04)
};
#define OFFSETafter_mstatus__chamaleon04                                     (OFFSETof_mstatus__chamaleon04+CAPACITY_mstatus__chamaleon04)

// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
MMACRO_VERIFYSIZEOF(MMACRO_MNUM, eOmc_motor_t, OFFSETafter_mstatus__chamaleon04-MMACRO_MOFF);



#undef MMACRO_PSTR 
#undef MMACRO_MSTR 
#undef MMACRO_PNUM
#undef MMACRO_MNUM  
#undef MMACRO_MOFF

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
