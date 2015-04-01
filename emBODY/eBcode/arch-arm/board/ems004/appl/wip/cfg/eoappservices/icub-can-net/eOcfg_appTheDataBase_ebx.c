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

/* @file       eOcfg_emsCanNetworkTopology.c
    @brief      This file keeps ems can network configuration for ebx
    @author     valentina.gaggero@iit.it
    @date       07/16/2012
**/
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"


#include "EOMtheEMSapplCfg_cfg.h" // to see EOMTHEEMSAPPLCFG_*

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_appTheDataBase.h"



#if  defined(EOMTHEEMSAPPLCFG_USE_EB1)
      #include    "eOcfg_applDataBase_eb1.c"
 #elif   defined(EOMTHEEMSAPPLCFG_USE_EB2)
      #include    "eOcfg_applDataBase_eb2.c"  
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB3)
      #include    "eOcfg_applDataBase_eb3.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB4)
      #include    "eOcfg_applDataBase_eb4.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
      #include    "eOcfg_applDataBase_eb5.c" 
#elif  defined(EOMTHEEMSAPPLCFG_USE_EB6)
      #include    "eOcfg_applDataBase_eb6.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7)
      #include    "eOcfg_applDataBase_eb7.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB8)
      #include    "eOcfg_applDataBase_eb8.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB9)
        #include    "eOcfg_applDataBase_eb9.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB10)
        #include    "eOcfg_applDataBase_eb10.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB11)
        #include    "eOcfg_applDataBase_eb11.c"
#else
    #error --> you must define an EBx
#endif


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


