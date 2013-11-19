/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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
#ifndef _HAL_STM32_SPI4ENCODER_HID_H_
#define _HAL_STM32_SPI4ENCODER_HID_H_


/* @file       hal_stm32_spi4encoder_hid.h
    @brief      This header file implements hidden interface to hal spi to be used only for the encoders.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/09/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "hal_stm32xx_include.h"
#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_spi4encoder.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
	uint32_t BRR_reg;
	uint32_t BSRR_reg;
} hal_spi4encoder_cs_gpio_conf;

typedef struct
{
    uint8_t                 working_encoder : 2;    // the currently used encoder
    uint8_t                 read_byte_pos : 2;    // the index (0, 1, 2) of the byte last read and placed in buff_encoder[]
    uint8_t                 busy : 1;
    uint8_t                 dummy : 3;
	uint8_t 				buff_encoder[3]; //contiene i byte letti. 3 perche' ogni valore letto dall'encoder e' lungo 18, quindi ci vogliono 3 byte ==> 3 letture da un byte.
	uint32_t 				encoder_result[hal_spi_muxes_number];
	hal_spi4encoder_cs_gpio_conf 	cs_encoder[hal_spi_muxes_number];
    uint32_t 				cs_pin_ena_slave;
	GPIO_TypeDef * 			encoder_gpio_port;
} hal_spi4encoder_encoderReadData_t;


typedef struct
{	
	hal_spi4encoder_encoderReadData_t enc_data;
    void (*cbk_on_rx)(void *p);
    void* arg_cbk;
} hal_spi4encoder_interfdatastructure_t;




// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern hal_spi4encoder_interfdatastructure_t* hal_spi4encoder_hid_interfdata[hal_spi_ports_number];
extern SPI_TypeDef * const hal_spi4encoder_interf_map[hal_spi_ports_number]; // = {SPI1, SPI2, SPI3};	


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// ok, but ... maybe inline functions ...


// get the pointer of the spi device inside stm32f4 lib, given the spi number spix as in hal_spi_port_t (0, 1, 2) 
#define hal_SPI4ENCODER_PERIPH_GET(spix) 			    (hal_spi4encoder_interf_map[spix])	

// get the pointer of hal-defined data structure
#define hal_SPI4ENCODER_GET(spix)         			    (hal_spi4encoder_hid_interfdata[spix])

// get the pointer of hal-defined encoder data
#define hal_SPI4ENCODER_ENCDATA_GET(spix)        		(hal_spi4encoder_hid_interfdata[spix]->enc_data)

// get the pointer of hal-defined chip select of encoder e on device spix
#define hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e)         (hal_spi4encoder_hid_interfdata[spix]->enc_data.cs_encoder[e])

// set the working encoder to e on device spix, where e is of type hal_spiEncoder_t (0, 1, 2)
#define hal_SPI4ENCODER_ENCODER_ID_SET(spix, e)	  		((hal_SPI4ENCODER_ENCDATA_GET(spix)).working_encoder = e)

#define hal_SPI4ENCODER_ENCODER_BUSY_SET(spix)	                ((hal_SPI4ENCODER_ENCDATA_GET(spix)).busy = 1)

#define hal_SPI4ENCODER_ENCODER_IS_BUSY_SET(spix)	            ((hal_SPI4ENCODER_ENCDATA_GET(spix)).busy)

#define  hal_SPI4ENCODER_ENCODER_BUSY_RESET(spix)	            ((hal_SPI4ENCODER_ENCDATA_GET(spix)).busy = 0)

// get the working encoder: it returns a value in hal_spiEncoder_t (0, 1, 2)
#define hal_SPI4ENCODER_ENCODER_ID_GET(spix)	        ((hal_SPI4ENCODER_ENCDATA_GET(spix)).working_encoder)

// reset the working encoder on device spix
#define hal_SPI4ENCODER_ENCODER_ID_RESET(spix)	  		(hal_SPI4ENCODER_ENCODER_ID_SET(spix, 0))

// reset the read byte pos
#define hal_SPI4ENCODER_READBYTE_POS_RESET(spix)        ((hal_SPI4ENCODER_ENCDATA_GET(spix)).read_byte_pos = 0)

// get the read byte pos
#define hal_SPI4ENCODER_READBYTE_POS_GET(spix) 			((hal_SPI4ENCODER_ENCDATA_GET(spix)).read_byte_pos)

// increment the read byte pos
#define hal_SPI4ENCODER_READBYTE_POS_INCR(spix)	        ((hal_SPI4ENCODER_ENCDATA_GET(spix)).read_byte_pos++)


//--------Comandi per abilitare/disabilitare le interrupt

#define hal_SPI4ENCODER_IT_RX_ENA(spix)     SPI_I2S_ITConfig(hal_SPI4ENCODER_PERIPH_GET(spix), SPI_I2S_IT_RXNE, ENABLE);
#define hal_SPI4ENCODER_IT_RX_DISA(spix)    SPI_I2S_ITConfig(hal_SPI4ENCODER_PERIPH_GET(spix), SPI_I2S_IT_RXNE, DISABLE);
#define hal_SPI4ENCODER_IS_IT_RX_ENA(spix)   (hal_SPI4ENCODER_PERIPH_GET(spix)->CR2 & 0x40)
//
//#define SPI_IT_RX_ENA(spix)     SPI_I2S_ITConfig(hal_SPI4ENCODER_PERIPH_GET(spix), SPI_I2S_IT_RXNE, ENABLE);
//#define SPI_IT_RX_DISA(spix)    SPI_I2S_ITConfig(hal_SPI4ENCODER_PERIPH_GET(spix), SPI_I2S_IT_RXNE, DISABLE);
//#define SPI_IS_ITRX_ENA(spix)   (hal_SPI4ENCODER_PERIPH_GET(spix)->CR2 & 0x40)


//--------Comandi per abilitare/disabilitare le periferiche
#define hal_SPI4ENCODER_ENA(spix)    	  hal_SPI4ENCODER_PERIPH_GET(spix)->CR1 |= 0x0040	  //tiro su il bit SPE (posizione 6)
#define hal_SPI4ENCODER_DISA(spix)        hal_SPI4ENCODER_PERIPH_GET(spix)->CR1 &= 0xFFBF	 




// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_spi4encoder_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_spi4encoder_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);

extern void hal_spi4encoder_hid_disa(hal_spi_port_t port);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




