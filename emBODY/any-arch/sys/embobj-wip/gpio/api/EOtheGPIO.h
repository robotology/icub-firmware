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
#ifndef _EOTHEGPIO_H_
#define _EOTHEGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheGPIO.h
    @brief      This header file implements public interface to the GPIO singleton.
    @author     marco.accame@iit.it
    @date       10/15/2009
**/

/** @defgroup eo_thegpio Object EOtheGPIO
    The EOtheGPIO is a singleton which is only responsible to initialise all objects derivated from EOioPin 
    (such as EOiopinInp, EOioPinOutput, EOioPinInputManaged, EOioPinOutputManaged), and to map them to the hardware. This job is done 
    with the aid of another singleton, derived from EOGPIOConfig, which exports name identifiers for the IO 
    pins, and contains the proper hardware configuration.
 
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOioPin.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


 

/** @typedef    typedef struct EOtheGPIO_hid EOtheGPIO
    @brief      EOtheGPIO is an opaque struct. It is used to implement data abstraction for the GPIO  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOtheGPIO_hid EOtheGPIO;


/** @typedef    typedef const struct eOgpio_iopinmap_t
    @brief      maps a EOiopin into hardware (port, pos), gives it an initial value, and assigns an ID.
 **/ 
typedef const struct  
{
    eOid08_t            id;         /**< the ID used to identify the pin */  
    eOiopinPort_t       port;       /**< the HW port */
    eOiopinPos_t        pos;        /**< the HW position inside the port */
    eOiopinVal_t        val;        /**< the initial value of the pin */
} eOgpio_iopinmap_t;


/** @typedef    typedef const struct eOgpio_cfg_t
    @brief      Contains the configuration for the EOtheGPIO object
 **/ 
typedef const struct  
{
    uint8_t                     ninp;       /**< Number of unmanaged input pins */ 
    uint8_t                     nout;       /**< Number of unmanaged output pins */  
    uint8_t                     nmnginp;    /**< Number of managed input pins */  
    uint8_t                     nmngout;    /**< Number of managed output pins */ 
    eOgpio_iopinmap_t *         inp_map;    /**< Mapping of the unmanaged input pins */
    eOgpio_iopinmap_t *         out_map;    /**< Mapping of the unmanaged output pins */
    eOgpio_iopinmap_t *         mnginp_map; /**< Mapping of the managed input pins */
    eOgpio_iopinmap_t *         mngout_map; /**< Mapping of the managed output pins */    
} eOgpio_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOtheGPIO * eo_gpio_Initialise(const eOgpio_cfg_t * const cfg)
    @brief      Initialise the singleton EOtheGPIO with a proper configuration which keeps information of how
                the GPIOs are mapped into the board. 
    @param      cfg             A const pointer to the constant configuration.
    @return     A valid and not-NULL const pointer to the EOtheGPIO singleton. In case of invalid cfg the function 
                will call  the error manager.
 **/
extern EOtheGPIO * eo_gpio_Initialise(const eOgpio_cfg_t * const cfg);


/** @fn         extern EOtheGPIO * eo_gpio_GetHandle(void)
    @brief      Gets the handle of the EOtheGPIO singleton 
    @return     Constant pointer to the singleton.
 **/
extern EOtheGPIO * eo_gpio_GetHandle(void);


/** @}            
    end of group eo_thegpio  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



