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
#ifndef _EOMFIFOPRODCONS_HID_H_
#define _EOMFIFOPRODCONS_HID_H_

// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       EOMprodConsSharedData_hid.h
    @brief      This header file implements hidden interface producer-consumer-shared-data object.
    @author     valentina.gaggero@iit.it
    @date       12/13/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EOfifo.h"
#include "EOVmutex.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMfifoProdCons.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMprodConsSharedData_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOMfifoProdCons_hid 
{
    eOsizeitem_t    item_size;
    eOsizecntnr_t    capacity;
    eOcallback_t    signalToProducer; /**< callback used by the object to signal producer that consumer had removed an item */
    void            *argprod;         /**< argument of callback signalToProducer */
    eOcallback_t    signalToConsumer; /**< callback used by the object to signal consumer that producer had inserted an item */
    void            *argcons;         /**< argument of callback  signalToConsumer */
    EOfifo          *fifo;            /**< the shared memory is implement like a fifo. */
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



