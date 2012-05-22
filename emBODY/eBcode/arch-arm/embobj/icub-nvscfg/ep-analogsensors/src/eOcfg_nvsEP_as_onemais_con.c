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

/* @file       eOcfg_nvsEP_as_onemais_con.c
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

#include "eOcfg_nvsEP_as_any_con_body.h"

#include "eOcfg_nvsEP_as_any_con_body_hid.h"    

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_onemais_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_onemais_con_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_hash(uint16_t id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// -- the 0 strain

//#define SMACRO_EXTERNALPREFIX_GETID                 EOK_cfg_nvsEP_as_any_con_body_NVID_sxx


// strain 00
//#define SMACRO_PSTR      _onemais
//#define SMACRO_STR       _s00
//#define SMACRO_PNUM      5
//#define SMACRO_NUM       0    
//#define SMACRO_OFF      (SMACRO_NUM*sizeof(eOsnsr_strain_t))

//#include "eOcfg_nvsEP_as_any_con_sxxmacro.c"


#define OFFSET_OF_END_OF_STRAINS    (strainOneMais_TOTALnumber*sizeof(eOsnsr_strain_t))


// -- the 1 mais

#define MMACRO_EXTERNALPREFIX_GETID                 EOK_cfg_nvsEP_as_any_con_body_NVID_mxx
        

// mais 00
#define MMACRO_PSTR    _onemais
#define MMACRO_STR    _m00
#define MMACRO_PNUM    5
#define MMACRO_NUM    0    
#define MMACRO_OFF    (OFFSET_OF_END_OF_STRAINS+MMACRO_NUM*sizeof(eOsnsr_mais_t))

#include "eOcfg_nvsEP_as_any_con_mxxmacro.c"





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


#define Snvs    (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber)

// strainOneMais_TOTALnumber is 1 and is the number of strain in the onemais
#define SNUM    (strainOneMais_TOTALnumber)


// it is the start of index for the strain
#define Sstart  (0)

#define Sindex(s, i)   (Sstart + (s)*Snvs + (i))




#define Mnvs    (EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber)

// mais_TOTALnumber is 0 and is the number of mais in the onemais
#define MNUM     (maisOneMais_TOTALnumber)


// it is the start of index for the motors
#define Mstart  (Snvs*SNUM+0)

#define Mindex(m, i)   (Mstart + (m)*Mnvs + (i))



EOtreenode eo_cfg_nvsEP_as_onemais_tree_con[] =
{
#if 0
    // strain s00
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_s00_sconfig__mode,
        EO_INIT(.index)     Sindex(0, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_s00_sconfig__datarate,
        EO_INIT(.index)     Sindex(0, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_s00_sconfig__signaloncefullscale,
        EO_INIT(.index)     Sindex(0, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_s00_sstatus__fullscale,
        EO_INIT(.index)     Sindex(0, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 4
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_s00_sstatus__calibratedvalues,
        EO_INIT(.index)     Sindex(0, 4),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 5
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_s00_sstatus__uncalibratedvalues,
        EO_INIT(.index)     Sindex(0, 5),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    }   
    
#endif     
    
    // the 1 mais
#if 1  

    // mais_00
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_m00_mconfig__mode,
        EO_INIT(.index)     Mindex(0, 0),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_m00_mconfig__datarate,
        EO_INIT(.index)     Mindex(0, 1),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },  
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_m00_mconfig__resolution,
        EO_INIT(.index)     Mindex(0, 2),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 3
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_as_onemais_m00_mstatus__the15values,
        EO_INIT(.index)     Mindex(0, 3),
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    }
    
#endif
       
    
};  EO_VERIFYsizeof(eo_cfg_nvsEP_as_onemais_tree_con, sizeof(EOtreenode)*(varsASonemais_TOTALnumber));





const EOconstvector  s_eo_cfg_nvsEP_as_onemais_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_as_onemais_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_onemais_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_as_onemais_tree_con
};


const EOconstvector* const eo_cfg_nvsEP_as_onemais_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_as_onemais_constvector_of_treenodes_EOnv_con;


const eOuint16_fp_uint16_t eo_cfg_nvsEP_as_onemais_fptr_hashfunction_id2index = eo_cfg_nvsEP_as_onemais_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_as_onemais_hashfunction_id2index(uint16_t id)
{

    #define IDTABLESSIZE        (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber*strainOneMais_TOTALnumber)
    
    #define IDTABLEMSIZE        (EOK_cfg_nvsEP_as_any_con_mxx_mnvindex_TOTALnumber*maisOneMais_TOTALnumber)    
    
    #define IDTABLESIZE         (IDTABLESSIZE+IDTABLEMSIZE)
    
    uint16_t index;
    
    static const uint16_t s_idtable[] = 
    { 
        // s00
//         EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__mode(0),                    EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__datarate(0),
//         EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sconfig__signaloncefullscale(0),     EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__fullscale(0), 
//         EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__calibratedvalues(0),        EOK_cfg_nvsEP_as_any_con_body_NVID_sxx_sstatus__uncalibratedvalues(0)  
        
        // m00
         EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__mode(0),                    EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__datarate(0),
         EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mconfig__resolution(0),              EOK_cfg_nvsEP_as_any_con_body_NVID_mxx_mstatus__the15values(0), 
    
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



EO_VERIFYproposition(xxx, ( EOK_cfg_nvsEP_as_any_con_sxx_maxnumof_nvs_in_strain == 16 ) );
EO_VERIFYproposition(xxx, ( EOK_cfg_nvsEP_as_any_con_mxx_maxnumof_nvs_in_mais == 16 ) );


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
        r += (EOK_cfg_nvsEP_as_any_con_sxx_snvindex_TOTALnumber*strainOneMais_TOTALnumber);
    }
    
    return(r);
}



extern eOnvID_t eo_cfg_nvsEP_as_onemais_strain_NVID_Get(eo_cfg_nvsEP_as_onemais_con_strainNumber_t s, eo_cfg_nvsEP_as_onemais_con_strainNVindex_t snvindex)
{
    //return(eo_cfg_nvsEP_as_any_con_body_strain_NVID_Get((eo_cfg_nvsEP_as_any_con_body_strainNumber_t)s, snvindex));
    return(EOK_uint16dummy);
}

extern eOnvID_t eo_cfg_nvsEP_as_onemais_mais_NVID_Get(eo_cfg_nvsEP_as_onemais_con_maisNumber_t m, eo_cfg_nvsEP_as_onemais_con_maisNVindex_t mnvindex)
{
    return(eo_cfg_nvsEP_as_any_con_body_mais_NVID_Get((eo_cfg_nvsEP_as_any_con_body_maisNumber_t)m, mnvindex));
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



