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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_I2H_H_
#define _EMBOT_I2H_H_

#include "embot_common.h"

#error DONT USE THIS FILE

namespace embot { namespace i2h {

//    // we derive a class from embot::i2h::Storage and we pass its pointer to whoever needs a Storage *
//    class Storage
//    {
//    public:
//        virtual bool isInitted() = 0;
//        virtual bool isAddressValid(std::uint32_t address) = 0;
//        virtual std::uint32_t getBaseAddress() = 0;
//        virtual std::uint32_t getSize() = 0;   
//        virtual bool fullerase() = 0;  
//        virtual bool erase(std::uint32_t address, std::uint32_t size) = 0;        
//        virtual bool read(std::uint32_t address, std::uint32_t size, void *data) = 0;
//        virtual bool write(std::uint32_t address, std::uint32_t size, const void *data) = 0;   

//        virtual ~Storage() {};    
//    }; 
//    
//    // or ... if we prefer to (or can only) use C language for hw access [i doubt that the stm hal compiles in c++11] we use this 
//    class cifStorage : public Storage
//    {
//    public:
////        la fccio derivata. poi metto nel costruttore o nel load una serie di funzioni. magari una struct const alle funzioni.
//        using fp_isinitted = bool (*)(void); 
//        using fp_isaddressvalid = bool (*)(std::uint32_t); 
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
//        
//        cifStorage(const cFun &cfun);
//        
//        virtual bool isInitted();
//        virtual bool isAddressValid(std::uint32_t address);
//        virtual std::uint32_t getBaseAddress();
//        virtual std::uint32_t getSize();   
//        virtual bool fullerase();  
//        virtual bool erase(std::uint32_t address, std::uint32_t size);        
//        virtual bool read(std::uint32_t address, std::uint32_t size, void *data);
//        virtual bool write(std::uint32_t address, std::uint32_t size, const void *data);       

//      private:         
//        cFun cif;        
//    };

    

}} // namespace embot { namespace i2h {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

