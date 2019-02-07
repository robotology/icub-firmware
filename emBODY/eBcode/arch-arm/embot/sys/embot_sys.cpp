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

#include "embot_sys.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"
#include "stdlib.h"

#include <new>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace sys {
    
    common::Time timeNow()
    {
        return osal_system_abstime_get();
    }
    
    std::uint32_t millisecondsNow()
    {
        osal_abstime_t t = osal_system_ticks_abstime_get() / 1000; // now t is expressed in millisec
        return static_cast<std::uint32_t>(t);        
    }
    
    common::relTime tickPeriod()
    {
        return osal_info_get_tick();
    }
        
    Task* taskRunning()
    {
        osal_task_t *p = osal_task_get(osal_callerAUTOdetect);

        if(nullptr == p)
        {
            return(nullptr);
        }

        return reinterpret_cast<Task*>(osal_task_extdata_get(p));       
    }

}} // namespace embot { namespace sys {


// --------------------------------------------------------------------------------------------------------------------
// - c code required by osal
// --------------------------------------------------------------------------------------------------------------------

extern "C" void* osal_ext_calloc(uint32_t s, uint32_t n)
{
    void* ret = calloc(s, n);
    return(ret);
}

extern "C" void* osal_ext_realloc(void* m, uint32_t s)
{
    void* ret = realloc(m, s);
    return(ret);
}

extern "C" void osal_ext_free(void* m)
{
    free(m);
}


// --------------------------------------------------------------------------------------------------------------------
// - override of new / delete etc w/ osal funtions
// --------------------------------------------------------------------------------------------------------------------

// here is what was ok in compiler v5 and c++98. but tht also worked for c++14 and complier armclang
//void *operator new(std::size_t size) throw(std::bad_alloc)
//{
//    void* ptr = osal_base_memory_new(size);
//    return ptr;
//}
//void* operator new(std::size_t size, const std::nothrow_t& nothrow_value) throw()
//{
//    void* ptr = osal_base_memory_new(size);
//    return ptr;    
//}
//void operator delete(void* mem) throw ()
//{
//    osal_base_memory_del(mem);
//}

// does it work?
void* operator new(std::size_t size) throw(std::bad_alloc)
{
    void* ptr = osal_base_memory_new(size);
    return ptr;
}

void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) noexcept
{
    void* ptr = osal_base_memory_new(size);
    return ptr;    
}

void operator delete (void* ptr) noexcept
{
    osal_base_memory_del(ptr);
}

    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

