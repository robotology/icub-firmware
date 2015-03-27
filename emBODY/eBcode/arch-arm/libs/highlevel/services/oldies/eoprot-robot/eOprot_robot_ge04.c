/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eOprot_robot_ge04.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       04/14/2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EOconstvector.h"
#include "EOnvSet.h"
#include "EOconstvector_hid.h"


#include "eOprot_b01.h"
#include "eOprot_b02.h"
#include "eOprot_b03.h"
#include "eOprot_b04.h"
#include "eOprot_b05.h"
#include "eOprot_b06.h"
#include "eOprot_b07.h"
#include "eOprot_b08.h"
#include "eOprot_b09.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOprot_robot.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOprot_robot_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


enum { eoprot_robot_ge04_numberofboards = 9 };
enum { eoprot_robot_ge04_id = 0x00000004 };

EO_VERIFYproposition(eoprot_robot_gasdfe, eoprot_boards_maxnumberof >= eoprot_robot_ge04_numberofboards);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if     defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)  
static void s_eoprot_robot_override_mn(void);
static void s_eoprot_robot_override_mc(void);
static void s_eoprot_robot_override_as(void);
static void s_eoprot_robot_override_sk(void);
#endif//defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)  



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_eoprot_robot_initted = 0;

static const eOnvset_DEVcfg_t * const s_eoprot_robot_ge04_theDEVcfgptrs[] =
{  
    &eoprot_b01_nvsetDEVcfg,
    &eoprot_b02_nvsetDEVcfg,
    &eoprot_b03_nvsetDEVcfg,
    &eoprot_b04_nvsetDEVcfg,
    &eoprot_b05_nvsetDEVcfg,
    &eoprot_b06_nvsetDEVcfg,
    &eoprot_b07_nvsetDEVcfg,
    &eoprot_b08_nvsetDEVcfg,
    &eoprot_b09_nvsetDEVcfg   
};  EO_VERIFYsizeof(s_eoprot_robot_ge04_theDEVcfgptrs, sizeof(eOnvset_DEVcfg_t*)*(eoprot_robot_ge04_numberofboards));



