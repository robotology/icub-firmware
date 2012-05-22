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

/* @file       eOcfg_nvsEP_mc_any_con_cxxmacro.c
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
#include "eOcfg_nvsEP_mc_any_con_cxx.h"    
#include "eOcfg_nvsEP_mc_any_con_cxxdefault.h"
      


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
//#define CMACRO_NAMEOFVARIABLE(pstr, mstr, var)            eo_cfg_nvsEP_mc ## pstr ## mstr ## var 

#define yy0nameofvariable(pstr, mstr, var)                  eo_cfg_nvsEP_mc ## pstr ## mstr ## var
#define CMACRO_NAMEOFVARIABLE(pstr, mstr, var)              yy0nameofvariable(pstr, mstr, var)

#define yy0verifysizeof(id, sname, ssize)                   typedef uint8_t GUARD##id##sname[ ( ssize == sizeof(sname) ) ? (1) : (0)];
#define CMACRO_VERIFYSIZEOF(id, sname, ssize)               yy0verifysizeof(id, sname, ssize)

#define yy0getnvid(extprefix, postfix, m)                   extprefix##postfix(m)
#define CMACRO_GETNVID(extprefix, postfix, m)               yy0getnvid(extprefix, postfix, m)

#define aa0const(extprefix, postfix)                        extprefix##postfix
#define CMACRO_CONST(extprefix, postfix)                    aa0const(extprefix, postfix)


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



#define OFFSETof_cconfig                                        (CMACRO_COFF)
#define CAPACITY_cconfig                                         sizeof(eOmc_controller_config_t)
extern EOnv_con_t CMACRO_NAMEOFVARIABLE(CMACRO_PSTR, CMACRO_CSTR, _cconfig) =
{   
 
    EO_INIT(.id)        CMACRO_GETNVID(CMACRO_EXTERNALPREFIX_GETID, _cconfig, CMACRO_CNUM),
    EO_INIT(.capacity)  CAPACITY_cconfig,
    //EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_cxxdefault_defaultvalue.cconfig,
    EO_INIT(.resetval)  (const void*)&CMACRO_CONST(CMACRO_EXTERNALPREFIX_CONST, .cconfig),
    EO_INIT(.offset)    OFFSETof_cconfig,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cconfig),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cconfig)
};
#define OFFSETafter_cconfig                                     (OFFSETof_cconfig+CAPACITY_cconfig)



#define OFFSETof_cconfig__durationofctrlloop                            (OFFSETof_cconfig)
#define CAPACITY_cconfig__durationofctrlloop                            sizeof(eOreltime_t)
extern EOnv_con_t CMACRO_NAMEOFVARIABLE(CMACRO_PSTR, CMACRO_CSTR, _cconfig__durationofctrlloop) =
{   
    EO_INIT(.id)        CMACRO_GETNVID(CMACRO_EXTERNALPREFIX_GETID, _cconfig__durationofctrlloop, CMACRO_CNUM), 
    EO_INIT(.capacity)  CAPACITY_cconfig__durationofctrlloop,
    //EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_cxxdefault_defaultvalue.cconfig.durationofctrlloop,
    EO_INIT(.resetval)  (const void*)&CMACRO_CONST(CMACRO_EXTERNALPREFIX_CONST, .cconfig.durationofctrlloop),
    EO_INIT(.offset)    OFFSETof_cconfig__durationofctrlloop,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cconfig__durationofctrlloop),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cconfig__durationofctrlloop)
};
#define OFFSETafter_cconfig__durationofctrlloop                        (OFFSETof_cconfig__durationofctrlloop+CAPACITY_cconfig__durationofctrlloop)



#define OFFSETof_cconfig__filler04                    (OFFSETafter_cconfig__durationofctrlloop)
#define CAPACITY_cconfig__filler04                    (4)
//
#define OFFSETafter_cconfig__filler04                (OFFSETof_cconfig__filler04+CAPACITY_cconfig__filler04)


#define OFFSETof_cstatus                            (OFFSETafter_cconfig)
#define CAPACITY_cstatus                            sizeof(eOmc_controller_status_t)
extern EOnv_con_t CMACRO_NAMEOFVARIABLE(CMACRO_PSTR, CMACRO_CSTR, _cstatus) =
{   
    EO_INIT(.id)        CMACRO_GETNVID(CMACRO_EXTERNALPREFIX_GETID, _cstatus, CMACRO_CNUM),    
    EO_INIT(.capacity)  CAPACITY_cstatus,
    //EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_cxxdefault_defaultvalue.cstatus,
    EO_INIT(.resetval)  (const void*)&CMACRO_CONST(CMACRO_EXTERNALPREFIX_CONST, .cstatus),
    EO_INIT(.offset)    OFFSETof_cstatus,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cstatus),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cstatus)
};
#define OFFSETafter_cstatus                (OFFSETof_cstatus+CAPACITY_cstatus)


#define OFFSETof_cstatus__alljomoinitted            (OFFSETof_cstatus)
#define CAPACITY_cstatus__alljomoinitted            (sizeof(eObool_t))
extern EOnv_con_t CMACRO_NAMEOFVARIABLE(CMACRO_PSTR, CMACRO_CSTR, _cstatus__alljomoinitted) =
{   
    EO_INIT(.id)        CMACRO_GETNVID(CMACRO_EXTERNALPREFIX_GETID, _cstatus__alljomoinitted, CMACRO_CNUM),    
    EO_INIT(.capacity)  CAPACITY_cstatus__alljomoinitted,
    //EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_cxxdefault_defaultvalue.cstatus.alljomoinitted,
    EO_INIT(.resetval)  (const void*)&CMACRO_CONST(CMACRO_EXTERNALPREFIX_CONST, .cstatus.alljomoinitted),
    EO_INIT(.offset)    OFFSETof_cstatus__alljomoinitted,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cstatus__alljomoinitted),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_cstatus__alljomoinitted)
};
#define OFFSETafter_cstatus__alljomoinitted          (OFFSETof_cstatus__alljomoinitted+CAPACITY_cstatus__alljomoinitted)


#define OFFSETof_cstatus_OTHERS                    (OFFSETafter_cstatus__alljomoinitted)
#define CAPACITY_cstatus_OTHERS                    (7)
//
#define OFFSETafter_cstatus_OTHERS                (OFFSETof_cstatus_OTHERS+CAPACITY_cstatus_OTHERS)


#define OFFSETof_ccmmnds__go2stateofcontroller                                        (OFFSETafter_cstatus)
#define CAPACITY_ccmmnds__go2stateofcontroller                                        sizeof(eOenum08_t)
extern EOnv_con_t CMACRO_NAMEOFVARIABLE(CMACRO_PSTR, CMACRO_CSTR, _ccmmnds__go2stateofcontroller) =
{   
    EO_INIT(.id)        CMACRO_GETNVID(CMACRO_EXTERNALPREFIX_GETID, _ccmmnds__go2stateofcontroller, CMACRO_CNUM),
    EO_INIT(.capacity)  CAPACITY_ccmmnds__go2stateofcontroller,
    //EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_mc_any_con_cxxdefault_defaultvalue.ccmmnds.go2stateofcontroller,
    EO_INIT(.resetval)  (const void*)&CMACRO_CONST(CMACRO_EXTERNALPREFIX_CONST, .ccmmnds.go2stateofcontroller),
    EO_INIT(.offset)    OFFSETof_ccmmnds__go2stateofcontroller,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_ccmmnds__go2stateofcontroller),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_mc_any_con_cxx_NVFUNTYP_ccmmnds__go2stateofcontroller)
};
#define OFFSETafter_ccmmnds__go2stateofcontroller                                     (OFFSETof_ccmmnds__go2stateofcontroller+CAPACITY_ccmmnds__go2stateofcontroller)


#define OFFSETof_cstatus_filler07                       (OFFSETafter_ccmmnds__go2stateofcontroller)
#define CAPACITY_cstatus_filler07                       (7)
//
#define OFFSETafter_cstatus_filler07                    (OFFSETof_cstatus_filler07+CAPACITY_cstatus_filler07)


// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
CMACRO_VERIFYSIZEOF(CMACRO_CNUM, eOmc_controller_t, OFFSETafter_cstatus_filler07-CMACRO_COFF);



#undef CMACRO_PSTR 
#undef CMACRO_CSTR 
#undef CMACRO_PNUM
#undef CMACRO_CNUM  
#undef CMACRO_COFF

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
