/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#if 0

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"

#include "EoProtocol.h"

#include "EOconstvector_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



    
    static const eOprot_EPcfg_t s_theEPcfgsOthersEB1EB3[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {4, 4, 1, 0, 0, 0, 0}     
        },     
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {1, 0, 1, 0, 0, 0, 0}        
        }      
    };
 
#if defined(TEST_EB2_EB4_WITHOUT_MC)
    
    // lamaledizionedelloscorpionedigiada
    static const eOprot_EPcfg_t s_theEPcfgsOthersEB2EB4[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {0, 0, 0, 0, 0, 0, 0}     
        },     
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {0, 0, 0, 1, 0, 0, 0}        
        },
        {           
            .endpoint           = eoprot_endpoint_skin,
            .numberofentities  = {1, 0, 0, 0, 0, 0, 0}     
        }          
    };
#else   // normal case
    static const eOprot_EPcfg_t s_theEPcfgsOthersEB2EB4[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {12, 12, 1, 0, 0, 0, 0}     
        },     
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {0, 1, 0, 1, 0, 0, 0}        
        },
        {           
            .endpoint           = eoprot_endpoint_skin,
            .numberofentities  = {1, 0, 0, 0, 0, 0, 0}     
        }          
    };
#endif

    static const eOprot_EPcfg_t s_theEPcfgsOthersEB5[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {3, 3, 1, 0, 0, 0, 0}     
        },     
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {0, 0, 1, 0, 0, 0, 0}        
        }      
    };


    static const eOprot_EPcfg_t s_theEPcfgsOthersEB6EB8[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {4, 4, 1, 0, 0, 0, 0}     
        },     
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {1, 0, 1, 0, 0, 0, 0}        
        }      
    };

#if     defined(ICUB_MEC_V1)
    #define EB7EB9_NUMBEROFSTRAINS     0
#elif   defined(ICUB_MEC_V2)
    #define EB7EB9_NUMBEROFSTRAINS     1
#else
    #error -> specify either ICUB_MEC_V1 or ICUB_MEC_V2
#endif    
    
    static const eOprot_EPcfg_t s_theEPcfgsOthersEB7EB9[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {2, 2, 1, 0, 0, 0, 0}     
        },     
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {EB7EB9_NUMBEROFSTRAINS, 0, 1, 0, 0, 0, 0}        
        }      
    };


    static const eOprot_EPcfg_t s_theEPcfgsOthersEB10EB11[] =
    { 
        {        
            .endpoint           = eoprot_endpoint_analogsensors,
            .numberofentities  = {0, 0, 0, 1, 0, 0, 0}        
        },        
        {           
            .endpoint           = eoprot_endpoint_skin,
            .numberofentities  = {2, 0, 0, 0, 0, 0, 0}     
        }     
    };
    

    static const eOprot_EPcfg_t s_theEPcfgsOthersEB15[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {4, 4, 1, 0, 0, 0, 0}     
        }   
    };

    static const eOprot_EPcfg_t s_theEPcfgsOthersEB21[] =
    {  
        {           
            .endpoint           = eoprot_endpoint_motioncontrol,
            .numberofentities  = {4, 4, 1, 0, 0, 0, 0}     
        }     
    };
    
    
EOconstvector s_eo_vectorof_EPcfg_eb1eb3 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB1EB3)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB1EB3)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB1EB3,
    .functions      = NULL   
};

EOconstvector s_eo_vectorof_EPcfg_eb2eb4 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB2EB4)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB2EB4)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB2EB4,
    .functions      = NULL   
};

EOconstvector s_eo_vectorof_EPcfg_eb5 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB5)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB5)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB5,
    .functions      = NULL   
};

EOconstvector s_eo_vectorof_EPcfg_eb6eb8 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB6EB8)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB6EB8)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB6EB8,
    .functions      = NULL   
};

EOconstvector s_eo_vectorof_EPcfg_eb7eb9 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB7EB9)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB7EB9)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB7EB9,
    .functions      = NULL   
};

EOconstvector s_eo_vectorof_EPcfg_eb10eb11 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB10EB11)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB10EB11)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB10EB11,
    .functions      = NULL   
};

EOconstvector s_eo_vectorof_EPcfg_eb15 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB15)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB15)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB15,
    .functions      = NULL   
};


EOconstvector s_eo_vectorof_EPcfg_eb21 = 
{
    .capacity       = sizeof(s_theEPcfgsOthersEB21)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersEB21)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersEB21,
    .functions      = NULL   
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





