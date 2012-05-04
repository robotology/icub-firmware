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
#ifndef _EOMAPPDATACOLLECTOR_H_
#define _EOMAPPDATACOLLECTOR_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOMappDataCollector.h
    @brief      This header file implements public interface to EOMappDataCollector object
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/

/** @defgroup eom_dataCollector Object EOMappDataCollector
    EOMappDataCollector  is an application object belong to application skeleton Ems.
    its task is collected data by eth(i.e from pc104), by can (i.e. icub can boards) by encoders (if there is any).
    this object must work only when the application is in realtime-ranning state. (see ThesystemController)
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//embobj
#include "eOcommon.h"

#include "EOappEncodersReader.h"
#include "EOappCanServicesProvider.h"
// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOMappDataCollector_hid EOMappDataCollector;


typedef struct
{    
//    eOvoid_fp_voidp_t           sigToMotorController;
//    void                        *argofsig;
    eOcallbackData_t            sig2appMotorController;
    EOappEncReader              *encReader_ptr;
    EOappCanSP                  *canSP_ptr;
} EOMappDataCollector_cfg_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern EOMappDataCollector* eom_dataCollector_New(EOMappDataCollector_cfg_t *cfg)
    @brief      Creates a new EOMappDataCollector object
    @return     The pointer to the required object or NULL upon failure.
 **/
extern EOMappDataCollector* eom_appDataCollector_New(EOMappDataCollector_cfg_t *cfg);

 
/** @fn         extern eOresult_t eom_dataCollector_Activate(EOMappDataCollector *p);
    @brief      activate the target object.Now it is ready to work. 
    @param      p               Pointer to the object.
    @return     eores_OK
 **/
extern eOresult_t eom_appDataCollector_Activate(EOMappDataCollector *p);


/** @fn         extern eOresult_t eom_appDataCollector_Deactivate(EOMappDataCollector *p);
    @brief      Deactivate the target object.
    @param      p               Pointer to the object.
    @return     eores_OK
 **/
extern eOresult_t eom_appDataCollector_Deactivate(EOMappDataCollector *p);


extern eOresult_t eom_appDataCollector_CollectDataStart(EOMappDataCollector *p);

extern eOresult_t eom_appDataCollector_CollectDataStop(EOMappDataCollector *p);




// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group eom_dataCollector  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


