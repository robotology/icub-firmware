/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "can_parser.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

// and whatever is needed 
#include "stm32hal.h"
#include "operations.h"




// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

static uint32_t s_id = 0;
static uint32_t s_size =0;
static uint8_t s_data[8] = {0};

static bool s_replyisavailable = false;


bool can_parser_decode(uint32_t id, uint8_t size, uint8_t *data)
{
    // example: we accept everything and we send back the very same frame.
    
    s_id = id;
    s_size = size;    
    std::memmove(s_data, data, sizeof(s_data));  
    s_replyisavailable = true;
    
    operations_Data_t dat;
    dat.type    = oper_ONE;
    dat.one     = 0x12345678;
    dat.two     = 0x1234;
    dat.three   = 0x4321;
    operations_test00(&dat);

    return s_replyisavailable;   
}


bool can_parser_getreply(uint32_t *id, uint8_t *size, uint8_t *data)
{
    if(false == s_replyisavailable)
    {
        return false;
    }
        
    *id = s_id;
    *size = s_size;
    std::memmove(data, s_data, sizeof(s_data));
    
    
    s_replyisavailable = false;
    
    return true;
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

