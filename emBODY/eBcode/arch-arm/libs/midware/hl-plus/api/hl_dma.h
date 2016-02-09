/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license DMA be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HL_DMA_H_
#define _HL_DMA_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_dma.h
	@brief      This file contains basic initialisation api for hl DMA utilities.
	@author     davide.pollarolo@iit.it
    @date       07/04/2015
**/

/** @defgroup doxy_group_hl_spi HL SPI

    The HL SPI ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    hl_dma1 = 0,
    hl_dma2 = 1
} hl_dma_t;

typedef enum
{
    hl_dma_stream0 = 0,
    hl_dma_stream1 = 1,
    hl_dma_stream2 = 2,
    hl_dma_stream3 = 3,
    hl_dma_stream4 = 4,
    hl_dma_stream5 = 5,
    hl_dma_stream6 = 6,
    hl_dma_stream7 = 7
} hl_dma_stream_t;

typedef enum
{
    hl_dma_channel0 = 0,
    hl_dma_channel1 = 1,
    hl_dma_channel2 = 2,
    hl_dma_channel3 = 3,
    hl_dma_channel4 = 4,
    hl_dma_channel5 = 5,
    hl_dma_channel6 = 6,
    hl_dma_channel7 = 7
} hl_dma_channel_t;

typedef struct
{
  uint32_t DMA_Channel;
  uint32_t DMA_PeripheralBaseAddr;
  uint32_t DMA_Memory0BaseAddr;
  uint32_t DMA_DIR;
  uint32_t DMA_BufferSize;
  uint32_t DMA_PeripheralInc;  
  uint32_t DMA_MemoryInc;              
  uint32_t DMA_PeripheralDataSize; 
  uint32_t DMA_MemoryDataSize;
  uint32_t DMA_Mode;
  uint32_t DMA_Priority;  
  uint32_t DMA_FIFOMode; 
  uint32_t DMA_FIFOThreshold;
  uint32_t DMA_MemoryBurst;   
  uint32_t DMA_PeripheralBurst;

} hl_dma_advcfg_t;

typedef struct
{
    hl_dma_stream_t    stream;
    hl_dma_advcfg_t    advdma_cfg;
} hl_dma_cfg_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_dma_cfg_t hl_dma_cfg_default;

//extern const hl_dma_mapping_t* hl_dma_map; 


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hl_result_t hl_dma_init(hl_dma_t id, const hl_dma_cfg_t *cfg)
    @brief      This function configures DMA.
    @param      id              identifies SPI id 
    @param      cfg             the configuration of the DMA peripheral
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_dma_init(hl_dma_t id, const hl_dma_cfg_t *cfg);

/** @fn         extern hl_boolval_t hl_dma_supported_is(hl_dma_t id)
    @brief      This function tells if a given DMA periph is supported.
    @param      id              identifies DMA
    @return     hl_true or hl_false
  */
extern hl_boolval_t hl_dma_supported_is(hl_dma_t id);


/** @fn         extern hl_boolval_t hl_dma_initted_is(hl_dma_t id)
    @brief      This function tells if a given DMA is initted.
    @param      id              identifies DMA port
    @return     hl_true or hl_false
  */
extern hl_boolval_t hl_dma_initted_is(hl_dma_t id);


/** @fn         extern hl_result_t hl_dma_enable(hl_dma_t id)
    @brief      This function starts DMA. It must be invoked after hl_dma_init.
    @param      id              identifies DMA 
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_dma_enable(hl_dma_t id);


/** @fn         extern hl_result_t hl_dma_disable(hl_dma_t id)
    @brief      This function disable DMA.
    @param      id              identifies DMA port 
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_dma_disable(hl_dma_t id);

/** @}            
    end of group doxy_group_hl_spi  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



