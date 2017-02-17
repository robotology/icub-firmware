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

#include "embot_i2h.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace i2h {
    
//    class cifStorage : public Storage
//    {
//    public:
////        la fccio derivata. poi metto nel costruttore o nel load una serie di funzioni. magari una struct const alle funzioni.
//        using fp_isinitted = bool (*)(void); 
//        using fp_isaddressvalid = bool (*)(void); 
//        using fp_getbaseaddress = std::uint32_t (*)(void); 
//        using fp_getsize = std::uint32_t (*)(void); 
//        using fp_fullerase = bool (*)(void); 
//        using fp_erase = bool (*)(std::uint32_t, std::uint32_t); 
//        using fp_read = bool (*)(std::uint32_t, std::uint32_t, void*); 
//        using fp_write = bool (*)(std::uint32_t, std::uint32_t, const void*); 
//        
//        struct cFun
//        {
//            fp_isinitted        isinitted;
//            fp_isaddressvalid   isaddressvalid;
//            fp_getbaseaddress   getbaseaddress;
//            fp_getsize          getsize;
//            fp_fullerase        fullerase;
//            fp_erase            erase;
//            fp_read             read;
//            fp_write            write;            
//        };
        
cifStorage::cifStorage(const cFun &cfun)
{
    //cif.isinitted = nullptr;
    std::memmove(&cif, &cfun, sizeof(cFun));
    // check if any entry is nullprt. in this case set all to nullptr;        
    if((nullptr == cif.isinitted) || (nullptr == cif.erase) || (nullptr == cif.fullerase) || (nullptr == cif.getbaseaddress) || 
       (nullptr == cif.getsize) || (nullptr == cif.isaddressvalid) || (nullptr == cif.read) || (nullptr == cif.write))
    {
        cif.isinitted = nullptr;
    }        
}
        
bool cifStorage::isInitted()
{
    if(nullptr == cif.isinitted)
    {
        return false;
    } 
    
    return cif.isinitted();
}


bool cifStorage::isAddressValid(std::uint32_t address)
{
    if(!isInitted())
    {
        return false;
    }
    
    return cif.isaddressvalid(address);    
}

std::uint32_t cifStorage::getBaseAddress()
{
    if(!isInitted())
    {
        return 0;
    }
    
    return cif.getbaseaddress();    
}

std::uint32_t cifStorage::getSize()
{
    if(!isInitted())
    {
        return 0;
    }
    
    return cif.getsize();    
}

bool cifStorage::fullerase()
{
    if(!isInitted())
    {
        return false;
    }
    
    return cif.fullerase();    
}


bool cifStorage::erase(std::uint32_t address, std::uint32_t size)
{
    if(!isInitted())
    {
        return false;
    }
    
    return cif.erase(address, size);    
}

bool cifStorage::read(std::uint32_t address, std::uint32_t size, void *data)
{
    if(!isInitted())
    {
        return false;
    }
    
    return cif.read(address, size, data);    
}

bool cifStorage::write(std::uint32_t address, std::uint32_t size, const void *data)
{
    if(!isInitted())
    {
        return false;
    }
    
    return cif.write(address, size, data);    
}


 
}} // namespace embot { namespace i2h {
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

