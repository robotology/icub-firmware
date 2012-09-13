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

/* @file       eOcfg_nvsEP_mc_lowerarm_con.c
    @brief      This file keeps ....
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
#include "EOnv_hid.h"
#include "EOtreenode_hid.h"
#include "EOconstvector_hid.h"


#include "eOcfg_nvsEP_mc_any_con_bodypart_hid.h"

#include "eOcfg_nvsEP_mc_any_con_jxx.h"    
#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h" 

#include "eOcfg_nvsEP_mc_any_con_mxx.h"    
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h" 

#include "eOcfg_nvsEP_mc_any_con_cxx.h"    
#include "eOcfg_nvsEP_mc_any_con_cxxdefault.h" 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_lowerarm_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//the number of joints used in the endpoint
#define EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS                                        (jointLowerArm_TOTALnumber)
//the number of motors used in the endpoint
#define EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS                                        (motorLowerArm_TOTALnumber)

//the total number of nvs in the endpoint
#define EOK_cfg_nvsEP_lowerarm_NUMofVARS                                             (controllerNVindex_TOTALnumber+(EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS)+(EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS))



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hash(uint16_t id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOmc_controller_t s_eo_cfg_nvsEP_mc_lowerarm_con_defaultvalue =
{
    EO_INIT(.cconfig)             
    {
        EO_INIT(.durationofctrlloop)        EOK_reltime1ms,
        EO_INIT(.filler04)                  {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.cstatus)                      
    {
        EO_INIT(.alljomoinitted)            eobool_false,
        EO_INIT(.numofjoints)               EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS,
        EO_INIT(.numofmotors)               EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS,
        EO_INIT(.stateofcontroller)         eomc_stateofcontroller_config,
        EO_INIT(.flagsinittedjoints)        0x0000,
        EO_INIT(.flagsinittedmotors)        0x0000   
    },
    EO_INIT(.ccmmnds)                      
    {
        EO_INIT(.go2stateofcontroller)      eomc_stateofcontroller_config,
        EO_INIT(.filler07)                  {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7}   
    }    
}; 

// -- the 12 joints

#define JMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx


// joint 00
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j00
#define JMACRO_PNUM    5
#define JMACRO_JNUM    0    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 01
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j01
#define JMACRO_PNUM    5
#define JMACRO_JNUM    1    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"



// joint 02
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j02
#define JMACRO_PNUM    5
#define JMACRO_JNUM    2    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 03
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j03
#define JMACRO_PNUM    5
#define JMACRO_JNUM    3    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 04
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j04
#define JMACRO_PNUM    5
#define JMACRO_JNUM    4    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 05
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j05
#define JMACRO_PNUM    5
#define JMACRO_JNUM    5    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 06
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j06
#define JMACRO_PNUM    5
#define JMACRO_JNUM    6    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 07
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j07
#define JMACRO_PNUM    5
#define JMACRO_JNUM    7    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 08
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j08
#define JMACRO_PNUM    5
#define JMACRO_JNUM    8    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 09
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j09
#define JMACRO_PNUM    5
#define JMACRO_JNUM    9    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 10
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j10
#define JMACRO_PNUM    5
#define JMACRO_JNUM    10    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 11
#define JMACRO_PSTR    _lowerarm
#define JMACRO_JSTR    _j11
#define JMACRO_PNUM    5
#define JMACRO_JNUM    11    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


#define OFFSET_OF_END_OF_JOINTS    (EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS*sizeof(eOmc_joint_t))


// -- the 12 motors

#define MMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx


// motor 00
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m00
#define MMACRO_PNUM    5
#define MMACRO_MNUM    0    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 01
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m01
#define MMACRO_PNUM    5
#define MMACRO_MNUM    1    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"



// motor 02
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m02
#define MMACRO_PNUM    5
#define MMACRO_MNUM    2    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 03
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m03
#define MMACRO_PNUM    5
#define MMACRO_MNUM    3    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 04
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m04
#define MMACRO_PNUM    5
#define MMACRO_MNUM    4    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 05
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m05
#define MMACRO_PNUM    5
#define MMACRO_MNUM    5    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 06
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m06
#define MMACRO_PNUM    5
#define MMACRO_MNUM    6    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 07
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m07
#define MMACRO_PNUM    5
#define MMACRO_MNUM    7    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 08
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m08
#define MMACRO_PNUM    5
#define MMACRO_MNUM    8    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 09
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m09
#define MMACRO_PNUM    5
#define MMACRO_MNUM    9    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 10
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m10
#define MMACRO_PNUM    5
#define MMACRO_MNUM    10    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 11
#define MMACRO_PSTR    _lowerarm
#define MMACRO_MSTR    _m11
#define MMACRO_PNUM    5
#define MMACRO_MNUM    11    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


#define OFFSET_OF_END_OF_MOTORS    (OFFSET_OF_END_OF_JOINTS+EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS*sizeof(eOmc_motor_t))


// the controller

#define CMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00

#define CMACRO_EXTERNALPREFIX_CONST                s_eo_cfg_nvsEP_mc_lowerarm_con_

#define CMACRO_PSTR    _lowerarm
#define CMACRO_CSTR    _c00
#define CMACRO_PNUM    5
#define CMACRO_CNUM    0    
#define CMACRO_COFF    (OFFSET_OF_END_OF_MOTORS+CMACRO_CNUM*sizeof(eOmc_controller_t))

#include "eOcfg_nvsEP_mc_any_con_cxxmacro.c"



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const eOmc_joint_t* eo_cfg_nvsEP_mc_lowerarm_joint_defaultvalue = &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue;
const eOmc_motor_t* eo_cfg_nvsEP_mc_lowerarm_motor_defaultvalue = &eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue;
const eOmc_controller_t* eo_cfg_nvsEP_mc_lowerarm_controller_defaultvalue = &s_eo_cfg_nvsEP_mc_lowerarm_con_defaultvalue;


// EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber is 12 and keeps the number of nvs per joint
#define Jnvs    (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber)

// EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS is 12 and is the number of joints in the lowerarm
#define JNUM    (EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS)


// it is the start of index for the joints
#define Jstart  (0)

// J004p is the index of the first nv of the joint 4.
#define J00p    (Jstart + 0)
#define J01p    (Jstart + 1*Jnvs)
#define J02p    (Jstart + 2*Jnvs)
#define J03p    (Jstart + 3*Jnvs)
#define J04p    (Jstart + 4*Jnvs)
#define J05p    (Jstart + 5*Jnvs)

#define Jindex(j, i)    (Jstart + (j)*Jnvs + (i))

#define Jndx06(j, i)    (Jstart + (6+j)*Jnvs + (i))




// EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber is 6 and keeps teh number of nvs per motor
#define Mnvs    (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber)

// EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS is 6 and is the number of motors in the lowerarm
#define MNUM     (EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS)


// it is the start of index for the motors
#define Mstart  (Jnvs*JNUM+0)

#define M00p    (Mstart + 0)
#define M01p    (Mstart + 1*Mnnv)
#define M02p    (Mstart + 2*Mnvs)
#define M03p    (Mstart + 3*Mnvs)
#define M04p    (Mstart + 4*Mnvs)
#define M05p    (Mstart + 5*Mnvs)

#define Mindex(m, i)   (Mstart + (m)*Mnvs + (i))

#define Mndx06(m, i)   (Mstart + (6+m)*Mnvs + (i))

// it is the start of index for the configuration
#define Cstart          (Mstart+Mnvs*MNUM+0)
#define Cindex(i)       (Cstart + (i))


EOtreenode eo_cfg_nvsEP_mc_lowerarm_tree_con[] =
{
    // joint-j00
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig,
        EO_INIT(.index)     Jindex( 0, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 0, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 0, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 0, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__impedance,
            EO_INIT(.index)     Jindex( 0, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 0, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 0, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 0, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 0, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 0, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 0,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 0,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 0,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 0,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 0,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jstatus,
        EO_INIT(.index)     Jindex( 0,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jstatus__basic,
            EO_INIT(.index)     Jindex( 0,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 0,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 0,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 0,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 0,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 0,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 0,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 0,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 0,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 0,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 0,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j00_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 0,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    // joint-j01
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig,
        EO_INIT(.index)     Jindex( 1, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 1, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 1, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 1, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__impedance,
            EO_INIT(.index)     Jindex( 1, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 1, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 1, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 1, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 1, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 1, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 1,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 1,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 1,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 1,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 1,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jstatus,
        EO_INIT(.index)     Jindex( 1,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jstatus__basic,
            EO_INIT(.index)     Jindex( 1,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 1,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 1,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 1,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 1,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 1,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 1,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 1,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 1,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 1,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 1,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j01_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 1,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    

    // joint-j02
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig,
        EO_INIT(.index)     Jindex( 2, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 2, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 2, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 2, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__impedance,
            EO_INIT(.index)     Jindex( 2, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 2, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 2, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 2, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 2, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 2, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 2,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 2,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 2,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 2,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 2,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jstatus,
        EO_INIT(.index)     Jindex( 2,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jstatus__basic,
            EO_INIT(.index)     Jindex( 2,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 2,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 2,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 2,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 2,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 2,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 2,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 2,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 2,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 2,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 2,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j02_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 2,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    


    // joint-j03
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig,
        EO_INIT(.index)     Jindex( 3, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 3, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 3, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 3, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__impedance,
            EO_INIT(.index)     Jindex( 3, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 3, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 3, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 3, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 3, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 3, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 3,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 3,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 3,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 3,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 3,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jstatus,
        EO_INIT(.index)     Jindex( 3,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jstatus__basic,
            EO_INIT(.index)     Jindex( 3,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 3,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 3,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 3,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 3,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 3,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 3,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 3,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 3,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 3,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 3,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j03_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 3,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    // joint-j04
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig,
        EO_INIT(.index)     Jindex( 4, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 4, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 4, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 4, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__impedance,
            EO_INIT(.index)     Jindex( 4, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 4, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 4, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 4, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 4, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 4, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 4,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 4,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 4,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 4,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 4,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jstatus,
        EO_INIT(.index)     Jindex( 4,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jstatus__basic,
            EO_INIT(.index)     Jindex( 4,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 4,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 4,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 4,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 4,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 4,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 4,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 4,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 4,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 4,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 4,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j04_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 4,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    
    // joint-j05
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig,
        EO_INIT(.index)     Jindex( 5, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 5, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 5, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 5, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__impedance,
            EO_INIT(.index)     Jindex( 5, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 5, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 5, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 5, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 5, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 5, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 5,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 5,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 5,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 5,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 5,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jstatus,
        EO_INIT(.index)     Jindex( 5,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jstatus__basic,
            EO_INIT(.index)     Jindex( 5,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 5,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 5,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 5,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 5,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 5,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 5,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 5,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 5,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 5,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 5,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j05_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 5,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    // joint-j06 
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig,
        EO_INIT(.index)     Jindex( 6, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 6, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 6, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 6, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__impedance,
            EO_INIT(.index)     Jindex( 6, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 6, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 6, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 6, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 6, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 6, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 6,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 6,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 6,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 6,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 6,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jstatus,
        EO_INIT(.index)     Jindex( 6,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jstatus__basic,
            EO_INIT(.index)     Jindex( 6,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 6,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 6,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 6,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 6,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 6,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 6,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 6,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 6,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 6,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 6,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j06_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 6,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    // joint-j07
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig,
        EO_INIT(.index)     Jindex( 7, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 7, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 7, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 7, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__impedance,
            EO_INIT(.index)     Jindex( 7, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 7, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 7, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 7, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 7, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 7, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 7,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 7,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 7,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 7,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 7,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jstatus,
        EO_INIT(.index)     Jindex( 7,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jstatus__basic,
            EO_INIT(.index)     Jindex( 7,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 7,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 7,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 7,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 7,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 7,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 7,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 7,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 7,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 7,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 7,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j07_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 7,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    

    // joint-j08
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig,
        EO_INIT(.index)     Jindex( 8, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 8, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 8, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 8, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__impedance,
            EO_INIT(.index)     Jindex( 8, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 8, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 8, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 8, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 8, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 8, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 8,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 8,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 8,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 8,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 8,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jstatus,
        EO_INIT(.index)     Jindex( 8,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jstatus__basic,
            EO_INIT(.index)     Jindex( 8,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 8,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 8,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 8,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 8,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 8,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 8,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 8,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 8,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 8,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 8,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j08_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 8,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    


    // joint-j09
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig,
        EO_INIT(.index)     Jindex( 9, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 9, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 9, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 9, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__impedance,
            EO_INIT(.index)     Jindex( 9, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 9, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 9, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 9, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 9, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 9, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 9,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 9,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 9,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 9,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 9,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jstatus,
        EO_INIT(.index)     Jindex( 9,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jstatus__basic,
            EO_INIT(.index)     Jindex( 9,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 9,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 9,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 9,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 9,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 9,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 9,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 9,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 9,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex( 9,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 9,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j09_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 9,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    // joint-j10
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig,
        EO_INIT(.index)     Jindex(10, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__pidposition,
            EO_INIT(.index)     Jindex(10, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex(10, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__pidtorque,
            EO_INIT(.index)     Jindex(10, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__impedance,
            EO_INIT(.index)     Jindex(10, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex(10, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex(10, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex(10, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__controlmode,
            EO_INIT(.index)     Jindex(10, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex(10, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex(10,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex(10,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex(10,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex(10,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex(10,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jstatus,
        EO_INIT(.index)     Jindex(10,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jstatus__basic,
            EO_INIT(.index)     Jindex(10,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jstatus__ofpid,
            EO_INIT(.index)     Jindex(10,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex(10,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex(10,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex(10,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex(10,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jcmmnds__calibration,
        EO_INIT(.index)     Jindex(10,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex(10,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex(10,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex(10,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex(10,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j10_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex(10,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    
    // joint-j11
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig,
        EO_INIT(.index)     Jindex(11, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__pidposition,
            EO_INIT(.index)     Jindex(11, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex(11, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__pidtorque,
            EO_INIT(.index)     Jindex(11, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__impedance,
            EO_INIT(.index)     Jindex(11, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex(11, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex(11, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex(11, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__controlmode,
            EO_INIT(.index)     Jindex(11, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex(11, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex(11,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex(11,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex(11,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex(11,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex(11,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jstatus,
        EO_INIT(.index)     Jindex(11,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jstatus__basic,
            EO_INIT(.index)     Jindex(11,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jstatus__ofpid,
            EO_INIT(.index)     Jindex(11,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex(11,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex(11,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex(11,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex(11,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jcmmnds__calibration,
        EO_INIT(.index)     Jindex(11,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex(11,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex(11,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jcmmnds__cmdcontrolxmode,
        EO_INIT(.index)     Jindex(11,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex(11,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_j11_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex(11,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },            
      

      
      
      
    // ---------- and in here come the 12 motors
    
    
    
    
    
    // motor-m00
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mconfig,
        EO_INIT(.index)     Mindex( 0, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 0, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 0, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 0, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 0, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mstatus,
        EO_INIT(.index)     Mindex( 0, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mstatus__basic,
            EO_INIT(.index)     Mindex( 0, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m00_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 0, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 
 
    // motor-m01
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mconfig,
        EO_INIT(.index)     Mindex( 1, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 1, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 1, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 1, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 1, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mstatus,
        EO_INIT(.index)     Mindex( 1, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mstatus__basic,
            EO_INIT(.index)     Mindex( 1, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m01_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 1, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 

    // motor-m02
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mconfig,
        EO_INIT(.index)     Mindex( 2, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 2, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 2, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 2, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 2, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mstatus,
        EO_INIT(.index)     Mindex( 2, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mstatus__basic,
            EO_INIT(.index)     Mindex( 2, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m02_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 2, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    


    // motor-m03
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mconfig,
        EO_INIT(.index)     Mindex( 3, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 3, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 3, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 3, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 3, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mstatus,
        EO_INIT(.index)     Mindex( 3, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mstatus__basic,
            EO_INIT(.index)     Mindex( 3, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m03_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 3, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  


    // motor-m04
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mconfig,
        EO_INIT(.index)     Mindex( 4, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 4, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 4, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 4, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 4, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mstatus,
        EO_INIT(.index)     Mindex( 4, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mstatus__basic,
            EO_INIT(.index)     Mindex( 4, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m04_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 4, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 
 
    // motor-m05
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mconfig,
        EO_INIT(.index)     Mindex( 5, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 5, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 5, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 5, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 5, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mstatus,
        EO_INIT(.index)     Mindex( 5, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mstatus__basic,
            EO_INIT(.index)     Mindex( 5, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m05_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 5, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    
    // motor-m06 
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mconfig,
        EO_INIT(.index)     Mindex( 6, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 6, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 6, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 6, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 6, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mstatus,
        EO_INIT(.index)     Mindex( 6, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mstatus__basic,
            EO_INIT(.index)     Mindex( 6, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m06_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 6, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 
 
    // motor-m07
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mconfig,
        EO_INIT(.index)     Mindex( 7, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 7, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 7, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 7, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 7, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mstatus,
        EO_INIT(.index)     Mindex( 7, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mstatus__basic,
            EO_INIT(.index)     Mindex( 7, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m07_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 7, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 

    // motor-m08
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mconfig,
        EO_INIT(.index)     Mindex( 8, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 8, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 8, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 8, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 8, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mstatus,
        EO_INIT(.index)     Mindex( 8, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mstatus__basic,
            EO_INIT(.index)     Mindex( 8, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m08_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 8, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    


    // motor-m09
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mconfig,
        EO_INIT(.index)     Mindex( 9, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 9, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 9, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 9, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 9, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mstatus,
        EO_INIT(.index)     Mindex( 9, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mstatus__basic,
            EO_INIT(.index)     Mindex( 9, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m09_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 9, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  


    // motor-10
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mconfig,
        EO_INIT(.index)     Mindex(10, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(10, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(10, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(10, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex(10, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mstatus,
        EO_INIT(.index)     Mindex(10, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mstatus__basic,
            EO_INIT(.index)     Mindex(10, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m10_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex(10, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 
 
    // motor-m11
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mconfig,
        EO_INIT(.index)     Mindex(11, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(11, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(11, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(11, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex(11, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mstatus,
        EO_INIT(.index)     Mindex(11, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mstatus__basic,
            EO_INIT(.index)     Mindex(11, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_m11_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex(11, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        


    // the controller
    // cconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_c00_cconfig,
        EO_INIT(.index)     Cindex( 0),
        EO_INIT(.nchildren) 1,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_c00_cconfig__durationofctrlloop,
            EO_INIT(.index)     Cindex( 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // cstatus
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_c00_cstatus,
        EO_INIT(.index)     Cindex( 2),
        EO_INIT(.nchildren) 1,
        EO_INIT(.dchildren) NULL
    },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_c00_cstatus__alljomoinitted,
            EO_INIT(.index)     Cindex( 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // ccmmnds    
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerarm_c00_ccmmnds__go2stateofcontroller,
        EO_INIT(.index)     Cindex( 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }    
        
         
};  EO_VERIFYsizeof(eo_cfg_nvsEP_mc_lowerarm_tree_con, sizeof(EOtreenode)*(EOK_cfg_nvsEP_lowerarm_NUMofVARS));





const EOconstvector  s_eo_cfg_nvsEP_mc_lowerarm_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_mc_lowerarm_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_lowerarm_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_mc_lowerarm_tree_con
};


const EOconstvector* const eo_cfg_nvsEP_mc_lowerarm_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_mc_lowerarm_constvector_of_treenodes_EOnv_con;


const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_lowerarm_fptr_hashfunction_id2index = eo_cfg_nvsEP_mc_lowerarm_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_mc_lowerarm_hashfunction_id2index(uint16_t id)
{

    // 28*12
    #define IDTABLEJSIZE        (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS)
    
    // 8*12
    #define IDTABLEMSIZE        (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS)  

    // 5*1
    #define IDTABLECSIZE        (EOK_cfg_nvsEP_mc_any_con_cxx_cnvindex_TOTALnumber)  
    
    #define IDTABLESIZE         (IDTABLEJSIZE+IDTABLEMSIZE+IDTABLECSIZE)
    
    
    uint16_t index;

    // in order to always have a hit the table s_idtable[] it must be of size equal to max{ s_hash(id) }, thus if we
    // use an id of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have ids = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16

    static const uint16_t s_idtable[] = 
    { 
        // j00
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(0),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(0), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(0),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(0), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(0),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(0),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(0),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(0), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(0),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(0),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(0),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(0),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(0),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(0),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(0),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(0),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(0),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(0),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(0),    
 
        // j01
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(1),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(1), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(1),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(1), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(1),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(1),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(1),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(1), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(1),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(1),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(1),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(1),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(1),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(1),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(1),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(1),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(1),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(1),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(1),   
        
        // j02
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(2),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(2), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(2),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(2), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(2),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(2),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(2),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(2), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(2),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(2),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(2),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(2),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(2),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(2),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(2),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(2),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(2),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(2),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(2),  
        
        // j03
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(3),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(3), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(3),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(3), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(3),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(3),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(3),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(3), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(3),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(3),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(3),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(3),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(3),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(3),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(3),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(3),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(3),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(3),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(3),          
 
        // j04
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(4),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(4), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(4),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(4), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(4),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(4),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(4),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(4), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(4),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(4),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(4),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(4),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(4),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(4),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(4),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(4),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(4),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(4),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(4),   

        // j05
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(5),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(5), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(5),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(5), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(5),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(5),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(5),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(5), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(5),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(5),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(5),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(5),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(5),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(5),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(5),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(5),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(5),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(5),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(5),  
        
        // j06
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(6),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(6), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(6),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(6), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(6),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(6),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(6),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(6), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(6),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(6),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(6),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(6),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(6),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(6),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(6),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(6),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(6),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(6),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(6),    
 
        // j07
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(7),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(7), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(7),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(7), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(7),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(7),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(7),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(7), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(7),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(7),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(7),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(7),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(7),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(7),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(7),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(7),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(7),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(7),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(7),   
        
        // j08
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(8),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(8), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(8),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(8), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(8),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(8),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(8),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(8), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(8),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(8),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(8),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(8),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(8),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(8),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(8),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(8),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(8),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(8),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(8),  
        
        // j09
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(9),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(9), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(9),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(9), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(9),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(9),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(9),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(9), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(9),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(9),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(9),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(9),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(9),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(9),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(9),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(9),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(9),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(9),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(9),          
 
        // j10
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(10),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(10), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(10),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(10), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(10),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(10),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(10),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(10), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(10),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(10),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(10),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(10),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(10),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(10),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(10),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(10),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(10),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(10),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(10),   

        // j11
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(11),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(11), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(11),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(11), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__impedance(11),                   EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(11),      
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(11),          EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(11), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__controlmode(11),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__motionmonitormode(11),       
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__des02FORjstatuschamaleon04(11),  EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU01(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder01FFU02(11),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU03(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__holder02FFU04(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus(11),                              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__basic(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__ofpid(11),                       EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jstatus__chamaleon04(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__externallymeasuredtorque(11),    EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder02FFU01(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jinputs__holder04FFU02(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__calibration(11),                 EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__setpoint(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(11),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__cmdcontrolxmode(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(11),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(11),  
        
        
        // m00
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(0), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(0),
        // m01
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(1), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(1),
        // m02
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(2), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(2),
        // m03
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(3), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(3),
        // m04
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(4), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(4),
        // m05
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(5), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(5),
        // m06
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(6), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(6), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(6),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(6), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(6), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(6), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(6), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(6),
        // m07
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(7), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(7), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(7),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(7), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(7), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(7), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(7), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(7),
        // m08
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(8), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(8), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(8),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(8), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(8), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(8), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(8), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(8),
        // m09
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(9), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(9), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(9),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(9), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(9), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(9), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(9), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(9),
        // m10
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(10), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(10), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(10),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(10), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(10), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(10), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(10), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(10),
        // m11
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(11), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(11), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(11),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(11), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(11), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(11), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(11), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(11),
        
        // controller c00
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00_cconfig(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00_cconfig__durationofctrlloop(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00_cstatus(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00_cstatus__alljomoinitted(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00_ccmmnds__go2stateofcontroller(0)
         
        
    };  EO_VERIFYsizeof(s_idtable, sizeof(uint16_t)*(IDTABLESIZE));
    
    index = s_hash(id);
 
    
    if((index < (IDTABLESIZE)) && (id == s_idtable[index]) )
    {
        return(index);
    }
    else
    {
        return(EOK_uint16dummy);
    }

      
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// but EOK_cfg_nvsEP_mc_any_con_jxx_maxnumof_nvs_in_joint must be 32
// and EOK_cfg_nvsEP_mc_any_con_mxx_maxnumof_nvs_in_motor 16

typedef uint8_t sdfg[ ( EOK_cfg_nvsEP_mc_any_con_jxx_maxnumof_nvs_in_joint == 32 ) ? (1) : (0)];
typedef uint8_t redf[ ( EOK_cfg_nvsEP_mc_any_con_mxx_maxnumof_nvs_in_motor == 16 ) ? (1) : (0)];
typedef uint8_t sdfh[ ( EOK_cfg_nvsEP_mc_any_con_cxx_maxnumof_nvs_in_controller == 32 ) ? (1) : (0)];


static uint16_t s_hash(uint16_t id)
{
    uint16_t off = EO_nv_OFF(id);
    uint16_t a;
    uint16_t b;
    uint16_t r;
    
    if(off < EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(0))
    {
        a = off >> 5;
        b = off - (a << 5);
        r = a*EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber+b;
    }
    else if(off < EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_controller(0))
    {
        off -= EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(0);
        a = off >> 4;
        b = off - (a << 4);
        r = a*EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber+b;
        r += (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS);
    }
    else
    {
        off -= EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_controller(0);
        a = off >> 5;
        b = off - (a << 5);
        r = a*EOK_cfg_nvsEP_mc_any_con_cxx_cnvindex_TOTALnumber+b;
        r += (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofMOTORS);
        r += (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerarm_NUMofJOINTS);
    }
    
    return(r);
}


extern eOnvID_t eo_cfg_nvsEP_mc_lowerarm_joint_NVID_Get(eo_cfg_nvsEP_mc_lowerarm_con_jointNumber_t j, eo_cfg_nvsEP_mc_lowerarm_con_jointNVindex_t jnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_joint_NVID_Get((eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t)j, (eOcfg_nvsEP_mc_jointNVindex_t)jnvindex));
}

extern eOnvID_t eo_cfg_nvsEP_mc_lowerarm_motor_NVID_Get(eo_cfg_nvsEP_mc_lowerarm_con_motorNumber_t m, eo_cfg_nvsEP_mc_lowerarm_con_motorNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_motor_NVID_Get((eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t)m, (eOcfg_nvsEP_mc_motorNVindex_t)mnvindex));
}



extern eOnvID_t eo_cfg_nvsEP_mc_lowerarm_controller_NVID_Get(eo_cfg_nvsEP_mc_lowerarm_con_controllerNVindex_t cnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_controller_NVID_Get((eOcfg_nvsEP_mc_controllerNVindex_t)cnvindex));
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



