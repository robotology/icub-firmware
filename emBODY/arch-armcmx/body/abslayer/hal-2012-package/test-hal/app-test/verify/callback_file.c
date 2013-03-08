/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       circular.c
    @brief      this file implements internals of a tool module used in hal
    @author     marco.accame@iit.it
    @date       10/29/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "verify.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "callback_file.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


extern uint8_t callback_of_setpoint(int32_t data, uint8_t joint)
{
    static const int32_t thej0board8_position_reference[5] = {0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000}; 
    static uint8_t initted = 0;
    static verify_t verify;

    uint8_t res = 127;
    
    if(0 == joint)
    {
        if(0 == initted)
        {
            verify_init(&verify, sizeof(thej0board8_position_reference)/sizeof(int32_t), thej0board8_position_reference);
            initted = 1;
        }
        
        int32_t* thereceived_position_setpoint_of_j0board8 = (int32_t*) &data; // od altra conversione ...
        
        
        res = verify_add(&verify, *thereceived_position_setpoint_of_j0board8);
        switch(res)
        {
            case 255:       /* sono nelle fasi iniziali, oppure nei primi 4 invii dei setpoint ciclici */
            {
                // non accendo nessun led.
            } break;
            
            case 1:         /* la prima volta che entro qui e' perche' ho ricevuto i primi 5 invii dei setpoint ciclici */
            {
                // accendo il led5 per segnalare che l'aggancio c'e' stato
            } break;

            case 0:         /* sono qui solo dopo che ho agganciato ma solo se uno degli ultimi 5 setpoint non sono quelli che mi aspetto */
            {
                // accendo il led4 per segnalare errore
            } break;           
        }
        
        // quindi: se va tutto bene il led5 e' acceso ed il led4 e' spento.
        //         nel momento del casino si accende anche il led5.
    
    }
    
    return(res);
    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



