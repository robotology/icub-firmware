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




#include "test-singleton.h"


#include "stdlib.h" 
#include "stdint.h" 
#include "osal.h"

#include <cstring>

#include <vector>



// - class Singleton2

struct Singleton2::Impl
{
    bool initted;
    uint32_t one;
    uint32_t two; 
    std::vector<uint8_t> v8;
    Impl() 
    {
        initted = false;
//        int a = sizeof(initted);
//        a = a;
        one = 1;
        two = 2;
        v8.reserve(16);
    }
    ~Impl()
    {
        v8.clear();
    }
};



Singleton2::Singleton2()
: pImpl(new Impl)
{   

}



void Singleton2::initialise(bool force)
{ 
    if(force)
    {
        pImpl->initted = false;
    }
    
    if(false == pImpl->initted)
    {
        pImpl->initted = true;
        pImpl->one = 0;
        pImpl->two = 0;
    }
}    

void Singleton2::increment(void)
{ 
    pImpl->one++;
    pImpl->v8.push_back(pImpl->one);
}  

void Singleton2::decrement(void)
{ 
    if(pImpl->one > 0)
    {
        pImpl->one--;
        pImpl->v8.pop_back();
    }
}  

uint32_t Singleton2::read(void)
{ 
    return pImpl->one;
} 



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






