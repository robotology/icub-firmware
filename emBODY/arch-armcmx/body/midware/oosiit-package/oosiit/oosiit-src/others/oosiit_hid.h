/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _OOSIIT_HID_H_
#define _OOSIIT_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


/* @file       oosiit_hid.h
    @brief      This header file implements hidden interface to a module.
    @author     marco.accame@iit.it
    @date       07/20/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------


#include "stdint.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "oosiit.h"
    
    
typedef struct
{    
    uint8_t     priority;
    uint16_t    stacksize;
    void*       extdata;
} osiit_hid_tsk_create_other_args_t;


// - declaration of externally defined variables ----------------------------------------------------------------------

extern const oosiit_cfg_t *oosiit_cfg_in_use;

// it must point to an array of proper size, given by ...
extern       uint32_t *oosiit_params_ram32data;

// it must point to an array of proper size, given by ...
extern       uint64_t *oosiit_params_ram64data;

// it may be required to be initted at compile time because it is called before main().
// its must contain OSIIT_PARAMS_STDLIB_32SIZETOT(ntask, nsysmutex) U32 elements.
extern       uint32_t *oosiit_params_stdlib32data;


extern volatile uint64_t oosiit_time;
//extern volatile uint64_t oosiit_idletime;
extern uint32_t oosiit_ns_per_unit_of_systick;
extern uint32_t oosiit_num_units_of_systick;

// - definition of hidden constants -----------------------------------------------------------------------------------

#if 0

// IIT-REDEF-begin
typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
// IIT-REDEF-end


typedef U32 TIME_t;
typedef U64 WIDETIME_t;
typedef U32 EVENT_t;
typedef U32 TMRSIG_iit;
#define MSB_TIME_t      0x80000000
//    #define NOTIMEOUT       0xffffffff
#define EXTRAOFFSET     8

#endif



// - definition of hidden types ---------------------------------------------------------------------------------------



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void* oosiit_ext_calloc(uint32_t s, uint32_t n);
extern void oosiit_ext_free(void* m);

//void oosiit_hid_initialise(void);

extern uint16_t oosiit_hid_params_get_stdlib32(const oosiit_cfg_t *cfg, uint16_t *stdlib32size);;
extern uint16_t oosiit_hid_params_get_ram32(const oosiit_cfg_t *cfg, uint16_t *ram32size);
extern uint16_t oosiit_hid_params_get_ram64(const oosiit_cfg_t *cfg, uint16_t *ram64size);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



