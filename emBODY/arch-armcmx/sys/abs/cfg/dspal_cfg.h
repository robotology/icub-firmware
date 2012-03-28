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
#ifndef _DSPAL_CFG_H_
#define _DSPAL_CFG_H_

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------


// <h> Configuration of DSPAL
// <i> It holds configuration of DSPAL

// <h> Porting specifics 
// <i> sssssssss
//   <o> DSP library type         <0=>   CMSISDSP
//   <i> Only CMSISDSP is now supported.
#ifndef DSPAL_DSPLIBTYPE
 #define DSPAL_DSPLIBTYPE      0
#endif


//   <o> CPU family         <0=>   Cortex M3
//   <i> Only Cortex M3 is now supported.
#ifndef DSPAL_CPUFAM
 #define DSPAL_CPUFAM      0
#endif

// </h>Porting specifics

// <h> There is nothing else to configure 
// <i> sssssssss


// </h>There is nothing else to configure 


// </h>






// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


