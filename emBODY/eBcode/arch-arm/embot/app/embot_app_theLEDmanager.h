/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EMBOT_APP_THELEDMANAGER_H_
#define _EMBOT_APP_THELEDMANAGER_H_

// legacy: theLEDmanager is now embot::svc::theLEDmanager, in embot/svc/embot_svc_theLEDmanager.h. this header keeps
// the old names for the existing code: they are the same types, so old and new names can be mixed. new code includes
// embot_svc_theLEDmanager.h and uses embot::svc. the relative path needs no include path to embot/svc.

#include "../svc/embot_svc_theLEDmanager.h"

namespace embot::app {

    using embot::svc::LEDwave;
    using embot::svc::LEDwaveSpec;
    using embot::svc::LEDhandle;
    using embot::svc::theLEDmanager;
    using embot::svc::LEDwaveC;
    using embot::svc::LEDwaveT;

} // namespace embot::app

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

