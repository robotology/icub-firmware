
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_COMM_H_
#define __EMBOT_APP_BLDC_COMM_H_


#include <memory>
#include <vector>
#include "embot_prot_can.h"



namespace embot { namespace app { namespace bldc {
    
    class COMM
    {
    public:
        
        static constexpr size_t MAXcapacity {1111};
        
        // transmission methods
        virtual bool add(const embot::prot::can::Frame &frame) = 0;
        virtual bool add(const std::vector<embot::prot::can::Frame> &frames) = 0;      

        // retrieval methods
        virtual bool get(size_t &remaining, embot::prot::can::Frame &frame) = 0;
        virtual bool get(size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve = COMM::MAXcapacity) = 0;          
      
    public:
        virtual ~COMM() {}          
    };

    class dummyCOMM : public COMM
    {
    public:
        
        dummyCOMM() {}
        virtual ~dummyCOMM() {}
            
        
        // transmission methods
        bool add(const embot::prot::can::Frame &frame) override { return false; }
        bool add(const std::vector<embot::prot::can::Frame> &frames) override { return false; }      

        // retrieval methods
        bool get(size_t &remaining, embot::prot::can::Frame &frame) override { return false; }
        bool get(size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve = COMM::MAXcapacity) override { return false; }               
    };
    
}}} // namespace embot { namespace app { namespace bldc


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
