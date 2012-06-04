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

/* @file       eOcfg_nvsEP_mc_lowerleg_con.c
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

#include "eOcfg_nvsEP_mc_lowerleg_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//the number of joints used in the endpoint
#define EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS                                        (jointLowerLeg_TOTALnumber)
//the number of motors used in the endpoint
#define EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS                                        (motorLowerLeg_TOTALnumber)

//the total number of nvs in the endpoint
#define EOK_cfg_nvsEP_lowerleg_NUMofVARS                                             (controllerNVindex_TOTALnumber+(EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS)+(EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS))



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


static const eOmc_controller_t s_eo_cfg_nvsEP_mc_lowerleg_con_defaultvalue =
{
    EO_INIT(.cconfig)             
    {
        EO_INIT(.durationofctrlloop)        EOK_reltime1ms,
        EO_INIT(.filler04)                  {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.cstatus)                      
    {
        EO_INIT(.alljomoinitted)            eobool_false,
        EO_INIT(.numofjoints)               EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS,
        EO_INIT(.numofmotors)               EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS,
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



// -- the 2 joints

#define JMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx


// joint 00
#define JMACRO_PSTR    _lowerleg
#define JMACRO_JSTR    _j00
#define JMACRO_PNUM    5
#define JMACRO_JNUM    0    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 01
#define JMACRO_PSTR    _lowerleg
#define JMACRO_JSTR    _j01
#define JMACRO_PNUM    5
#define JMACRO_JNUM    1    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"



#define OFFSET_OF_END_OF_JOINTS    (EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS*sizeof(eOmc_joint_t))


// -- the 2 motors

#define MMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx


// motor 00
#define MMACRO_PSTR    _lowerleg
#define MMACRO_MSTR    _m00
#define MMACRO_PNUM    5
#define MMACRO_MNUM    0    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 01
#define MMACRO_PSTR    _lowerleg
#define MMACRO_MSTR    _m01
#define MMACRO_PNUM    5
#define MMACRO_MNUM    1    
#define MMACRO_MOFF    (OFFSET_OF_END_OF_JOINTS+MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


#define OFFSET_OF_END_OF_MOTORS    (OFFSET_OF_END_OF_JOINTS+EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS*sizeof(eOmc_motor_t))


// the controller

#define CMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_c00

#define CMACRO_EXTERNALPREFIX_CONST                s_eo_cfg_nvsEP_mc_lowerleg_con_defaultvalue

#define CMACRO_PSTR    _lowerleg
#define CMACRO_CSTR    _c00
#define CMACRO_PNUM    5
#define CMACRO_CNUM    0    
#define CMACRO_COFF    (OFFSET_OF_END_OF_MOTORS+CMACRO_CNUM*sizeof(eOmc_controller_t))

#include "eOcfg_nvsEP_mc_any_con_cxxmacro.c"



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const eOmc_joint_t* eo_cfg_nvsEP_mc_lowerleg_joint_defaultvalue = &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue;
const eOmc_motor_t* eo_cfg_nvsEP_mc_lowerleg_motor_defaultvalue = &eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue;
const eOmc_controller_t* eo_cfg_nvsEP_mc_lowerleg_controller_defaultvalue = &s_eo_cfg_nvsEP_mc_lowerleg_con_defaultvalue;


#define Jnvs    (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber)


#define JNUM    (EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS)


// it is the start of index for the joints
#define Jstart          (0)
#define Jindex(j, i)    (Jstart + (j)*Jnvs + (i))




// EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber is 6 and keeps teh number of nvs per motor
#define Mnvs    (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber)

// EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS is 6 and is the number of motors in the leg
#define MNUM     (EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS)


// it is the start of index for the motors
#define Mstart          (Jnvs*JNUM+0)
#define Mindex(m, i)    (Mstart + (m)*Mnvs + (i))


// it is the start of index for the configuration
#define Cstart          (Mstart+Mnvs*MNUM+0)
#define Cindex(i)       (Cstart + (i))


EOtreenode eo_cfg_nvsEP_mc_lowerleg_tree_con[] =
{
    // joint-j00
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig,
        EO_INIT(.index)     Jindex( 0, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 0, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 0, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 0, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__impedance,
            EO_INIT(.index)     Jindex( 0, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 0, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 0, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 0, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 0, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 0, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 0,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 0,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 0,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 0,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 0,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jstatus,
        EO_INIT(.index)     Jindex( 0,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jstatus__basic,
            EO_INIT(.index)     Jindex( 0,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 0,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 0,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 0,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 0,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 0,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 0,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 0,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 0,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jcmmnds__holder01FFU01,
        EO_INIT(.index)     Jindex( 0,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 0,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j00_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 0,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
       
    // joint-j01
    // jconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig,
        EO_INIT(.index)     Jindex( 1, 0),
        EO_INIT(.nchildren) 14,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__pidposition,
            EO_INIT(.index)     Jindex( 1, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__pidvelocity,
            EO_INIT(.index)     Jindex( 1, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__pidtorque,
            EO_INIT(.index)     Jindex( 1, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__impedance,
            EO_INIT(.index)     Jindex( 1, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__minpositionofjoint,
            EO_INIT(.index)     Jindex( 1, 5),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },         
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__maxpositionofjoint,
            EO_INIT(.index)     Jindex( 1, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },   
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     Jindex( 1, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 8
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__controlmode,
            EO_INIT(.index)     Jindex( 1, 8),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 9
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__motionmonitormode,
            EO_INIT(.index)     Jindex( 1, 9),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //10
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__des02FORjstatuschamaleon04,
            EO_INIT(.index)     Jindex( 1,10),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
       {   //11
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__holder01FFU01,
            EO_INIT(.index)     Jindex( 1,11),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //12
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__holder01FFU02,
            EO_INIT(.index)     Jindex( 1,12),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //13
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__holder02FFU03,
            EO_INIT(.index)     Jindex( 1,13),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        }, 
       {   //14
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jconfig__holder02FFU04,
            EO_INIT(.index)     Jindex( 1,14),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },

    // jstatus
    {   //15
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jstatus,
        EO_INIT(.index)     Jindex( 1,15),
        EO_INIT(.nchildren) 3,
        EO_INIT(.dchildren) NULL
    },
        {   //16
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jstatus__basic,
            EO_INIT(.index)     Jindex( 1,16),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },    
        {   //17
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jstatus__ofpid,
            EO_INIT(.index)     Jindex( 1,17),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },     
        {   //18
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jstatus__chamaleon04,
            EO_INIT(.index)     Jindex( 1,18),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  

    // jinputs
    {   //19
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jinputs__externallymeasuredtorque,
        EO_INIT(.index)     Jindex( 1,19),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //20
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jinputs__holder02FFU01,
        EO_INIT(.index)     Jindex( 1,20),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },  
    {   //21
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jinputs__holder04FFU02,
        EO_INIT(.index)     Jindex( 1,21),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 

    
    // jcmmnds
    {   //22
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jcmmnds__calibration,
        EO_INIT(.index)     Jindex( 1,22),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }, 
    {   //23
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jcmmnds__setpoint,
        EO_INIT(.index)     Jindex( 1,23),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //24
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jcmmnds__stoptrajectory,
        EO_INIT(.index)     Jindex( 1,24),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },
    {   //25
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jcmmnds__holder01FFU01,
        EO_INIT(.index)     Jindex( 1,25),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    {   //26
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jcmmnds__holder01FFU02,
        EO_INIT(.index)     Jindex( 1,26),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },        
    {   //27
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_j01_jcmmnds__holder01FFU03,
        EO_INIT(.index)     Jindex( 1,27),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    },    
    
          
      
    // ---------- and in here come the 4 motors
    
    // motor-m00
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mconfig,
        EO_INIT(.index)     Mindex( 0, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 0, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 0, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 0, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 0, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mstatus,
        EO_INIT(.index)     Mindex( 0, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mstatus__basic,
            EO_INIT(.index)     Mindex( 0, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m00_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 0, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },       
 
 
    // motor-m01
    // mconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mconfig,
        EO_INIT(.index)     Mindex( 1, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex( 1, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex( 1, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex( 1, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mconfig__des02FORmstatuschamaleon04,
            EO_INIT(.index)     Mindex( 1, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // mstatus
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mstatus,
        EO_INIT(.index)     Mindex( 1, 5),
        EO_INIT(.nchildren) 2,
        EO_INIT(.dchildren) NULL
    },  
         {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mstatus__basic,
            EO_INIT(.index)     Mindex( 1, 6),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },      
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_m01_mstatus__chamaleon04,
            EO_INIT(.index)     Mindex( 1, 7),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },
        


    // the controller
    // cconfig
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_c00_cconfig,
        EO_INIT(.index)     Cindex( 0),
        EO_INIT(.nchildren) 1,
        EO_INIT(.dchildren) NULL
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_c00_cconfig__durationofctrlloop,
            EO_INIT(.index)     Cindex( 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // cstatus
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_c00_cstatus,
        EO_INIT(.index)     Cindex( 2),
        EO_INIT(.nchildren) 1,
        EO_INIT(.dchildren) NULL
    },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_c00_cstatus__alljomoinitted,
            EO_INIT(.index)     Cindex( 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.dchildren) NULL
        },  
    // ccmmnds    
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_lowerleg_c00_ccmmnds__go2stateofcontroller,
        EO_INIT(.index)     Cindex( 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.dchildren) NULL        
    }         
        
};  EO_VERIFYsizeof(eo_cfg_nvsEP_mc_lowerleg_tree_con, sizeof(EOtreenode)*(EOK_cfg_nvsEP_lowerleg_NUMofVARS));





const EOconstvector  s_eo_cfg_nvsEP_mc_lowerleg_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_mc_lowerleg_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_lowerleg_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_mc_lowerleg_tree_con
};


const EOconstvector* const eo_cfg_nvsEP_mc_lowerleg_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_mc_lowerleg_constvector_of_treenodes_EOnv_con;


const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_lowerleg_fptr_hashfunction_id2index = eo_cfg_nvsEP_mc_lowerleg_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_mc_lowerleg_hashfunction_id2index(uint16_t id)
{

    // 28*2
    #define IDTABLEJSIZE        (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS)
    
    // 8*2
    #define IDTABLEMSIZE        (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS)  

    // 5*1
    #define IDTABLECSIZE        (EOK_cfg_nvsEP_mc_any_con_cxx_cnvindex_TOTALnumber)  
    
    #define IDTABLESIZE         (IDTABLEJSIZE+IDTABLEMSIZE+IDTABLECSIZE)
    

    // in order to always have a hit the table s_idtable[] it must be of size equal to max{ s_hash(id) }, thus if we
    // use an id of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have ids = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16

    uint16_t index;
    
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
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(0),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU01(0),
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
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__stoptrajectory(1),              EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU01(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU02(1),               EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jcmmnds__holder01FFU03(1),   
        
        // m00
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(0), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(0),
        // m01
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__des02FORmstatuschamaleon04(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus(1), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__basic(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mstatus__chamaleon04(1),
        
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

EO_VERIFYproposition(xxx, ( EOK_cfg_nvsEP_mc_any_con_jxx_maxnumof_nvs_in_joint == 32 ) );
EO_VERIFYproposition(xxx, ( EOK_cfg_nvsEP_mc_any_con_mxx_maxnumof_nvs_in_motor == 16 ) );
EO_VERIFYproposition(xxx, ( EOK_cfg_nvsEP_mc_any_con_cxx_maxnumof_nvs_in_controller == 32 ) );

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
        r += (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS);
    }
    else
    {
        off -= EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_controller(0);
        a = off >> 5;
        b = off - (a << 5);
        r = a*EOK_cfg_nvsEP_mc_any_con_cxx_cnvindex_TOTALnumber+b;
        r += (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofMOTORS);
        r += (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_lowerleg_NUMofJOINTS);
    }
    
    return(r);
}


extern eOnvID_t eo_cfg_nvsEP_mc_lowerleg_joint_NVID_Get(eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t j, eo_cfg_nvsEP_mc_lowerleg_con_jointNVindex_t jnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_joint_NVID_Get((eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t)j, (eOcfg_nvsEP_mc_jointNVindex_t)jnvindex));
}

extern eOnvID_t eo_cfg_nvsEP_mc_lowerleg_motor_NVID_Get(eo_cfg_nvsEP_mc_lowerleg_con_motorNumber_t m, eo_cfg_nvsEP_mc_lowerleg_con_motorNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_motor_NVID_Get((eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t)m, (eOcfg_nvsEP_mc_motorNVindex_t)mnvindex));
}


extern eOnvID_t eo_cfg_nvsEP_mc_lowerleg_controller_NVID_Get(eo_cfg_nvsEP_mc_lowerleg_con_controllerNVindex_t cnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_controller_NVID_Get((eOcfg_nvsEP_mc_controllerNVindex_t)cnvindex));
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



