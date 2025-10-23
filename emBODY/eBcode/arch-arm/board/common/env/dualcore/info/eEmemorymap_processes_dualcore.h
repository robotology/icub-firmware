
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EEMEMORYMAP_PROCESSES_DUALCORE_H_
#define __EEMEMORYMAP_PROCESSES_DUALCORE_H_

#if 0
	this contains the default macros needed by the processes to place their eEmoduleInfo_t
#endif

//#include "eEmemorymap_sharedservices_dualcore.h"


#if !defined(STM32HAL_dualcore_BOOT_cm7master) && !defined(STM32HAL_dualcore_BOOT_cm4master)
	#error use this file only for a dualcore project
#endif


// placements of ROM placements variables


// eloader must place its eEmoduleInfo_t at start of ROM for master cpu + 0 + 1024
#if defined(STM32HAL_dualcore_BOOT_cm7master)
#define EENV_MODULEINFO_LOADER_AT ".ARM.__at_0x08000400"
#elif defined(STM32HAL_dualcore_BOOT_cm4master)
#define EENV_MODULEINFO_LOADER_AT ".ARM.__at_0x08100400"
#endif

// eupdater must place its eEmoduleInfo_t at start of ROM for master cpu + 128k + 1024
#if defined(STM32HAL_dualcore_BOOT_cm7master)
#define EENV_MODULEINFO_UPDATER_AT ".ARM.__at_0x08020400"
#elif defined(STM32HAL_dualcore_BOOT_cm4master)
#define EENV_MODULEINFO_UPDATER_AT ".ARM.__at_0x08120400"
#endif

// appl.yri must place its eEmoduleInfo_t at start of ROM for master cpu + 384k + 1024
#if defined(STM32HAL_dualcore_BOOT_cm7master)
#define EENV_MODULEINFO_APPL_YRI_AT ".ARM.__at_0x08060400"
#elif defined(STM32HAL_dualcore_BOOT_cm4master)
#define EENV_MODULEINFO_APPL_YRI_AT ".ARM.__at_0x08160400"
#endif
#define EENV_MODULEINFO_APPLICATION_AT EENV_MODULEINFO_APPL_YRI_AT

// appl.mot must place its eEmoduleInfo_t at start of ROM for slave cpu + 0 + 1024
#if defined(STM32HAL_dualcore_BOOT_cm7master)
#define EENV_MODULEINFO_APPL_MOT_AT ".ARM.__at_0x08100400"
#elif defined(STM32HAL_dualcore_BOOT_cm4master)
#define EENV_MODULEINFO_APPL_MOT_AT ".ARM.__at_0x08000400"
#endif


// appl.mot must place its embot::app::icc::Signature at start of ROM for slave cpu + 0 + 2048
#if defined(STM32HAL_dualcore_BOOT_cm7master)
#define EENV_SIGNATURE_APPL_MOT_AT ".ARM.__at_0x08100800"
#elif defined(STM32HAL_dualcore_BOOT_cm4master)
#define EENV_SIGNATURE_APPL_MOT_AT ".ARM.__at_0x08000800"
#endif

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



