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




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "verify.h"


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

static uint8_t s_verify_search_for_synch(verify_t *verif)
{
    uint8_t res = 0;    
    uint8_t i;
    int j;
    verify_pair_t* valuesrec = verif->tmp;
    const verify_pair_t* reference = verif->reference;
    
    memset(valuesrec, 0, verif->capacity*sizeof(verify_pair_t));
    
    circular_get_from_latest_minus(&verif->received, 0, (uint8_t*)valuesrec);

    // try to find the latest received value in reference 
    for(i=0; i<verif->capacity; i++)
    {
        if(0 == memcmp(&valuesrec[0], &reference[i], sizeof(verify_pair_t)))
        {   // ok ... found the first. now i verify if all the others are ok.
            uint8_t allmatched = 1;
            for(j=1; j<verif->capacity; j++)
            {
                verify_pair_t ref = {0,0};
                verify_pair_t rec = {0,0};
                int32_t indexrec = i-j;
                if(indexrec < 0)
                {
                    indexrec += verif->capacity;
                }
                memcpy(&ref, &reference[indexrec], sizeof(verify_pair_t));
                circular_get_from_latest_minus(&verif->received, j, (uint8_t*)&rec);
                
                if(0 != memcmp(&ref, &rec, sizeof(verify_pair_t))) //(ref != rec)
                {
                    allmatched = 0;
                    break;
                }
            }
            
            if(1 == allmatched)
            {
                res = 1;
                return(res);
            }
        }
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


extern void verify_init(verify_t *verif, uint8_t capacity, const verify_pair_t *reference)
{
    verif->synched = 0;
    verif->capacity = capacity;
    verif->reference = reference;
    
    circular_init(&verif->received, capacity, sizeof(verify_pair_t));
    circular_init(&verif->synchref, capacity, sizeof(verify_pair_t));
    
    verif->tmp = calloc(verif->capacity, sizeof(verify_pair_t));
}

extern uint8_t verify_add(verify_t *verif, verify_pair_t pair)
{
    uint8_t res = 1;
    uint8_t need2synch = (0 == verif->synched) ? 1 : 0;
    
    circular_put(&verif->received, (uint8_t*)&pair);
    
    if(1 == need2synch)
    {
        // attempt synch
        
        
        verif->synched = s_verify_search_for_synch(verif);
       
        // if synch is ok, then put
        //verify->synched = 1;
        
        if(1 == verif->synched)
        {
            // copy all the received
            uint8_t i;
            for(i=0; i<verif->capacity; i++)
            {
                verify_pair_t val = {0,0};
                circular_get_from_earliest(&verif->received, i, (uint8_t*)&val);
                circular_put(&verif->synchref, (uint8_t*)&val);               
            }
        }
    }
    else
    {
        // advance synched reference, so that it is aligned with received values
        circular_put(&verif->synchref, NULL);   
    }
    
   
    
    if(0 == verif->synched)
    {
        res = 255;
    }
    else
    {
        // verify synch
        
        // get all the latest N values. if they are all equal, ... returns 1 else 0
            uint8_t i;
            for(i=0; i<verif->capacity; i++)
            {
                verify_pair_t valrec = {0,0};
                verify_pair_t valref = {0,0};
                circular_get_from_latest_minus(&verif->received, i, (uint8_t*)&valrec);
                circular_get_from_latest_minus(&verif->synchref, i, (uint8_t*)&valref);  
                if(0 != memcmp(&valrec, &valref, sizeof(verify_pair_t))) //valrec != valref)
                {
                    res = 0;
                    break;
                }
            }        
        
        
    }
    
    return(res);
}



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



