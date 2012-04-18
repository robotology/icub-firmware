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

/* @file       eOcfg_nvsEP_mc_leftleg_con.c
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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_leftleg_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//the number of joints used in the endpoint
#define EOK_cfg_nvsEP_mc_leftleg_NUMofJOINTS                                        (6)
//the number of motors used in the endpoint
#define EOK_cfg_nvsEP_mc_leftleg_NUMofMOTORS                                        (6)

//the total number of nvs in the endpoint
#define EOK_cfg_nvsEP_leftleg_NUMofVARS                                             ((EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_leftleg_NUMofJOINTS)+(EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_leftleg_NUMofMOTORS))



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


// -- the 6 joints

#define JMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx


// joint 00
#define JMACRO_PSTR    _leftleg
#define JMACRO_JSTR    _j00
#define JMACRO_PNUM    5
#define JMACRO_JNUM    0    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 01
#define JMACRO_PSTR    _leftleg
#define JMACRO_JSTR    _j01
#define JMACRO_PNUM    5
#define JMACRO_JNUM    1    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"



// joint 02
#define JMACRO_PSTR    _leftleg
#define JMACRO_JSTR    _j02
#define JMACRO_PNUM    5
#define JMACRO_JNUM    2    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 03
#define JMACRO_PSTR    _leftleg
#define JMACRO_JSTR    _j03
#define JMACRO_PNUM    5
#define JMACRO_JNUM    3    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 04
#define JMACRO_PSTR    _leftleg
#define JMACRO_JSTR    _j04
#define JMACRO_PNUM    5
#define JMACRO_JNUM    4    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// joint 05
#define JMACRO_PSTR    _leftleg
#define JMACRO_JSTR    _j05
#define JMACRO_PNUM    5
#define JMACRO_JNUM    5    
#define JMACRO_JOFF    (JMACRO_JNUM*sizeof(eOmc_joint_t))

#include "eOcfg_nvsEP_mc_any_con_jxxmacro.c"


// -- the 6 motors

#define MMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx


// motor 00
#define MMACRO_PSTR    _leftleg
#define MMACRO_MSTR    _m00
#define MMACRO_PNUM    5
#define MMACRO_MNUM    0    
#define MMACRO_MOFF    (MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 01
#define MMACRO_PSTR    _leftleg
#define MMACRO_MSTR    _m01
#define MMACRO_PNUM    5
#define MMACRO_MNUM    1    
#define MMACRO_MOFF    (MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"



// motor 02
#define MMACRO_PSTR    _leftleg
#define MMACRO_MSTR    _m02
#define MMACRO_PNUM    5
#define MMACRO_MNUM    2    
#define MMACRO_MOFF    (MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 03
#define MMACRO_PSTR    _leftleg
#define MMACRO_MSTR    _m03
#define MMACRO_PNUM    5
#define MMACRO_MNUM    3    
#define MMACRO_MOFF    (MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 04
#define MMACRO_PSTR    _leftleg
#define MMACRO_MSTR    _m04
#define MMACRO_PNUM    5
#define MMACRO_MNUM    4    
#define MMACRO_MOFF    (MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"


// motor 05
#define MMACRO_PSTR    _leftleg
#define MMACRO_MSTR    _m05
#define MMACRO_PNUM    5
#define MMACRO_MNUM    5    
#define MMACRO_MOFF    (MMACRO_MNUM*sizeof(eOmc_motor_t))

#include "eOcfg_nvsEP_mc_any_con_mxxmacro.c"



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

// EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber is 12 and keeps the number of nvs per joint
#define Jnvs    (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber)

// EOK_cfg_nvsEP_mc_leftleg_NUMofJOINTS is 6 and is the number of joints in the leftleg
#define JNUM    (EOK_cfg_nvsEP_mc_leftleg_NUMofJOINTS)


// it is the start of index for the joints
#define Jstart  (0)

// J004p is the index of the first nv of the joint 4.
#define J00p    (Jstart + 0)
#define J01p    (Jstart + 1*Jnvs)
#define J02p    (Jstart + 2*Jnvs)
#define J03p    (Jstart + 3*Jnvs)
#define J04p    (Jstart + 4*Jnvs)
#define J05p    (Jstart + 5*Jnvs)

#define Jindex(j, i)   (Jstart + (j)*Jnvs + (i))




// EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber is 6 and keeps teh number of nvs per motor
#define Mnvs    (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber)

// EOK_cfg_nvsEP_mc_leftleg_NUMofMOTORS is 6 and is the number of motors in the leftleg
#define MNUM     (EOK_cfg_nvsEP_mc_leftleg_NUMofMOTORS)


// it is the start of index for the motors
#define Mstart  (Jnvs*JNUM+0)

#define M00p    (Mstart + 0)
#define M01p    (Mstart + 1*Mnnv)
#define M02p    (Mstart + 2*Mnvs)
#define M03p    (Mstart + 3*Mnvs)
#define M04p    (Mstart + 4*Mnvs)
#define M05p    (Mstart + 5*Mnvs)

#define Mindex(m, i)   (Mstart + (m)*Mnvs + (i))


//extern EOtreenode eo_cfg_nvsEP_mc_leftleg_tree_con[];

extern EOtreenode eo_cfg_nvsEP_mc_leftleg_tree_con[EOK_cfg_nvsEP_leftleg_NUMofVARS] =
{
    // joint00
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig,
        EO_INIT(.index)     0+J00p, // or Jindex(0, 0)
        EO_INIT(.nchildren) 7,
        EO_INIT(.ichildren) {1+J00p, 2+J00p, 3+J00p, 4+J00p, 5+J00p, 6+J00p, 7+J00p},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[1+J00p], &eo_cfg_nvsEP_mc_leftleg_tree_con[2+J00p], &eo_cfg_nvsEP_mc_leftleg_tree_con[3+J00p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[4+J00p], &eo_cfg_nvsEP_mc_leftleg_tree_con[5+J00p], &eo_cfg_nvsEP_mc_leftleg_tree_con[6+J00p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[7+J00p]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__pidposition,
            EO_INIT(.index)     1+J00p, // or Jindex(0, 1)
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__pidvelocity,
            EO_INIT(.index)     2+J00p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__pidtorque,
            EO_INIT(.index)     3+J00p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__minpositionofjoint,
            EO_INIT(.index)     4+J00p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__maxpositionofjoint,
            EO_INIT(.index)     5+J00p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     6+J00p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00_jconfig__upto02descrforchameleon02,
            EO_INIT(.index)     7+J00p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
    {   // 8
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00__jstatus,
        EO_INIT(.index)     8+J00p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 9
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00__calibrator,
        EO_INIT(.index)     9+J00p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //10
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00__setpoint,
        EO_INIT(.index)     10+J00p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //11
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j00__controlmode,
        EO_INIT(.index)     11+J00p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },


    // joint01
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig,
        EO_INIT(.index)     0+J01p,
        EO_INIT(.nchildren) 7,
        EO_INIT(.ichildren) {1+J01p, 2+J01p, 3+J01p, 4+J01p, 5+J01p, 6+J01p, 7+J01p},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[1+J01p], &eo_cfg_nvsEP_mc_leftleg_tree_con[2+J01p], &eo_cfg_nvsEP_mc_leftleg_tree_con[3+J01p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[4+J01p], &eo_cfg_nvsEP_mc_leftleg_tree_con[5+J01p], &eo_cfg_nvsEP_mc_leftleg_tree_con[6+J01p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[7+J01p]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__pidposition,
            EO_INIT(.index)     1+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__pidvelocity,
            EO_INIT(.index)     2+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__pidtorque,
            EO_INIT(.index)     3+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__minpositionofjoint,
            EO_INIT(.index)     4+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__maxpositionofjoint,
            EO_INIT(.index)     5+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     6+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01_jconfig__upto02descrforchameleon02,
            EO_INIT(.index)     7+J01p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
    {   // 8
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01__jstatus,
        EO_INIT(.index)     8+J01p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 9
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01__calibrator,
        EO_INIT(.index)     9+J01p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //10
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01__setpoint,
        EO_INIT(.index)     10+J01p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //11
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j01__controlmode,
        EO_INIT(.index)     11+J01p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    

    // joint02
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig,
        EO_INIT(.index)     0+J02p,
        EO_INIT(.nchildren) 7,
        EO_INIT(.ichildren) {1+J02p, 2+J02p, 3+J02p, 4+J02p, 5+J02p, 6+J02p, 7+J02p},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[1+J02p], &eo_cfg_nvsEP_mc_leftleg_tree_con[2+J02p], &eo_cfg_nvsEP_mc_leftleg_tree_con[3+J02p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[4+J02p], &eo_cfg_nvsEP_mc_leftleg_tree_con[5+J02p], &eo_cfg_nvsEP_mc_leftleg_tree_con[6+J02p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[7+J02p]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__pidposition,
            EO_INIT(.index)     1+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__pidvelocity,
            EO_INIT(.index)     2+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__pidtorque,
            EO_INIT(.index)     3+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__minpositionofjoint,
            EO_INIT(.index)     4+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__maxpositionofjoint,
            EO_INIT(.index)     5+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     6+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02_jconfig__upto02descrforchameleon02,
            EO_INIT(.index)     7+J02p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
    {   // 8
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02__jstatus,
        EO_INIT(.index)     8+J02p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 9
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02__calibrator,
        EO_INIT(.index)     9+J02p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //10
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02__setpoint,
        EO_INIT(.index)     10+J02p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //11
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j02__controlmode,
        EO_INIT(.index)     11+J02p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },

    
    // joint03
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig,
        EO_INIT(.index)     0+J03p,
        EO_INIT(.nchildren) 7,
        EO_INIT(.ichildren) {1+J03p, 2+J03p, 3+J03p, 4+J03p, 5+J03p, 6+J03p, 7+J03p},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[1+J03p], &eo_cfg_nvsEP_mc_leftleg_tree_con[2+J03p], &eo_cfg_nvsEP_mc_leftleg_tree_con[3+J03p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[4+J03p], &eo_cfg_nvsEP_mc_leftleg_tree_con[5+J03p], &eo_cfg_nvsEP_mc_leftleg_tree_con[6+J03p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[7+J03p]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__pidposition,
            EO_INIT(.index)     1+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__pidvelocity,
            EO_INIT(.index)     2+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__pidtorque,
            EO_INIT(.index)     3+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__minpositionofjoint,
            EO_INIT(.index)     4+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__maxpositionofjoint,
            EO_INIT(.index)     5+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     6+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03_jconfig__upto02descrforchameleon02,
            EO_INIT(.index)     7+J03p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
    {   // 8
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03__jstatus,
        EO_INIT(.index)     8+J03p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 9
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03__calibrator,
        EO_INIT(.index)     9+J03p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //10
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03__setpoint,
        EO_INIT(.index)     10+J03p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //11
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j03__controlmode,
        EO_INIT(.index)     11+J03p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },


    // joint04
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig,
        EO_INIT(.index)     0+J04p,
        EO_INIT(.nchildren) 7,
        EO_INIT(.ichildren) {1+J04p, 2+J04p, 3+J04p, 4+J04p, 5+J04p, 6+J04p, 7+J04p},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[1+J04p], &eo_cfg_nvsEP_mc_leftleg_tree_con[2+J04p], &eo_cfg_nvsEP_mc_leftleg_tree_con[3+J04p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[4+J04p], &eo_cfg_nvsEP_mc_leftleg_tree_con[5+J04p], &eo_cfg_nvsEP_mc_leftleg_tree_con[6+J04p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[7+J04p]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__pidposition,
            EO_INIT(.index)     1+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__pidvelocity,
            EO_INIT(.index)     2+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__pidtorque,
            EO_INIT(.index)     3+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__minpositionofjoint,
            EO_INIT(.index)     4+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__maxpositionofjoint,
            EO_INIT(.index)     5+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     6+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04_jconfig__upto02descrforchameleon02,
            EO_INIT(.index)     7+J04p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
    {   // 8
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04__jstatus,
        EO_INIT(.index)     8+J04p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 9
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04__calibrator,
        EO_INIT(.index)     9+J04p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //10
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04__setpoint,
        EO_INIT(.index)     10+J04p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //11
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j04__controlmode,
        EO_INIT(.index)     11+J04p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },

    // joint05
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig,
        EO_INIT(.index)     0+J05p,
        EO_INIT(.nchildren) 7,
        EO_INIT(.ichildren) {1+J05p, 2+J05p, 3+J05p, 4+J05p, 5+J05p, 6+J05p, 7+J05p},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[1+J05p], &eo_cfg_nvsEP_mc_leftleg_tree_con[2+J05p], &eo_cfg_nvsEP_mc_leftleg_tree_con[3+J05p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[4+J05p], &eo_cfg_nvsEP_mc_leftleg_tree_con[5+J05p], &eo_cfg_nvsEP_mc_leftleg_tree_con[6+J05p], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[7+J05p]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__pidposition,
            EO_INIT(.index)     1+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__pidvelocity,
            EO_INIT(.index)     2+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__pidtorque,
            EO_INIT(.index)     3+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__minpositionofjoint,
            EO_INIT(.index)     4+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 5
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__maxpositionofjoint,
            EO_INIT(.index)     5+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
        {   // 6
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__velocitysetpointtimeout,
            EO_INIT(.index)     6+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
        {   // 7
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05_jconfig__upto02descrforchameleon02,
            EO_INIT(.index)     7+J05p,
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },   
    {   // 8
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05__jstatus,
        EO_INIT(.index)     8+J05p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 9
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05__calibrator,
        EO_INIT(.index)     9+J05p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //10
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05__setpoint,
        EO_INIT(.index)     10+J05p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   //11
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_j05__controlmode,
        EO_INIT(.index)     11+J05p,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    
    
    #warning and in here come the 6 motors

    // motor00
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m00_mconfig,
        EO_INIT(.index)     Mindex(0, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.ichildren) {Mindex(0, 1), Mindex(0, 2), Mindex(0, 3), Mindex(0, 4)},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(0, 1)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(0, 2)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(0, 3)], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(0, 4)]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m00_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(0, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m00_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(0, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m00_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(0, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m00_mconfig__upto02descrforchameleon06,
            EO_INIT(.index)     Mindex(0, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m00__mstatus,
        EO_INIT(.index)     Mindex(0, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
 
    
    // motor01
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m01_mconfig,
        EO_INIT(.index)     Mindex(1, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.ichildren) {Mindex(1, 1), Mindex(1, 2), Mindex(1, 3), Mindex(1, 4)},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(1, 1)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(1, 2)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(1, 3)], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(1, 4)]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m01_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(1, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m01_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(1, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m01_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(1, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m01_mconfig__upto02descrforchameleon06,
            EO_INIT(.index)     Mindex(1, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m01__mstatus,
        EO_INIT(.index)     Mindex(1, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    

    // motor02
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m02_mconfig,
        EO_INIT(.index)     Mindex(2, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.ichildren) {Mindex(2, 1), Mindex(2, 2), Mindex(2, 3), Mindex(2, 4)},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(2, 1)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(2, 2)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(2, 3)], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(2, 4)]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m02_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(2, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m02_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(2, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m02_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(2, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m02_mconfig__upto02descrforchameleon06,
            EO_INIT(.index)     Mindex(2, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m02__mstatus,
        EO_INIT(.index)     Mindex(2, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  


    // motor03
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m03_mconfig,
        EO_INIT(.index)     Mindex(3, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.ichildren) {Mindex(3, 1), Mindex(3, 2), Mindex(3, 3), Mindex(3, 4)},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(3, 1)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(3, 2)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(3, 3)], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(3, 4)]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m03_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(3, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m03_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(3, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m03_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(3, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m03_mconfig__upto02descrforchameleon06,
            EO_INIT(.index)     Mindex(3, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m03__mstatus,
        EO_INIT(.index)     Mindex(3, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },      
    
    
    // motor04
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m04_mconfig,
        EO_INIT(.index)     Mindex(4, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.ichildren) {Mindex(4, 1), Mindex(4, 2), Mindex(4, 3), Mindex(4, 4)},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(4, 1)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(4, 2)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(4, 3)], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(4, 4)]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m04_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(4, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m04_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(4, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m04_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(4, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m04_mconfig__upto02descrforchameleon06,
            EO_INIT(.index)     Mindex(4, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m04__mstatus,
        EO_INIT(.index)     Mindex(4, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },     
    

    // motor05
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m05_mconfig,
        EO_INIT(.index)     Mindex(5, 0),
        EO_INIT(.nchildren) 4,
        EO_INIT(.ichildren) {Mindex(5, 1), Mindex(5, 2), Mindex(5, 3), Mindex(5, 4)},
        EO_INIT(.pchildren) {&eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(5, 1)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(5, 2)], &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(5, 3)], 
                             &eo_cfg_nvsEP_mc_leftleg_tree_con[Mindex(5, 4)]}
    },
        {   // 1
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m05_mconfig__pidcurrent,
            EO_INIT(.index)     Mindex(5, 1),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },  
        {   // 2
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m05_mconfig__maxvelocityofmotor,
            EO_INIT(.index)     Mindex(5, 2),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },
        {   // 3
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m05_mconfig__maxcurrentofmotor,
            EO_INIT(.index)     Mindex(5, 3),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },        
        {   // 4
            EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m05_mconfig__upto02descrforchameleon06,
            EO_INIT(.index)     Mindex(5, 4),
            EO_INIT(.nchildren) 0,
            EO_INIT(.ichildren) {0},
            EO_INIT(.pchildren) {NULL}
        },         
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_mc_leftleg_m05__mstatus,
        EO_INIT(.index)     Mindex(5, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    }         
    
};





const EOconstvector  s_eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_mc_leftleg_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_leftleg_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_mc_leftleg_tree_con
};


extern const EOconstvector* const eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con;


extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_leftleg_fptr_hashfunction_id2index = eo_cfg_nvsEP_mc_leftleg_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_mc_leftleg_hashfunction_id2index(uint16_t id)
{

    // 12*6
    #define IDTABLEJSIZE     (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_leftleg_NUMofJOINTS)
    
    // 6*6
    #define IDTABLEMSIZE     (EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_leftleg_NUMofMOTORS)    
    

    // in order to always have a hit the table s_idtable[] it must be of size equal to max{ s_hash(id) }, thus if we
    // use an id of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have ids = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16

    static const uint16_t s_idtable[IDTABLEJSIZE+IDTABLEMSIZE] = 
    { 
        // j00
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(0), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(0),
        // j01
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(1), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(1),
        // j02
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(2), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(2),       
        // j03
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(3), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(3),       
        // j04
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(4), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(4),       
        // j05
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidposition(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidvelocity(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__pidtorque(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__minpositionofjoint(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__maxpositionofjoint(5), 
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__velocitysetpointtimeout(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx_jconfig__upto02descrforchameleon02(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__jstatus(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__calibrator(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__setpoint(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_jxx__controlmode(5),

        // m00
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(0),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(0), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(0), 
        // m01
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(1),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(1), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(1), 
        // m02
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(2),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(2), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(2), 
        // m03
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(3),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(3), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(3), 
        // m04
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(4),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(4), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(4), 
       // m05
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__pidcurrent(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxvelocityofmotor(5),
        EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__maxcurrentofmotor(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx_mconfig__upto02descrforchameleon06(5), EOK_cfg_nvsEP_mc_any_con_bodypart_NVID_mxx__mstatus(5) 
                                        
    };
    
    uint16_t index = s_hash(id);
    
    #warning --> so far the hashing is ok only for teh joints. for the motor it is a different story 
    
    if((index < (IDTABLEJSIZE+IDTABLEMSIZE)) && (id == s_idtable[index]) )
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


// but EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_joint must be 32

typedef uint8_t sdfg[ ( EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_joint == 32 ) ? (1) : (0)];
typedef uint8_t redf[ ( EOK_cfg_nvsEP_mc_any_con_bodypart_maxnumof_nvs_in_motor == 16 ) ? (1) : (0)];

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
    else
    {
        off -= EOK_cfg_nvsEP_mc_any_con_bodypart_firstNVIDoff_of_motor(0);
        a = off >> 4;
        b = off - (a << 4);
        r = a*EOK_cfg_nvsEP_mc_any_con_mxx_mnvindex_TOTALnumber+b;
        r += (EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber*EOK_cfg_nvsEP_mc_leftleg_NUMofJOINTS);
    }
    
    return(r);
}


extern eOnvEP_t eo_cfg_nvsEP_mc_leftlef_EP_Get(void)
{
    return(EOK_cfg_nvsEP_mc_leftleg_EP);
}

extern eOnvID_t eo_cfg_nvsEP_mc_leftleg_NVID_for_joint_var_Get(eo_cfg_nvsEP_mc_leftleg_con_jointNumber_t j, eo_cfg_nvsEP_mc_leftleg_con_jointNVindex_t jnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(j, jnvindex));
}

extern eOnvID_t eo_cfg_nvsEP_mc_leftleg_NVID_for_motor_var_Get(eo_cfg_nvsEP_mc_leftleg_con_motorNumber_t m, eo_cfg_nvsEP_mc_leftleg_con_motorNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get(m, mnvindex));
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



