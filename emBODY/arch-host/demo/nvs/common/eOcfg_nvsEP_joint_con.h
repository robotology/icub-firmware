
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOCFG_NVSEP_JOINT_CON_H_
#define _EOCFG_NVSEP_JOINT_CON_H_




/** @file       eOcfg_nvsEP_joint_con.h
	@brief      This header file gives the constant configuration for the NVs in the joint endpoint port
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_asfdgr Configuation of the NVs for the updater
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOconstvector.h"

#include "eEcommon.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoMotionControl.h"



// - public #define  --------------------------------------------------------------------------------------------------

#define EOK_cfg_nvsEP_joint                                          (64*2)

#define EOK_cfg_nvsEP_joint_endpoint                                 (2)


// -- the fun and typ of all the nv in the endpoint

#define EOK_cfg_nvsEP_joint_NVFUNTYP__setPoint                       EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_joint_NVFUNTYP__cfg                            EO_nv_FUNTYP(eo_nv_FUN_cfg, eo_nv_TYP_pkd)
#define EOK_cfg_nvsEP_joint_NVFUNTYP__status                         EO_nv_FUNTYP(eo_nv_FUN_out, eo_nv_TYP_pkd)

// -- the identifiers of all the nv in the endpoint

#define EOK_cfg_nvsEP_joint_NVID__cfg                                EO_nv_ID(EOK_cfg_nvsEP_joint_NVFUNTYP__cfg,            0+EOK_cfg_nvsEP_joint)
#define EOK_cfg_nvsEP_joint_NVID__setPoint                           EO_nv_ID(EOK_cfg_nvsEP_joint_NVFUNTYP__setPoint,      1+EOK_cfg_nvsEP_joint)
#define EOK_cfg_nvsEP_joint_NVID__status                             EO_nv_ID(EOK_cfg_nvsEP_joint_NVFUNTYP__status,      2+EOK_cfg_nvsEP_joint)

// - declaration of public user-defined types ------------------------------------------------------------------------- 



// - very important information: on ARM alignment is done using "#pragma pack(8)", thus ... there are rules to follow

typedef struct                  // 128 = 84+12+28+4
{
	eOmc_joint_config_t         cfg;
	eOmc_setpoint_t             setPoint;
    eOmc_joint_status_t         status;
    uint8_t                     notanv_filler0[4];
} eo_cfg_nvsEP_joint_t;         EO_VERIFYsizeof(eo_cfg_nvsEP_joint_t, 128);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eo_cfg_nvsEP_joint_t eo_cfg_nvsEP_joint_default;

// EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_joint_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_joint_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_joint_fptr_hashfunction_id2index;



// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_joint_hashfunction_id2index(uint16_t id);





/** @}            
    end of group eo_asfdgr  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




