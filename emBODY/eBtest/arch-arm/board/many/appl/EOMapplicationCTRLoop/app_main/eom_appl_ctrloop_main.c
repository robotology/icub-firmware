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

/* @file       eom_emsappl_main.c
	@brief      This file keeps the main of an application on ems using the embobj
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




#include "stdint.h"
#include "stdlib.h"
#include "string.h"



#include "EOMtheSystem.h"

#include "EOMtheEMSapplCfg.h"
#include "EOMtheEMSappl.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emsappl_main_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_Initialise();

    eom_sys_Initialise( &emscfg->wsyscfg.msyscfg, 
                        &emscfg->wsyscfg.mempoolcfg,                         
                        &emscfg->wsyscfg.errmancfg,                 
                        &emscfg->wsyscfg.tmrmancfg, 
                        &emscfg->wsyscfg.cbkmancfg 
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         static void s_eom_emsappl_main_init(void)
    @brief      It initialises the emsappl 
    @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init(void)
{
   
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    eom_emsappl_Initialise(&emscfg->applcfg);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



