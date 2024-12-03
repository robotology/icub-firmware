/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EUPDATERCANGTW_HID_H_
#define _EUPDATERCANGTW_HID_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

#define USE_PROG_ID
#undef USE_EXTRA64

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"

#include "eupdater_cangtw.h"

#include "eOcfg_sm_CanGtw.h"

#define UDPCANSIGN  0x12

typedef struct
{
    uint8_t         signature;      // must be 0x12
    uint8_t         canframenumof;  // 
#ifdef USE_PROG_ID
    uint8_t         dummy[2];    
    uint32_t        progressive;
#else
    uint8_t         dummy[6];
#endif
} eupdater_cangtw_udp_header_t; // 8 bytes

typedef struct
{
    uint8_t         canbus;     // the can bus (1 or 2)
    uint8_t         len;        // the
    uint16_t        canid;      // the id
    uint8_t         dummy[4];
    uint8_t         data[8];
} eupdater_cangtw_udp_canframe_t;   // 16 bytes

typedef struct
{
    eupdater_cangtw_udp_header_t    header;
    eupdater_cangtw_udp_canframe_t  onecanframe;
#ifdef USE_EXTRA64
    uint8_t         extra64[64];
#endif    
} eupdater_cangtw_udp_simpleframe_t;

enum { eupdater_cangtw_udp_packet_maxsize = sizeof(eupdater_cangtw_udp_simpleframe_t)+8 };

typedef enum 
{
    event_cangtw_sock_rec     = 0x00000001,
    event_cangtw_can1_rec     = 0x00000002,
    event_cangtw_can2_rec     = 0x00000004,

    event_cangtw_start        = 0x00000008,
    event_cangtw_canstable    = 0x00000010,
    event_cangtw_go2run       = 0x00000040
} eupdater_cangtw_task_event_t;

// - declaration of extern hidden variables

extern EOMtask*             eupdater_task_cangateway;
extern EOsocketDatagram*    eupdater_sock_cangateway;


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void task_cangateway(void *p);

extern void cangateway_hid_hal_init(void);
extern void cangateway_hid_hal_enable(void);

 


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


