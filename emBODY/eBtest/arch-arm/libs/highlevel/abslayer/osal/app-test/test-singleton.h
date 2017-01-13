
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _TESTSINGLETON_H_
#define _TESTSINGLETON_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       test-singleton.h
    @brief      ceecwe
    @author     marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup asdfg Evedwdnt Vieddddwer

    The cewcwev edde. 
    
    @{        
 **/

#include "stdlib.h" 
#include "stdint.h" 


// new arm compiler version 6
// https://developer.arm.com/products/software-development-tools/compilers/arm-compiler-6
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0742-/index.html


// see: 
// 1. http://stackoverflow.com/questions/1008019/c-singleton-design-pattern
// 2. http://stackoverflow.com/questions/3940663/c-singleton-implementation-pimpl-idiom-for-singletons-pros-and-cons


// EXTEND_TO_AVOID_COPY is very ncessary because it gives error if one calls the 
// WRONG:   Singleton2 ss = Singleton2::getInstance();  // it get the handle and then it copy it into ss.
// instead of the correct
// CORRECT: Singleton2& ss = Singleton2::getInstance(); // ss is THE handle.
// see also: http://en.cppreference.com/w/cpp/language/copy_constructor

#define REMOVE_COPY_CTOR_AND_ASSIGNMENT

#define USE_CPP11

#if(__cplusplus != 201103L)
    #undef USE_CPP11
#endif

class Singleton2 {
    
public:    
    static Singleton2& getInstance()
    {
        static Singleton2* p = new Singleton2();
        return *p;
    }

public:
    void initialise(bool force = false);        
    void increment();    
    void decrement();    
    uint32_t read();    

private:
    Singleton2();  

#if defined(REMOVE_COPY_CTOR_AND_ASSIGNMENT)

#if !defined(USE_CPP11)
private:
    Singleton2(const Singleton2&);      // don't implement
    Singleton2(Singleton2&);            // don't implement
    void operator=(const Singleton2&);  // don't implement
    void operator=(Singleton2&);        // don't implement
#else
    public:
    // remove copy constructors
    Singleton2(const Singleton2&) = delete;
    Singleton2(Singleton2&) = delete;
    // remove copy assignment operator
    void operator=(const Singleton2&) = delete;
    void operator=(Singleton2&) = delete;
#endif

#endif //EXTEND_TO_AVOID_COPY

private:    
    struct Impl;
    Impl *pImpl;
};


 
 /** @}            
    end of group asdfg  
 **/
 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

