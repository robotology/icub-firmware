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

#ifndef _IPAL_MODULES_H_
#define _IPAL_MODULES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       ipal_modules.h
    @brief      This header file defines the modules to be compiled in ipal
    @author     marco.accame@iit.it
    @date       11/18/2011
 **/


// - modules to be built ----------------------------------------------------------------------------------------------

#define     IPAL_USE_ARP
#define     IPAL_USE_BASE
#define     IPAL_USE_DHCP
#define     IPAL_USE_ETH
#define     IPAL_USE_FTP
#define     IPAL_USE_IGMP
#define     IPAL_USE_SYS
#define     IPAL_USE_TCP
#define     IPAL_USE_TELNET
#define     IPAL_USE_TFTP
#define     IPAL_USE_UDP






// - exceptions -------------------------------------------------------------------------------------------------------


#ifdef  IPAL_MODE_FULL
    // we keep all
#endif//IPAL_MODE_BASE


#ifdef  IPAL_MODE_BASE
    // we keep only udp and related
    #undef  IPAL_USE_DHCP
    #undef  IPAL_USE_FTP
    #undef  IPAL_USE_TCP
    #undef  IPAL_USE_TELNET 
    #undef  IPAL_USE_TFTP
#endif//IPAL_MODE_BASE


//auxiliary modules
#ifdef IPAL_USE_UDP
    #define IPAL_USE_FIFO
#endif

// - cross dependencies -----------------------------------------------------------------------------------------------

#if 0 // to be verified ...

#ifdef  IPAL_USE_DHCP
    #define IPAL_USE_UDP
#endif//IPAL_USE_DHCP      

#ifdef  IPAL_USE_FTP
    #define IPAL_USE_UDP
#endif//IPAL_USE_FTP
     
#endif





#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------






