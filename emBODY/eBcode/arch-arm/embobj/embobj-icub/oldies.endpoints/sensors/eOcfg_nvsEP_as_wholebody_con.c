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

/* @file       eOcfg_nvsEP_as_wholebody_con.c
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


#include "eOcfg_nvsEP_as_any_con_sxx.h"    
#include "eOcfg_nvsEP_as_any_con_sxxdefault.h" 

#include "eOcfg_nvsEP_as_any_con_mxx.h"    
#include "eOcfg_nvsEP_as_any_con_mxxdefault.h" 

#include "eOcfg_nvsEP_as_any_con_body_hid.h"    



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_wholebody_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_wholebody_con_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



EO_VERIFYproposition(xxx, strainNVindex_sconfig__mode                   == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__mode);
EO_VERIFYproposition(xxx, strainNVindex_sconfig__datarate               == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__datarate);
EO_VERIFYproposition(xxx, strainNVindex_sconfig__signaloncefullscale    == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sconfig__signaloncefullscale);
EO_VERIFYproposition(xxx, strainNVindex_sstatus__fullscale              == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__fullscale);
EO_VERIFYproposition(xxx, strainNVindex_sstatus__calibratedvalues       == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__calibratedvalues);
EO_VERIFYproposition(xxx, strainNVindex_sstatus__uncalibratedvalues     == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_sstatus__uncalibratedvalues);



EO_VERIFYproposition(xxx, strainNVindex_TOTALnumber                     == EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber);


EO_VERIFYproposition(xxx, maisNVindex_mconfig__mode                     == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__mode);
EO_VERIFYproposition(xxx, maisNVindex_mconfig__datarate                 == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__datarate);
EO_VERIFYproposition(xxx, maisNVindex_mconfig__resolution               == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mconfig__resolution);
EO_VERIFYproposition(xxx, maisNVindex_mstatus__the15values              == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_mstatus__the15values);

EO_VERIFYproposition(xxx, maisNVindex_TOTALnumber                       == EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber);


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hash(uint16_t id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// -- the 4 strain

#define SMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_as_any_con_body_NVID_sxx


// strain 00
#define SMACRO_PSTR      _wholebody
#define SMACRO_STR       _s00
#define SMACRO_PNUM      5
#define SMACRO_NUM       0    
#define SMACRO_OFF      (SMACRO_NUM*sizeof(eOsnsr_strain_t))

#include "eOcfg_nvsEP_as_any_con_sxxmacro.c"


// strain 01
#define SMACRO_PSTR      _wholebody
#define SMACRO_STR       _s01
#define SMACRO_PNUM      5
#define SMACRO_NUM       1    
#define SMACRO_OFF      (SMACRO_NUM*sizeof(eOsnsr_strain_t))

#include "eOcfg_nvsEP_as_any_con_sxxmacro.c"



// strain 02
#define SMACRO_PSTR      _wholebody
#define SMACRO_STR       _s02
#define SMACRO_PNUM      5
#define SMACRO_NUM       2    
#define SMACRO_OFF      (SMACRO_NUM*sizeof(eOsnsr_strain_t))

#include "eOcfg_nvsEP_as_any_con_sxxmacro.c"


// strain 03
#define SMACRO_PSTR      _wholebody
#define SMACRO_STR       _s03
#define SMACRO_PNUM      5
#define SMACRO_NUM       3    
#define SMACRO_OFF       (SMACRO_NUM*sizeof(eOsnsr_strain_t))

#include "eOcfg_nvsEP_as_any_con_sxxmacro.c"

#define OFFSET_OF_END_OF_STRAINS    (strainWholeBody_TOTALnumber*sizeof(eOsnsr_strain_t))


// -- the 2 mais

#define MMACRO_EXTERNALPREFIX_GETID                EOK_cfg_nvsEP_as_any_con_body_NVID_mxx
        

// mais 00
#define MMACRO_PSTR    _wholebody
#define MMACRO_STR    _m00
#define MMACRO_PNUM    5
#define MMACRO_NUM    0    
#define MMACRO_OFF    (OFFSET_OF_END_OF_STRAINS+MMACRO_NUM*sizeof(eOsnsr_mais_t))

#include "eOcfg_nvsEP_as_any_con_mxxmacro.c"


// mais 01
#define MMACRO_PSTR    _wholebody
#define MMACRO_STR    _m01
#define MMACRO_PNUM    5
#define MMACRO_NUM    1    
#define MMACRO_OFF    (OFFSET_OF_END_OF_STRAINS+MMACRO_NUM*sizeof(eOsnsr_mais_t))

#include "eOcfg_nvsEP_as_any_con_mxxmacro.c"



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


extern const eOsnsr_strain_t* eo_cfg_nvsEP_as_wholebody_strain_defaultvalue = &eo_cfg_nvsEP_as_any_con_sxxdefault_defaultvalue;
extern const eOsnsr_mais_t* eo_cfg_nvsEP_as_wholebody_mais_defaultvalue = &eo_cfg_nvsEP_as_any_con_mxxdefault_defaultvalue;

// EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber is  4 and keeps the number of nvs per strain
#define Snvs    (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber)

// strain_TOTALnumber is 4 and is the number of strain in the wholebody
#define SNUM    (strainWholeBody_TOTALnumber)


// it is the start of index for the strain
#define Sstart  (0)

#define Sindex(s, i)   (Sstart + (s)*Snvs + (i))




// EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber is 4 and keeps the number of nvs per mais
#define Mnvs    (EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber)

// mais_TOTALnumber is 2 and is the number of mais in the wholebody
#define MNUM     (maisWholeBody_TOTALnumber)


// it is the start of index for the motors
#define Mstart  (Snvs*SNUM+0)

#define Mindex(m, i)   (Mstart + (m)*Mnvs + (i))


//extern EOtreenode eo_cfg_nvsEP_as_wholebody_tree_con[];

extern EOtreenode eo_cfg_nvsEP_as_wholebody_tree_con[] =
{
    // strainLeftArm_00
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s00_sconfig__mode,
        EO_INIT(.index)     Sindex(0, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s00_sconfig__datarate,
        EO_INIT(.index)     Sindex(0, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s00_sconfig__signaloncefullscale,
        EO_INIT(.index)     Sindex(0, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s00_sstatus__fullscale,
        EO_INIT(.index)     Sindex(0, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s00_sstatus__calibratedvalues,
        EO_INIT(.index)     Sindex(0, 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s00_sstatus__uncalibratedvalues,
        EO_INIT(.index)     Sindex(0, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },   
    
    // strainRigthArm_01
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s01_sconfig__mode,
        EO_INIT(.index)     Sindex(1, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s01_sconfig__datarate,
        EO_INIT(.index)     Sindex(1, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s01_sconfig__signaloncefullscale,
        EO_INIT(.index)     Sindex(1, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s01_sstatus__fullscale,
        EO_INIT(.index)     Sindex(1, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s01_sstatus__calibratedvalues,
        EO_INIT(.index)     Sindex(1, 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s01_sstatus__uncalibratedvalues,
        EO_INIT(.index)     Sindex(1, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },   
    
    // strainLeftLeg_02
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s02_sconfig__mode,
        EO_INIT(.index)     Sindex(2, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s02_sconfig__datarate,
        EO_INIT(.index)     Sindex(2, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s02_sconfig__signaloncefullscale,
        EO_INIT(.index)     Sindex(2, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s02_sstatus__fullscale,
        EO_INIT(.index)     Sindex(2, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },   
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s02_sstatus__calibratedvalues,
        EO_INIT(.index)     Sindex(2, 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s02_sstatus__uncalibratedvalues,
        EO_INIT(.index)     Sindex(2, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },   
    
    // strainRigthLeg_03
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s03_sconfig__mode,
        EO_INIT(.index)     Sindex(3, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s03_sconfig__datarate,
        EO_INIT(.index)     Sindex(3, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s03_sconfig__signaloncefullscale,
        EO_INIT(.index)     Sindex(3, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s03_sstatus__fullscale,
        EO_INIT(.index)     Sindex(3, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s03_sstatus__calibratedvalues,
        EO_INIT(.index)     Sindex(3, 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_s03_sstatus__uncalibratedvalues,
        EO_INIT(.index)     Sindex(3, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },   
    
    
    // the 2 mais
    
    // maisLeftHand_00
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m00_mconfig__mode,
        EO_INIT(.index)     Mindex(0, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m00_mconfig__datarate,
        EO_INIT(.index)     Mindex(0, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m00_mconfig__resolution,
        EO_INIT(.index)     Mindex(0, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m00_mstatus__the15values,
        EO_INIT(.index)     Mindex(0, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  

    // maisRigthHand_01
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m01_mconfig__mode,
        EO_INIT(.index)     Mindex(1, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m01_mconfig__datarate,
        EO_INIT(.index)     Mindex(1, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m01_mconfig__resolution,
        EO_INIT(.index)     Mindex(1, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_wholebody_m01_mstatus__the15values,
        EO_INIT(.index)     Mindex(1, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    }         
    
};  EO_VERIFYsizeof(eo_cfg_nvsEP_as_wholebody_tree_con, sizeof(EOtreenode)*(varsASwholebody_TOTALnumber));





const EOconstvector  s_eo_cfg_nvsEP_as_wholebody_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_as_wholebody_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_wholebody_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_as_wholebody_tree_con
};


extern const EOconstvector* const eo_cfg_nvsEP_as_wholebody_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_as_wholebody_constvector_of_treenodes_EOnv_con;


extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_as_wholebody_fptr_hashfunction_id2index = eo_cfg_nvsEP_as_wholebody_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_as_wholebody_hashfunction_id2index(uint16_t id)
{

    // 4*4
    #define IDTABLESSIZE        (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber*strainWholeBody_TOTALnumber)
    
    // 4*2
    #define IDTABLEMSIZE        (EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber*maisWholeBody_TOTALnumber)    
    
    #define IDTABLESIZE         (IDTABLESSIZE+IDTABLEMSIZE)
    

    // in order to always have a hit the table s_idtable[] it must be of size equal to max{ s_hash(id) }, thus if we
    // use an id of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have ids = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16

    static const uint16_t s_idtable[] = 
    { 
        // s00
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__mode(0),                   EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__datarate(0),
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__signaloncefullscale(0),    EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__fullscale(0), 
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__calibratedvalues(0),       EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__uncalibratedvalues(0),  
        // s01
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__mode(1),                   EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__datarate(1),
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__signaloncefullscale(1),    EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__fullscale(1), 
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__calibratedvalues(1),       EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__uncalibratedvalues(1),  
        // s02
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__mode(2),                   EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__datarate(2),
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__signaloncefullscale(2),    EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__fullscale(2), 
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__calibratedvalues(2),       EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__uncalibratedvalues(2),  
        // s03
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__mode(3),                   EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__datarate(3),
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__signaloncefullscale(3),    EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__fullscale(3),         
        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__calibratedvalues(3),       EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__uncalibratedvalues(3),  
        
        // m00
        EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__mode(0),                   EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__datarate(0),
        EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__resolution(0),             EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mstatus__the15values(0), 
        // m01
        EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__mode(1),                   EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__datarate(1),
        EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__resolution(1),             EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mstatus__the15values(1)      
    };  EO_VERIFYsizeof(s_idtable, sizeof(uint16_t)*(IDTABLESIZE));
    
    uint16_t index = s_hash(id);
    
   
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



typedef uint8_t sdfg[ ( EOK_cfg_nvsEP_as_any_con_sxx_maxnumof_nvs_in_strain == 16 ) ? (1) : (0)];
typedef uint8_t redf[ ( EOK_cfg_nvsEP_as_any_con_mxx_maxnumof_nvs_in_mais == 16 ) ? (1) : (0)];

static uint16_t s_hash(uint16_t id)
{
    uint16_t off = EO_nv_OFF(id);
    uint16_t a;
    uint16_t b;
    uint16_t r;
    
    if(off < EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_mais(0))
    {
        a = off >> 4;
        b = off - (a << 4);
        r = a*EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber+b;
    }
    else
    {
        off -= EOK_cfg_nvsEP_as_any_con_body_firstNVIDoff_of_mais(0);
        a = off >> 4;
        b = off - (a << 4);
        r = a*EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber+b;
        r += (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber*strainWholeBody_TOTALnumber);
    }
    
    return(r);
}



extern eOnvID_t eo_cfg_nvsEP_as_wholebody_strain_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_strainNumber_t s, eo_cfg_nvsEP_as_wholebody_con_strainNVindex_t snvindex)
{
    return(eo_cfg_nvsEP_as_any_con_body_strain_NVID_Get((eo_cfg_nvsEP_as_any_con_body_strainNumber_t)s, snvindex));    
}


extern eOnvID_t eo_cfg_nvsEP_as_wholebody_mais_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_maisNumber_t m, eo_cfg_nvsEP_as_wholebody_con_maisNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_as_any_con_body_mais_NVID_Get((eo_cfg_nvsEP_as_any_con_body_maisNumber_t)m, mnvindex));
} 



// extern eOnvID_t eo_cfg_nvsEP_as_wholebody_strain_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_strainNumber_t s, eo_cfg_nvsEP_as_wholebody_con_strainNVindex_t snvindex)
// {
//     if((s >= strainWholeBody_TOTALnumber) || (snvindex >= EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber))
//     {
//         return(EOK_uint16dummy);
//     }
//     return(EO_nv_ID(eo_cfg_nvsEP_as_any_con_sxx_funtyp[snvindex], EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_strain(s, snvindex)));    
// }

// extern eOnvID_t eo_cfg_nvsEP_as_wholebody_mais_NVID_Get(eo_cfg_nvsEP_as_wholebody_con_maisNumber_t m, eo_cfg_nvsEP_as_wholebody_con_maisNVindex_t mnvindex)
// {
//     if((m >= maisWholeBody_TOTALnumber) || (mnvindex >= EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber))
//     {
//         return(EOK_uint16dummy);
//     }
//     return(EO_nv_ID(eo_cfg_nvsEP_as_any_con_mxx_funtyp[mnvindex], EOK_cfg_nvsEP_as_any_con_body_NVIDoff_of_mais(m, mnvindex)));    
// }

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



