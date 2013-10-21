// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* Copyright (C) <year>  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino
 * email: alberto.cardellino@iit.it
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

#ifndef __debugfunctions__
#define __debugfunctions__

// usual includes
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <signal.h>
#include <iostream>



// EO includes
#include "EOnv_hid.h"
#include <EOropframe_hid.h>


#define eb1_hex_addr    0x0101000a
#define eb2_hex_addr    0x0201000a
#define eb3_hex_addr    0x0301000a
#define eb4_hex_addr    0x0401000a
#define eb5_hex_addr    0x0501000a
#define eb6_hex_addr    0x0601000a
#define eb7_hex_addr    0x0701000a
#define eb8_hex_addr    0x0801000a
#define eb9_hex_addr    0x0901000a
#define pc104_hex_addr  0x6801000a

#define PORT "12345"

#define BOARD_NUM           10
#define SOGLIA              1700
#define MAX_ACQUISITION     -1 // negative values means capture until error     100*1000

typedef struct
{
    int missing;
    int late;
} Errors;

typedef struct
{
    EOropframeHeader_t  previousPkt;
    struct timeval      sniffTimePrev;
    int                 totPcktRecv;
    Errors              errors;
} DebugData;


void check_received_pkt(const in_addr sender_addr, const in_addr destin_addr, void *pkt, uint16_t pkt_size, const timeval *time);
void check_seqNum(int boardNum, EOropframeHeader_t *pkt, DebugData *debugData, const struct timeval *sniffTime);
bool do_real_check_positons(int board, void *pkt, uint16_t size);

void print_data(int board, DebugData *data);
void printEverything(void);

int gettimeofday(struct timeval *tv, struct timezone *tz);

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

#ifdef _WIN32_
struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};
#endif
static int timeval_subtract(struct timeval *_result, struct timeval *_x, struct timeval *_y)
{
    /* Perform the carry for the later subtraction by updating y. */

    if(_x->tv_usec < _y->tv_usec)
    {
        int nsec    = (_y->tv_usec - _x->tv_usec) / 1000000 + 1;

        _y->tv_usec -= 1000000 * nsec;
        _y->tv_sec  += nsec;
    }

    if(_x->tv_usec - _y->tv_usec > 1000000)
    {
        int nsec    = (_x->tv_usec - _y->tv_usec) / 1000000;

        _y->tv_usec += 1000000 * nsec;
        _y->tv_sec  -= nsec;
    }

    /* Compute the time remaining to wait. tv_usec is certainly positive. */

    _result->tv_sec  = _x->tv_sec  - _y->tv_sec;
    _result->tv_usec = _x->tv_usec - _y->tv_usec;

    /* Return 1 if result is negative. */

    return _x->tv_sec < _y->tv_sec;
}



#endif  // __debugfunctions__
