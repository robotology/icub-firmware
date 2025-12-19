
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __EEMEMORYMAP_H_
#define __EEMEMORYMAP_H_


// in here we impose the macros required by the shared services
#include "eEmemorymap_sharedservices_dualcore.h"

// and by the processes
#include "eEmemorymap_processes_dualcore.h"

// and something else
#include "embot_hw_memory_mapping.h"

//// others

constexpr uint32_t EENV_MEMMAP_ELOADER_ROMADDR {embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eloader).address};
constexpr uint32_t EENV_MEMMAP_ELOADER_ROMSIZE {embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eloader).size};
constexpr uint32_t EENV_MEMMAP_EAPPLICATION_ROMADDR {embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eapplication00).address};
constexpr uint32_t EENV_MEMMAP_EAPPLICATION_ROMSIZE {embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eapplication00).size};
constexpr uint32_t EENV_MEMMAP_EUPDATER_ROMADDR {embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eupdater).address};
constexpr uint32_t EENV_MEMMAP_EUPDATER_ROMSIZE {embot::hw::flash::bsp::partition(embot::hw::flash::Partition::ID::eupdater).size};

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



