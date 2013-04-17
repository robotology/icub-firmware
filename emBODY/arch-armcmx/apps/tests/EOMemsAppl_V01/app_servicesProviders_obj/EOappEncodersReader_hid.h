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
#ifndef _EOAPPENCODERSREADER_HID_H_
#define _EOAPPENCODERSREADER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappEncodersReader.h
    @brief     This file provides hidden interface to encodersReader obj.
    @author    valentina.gaggero@iit.it
    @date       02/17/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
//abstlayaer
#include "hal_encoder.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOappEncodersReader.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------
// the application must read 6 encoders: 3 connected with sP1 1 and others 3 connected with SPI3.
//
enum { encodersNum = 6 };
enum { encodersNumPerSPI = 3 };


//struct configEncSPIX
//{
//    uint8_t list[encodersNumPerSPI];    /**< for the encoder i-th: in list[i] the encoder i-th finds the number of encoder it must start when it has finish its onw read. */
//    uint8_t first;                      /**< indicates the first encoder to read */
//};


typedef struct
{
    uint8_t list[encodersNumPerSPI];    /**< for the encoder i-th: in list[i] the encoder i-th finds the number of encoder it must start when it has finish its onw read. */
    uint8_t first;                      /**< indicates the first encoder to read */
} EOappEncReader_configEncSPIXReadSequence_hid_t;

typedef enum
{
  eOEncReader_readSt__idle      = 0,
  eOEncReader_readSt__started   = 1,
  eOEncReader_readSt__finished  = 2  
} eOappEncReader_readStatusSPIX_t;


typedef struct
{
  EOappEncReader_configEncSPIXReadSequence_hid_t readSeq;  /**< contains the sequence of reading of encoders connected to SPIX (1 or 3)*/ 
  eOappEncReader_readStatusSPIX_t                st;       /**< contains the status of reading on SPIX (1 or 3) */
} EOappEncReader_confEncDataPerSPI_hid_t;


struct EOappEncReader_hid
{
    eOappEncReader_cfg_t    cfg;
    uint16_t                halConfigEncMask;
//    struct configEncSPIX    configuredEnc_SPI1;     /**< contains the sequence of reading of encoders connected to SPI1*/ 
//    struct configEncSPIX    configuredEnc_SPI3;     /**< contains the sequence of reading of encoders connected to SPI3 */ 
    EOappEncReader_confEncDataPerSPI_hid_t configuredEnc_SPI1;
    EOappEncReader_confEncDataPerSPI_hid_t configuredEnc_SPI3;
}; 

#define eo_EncoderErrorDEBUG_id 6
typedef struct
{
    uint16_t parityCheck[6];
    uint16_t status[6];
} EOencoderErrorDEBUG_t;
extern EOencoderErrorDEBUG_t EOencoderErrorDEBUG;


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



