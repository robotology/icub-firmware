
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
#ifndef _TESTCLASS_H_
#define _TESTCLASS_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       testclass.h
    @brief      ceecwe
    @author     marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup decwecw Event Viewer

    The cewcwev edde. 
    
    @{        
 **/

#include "stdlib.h" 
#include "stdint.h" 



// not all features are supported ....
// http://www.keil.com/support/man/docs/armcc/armcc_chr1407404265784.htm


class Widget1 {
    
public:

    Widget1();  
    ~Widget1();

    void initialise(void);    
    
    void increment();
    
    void decrement();
    
    uint32_t read();

private:

    uint32_t one;
    uint32_t two;
};


class Widget2 {
    
public:

    Widget2();  
    ~Widget2();

    void initialise(void);    
    
    void increment();
    
    void decrement();
    
    uint32_t read();

private:
    
    struct Impl;
    Impl *pImpl;
};


 
 /** @}            
    end of group decwecw  
 **/
 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

