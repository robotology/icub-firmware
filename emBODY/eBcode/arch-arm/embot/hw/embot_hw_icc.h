
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#if 0

## The `embot::hw::icc` driver 

This driver contains HW resources used for inter-core-communication. They are the following.
    
- The `SIG` is used to send or receive a binary notification between two threads 
  running on different cores (but also on the same core).

- The `MEM` offers very basic data exchange across cores. Data is written and read in atomic way, 
  but no notification is offered, so the receiver must continuously poll. 
  
- The `LTR` offers advanced data exchange across cores w/ a post() and a read(). The data exchange is 
  always acked by the reader.


#endif 

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_H_
#define __EMBOT_HW_ICC_H_

#include "embot_core.h"
#include "embot_hw_types.h"


namespace embot::hw::icc {
    
    enum class DIR : uint8_t { none = 0, tx = 1, rx = 2, txrx = 3 };
    
    enum class SIG : uint8_t {  one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, 
                                none = 31, maxnumberof = 8 };
    
    enum class MEM : uint8_t {  one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, 
                                none = 31, maxnumberof = 8 };
    
    enum class LTR : uint8_t {  one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, 
                                none = 31, maxnumberof = 8 };
//    enum class MBX : uint8_t {  one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, 
//                                none = 31, maxnumberof = 8 };
    
} // namespace embot::hw::icc {   


    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


