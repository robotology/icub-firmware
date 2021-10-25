/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 *
 * This Software is licensed under an Arm proprietary license.
 *
 * -----------------------------------------------------------------------------
 *
 * Project:     CMSIS-RTOS RTX
 * Title:       RTX derived definitions
 *
 * -----------------------------------------------------------------------------
 */

#ifndef RTX_DEF_H_
#define RTX_DEF_H_

#ifdef   _RTE_
#include "RTE_Components.h"
#endif

#if defined(CMSISOS2_ICUB_CHANGES)
#include "../src/cmsis5/cmsis/RTOS2/RTX/Config/RTX_Config.h"
#else
#include "RTX_Config.h"
#endif

#if (defined(OS_OBJ_MEM_USAGE) && (OS_OBJ_MEM_USAGE != 0))
  #define RTX_OBJ_MEM_USAGE
#endif

#if (defined(OS_STACK_CHECK) && (OS_STACK_CHECK != 0))
  #define RTX_STACK_CHECK
#endif

#ifdef  RTE_CMSIS_RTOS2_RTX5_ARMV8M_NS
  #define DOMAIN_NS             1
#endif

#endif  // RTX_DEF_H_
