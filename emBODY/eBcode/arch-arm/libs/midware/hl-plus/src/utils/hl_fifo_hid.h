/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HL_FIFO_HID_H_
#define _HL_FIFO_HID_H_


/* @file       hl_fifo_hid.h
    @brief      this header file gives hidden interface to hl tools.
    @author     marco.accame@iit.it
    @date       10/29/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hl_fifo.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section

// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint8_t * hl_fifo_front01(hl_fifo_t *fifo);
extern hl_result_t hl_fifo_get01(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining);
extern hl_result_t hl_fifo_put01(hl_fifo_t *fifo, uint8_t *data);

extern uint8_t * hl_fifo_front02(hl_fifo_t *fifo);
extern hl_result_t hl_fifo_get02(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining);
extern hl_result_t hl_fifo_put02(hl_fifo_t *fifo, uint8_t *data);

extern uint8_t * hl_fifo_front04(hl_fifo_t *fifo);
extern hl_result_t hl_fifo_get04(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining);
extern hl_result_t hl_fifo_put04(hl_fifo_t *fifo, uint8_t *data);

extern uint8_t * hl_fifo_front08(hl_fifo_t *fifo);
extern hl_result_t hl_fifo_get08(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining);
extern hl_result_t hl_fifo_put08(hl_fifo_t *fifo, uint8_t *data);

extern uint8_t * hl_fifo_front16(hl_fifo_t *fifo);
extern hl_result_t hl_fifo_get16(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining);
extern hl_result_t hl_fifo_put16(hl_fifo_t *fifo, uint8_t *data);


// - definition of extern hidden inline functions ---------------------------------------------------------------------



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




