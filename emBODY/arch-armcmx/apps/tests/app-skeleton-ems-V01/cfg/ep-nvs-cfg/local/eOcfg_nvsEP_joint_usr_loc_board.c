
/* @file       eOcfg_nvsEP_base_usr_loc_anydev.c
    @brief      This file keeps constant configuration for the NVs of the updater
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
#include "eOcfg_nvsEP_joint_con.h"
#include "EOMtheCallbackManager.h"




#include "EOnv_hid.h"


#include "EOconstvector_hid.h"

#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "eEcommon.h"

#include "EoMotionControl.h"

#include "EOappCanServicesProvider.h"

#include "eom_appSkeletonEms_body.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_joint_usr_loc_board.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__cfg(const EOnv* nv, const eOabstime_t time, const uint32_t sign);

static void s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__setPoint(const EOnv* nv, const eOabstime_t time, const uint32_t sign);

static void s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__status(const EOnv* nv, const eOabstime_t time, const uint32_t sign);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
                                  
static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_joint_usr_loc_board_action_peripheralinterface__cfg =
{
    EO_INIT(.init)      NULL,
    EO_INIT(.update)    s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__cfg
};


static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_joint_usr_loc_board_action_peripheralinterface__setPoint =
{
    EO_INIT(.init)      NULL,
    EO_INIT(.update)    s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__setPoint
};

static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_joint_usr_loc_board_action_peripheralinterface__status =
{
    EO_INIT(.init)      NULL,
    EO_INIT(.update)    s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__status
};

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

static const EOnv_usr_t s_eo_cfg_nvsEP_joint_usr_loc_board_array_of_EOnv_usr[] =
{
    {   // 00 __cfg
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_joint_usr_loc_board_action_peripheralinterface__cfg,
        EO_INIT(.on_rop_reception)      NULL, //metti qui nella azione di before lo sto di tutta la barachetta!!!
        EO_INIT(.stg_address)           EOK_uint32dummy
    },
    {   // 01 __setPoint
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_joint_usr_loc_board_action_peripheralinterface__setPoint,
        EO_INIT(.on_rop_reception)      NULL,
        EO_INIT(.stg_address)           EOK_uint32dummy
    }
    ,
    {   // 01 __status
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_joint_usr_loc_board_action_peripheralinterface__status,
        EO_INIT(.on_rop_reception)      NULL,
        EO_INIT(.stg_address)           EOK_uint32dummy
    }
};



const EOconstvector  s_eo_cfg_nvsEP_joint_usr_loc_board_constvector_of_EOnv_usr = 
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_joint_usr_loc_board_array_of_EOnv_usr)/sizeof(EOnv_usr_t), 
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_joint_usr_loc_board_array_of_EOnv_usr
};


extern const EOconstvector* const eo_cfg_nvsEPjoint_usr_loc_board_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_joint_usr_loc_board_constvector_of_EOnv_usr;

extern eo_cfg_nvsEP_joint_t* eo_cfg_nvsEP_joint_usr_loc_board_mem_local = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_joint_usr_loc_board_initialise(void* loc, void* rem)
{    
    eo_cfg_nvsEP_joint_t *vol = (eo_cfg_nvsEP_joint_t*)loc;
    // in here rem is NULL....

    eo_cfg_nvsEP_joint_usr_loc_board_mem_local = vol;

    // assign default values
    memcpy(vol, &eo_cfg_nvsEP_joint_default, sizeof(eo_cfg_nvsEP_joint_t)); //sizeof(eo_cfg_nvsEP_base_t));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__cfg(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmc_joint_config_t *cfg = nv->loc;

    eOmc_jointUniqueId_t jUniqueId = 0;
    EOappCanSP *appCanSP_ptr = (EOappCanSP *)eom_appSkeletonEms_body_services_can_getHandle();

    eo_appCanSP_ConfigJoint(appCanSP_ptr, jUniqueId, cfg);

 //   eo_appCanSP_ConfigJoint(jUniqueId, cfg);
 #warning: vale-->complete action_update__cfg
}


static void s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__setPoint(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmc_setpoint_t *setPoint = nv->loc;

    eOmc_jointUniqueId_t jUniqueId = 0;
//    eo_ap_canMod_SendSetPoint(jUniqueId, setPoint);
#warning: vale-->complete action_update__setPoint
    
}


static void s_eo_cfg_nvsEP_joint_usr_loc_board_action_update__status(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmc_setpoint_t *setPoint = nv->loc;
    #warning: vale-->complete action_update_status     
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



