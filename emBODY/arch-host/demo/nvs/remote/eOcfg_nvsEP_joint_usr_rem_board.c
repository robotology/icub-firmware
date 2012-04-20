
/* @file       eOcfg_nvsEP_joint_usr_rem_board.c
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

#include <pthread.h>

#include "EoCommon.h"
#include "eOcfg_nvsEP_base_con.h"

#include "EOnv_hid.h"

#include "EOconstvector_hid.h"

#include "eEcommon.h"
//#include "shalBASE.h"
//#include "shalPART.h"
//#include "shalINFO.h"


//#include "hal.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_joint_usr_rem_board.h"


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

static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception_act__cfg(const EOnv* nv, const eOabstime_t time, const uint32_t sign);
static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__cfg(const EOnv* nv, const eOabstime_t time, const uint32_t sign);

static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__setPoint(const EOnv* nv, const eOabstime_t time, const uint32_t sign);

static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__status(const EOnv* nv, const eOabstime_t time, const uint32_t sign);

static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception_act__cfg(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eo_cfg_nvsEP_joint_t *cfg = (eo_cfg_nvsEP_joint_t*)nv->rem;
}

static const eOnv_fn_onrop_2fn_t  s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception__cfg =
{
	 EO_INIT(.bef)	s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception_act__cfg,
	 EO_INIT(.aft)	s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception_act__cfg
};

static const eOnv_fn_onrop_rx_loc_t	s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_rop_reception_rem__cfg =
{
	EO_INIT(.set)	&s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception__cfg,
	EO_INIT(.ask)	&s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_SigSay_reception__cfg
};

static const eOnv_fn_onrop_rx_t s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_rop_reception__cfg=
{
	EO_INIT(.loc)	&s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_rop_reception_rem__cfg
};

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_joint_usr_rem_board_action_peripheralinterface__cfg =
{
    EO_INIT(.init)      NULL,
    EO_INIT(.update)    s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__cfg
};


static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_joint_usr_rem_board_action_peripheralinterface__setPoint =
{
    EO_INIT(.init)      NULL,
    EO_INIT(.update)    s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__setPoint
};

static const eOnv_fn_peripheral_t s_eo_cfg_nvsEP_joint_usr_rem_board_action_peripheralinterface__status =
{
    EO_INIT(.init)      NULL,
    EO_INIT(.update)    s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__status
};



static char s_str[128];

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

static EOnv_usr_t s_eo_cfg_nvsEP_joint_usr_rem_board_array_of_EOnv_usr[] =
{
    {   // 00 __cfg
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_joint_usr_rem_board_action_peripheralinterface__cfg,
        EO_INIT(.on_rop_reception)      NULL, //&s_eo_cfg_nvsEP_joint_usr_rem_board_action_on_rop_reception__cfg,
        EO_INIT(.stg_address)           EOK_uint32dummy
    },
    {   // 01 __setPoint
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_joint_usr_rem_board_action_peripheralinterface__setPoint,
        EO_INIT(.on_rop_reception)      NULL,
        EO_INIT(.stg_address)           EOK_uint32dummy
    },
    {   // 02 __status
        EO_INIT(.peripheralinterface)   &s_eo_cfg_nvsEP_joint_usr_rem_board_action_peripheralinterface__status,
        EO_INIT(.on_rop_reception)      NULL,
        EO_INIT(.stg_address)           EOK_uint32dummy
    }
};



const EOconstvector  s_eo_cfg_nvsEP_joint_usr_rem_board_constvector_of_EOnv_usr =
{
    EO_INIT(.size)              sizeof(s_eo_cfg_nvsEP_joint_usr_rem_board_array_of_EOnv_usr)/sizeof(EOnv_usr_t),
    EO_INIT(.item_size)         sizeof(EOnv_usr_t),
    EO_INIT(.item_array_data)   s_eo_cfg_nvsEP_joint_usr_rem_board_array_of_EOnv_usr
};


extern const EOconstvector* const eo_cfg_nvsEP_joint_usr_rem_board_constvector_of_EOnv_usr = &s_eo_cfg_nvsEP_joint_usr_rem_board_constvector_of_EOnv_usr;


extern eo_cfg_nvsEP_joint_t* eo_cfg_nvsEP_joint_usr_rem_board_mem_local  = NULL;
extern eo_cfg_nvsEP_joint_t* eo_cfg_nvsEP_joint_usr_rem_board_mem_remote = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_joint_usr_rem_board_initialise(void* loc, void* rem)
{    
    eo_cfg_nvsEP_joint_t *locmem = (eo_cfg_nvsEP_joint_t*)loc;
    eo_cfg_nvsEP_joint_t *remmem = (eo_cfg_nvsEP_joint_t*)rem;

    eo_cfg_nvsEP_joint_usr_rem_board_mem_local      = locmem;
    eo_cfg_nvsEP_joint_usr_rem_board_mem_remote     = remmem;

    // assign default values
    memcpy(loc, &eo_cfg_nvsEP_joint_default, sizeof(eo_cfg_nvsEP_joint_t));
    memcpy(rem, &eo_cfg_nvsEP_joint_default, sizeof(eo_cfg_nvsEP_joint_t));


    //#warning --> initialise whatever we want with variables
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__cfg(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	// unused, just to check if the received values is stored correctly.
	// move the unlock mutex in the on_rop_receprion->say callback
	eo_cfg_nvsEP_joint_t *cfg = (eo_cfg_nvsEP_joint_t*)nv->rem;
	pthread_mutex_unlock(nv->mtx);
    snprintf(s_str, sizeof(s_str)-1, "REM has updated cfg.");
    hal_trace_puts(s_str);
}

static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__setPoint(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eEmoduleInfo_t *setPoint = nv->rem;
    eOabstime_t t = time;
    uint32_t s = sign;
    setPoint = setPoint;
    t = t;
    s = s;
    snprintf(s_str, sizeof(s_str)-1, "REM has updated applinfo w/ appl.info.entity.version = (maj = %d, min = %d)", 
                                      setPoint->info.entity.version.major, setPoint->info.entity.version.minor);
    hal_trace_puts(s_str);

}

static void s_eo_cfg_nvsEP_joint_usr_rem_board_action_update__status(const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eEmoduleInfo_t *status = nv->rem;
    eOabstime_t t = time;
    uint32_t s = sign;
    status = status;
    t = t;
    s = s;
    snprintf(s_str, sizeof(s_str)-1, "REM has updated applinfo w/ appl.info.entity.version = (maj = %d, min = %d)", 
    		status->info.entity.version.major, status->info.entity.version.minor);
    hal_trace_puts(s_str);

}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



