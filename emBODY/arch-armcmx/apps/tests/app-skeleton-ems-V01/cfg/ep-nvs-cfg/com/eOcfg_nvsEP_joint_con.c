
/* @file       eOcfg_nvsEP_joint_con.c
    @brief      This file keeps constant configuration for the NVs of the joint
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

#include "eEcommon.h"
//#include "shalPART.h"
//#include "shalINFO.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_joint_con.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define EOK_cfg_nvsEP_base_numberof                                 10







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



// this struct contains the default values of the entities of the whole device

extern const eo_cfg_nvsEP_joint_t eo_cfg_nvsEP_joint_default =
{
    EO_INIT(.cfg)             {0},
    EO_INIT(.setPoint)        {0},
    EO_INIT(.status)
    {
        EO_INIT(.position)          0xAABBCCDD,          
        EO_INIT(.velocity)          0x1122,          
        EO_INIT(.acceleration)      0x4455, 
        EO_INIT(.torque)            0x6677,  
    },
    EO_INIT(.notanv_filler0)  {0}
}; 

  



// now we have the const netvars


#define OFFSETof__cfg                               (0)
#define CAPACITY__cfg                                sizeof(eOmc_joint_config_t)
extern EOnv_con_t eo_cfg_nvsEP_joint__cfg =
{   // pos =  0
    EO_INIT(.id)        EOK_cfg_nvsEP_joint_NVID__cfg,
    EO_INIT(.capacity)  CAPACITY__cfg,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_joint_default.cfg,
    EO_INIT(.offset)    OFFSETof__cfg,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_joint_NVFUNTYP__cfg),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_joint_NVFUNTYP__cfg)
};
#define OFFSETafter__cfg                      (OFFSETof__cfg+CAPACITY__cfg)


#define OFFSETof__setPoint                   OFFSETafter__cfg
#define CAPACITY__setPoint                   sizeof(eOmc_setpoint_t)
extern EOnv_con_t eo_cfg_nvsEP_joint__setPoint =
{   // pos =  1
    EO_INIT(.id)        EOK_cfg_nvsEP_joint_NVID__setPoint,
    EO_INIT(.capacity)  CAPACITY__setPoint,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_joint_default.setPoint,
    EO_INIT(.offset)    OFFSETof__setPoint,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_joint_NVFUNTYP__setPoint),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_joint_NVFUNTYP__setPoint)
};
#define OFFSETafter__setPoint                (OFFSETof__setPoint+CAPACITY__setPoint)


#define OFFSETof__status                   OFFSETafter__setPoint
#define CAPACITY__status                   sizeof(eOmc_joint_status_t)
extern EOnv_con_t eo_cfg_nvsEP_joint__status =
{   // pos =  2
    EO_INIT(.id)        EOK_cfg_nvsEP_joint_NVID__status,
    EO_INIT(.capacity)  CAPACITY__status,
    EO_INIT(.resetval)  (const void*)&eo_cfg_nvsEP_joint_default.status,
    EO_INIT(.offset)    OFFSETof__status,
    EO_INIT(.typ)       EO_nv_TYP(EOK_cfg_nvsEP_joint_NVFUNTYP__status),
    EO_INIT(.fun)       EO_nv_FUN(EOK_cfg_nvsEP_joint_NVFUNTYP__status)
};
#define OFFSETafter__status                (OFFSETof__status+CAPACITY__status)

#define OFFSETof__notanv_filler0                    OFFSETafter__status
#define CAPACITY__notanv_filler0                    sizeof(eo_cfg_nvsEP_joint_default.notanv_filler0)
#define OFFSETafter__notanv_filler0                 (OFFSETof__notanv_filler0 + CAPACITY__notanv_filler0)


// guard on alignement of variables. if it doesnt compile then ... the compiler has surely inserted some holes
typedef uint8_t eo_cfg_nvsEP_joint_t_GUARD[ ( (OFFSETafter__notanv_filler0) == sizeof(eo_cfg_nvsEP_joint_t) ) ? (1) : (0)];

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

extern EOtreenode eo_cfg_nvsEP_joint_tree_con[];

extern EOtreenode eo_cfg_nvsEP_joint_tree_con[] =
{
    {   // 0
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_joint__cfg,
        EO_INIT(.index)     0,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 1
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_joint__setPoint,
        EO_INIT(.index)     1,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    },
    {   // 2
        EO_INIT(.data)      (void*)&eo_cfg_nvsEP_joint__status,
        EO_INIT(.index)     2,
        EO_INIT(.nchildren) 0,
        EO_INIT(.ichildren) {0},
        EO_INIT(.pchildren) {NULL}
    }
};

//typedef uint8_t eo_cfg_nvsEP_base_guard_t[ (EOK_cfg_nvsEP_base_numberof == (sizeof(eo_cfg_nvsEP_base_tree_con)/sizeof(const EOtreenode))) ? (1) : (0)];



const EOconstvector  s_eo_cfg_nvsEP_joint_constvector_of_treenodes_EOnv_con = 
{
    EO_INIT(.size)              sizeof(eo_cfg_nvsEP_joint_tree_con)/sizeof(EOtreenode), //EOK_cfg_nvsEP_joint_numberof,
    EO_INIT(.item_size)         sizeof(EOtreenode),
    EO_INIT(.item_array_data)   eo_cfg_nvsEP_joint_tree_con
};


extern const EOconstvector* const eo_cfg_nvsEP_joint_constvector_of_treenodes_EOnv_con = &s_eo_cfg_nvsEP_joint_constvector_of_treenodes_EOnv_con;


extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_joint_fptr_hashfunction_id2index = eo_cfg_nvsEP_joint_hashfunction_id2index;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_joint_hashfunction_id2index(uint16_t id)
{
    #define IDTABLESIZE     3

    // in order to always have a hit the table s_idtable[] it must be of size equal to max{ s_hash(id) }, thus if we
    // use an id of value 16 and s_hash() just keeps the lsb, then the size must be 17 
    // if there are holes, they shall have EOK_uint16dummy in other entries. for example, if we have ids = {0, 7, 16}
    // then the table shall be of size 17, shall contain 0xffff everywhere but in positions 0, 7, 16 where the values
    // are ... 0, 7, 16

    static const uint16_t s_idtable[IDTABLESIZE] = 
    { 
        EOK_cfg_nvsEP_joint_NVID__cfg,  
        EOK_cfg_nvsEP_joint_NVID__setPoint,
        EOK_cfg_nvsEP_joint_NVID__status
    };
    
    uint16_t index = s_hash(id);
    
    if((index < IDTABLESIZE) && (id == s_idtable[index]) )
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



static uint16_t s_hash(uint16_t id)
{
    return((id - EOK_cfg_nvsEP_joint) & 0x03FF);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



