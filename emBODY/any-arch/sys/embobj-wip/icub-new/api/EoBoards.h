/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _EOBOARDS_H_
#define _EOBOARDS_H_


#ifdef __cplusplus
extern "C" {
#endif

/** @file       EoBoards.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_cevcwervcrev5555 Configuation of the vrfverver
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMeasures.h"

#include "EoMotionControl.h" // serve?

// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef uint16_t eObrd_boardId_t;


typedef enum
{
    eobrd_mc4                       = 0,
    eobrd_strain                    = 1,
    eobrd_1foc                      = 2,
    eobrd_mais                      = 3,
    eobrd_ems                       = 4,
    eobdr_aea                       = 5,
    eobrd_skin                       = 6 	
} eObrd_types_t;


typedef struct                  // size is: 1+1+0 = 2
{
    uint8_t                     major;
    uint8_t                     minor;    
} eObrd_version_t;              EO_VERIFYsizeof(eObrd_version_t, 2);

typedef struct                  // size is: 1+1+2+2+0 = 6
{
    eOenum08_t                  boardtype;
    uint8_t                     firmwarebuildnumber;
    eObrd_version_t             firmwareversion;
    eObrd_version_t             protocolversion;   
} eObrd_typeandversions_t;      EO_VERIFYsizeof(eObrd_typeandversions_t, 6);


typedef struct
{   
    uint8_t                     fault_undervoltage                  : 1; //pos 00
    uint8_t                     fault_overvoltage                   : 1; //pos 01
    uint8_t                     fault_external                      : 1; //pos 02
    uint8_t                     highcurrent                         : 1; //pos 03 
    uint8_t                     error_motorhall                     : 1; //pos 04
    uint8_t                     error_absoluteencodereading         : 1; //pos 05
    uint8_t                     error_halltable                     : 1; //pos 06
    uint8_t                     error_hallglitch                    : 1; //pos 07    
} eObrd_axis_faults_t;          EO_VERIFYsizeof(eObrd_axis_faults_t, 1);


typedef struct
{
    eOenum08_t                  controlmode;        // use values of enum eObrd_controlmode_t  
    eObrd_axis_faults_t         faults;             // use the fields of the struct with 1 or 0.
} eObrd_axis_status_t;          EO_VERIFYsizeof(eObrd_axis_status_t, 2);


typedef struct
{
    uint32_t                    can_txsoftwarebufferoverrun        : 1; //pos 00
    uint32_t                    can_busoff                         : 1; //pos 01
    uint32_t                    can_transmittererror               : 1; //pos 02 
    uint32_t                    can_receivererror                  : 1; //pos 03  
    uint32_t                    can_transmitterwarning             : 1; //pos 04 
    uint32_t                    can_receiverwarning                : 1; //pos 05
    uint32_t                    can_receiverhardwareoverrun        : 1; //pos 06
    uint32_t                    notusedpos07                       : 1; //pos 07     
    uint32_t                    brd_mainloopoverflow               : 1; //pos 08
    uint32_t                    brd_overtemperaturech1             : 1; //pos 09
    uint32_t                    brd_overtemperaturech2             : 1; //pos 10 
    uint32_t                    brd_errortemperaturech1            : 1; //pos 11  
    uint32_t                    brd_errortemperaturech2            : 1; //pos 12 
    uint32_t                    brd_i2t                            : 1; //pos 13
    uint32_t                    notusedpos14                       : 1; //pos 14
    uint32_t                    notusedpos15                       : 1; //pos 15     
    uint32_t                    alg_initialrotoraligninprog        : 1; //pos 16
    uint32_t                    alg_initialrotoraligncomplete      : 1; //pos 17
    uint32_t                    alg_currentrampupinprog            : 1; //pos 18
    uint32_t                    alg_currentrampdowninprog          : 1; //pos 19 
    uint32_t                    alg_currentrampupcomplete          : 1; //pos 20  
    uint32_t                    alg_currentrampdowncomplete        : 1; //pos 21 
    uint32_t                    notusedfrom22to31                  :10; //pos 22-31
} eObrd_canboard_mc_info_t;     EO_VERIFYsizeof(eObrd_canboard_mc_info_t, 4);            



typedef struct                      // size is: 4+2+2+2+2+0 = 12
{
    eObrd_canboard_mc_info_t        canboardmcinfo;            // use fields of struct with 0 or 1  
    eObrd_axis_status_t             axis0;
    eObrd_axis_status_t             axis1;
    eObrd_axis_status_t             axis2;
    eObrd_axis_status_t             axis3;    
} eObrd_mc4_status_t;               EO_VERIFYsizeof(eObrd_mc4_status_t, 12);


typedef struct                      // size is: 4+2+2+0 = 8
{
    eObrd_canboard_mc_info_t        canboardmcinfo;            // use fields of struct with 0 or 1  
    eObrd_axis_status_t             axis0;
    eObrd_axis_status_t             axis1;
} eObrd_halfmc4_status_t;           EO_VERIFYsizeof(eObrd_halfmc4_status_t, 8);


typedef struct                      // size is: 4+2+2+0 = 8
{
    eObrd_canboard_mc_info_t        canboardmcinfo;            // use fields of struct with 0 or 1  
    eObrd_axis_status_t             axis0;
    uint8_t                         filler02[2];
} eObrd_1foc_status_t;              EO_VERIFYsizeof(eObrd_1foc_status_t, 8);


typedef struct                      // size is: 4+4+2+2+0 = 12
{
    eObrd_canboard_mc_info_t        canboardmcinfo0;            // use fields of struct with 0 or 1 
    eObrd_canboard_mc_info_t        canboardmcinfo1;            // use fields of struct with 0 or 1     
    eObrd_axis_status_t             axis0;
    eObrd_axis_status_t             axis1;
} eObrd_2foc_status_t;              EO_VERIFYsizeof(eObrd_2foc_status_t, 12);


typedef struct                      // size is: 4+0 = 4
{
    uint8_t                         tobedefined[4];            // used with ... see hal_encoder.h for bitfield formatting  
} eObrd_aea_status_t;               EO_VERIFYsizeof(eObrd_aea_status_t, 4);


typedef struct                          // size is: 8+0 = 8
{
    uint8_t                         tobedefined[4];             
} eObrd_mais_status_t;              EO_VERIFYsizeof(eObrd_mais_status_t, 4);


typedef struct                      // size is: 4+0 = 4
{
    uint8_t                         tobedefined[4];    
} eObrd_strain_status_t;            EO_VERIFYsizeof(eObrd_strain_status_t, 4);


typedef struct                      // size is: 4+0 = 4
{
    uint8_t                         tobedefined[4];    
} eObrd_ems_status_t;               EO_VERIFYsizeof(eObrd_strain_status_t, 4);




    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



// - removed stuff


#if 0
typedef enum
{
    eobrd_axisfault_flag_none                     = 0x00,
    eobrd_axisfault_flag_undervoltage             = 0x01 << 0,
    eobrd_axisfault_flag_overvoltage              = 0x01 << 1,
    eobrd_axisfault_flag_external                 = 0x01 << 2, 
    eobrd_axisfault_flag_highcurrent              = 0x01 << 3,  
    eobrd_axisfault_flag_motorhall                = 0x01 << 4, 
    eobrd_axisfault_flag_absoluteencodereading    = 0x01 << 5,
    eobrd_axisfault_flag_halltable                = 0x01 << 6,
    eobrd_axisfault_flag_hallglitch               = 0x01 << 7     
} eObrd_axis_fault_flags_t;
#endif

#if 0
typedef enum
{
    eobrd_canboard_mc_info_flag_none                             = 0,
    eobrd_canboard_mc_info_flag_can_txsoftwarebufferoverrun      = 0x00000001 << 0,
    eobrd_canboard_mc_info_flag_can_busoff                       = 0x00000001 << 1,
    eobrd_canboard_mc_info_flag_can_transmittererror             = 0x00000001 << 2, 
    eobrd_canboard_mc_info_flag_can_receivererror                = 0x00000001 << 3,  
    eobrd_canboard_mc_info_flag_can_transmitterwarning           = 0x00000001 << 4, 
    eobrd_canboard_mc_info_flag_can_receiverwarning              = 0x00000001 << 5,
    eobrd_canboard_mc_info_flag_can_receiverhardwareoverrun      = 0x00000001 << 6,
    eobrd_canboard_mc_info_flag_notusedpos07                     = 0x00000001 << 7,     
    eobrd_canboard_mc_info_flag_brd_mainloopoverflow             = 0x00000001 << 8,
    eobrd_canboard_mc_info_flag_brd_overtemperaturech1           = 0x00000001 << 9,
    eobrd_canboard_mc_info_flag_brd_overtemperaturech2           = 0x00000001 << 10, 
    eobrd_canboard_mc_info_flag_brd_errortemperaturech1          = 0x00000001 << 11,  
    eobrd_canboard_mc_info_flag_brd_errortemperaturech2          = 0x00000001 << 12, 
    eobrd_canboard_mc_info_flag_brd_i2t                          = 0x00000001 << 13,
    eobrd_canboard_mc_info_flag_notusedpos14                     = 0x00000001 << 14,
    eobrd_canboard_mc_info_flag_notusedpos15                     = 0x00000001 << 15,     
    eobrd_canboard_mc_info_flag_alg_initialrotoraligninprog      = 0x00000001 << 16,
    eobrd_canboard_mc_info_flag_alg_initialrotoraligncomplete    = 0x00000001 << 17,
    eobrd_canboard_mc_info_flag_alg_currentrampupinprog          = 0x00000001 << 18,
    eobrd_canboard_mc_info_flag_alg_currentrampdowninprog        = 0x00000001 << 19, 
    eobrd_canboard_mc_info_flag_alg_currentrampupcomplete        = 0x00000001 << 20,  
    eobrd_canboard_mc_info_flag_alg_currentrampdowncomplete      = 0x00000001 << 21, 
    eobrd_canboard_mc_info_flag_notusedpos22                     = 0x00000001 << 22,
    eobrd_canboard_mc_info_flag_notusedpos23                     = 0x00000001 << 23     
} eObrd_canboard_mc_info_flags_t;
#endif



/** @}            
    end of group eo_cevcwervcrev5555  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




