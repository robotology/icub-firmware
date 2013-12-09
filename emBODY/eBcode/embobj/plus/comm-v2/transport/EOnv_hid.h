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
#ifndef _EONV_HID_H_
#define _EONV_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOnv_hid.h
    @brief      This header file implements hidden interface to a netvar object.
    @author     marco.accame@iit.it
    @date       09/06/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"
#include "EOVmutex.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOnv.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------



typedef void (*eOvoid_fp_cnvp_t)                            (const EOnv*);
typedef void (*eOvoid_fp_cnvp_cropdesp_t)                   (const EOnv*, const eOropdescriptor_t*);

typedef eOresult_t (*eOres_fp_cnvp_t)                       (const EOnv*);
typedef eOresult_t (*eOres_fp_cnvp_cropdesp_t)              (const EOnv*, const eOropdescriptor_t*);


typedef const struct EOnv_rom_T         // 16 bytes on arm 
{
    uint16_t                            capacity;   // the capacity of the nv
    eOenum08_t                          rwmode;     
    uint8_t                             dummy;    
    const void*                         resetval;   // the reset value of the nv 
    eOvoid_fp_cnvp_t                    init;       // called at startup to init the nv value in a particular mode or to init data structures associated to the nv
    eOvoid_fp_cnvp_cropdesp_t           update;     // called after the nv value is changed by the protocol parser or by any other object (in this latter case ropdes is NULL)   
} EOnv_rom_t;                           //EO_VERIFYsizeof(EOnv_rom_t, 16); 



struct EOnv_hid                    // 24 bytes ... multiple of 8
{
    eOipv4addr_t                    ip;         // ip address of the device owning the nv. if equal to eok_ipv4addr_localhost, then the nv is owned by the device.
    eOnvBRD_t                       brd;        // brd number. it is a short of the ip address.
    eObool_t                        cached;     // if eobool_true then the variabale contains values which resides on a another entity (e.g., a can board) 
    uint8_t                         filler2[2];
    eOnvID32_t                      id32;
    EOnv_rom_t*                     rom;        // pointer to the constant part common to every device which uses this nv
    void*                           ram;        // the ram which keeps the LOCAL value of nv 
    EOVmutexDerived*                mtx;        // the mutex which protects concurrent access to the ram of this nv (or rem ...) 
};  EO_VERIFYsizeof(EOnv, 24);   




// - declaration of extern hidden functions ---------------------------------------------------------------------------

///** @fn         extern EOnv * eo_nv_hid_New(uint8_t fun, uint8_t typ, uint32_t otherthingsmaybe)
//    @brief      Creates a new netvar object. 
//    @return     The pointer to the required object.
// **/
//extern EOnv * eo_nv_hid_New(uint8_t fun, uint8_t typ, uint32_t otherthingsmaybe);


extern eOresult_t eo_nv_hid_Load(EOnv *nv, eOipv4addr_t ip, eOnvBRD_t brd, eObool_t cached, eOnvID32_t id32, EOnv_rom_t* rom, void* ram, EOVmutexDerived* mtx);

extern void eo_nv_hid_Fast_LocalMemoryGet(EOnv *nv, void* dest);

extern eObool_t eo_nv_hid_isWritable(const EOnv *netvar);
extern eObool_t eo_nv_hid_isLocal(const EOnv *netvar);
extern eObool_t eo_nv_hid_isUpdateable(const EOnv *netvar);


extern eOresult_t eo_nv_hid_ResetROP(const EOnv *nv, eOnvUpdate_t upd, const eOropdescriptor_t *ropdes);
extern eOresult_t eo_nv_hid_SetROP(const EOnv *nv, const void *dat, eOnvUpdate_t upd, const eOropdescriptor_t *ropdes);
extern eOresult_t eo_nv_hid_remoteSetROP(const EOnv *nv, const void *dat, eOnvUpdate_t upd, const eOropdescriptor_t* ropdes);


EO_extern_inline eOnvEP8_t eo_nv_hid_id32_extract_ep8(eOnvID32_t id32)
{
    return((id32>>24)&0xff);
}

// - declaration of extern hidden online functions -------------------------------------------------------------------
// empty-section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




