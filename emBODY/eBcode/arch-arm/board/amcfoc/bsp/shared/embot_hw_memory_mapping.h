
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MEMORY_MAPPING_H_
#define __EMBOT_HW_MEMORY_MAPPING_H_

#include "embot_hw_bsp_config.h"

// in here we can get as a constexpr the various partitions of the flash (address + size) w/ embot::hw::flash::bsp::partition()
#include "embot_hw_flash_bsp_amcfoc.h"

// in here we impose the macros required by the shared services
//#include "eEmemorymap_sharedservices_dualcore.h"

// in here we get the placements needed by loader, updater, appl.yri, appl.mot
#include "eEmemorymap_processes_dualcore.h"

// for all other constexpr values, use:
// embot::hw::flash::bsp::partition(id).address or .size
// w/ id = embot::hw::flash::Partition::ID::eloader or .eupdater or .eapplication00 or eapplication01





#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



