/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EoProtocolEPmn_fun_overridden.c
    @brief      This file keeps c...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


#if !defined(EOPROT_USE_MN_VERSION_1_0) & !defined(EOPROT_USE_MN_VERSION_2_0)
    #define EOPROT_USE_MN_VERSION_1_0 
#endif

#if     !defined(EOPROT_USE_MN_VERSION_1_0)
        // marco.accame:    keep the double mode just for some days, until the blue robot is done.
        //                  forgive me for the double code block in the file 
        
        #include "EoProtocolEPmn_fun_overridden-prot-v2.0.c"
        
#else

        #include "EoProtocolEPmn_fun_overridden-prot-v1.0.c"

#endif



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

