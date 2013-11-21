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
#ifndef _EUPDATERCANGTW_H_
#define _EUPDATERCANGTW_H_

// - doxy begin -------------------------------------------------------------------------------------------------------





// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define _START_CANGTW_WHEN_STAY_FOREVER_
#undef _START_CANGTW_WHEN_STAY_FOREVER_

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern void eupdater_cangtw_init(void);

extern void eupdater_cangtw_start(eOipv4addr_t remipaddr);

extern eOipv4port_t eupdater_cangtw_get_remote_port(void);

extern eOipv4addr_t eupdater_cangtw_get_remote_addr(void);

extern void eupdater_cangtw_set_remote_addr(eOipv4addr_t remaddr);



// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section
 


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


