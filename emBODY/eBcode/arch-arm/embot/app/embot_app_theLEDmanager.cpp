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

// legacy: the projects that compile this file get the code of embot/svc/embot_svc_theLEDmanager.cpp, w/out any change
// to the project (no new file, no include path to embot/svc: the includes w/ quotes of that file are searched first in
// its own folder). a project must compile either this file or embot_svc_theLEDmanager.cpp, never both: the linker
// would find every symbol twice.

#include "embot_app_theLEDmanager.h"
#include "../svc/embot_svc_theLEDmanager.cpp"

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

