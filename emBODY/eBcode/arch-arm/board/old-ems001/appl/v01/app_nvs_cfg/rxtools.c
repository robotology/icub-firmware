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



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal_system.h"
#include "stdlib.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "rxtools.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
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
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern int32_t rxtools_verify_reception(rxtools_rec_status_t* status, rxtools_tx_inrop_t* txinrop, uint32_t maxtimegap, rxtools_results_t* results)
{
    int32_t retvalue = 0;
    
    if((NULL == status) || (NULL == results) || (NULL == txinrop))
    {
        return(-2);
    }
        
    results->flags = 0;
    
    // verification on progressive number
    if((0xffffffff != txinrop->txprog) && (0xffffffff != status->receiverprog))
    {   // the verification on progressive number is enabled only if txprog and recevir prog are a valid numbers
        if(txinrop->txprog != (status->receiverprog+1))
        {
            retvalue = -1;
            results->flags |= 0x01;    
            results->deltaprognumber = txinrop->txprog - status->receiverprog;          
        }
        
        status->receiverprog = txinrop->txprog;
    }


    // verification on delta rx time
    if(status->receivertime != 0xffffffff)
    {   // the verification on gap between two receptions is enabled only if we pass a valid previous received time
        
        uint64_t t = osal_system_ticks_abstime_get() / 1000;
        uint32_t timenow = (uint32_t)t;
        uint32_t timegap = timenow - status->receivertime;
        
        if(timegap > maxtimegap)
        {
            retvalue = -1;
            results->flags |= 0x02;    
            results->deltarxtime = timegap;  
        }
        
        status->receivertime = timenow;
    }    
    
      
    
    return(retvalue);    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



