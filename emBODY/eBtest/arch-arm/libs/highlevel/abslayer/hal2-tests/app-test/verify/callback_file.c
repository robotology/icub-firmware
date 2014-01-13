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

#define USE_VALUES_BOARD8

extern uint8_t callback_of_setpoint_all_joints(verify_pair_t pair, uint8_t joint)
{
    #define NJOINTS     4
    #define NSAMPLES    5
    
    static const verify_pair_t thej0123board6_reference[NJOINTS][NSAMPLES] = // errata
    {
        {   // j0
            {0x0000dead, 0x00000e38}, {0x0000dead, 0x00000e38}, {0x0000dead, 0x00000e38}, {0x0000dead, 0x00000e38}, {0x0000dead, 0x00000e38}
        },
        {   // j1
            {0x00007f9d, 0x00000e38}, {0x000098e3, 0x00000e38}, {0x00008000, 0x00000e38}, {0x0000871c, 0x00000e38}, {0x00008888, 0x00000e38}
        },
        {   // j2
            {0x00008000, 0x00000e38}, {0x00008000, 0x00000e38}, {0x00008024, 0x00000e38}, {0x00008014, 0x00000e38}, {0x00008003, 0x00000e38}
        }, 
        {   // j3
            {0x00008000, 0x00000e38}, {0x00008000, 0x00000e38}, {0x00008000, 0x00000e38}, {0x00008000, 0x00000e38}, {0x00004000, 0x00000e38}
        }        
    }; 
    
    static const verify_pair_t thej0123board8_reference[NJOINTS][NSAMPLES] = 
    {
        {   // j0
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38}
        },
        {   // j1
            {0xffff8000, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffff838f, 0x00000e38}, {0xffff871d, 0x00000e38}, {0xffff8e38, 0x00000e38}
        },
        {   // j2
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38}
        }, 
        {   // j3
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38}
        }        
    }; 

#if     defined(USE_VALUES_BOARD6)    
    const verify_pair_t (*reference)[NSAMPLES] = thej0123board6_reference; 
#elif   defined(USE_VALUES_BOARD8)
    const verify_pair_t (*reference)[NSAMPLES] = thej0123board8_reference;
#else
    #error --> must define either USE_VALUES_BOARD6 OR USE_VALUES_BOARD8
#endif  
    
    static uint8_t initted[NJOINTS] = {0};
    static verify_t verify[NJOINTS] = {0};

    uint8_t res = 127;
    
    if(joint < NJOINTS)
    {
        if(0 == initted[joint])
        {
            verify_init(&verify[joint], NSAMPLES, reference[joint]); //thej0123board8_reference[joint]);
            initted[joint] = 1;
        }
        
      
        res = verify_add(&verify[joint], pair);
        
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

extern uint8_t callback_of_setpoint(verify_pair_t pair, uint8_t joint)
{
    static const verify_pair_t thej0board8_position_reference[5] = 
    {
        {0xffffb8e4, 0x00000e38}, 
        {0xffff98e4, 0x00000e38},
        {0xffffc000, 0x00000e38},
        {0xffff78e4, 0x00000e38},
        {0xffff8000, 0x00000e38}
    }; 
    static uint8_t initted = 0;
    static verify_t verify;

    uint8_t res = 127;
    
    if(0 == joint)
    {
        if(0 == initted)
        {
            verify_init(&verify, sizeof(thej0board8_position_reference)/sizeof(verify_pair_t), thej0board8_position_reference);
            initted = 1;
        }
        
       
        
        res = verify_add(&verify, pair);
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



