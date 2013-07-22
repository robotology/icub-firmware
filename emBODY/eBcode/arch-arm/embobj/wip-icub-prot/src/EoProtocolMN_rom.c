/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EoProtocolMN_rom.c
    @brief      This file keeps ....
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv_hid.h"
#include "EOconstvector_hid.h"

#include "EoProtocolMN.h"
#include "EoMotionControl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMN_rom.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a comm
static const eOmn_comm_t eoprot_mn_rom_comm_defaultvalue = { 0 };

// - default value of a appl
static const eOmn_appl_t eoprot_mn_rom_appl_defaultvalue = { 0 };


// - descriptors for the variables of a comm

static EOnv_rom_t eoprot_mn_rom_descriptor_comm_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_comm_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mn_comm_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_comm_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mn_comm_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_comm_wholeitem
};

static EOnv_rom_t eoprot_mn_rom_descriptor_comm_cmmnds_ropsigcfg =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_comm_defaultvalue.cmmnds.ropsigcfg),
    EO_INIT(.rwmode)    eoprot_rwm_mn_comm_cmmnds_ropsigcfg,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_comm_defaultvalue.cmmnds.ropsigcfg,
    EO_INIT(.init)      eoprot_fun_INIT_mn_comm_cmmnds_ropsigcfg,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_comm_cmmnds_ropsigcfg
};


// - descriptors for the variables of a appl


static EOnv_rom_t eoprot_mn_rom_descriptor_appl_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_wholeitem
};

static EOnv_rom_t eoprot_mn_rom_descriptor_appl_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_config,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_config
};

static EOnv_rom_t eoprot_mn_rom_descriptor_appl_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_status,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_status
};

static EOnv_rom_t eoprot_mn_rom_descriptor_appl_cmmnds_go2state =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue.cmmnds.go2state),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_cmmnds_go2state,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue.cmmnds.go2state,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_cmmnds_go2state,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_cmmnds_go2state
};







// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

// -- the folded array of descriptors: to be changed if any new tag is added

const EOnv_rom_t * const eoprot_mn_rom_folded_descriptors[] =
{
    // here are eoprot_mn_tags_comm_numberof descriptors for the comms (equal for every comm)
    &eoprot_mn_rom_descriptor_comm_wholeitem,
    &eoprot_mn_rom_descriptor_comm_cmmnds_ropsigcfg,

    // here are eoprot_mn_tags_appl_numberof descriptors for the appls (equal for every appl)
    &eoprot_mn_rom_descriptor_appl_wholeitem,
    &eoprot_mn_rom_descriptor_appl_config,
    &eoprot_mn_rom_descriptor_appl_status,
    &eoprot_mn_rom_descriptor_appl_cmmnds_go2state
         
};  EO_VERIFYsizeof(eoprot_mn_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_tags_mn_comm_numberof+eoprot_tags_mn_appl_numberof));


// the other constants: to be changed when a new entity is added

const uint8_t eoprot_mn_rom_tags_numberof[] = 
{
    eoprot_tags_mn_comm_numberof, 
    eoprot_tags_mn_appl_numberof
};  EO_VERIFYsizeof(eoprot_mn_rom_tags_numberof, eoprot_entities_mn_numberof*sizeof(uint8_t));

extern const uint16_t eoprot_mn_rom_entities_sizeof[] = 
{
    sizeof(eOmn_comm_t), 
    sizeof(eOmn_appl_t)
};  EO_VERIFYsizeof(eoprot_mn_rom_entities_sizeof, eoprot_entities_mn_numberof*sizeof(uint16_t));

const uint32_t* eoprot_mn_rom_entities_defval[] = 
{
    (const uint32_t*)&eoprot_mn_rom_comm_defaultvalue, 
    (const uint32_t*)&eoprot_mn_rom_appl_defaultvalue
};  EO_VERIFYsizeof(eoprot_mn_rom_entities_defval, eoprot_entities_mn_numberof*sizeof(uint32_t*)); 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



