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

#ifndef _IPAL_NUMBEROF_H_
#define _IPAL_NUMBEROF_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       ipal_numberof.h
    @brief      This header file defines the sizes of modules to be compiled in ipal
    @author     marco.accame@iit.it
    @date       11/18/2011
 **/

// - number of  -------------------------------------------------------------------------------------------------------

#ifdef IPAL_MEMORY_SMALL

    #define IPAL_NUMBEROF_SYSMEMPOOLMAXSIZE          (4*1024)
    #define IPAL_NUMBEROF_ARPCACHETABLEMAXSIZE       8
    #define IPAL_NUMBEROF_IGMPMAXGROUPS              4
    #define IPAL_NUMBEROF_UDPMAXSOCKETS              4
    #define IPAL_NUMBEROF_TCPMAXSOCKETS              4
    #define IPAL_NUMBEROF_TFTPMAXSESSIONS            1
    #define IPAL_NUMBEROF_FTPMAXSESSIONS             1
    #define IPAL_NUMBEROF_TELNETMAXSESSIONS          1
    #define IPAL_NUMBEROF_DNSCACHETABLEMAXSIZE       1

#elif IPAL_MEMORY_MEDIUM

    #define IPAL_NUMBEROF_SYSMEMPOOLMAXSIZE          (6*1024)
    #define IPAL_NUMBEROF_ARPCACHETABLEMAXSIZE       12
    #define IPAL_NUMBEROF_IGMPMAXGROUPS              6
    #define IPAL_NUMBEROF_UDPMAXSOCKETS              6
    #define IPAL_NUMBEROF_TCPMAXSOCKETS              6
    #define IPAL_NUMBEROF_TFTPMAXSESSIONS            1
    #define IPAL_NUMBEROF_FTPMAXSESSIONS             1
    #define IPAL_NUMBEROF_TELNETMAXSESSIONS          1
    #define IPAL_NUMBEROF_DNSCACHETABLEMAXSIZE       1

#elif IPAL_MEMORY_LARGE

    #define IPAL_NUMBEROF_SYSMEMPOOLMAXSIZE          (8*1024)
    #define IPAL_NUMBEROF_ARPCACHETABLEMAXSIZE       24
    #define IPAL_NUMBEROF_IGMPMAXGROUPS              12
    #define IPAL_NUMBEROF_UDPMAXSOCKETS              12
    #define IPAL_NUMBEROF_TCPMAXSOCKETS              12
    #define IPAL_NUMBEROF_TFTPMAXSESSIONS            1
    #define IPAL_NUMBEROF_FTPMAXSESSIONS             1
    #define IPAL_NUMBEROF_TELNETMAXSESSIONS          1
    #define IPAL_NUMBEROF_DNSCACHETABLEMAXSIZE       1

#endif


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------






