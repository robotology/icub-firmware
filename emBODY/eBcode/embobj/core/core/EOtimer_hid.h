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
#ifndef _EOTIMER_HID_H_
#define _EOTIMER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


/* @file        EOtimer_hid.h
    @brief      This header file implements hidden interface to a timer object.
    @author     marco.accame@iit.it
    @date       08/03/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOaction_hid.h" // to allow access to the full type: we can use EOaction and not only its opaque pointer

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtimer.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define EOTIMER_STATUS_IDLE             0
#define EOTIMER_STATUS_RUNNING          1
#define EOTIMER_STATUS_COMPLETED        2

#define EOTIMER_MODE_ONESHOT            0
#define EOTIMER_MODE_FOREVER            1


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOtimer_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtimer_hid 
{
    uint64_t    startat;            /**< keeps a fixed value with start information            */
    uint32_t    expirytime;         /**< keeps a fixed value with expiry time                  */
    uint32_t    counting;           /**< keeps counter for expiry time                         */
    union 
    {
        void        *osaltimer;     /**< used only in multitasking execution environment       */
        uint64_t    nextexpiry;     /**< used only in singletask execution environment         */
        void        *other;         /**< used in yee or nee                                    */
    } envir;
    uint8_t     status: 2;          /**< status of the timer: idle 0, running 1, completed 2   */
    uint8_t     mode:   1;          /**< mode of the timer, singleshot 0, forever 1            */
    uint8_t     dummy:  5;          /**< for future use                                        */
    EOaction    onexpiry;           /**< action to be executed on expiry                       */
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

/* @fn         extern void eo_timer_hid_Reset(EOtimer *t, eOtimerStatus_t stat)
    @param      Resets the content of the timer object and sets it to a given status 
    @param      t               Pointer to the timer object.
    @param      stat            Required status.
    @warning    This function is to be used only by derived or friend objects.
 **/
extern void eo_timer_hid_Reset(EOtimer *t, eOtimerStatus_t stat);

extern void eo_timer_hid_Reset_but_not_osaltime(EOtimer *t, eOtimerStatus_t stat);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