static const EOconstvector s_eoprot_robot_ge04_constvectofDEVcfgptr = 
{
    EO_INIT(.size)                  sizeof(s_eoprot_robot_ge04_theDEVcfgptrs)/sizeof(eOnvset_DEVcfg_t*),
    EO_INIT(.item_size)             sizeof(eOnvset_DEVcfg_t*),
    EO_INIT(.item_array_data)       s_eoprot_robot_ge04_theDEVcfgptrs
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

#if 0 
    // use one for each board ... so that it is tuned with the board-transceiver
    EO_INIT(.sizes)
    {
        EO_INIT(.capacityoftxpacket)                EOK_HOSTTRANSCEIVER_capacityoftxpacket,
        EO_INIT(.capacityofrop)                     EOK_HOSTTRANSCEIVER_capacityofrop,
        EO_INIT(.capacityofropframeregulars)        EOK_HOSTTRANSCEIVER_capacityofropframeregulars,
        EO_INIT(.capacityofropframeoccasionals)     EOK_HOSTTRANSCEIVER_capacityofropframeoccasionals,
        EO_INIT(.capacityofropframereplies)         EOK_HOSTTRANSCEIVER_capacityofropframereplies,
        EO_INIT(.maxnumberofregularrops)            EOK_HOSTTRANSCEIVER_maxnumberofregularrops       
    },    
#endif

// the configuration of the eth protocol for the robot
//extern const eOprot_ROBOTcfg_t eoprot_robot_ROBOTcfg;
const eOprot_ROBOTcfg_t eoprot_robot_ROBOTcfg =
{
    EO_INIT(.id)                        eoprot_robot_ge04_id,
    EO_INIT(.family)                    0x00000001,
    EO_INIT(.name)                      "icub-ge04",
    EO_INIT(.vectorof_devcfgptr)        &s_eoprot_robot_ge04_constvectofDEVcfgptr
//    EO_INIT(.vectorof_transizesptr)     &s_eoprot_robot_ge04_constvectoftranssizesptr .... eOtransceiver_sizes_t
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eoprot_robot_Initialise(void)
{
    // at first: initialise the number of boards in protocol.
    if(eores_OK != eoprot_config_board_numberof(eoprot_robot_ge04_numberofboards))
    {
        return(eores_NOK_generic);
    }
    
    // then: override what is needed
    
#if     defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)    
  
    s_eoprot_robot_override_mn();
    
    s_eoprot_robot_override_mc();
    
    s_eoprot_robot_override_as();
    
    s_eoprot_robot_override_sk();

#endif//defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)   
   
    s_eoprot_robot_initted = 1;
       
    return(eores_OK);
}

extern uint32_t eoprot_robot_id_get(void)
{
    return(eoprot_robot_ROBOTcfg.id);
}

extern uint32_t eoprot_robot_family_get(void)
{
    return(eoprot_robot_ROBOTcfg.family);
}

extern const char * eoprot_robot_name_get(void)
{
    return(eoprot_robot_ROBOTcfg.name);
}


extern uint8_t eoprot_robot_DEVcfg_numberof(void)
{
    return(eo_constvector_Size(eoprot_robot_ROBOTcfg.vectorof_devcfgptr));
}

extern const eOnvset_DEVcfg_t* eoprot_robot_DEVcfg_get(uint8_t n)
{
    if(n > eoprot_robot_DEVcfg_numberof())
    {
        return(NULL);
    }
    
    if(0 == s_eoprot_robot_initted)
    {
        eoprot_robot_Initialise();
    }

    const eOnvset_DEVcfg_t **p = (const eOnvset_DEVcfg_t**) eo_constvector_At(eoprot_robot_ROBOTcfg.vectorof_devcfgptr, n);
    return(*p);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)  

// nothing to override in endpoint management.
// the robot just send rops set<>. it never sends rops ask<> to variables of this endpoint, nor it receives and sig<> 
static void s_eoprot_robot_override_mn(void)
{  

}


static void s_eoprot_robot_override_mc(void)
{
    static const eOprot_callbacks_endpoint_descriptor_t mc_callbacks_descriptor_endp = 
    { 
        EO_INIT(.endpoint)          eoprot_endpoint_motioncontrol, 
        EO_INIT(.raminitialise)     NULL // or any xxeoprot_fun_INITIALISE_mc 
    };
    
    static const eOprot_callbacks_variable_descriptor_t mc_callbacks_descriptors_vars[] = 
    { 
        // joint
        {   // joint_config
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_config,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_config
        },
        {   // joint_config_pidposition
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_config_pidposition,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_config_pidposition
        },
        {   // joint_config_impedance
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_config_impedance,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_config_impedance
        },
        {   // joint_config_pidtorque
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_config_pidtorque,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_config_pidtorque
        },
        {   // joint_status
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_status,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_status
        },
        {   // joint_status_basic
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_status_basic,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_status_basic
        },
        {   // joint_cmmnds_setpoint
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_cmmnds_setpoint,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_cmmnds_setpoint
        },
        {   // joint_config_limitsofjoint
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_joint,
            EO_INIT(.tag)           eoprot_tag_mc_joint_config_limitsofjoint,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_joint_config_limitsofjoint
        },
        // motor
        {   // motor_config
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_motor,
            EO_INIT(.tag)           eoprot_tag_mc_motor_config,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_motor_config
        },        
        {   // motor_config_maxcurrentofmotor
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_motor,
            EO_INIT(.tag)           eoprot_tag_mc_motor_config_maxcurrentofmotor,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor
        },
        {   // motor_config_gearboxratio
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_motor,
            EO_INIT(.tag)           eoprot_tag_mc_motor_config_gearboxratio,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_motor_config_gearboxratio
        },
        {   // motor_config_rotorencoder
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_motor,
            EO_INIT(.tag)           eoprot_tag_mc_motor_config_rotorencoder,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_motor_config_rotorencoder
        },
        {   // motor_status_basic
            EO_INIT(.endpoint)      eoprot_endpoint_motioncontrol,
            EO_INIT(.entity)        eoprot_entity_mc_motor,
            EO_INIT(.tag)           eoprot_tag_mc_motor_status_basic,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_mc_motor_status_basic
        }                
    };


    // ------------------------------------------------------------------------------------------------------------------------------------
    // -- general ram initialise of mc endpoint called by every board.
    
    // we dont do any general initialisation, even if we could do it with a xxeoprot_fun_INITIALISE_mc() function
    //eoprot_config_callbacks_endpoint_set(&mc_callbacks_descriptor_endp);


    // ------------------------------------------------------------------------------------------------------------------------------------
    // -- override of the callbacks of variables of mc. common to every board. we use the id, even if the eoprot_config_variable_callback()
    //    operates on any index.
    
    uint32_t number = sizeof(mc_callbacks_descriptors_vars)/sizeof(mc_callbacks_descriptors_vars[0]);
    uint32_t i = 0;
    
    for(i=0; i<number; i++)
    {
        eoprot_config_callbacks_variable_set(&mc_callbacks_descriptors_vars[i]);
    }

}


static void s_eoprot_robot_override_as(void)
{
    static const eOprot_callbacks_endpoint_descriptor_t as_callbacks_descriptor_endp = 
    { 
        EO_INIT(.endpoint)          eoprot_endpoint_analogsensors, 
        EO_INIT(.raminitialise)     NULL // or any xxeoprot_fun_INITIALISE_as 
    };
    
    static const eOprot_callbacks_variable_descriptor_t as_callbacks_descriptors_vars[] = 
    { 
        // strain
        {   // strain_status_calibratedvalues
            EO_INIT(.endpoint)      eoprot_endpoint_analogsensors,
            EO_INIT(.entity)        eoprot_entity_as_strain,
            EO_INIT(.tag)           eoprot_tag_as_strain_status_calibratedvalues,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_as_strain_status_calibratedvalues
        },
        {   // strain_status_uncalibratedvalues
            EO_INIT(.endpoint)      eoprot_endpoint_analogsensors,
            EO_INIT(.entity)        eoprot_entity_as_strain,
            EO_INIT(.tag)           eoprot_tag_as_strain_status_uncalibratedvalues,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_as_strain_status_uncalibratedvalues
        },
        // mais
        {   // mais_status_the15values
            EO_INIT(.endpoint)      eoprot_endpoint_analogsensors,
            EO_INIT(.entity)        eoprot_entity_as_mais,
            EO_INIT(.tag)           eoprot_tag_as_mais_status_the15values,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_as_mais_status_the15values
        }           
    };


    // ------------------------------------------------------------------------------------------------------------------------------------
    // -- general ram initialise of as endpoint called by every board.
    
    // we dont do any general initialisation, even if we could do it with a xxeoprot_fun_INITIALISE_as() function
    //eoprot_config_callbacks_endpoint_set(&as_callbacks_descriptor_endp);



    // ------------------------------------------------------------------------------------------------------------------------------------
    // -- override of the callbacks of variables of mc. common to every board. we use the id, even if the eoprot_config_variable_callback()
    //    operates on any index.
    
    uint32_t number = sizeof(as_callbacks_descriptors_vars)/sizeof(as_callbacks_descriptors_vars[0]);
    uint32_t i = 0;
    
    for(i=0; i<number; i++)
    {
        eoprot_config_callbacks_variable_set(&as_callbacks_descriptors_vars[i]);
    }
   
}


static void s_eoprot_robot_override_sk(void)
{
    static const eOprot_callbacks_endpoint_descriptor_t sk_callbacks_descriptor_endp = 
    { 
        EO_INIT(.endpoint)          eoprot_endpoint_skin, 
        EO_INIT(.raminitialise)     NULL // or any xxeoprot_fun_INITIALISE_sk 
    };
    
    static const eOprot_callbacks_variable_descriptor_t sk_callbacks_descriptors_vars[] = 
    { 
        // skin
        {   // strain_status_calibratedvalues
            EO_INIT(.endpoint)      eoprot_endpoint_skin,
            EO_INIT(.entity)        eoprot_entity_sk_skin,
            EO_INIT(.tag)           eoprot_tag_sk_skin_status_arrayof10canframes,
            EO_INIT(.init)          NULL,
            EO_INIT(.update)        eoprot_fun_UPDT_sk_skin_status_arrayof10canframes
        }    
    };


    // ------------------------------------------------------------------------------------------------------------------------------------
    // -- general ram initialise of sk endpoint called by every board.
    
    // we dont do any general initialisation, even if we could do it with a xxeoprot_fun_INITIALISE_sk() function
    //eoprot_config_callbacks_endpoint_set(&sk_callbacks_descriptor_endp);


    // ------------------------------------------------------------------------------------------------------------------------------------
    // -- override of the callbacks of variables of mc. common to every board. we use the id, even if the eoprot_config_variable_callback()
    //    operates on any index.
    
    uint32_t number = sizeof(sk_callbacks_descriptors_vars)/sizeof(sk_callbacks_descriptors_vars[0]);
    uint32_t i = 0;
    
    for(i=0; i<number; i++)
    {
        eoprot_config_callbacks_variable_set(&sk_callbacks_descriptors_vars[i]);
    }
}


#endif//defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)  


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




