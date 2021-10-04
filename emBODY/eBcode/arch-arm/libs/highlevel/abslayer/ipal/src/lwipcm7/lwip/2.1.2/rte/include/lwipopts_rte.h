/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LWIP_LWIPOPTS_RTE_H
#define LWIP_LWIPOPTS_RTE_H

#include "RTE_Components.h"

#ifndef LWIP_IPV4
#ifdef  RTE_Network_IPv4
#define LWIP_IPV4               1
#else
#define LWIP_IPV4               0
#endif
#endif

#ifndef LWIP_IPV6
#ifdef  RTE_Network_IPv6
#define LWIP_IPV6               1
#else
#define LWIP_IPV6               0
#endif
#endif

#ifndef NO_SYS
#ifdef  RTE_Network_RTOS
#define NO_SYS                  0
#else
#define NO_SYS                  1
#endif
#endif

#ifndef LWIP_TIMERS
#define LWIP_TIMERS             (!NO_SYS)
#endif

#ifndef SYS_LIGHTWEIGHT_PROT
#define SYS_LIGHTWEIGHT_PROT    (!NO_SYS)
#endif

#ifndef LWIP_ETHERNET
#ifdef  RTE_Network_Interface_Ethernet
#define LWIP_ETHERNET           1
#else
#define LWIP_ETHERNET           0
#endif
#endif

#ifndef PPP_SUPPORT
#ifdef  RTE_Network_Interface_PPP
#define PPP_SUPPORT             1
#else
#define PPP_SUPPORT             0
#endif
#endif

#endif /* LWIP_LWIPOPTS_RTE_H */
