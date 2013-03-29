/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EODEB_EOPROTOPARSER_H_
#define _EODEB_EOPROTOPARSER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eODeb_eoProtoParser.h
    @brief      
    @author     valentina.gaggero@iit.it
    @date       03/18/2013
**/

/** @defgroup 
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOarray.h"
#include "EOrop.h"
#include "eOtheEthLowLevelParser.h"


// - public #define  --------------------------------------------------------------------------------------------------
#define eODeb_eoProtoParser_maxNV2find     40

#define ALL_EP 							   0xFFFF
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct eODeb_eoProtoParser_hid eODeb_eoProtoParser;


typedef struct
{
    eOropdescriptor_t   desc; 
    uint64_t            time;
    //put here other info you want collect while parsing rop
} eODeb_eoProtoParser_ropAdditionalInfo_t;



/* this callback is invoked when the received seqNum is different from expected one*/
typedef     void        (*eODeb_eoProtoParser_cbk_onErrSeqNum_t)  (eOethLowLevParser_packetInfo_t *pktInfo_ptr, uint32_t rec_seqNum, uint32_t expected_seqNum);

/* this callback is invoked when the given nv is found!!*/
typedef     void        (*eODeb_eoProtoParser_cbk_onNVfound_t)    (eOethLowLevParser_packetInfo_t *pktInfo_ptr, eODeb_eoProtoParser_ropAdditionalInfo_t *ropAddInfo_ptr);

/* this callback is invoked when the received packet doesn't contain a valid ropframe*/
typedef     void        (*eODeb_eoProtoParser_cbk_onInvalidRopframe_t)    (eOethLowLevParser_packetInfo_t *pktInfo_ptr);


/* contains data to check the seq num*/
typedef struct
{
    eODeb_eoProtoParser_cbk_onErrSeqNum_t       cbk_onErrSeqNum;
} eODeb_eoProtoParser_cfg_checkSequenceNumber_t;

/* the couple nvid and ep identifies one nvid univocally*/
typedef struct
{
    eOnvEP_t        ep;
    eOnvID_t        id;
} eODeb_eoProtoParser_nvidEp_couple_t;


/* thi struct is the list of couple <ep, nvid>.*/
typedef struct
{
    eOarray_head_t                          head;
    eODeb_eoProtoParser_nvidEp_couple_t    data[eODeb_eoProtoParser_maxNV2find];
} eODeb_eoProtoParser_NVsArray_t;

/*this struct contains a list of nvs to search in ropframe and the callback to invoke when one of them is found. */
typedef struct
{
    eODeb_eoProtoParser_NVsArray_t                 NVs2searchArray;
    eODeb_eoProtoParser_cbk_onNVfound_t            cbk_onNVfound;
} eODeb_eoProtoParser_cfg_checkNV_t;

/*this struct contains callbeck to invoke when a invalid ropframe is received */
typedef struct
{
    eODeb_eoProtoParser_cbk_onInvalidRopframe_t       cbk;
} eODeb_eoProtoParser_cfg_checkInvalidRopframe_t;



typedef struct
{
    struct
    {
        eODeb_eoProtoParser_cfg_checkSequenceNumber_t         seqNum;
        eODeb_eoProtoParser_cfg_checkNV_t                     nv;
        eODeb_eoProtoParser_cfg_checkInvalidRopframe_t        invalidRopFrame; //this check is always done, but only if cbk is configured it is invocked
    }checks;
} eODeb_eoProtoParser_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern eODeb_eoProtoParser * eODeb_eoProtoParser_Initialise(const eODeb_eoProtoParser_cfg_t *cfg);
extern eODeb_eoProtoParser * eODeb_eoProtoParser_GetHandle(void);
extern eOresult_t eODeb_eoProtoParser_RopFrameDissect(eODeb_eoProtoParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr);


/** @}            
    end of group  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

