
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it contains pimpl class(es) for handling diagnostics in an ETH node
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_DIAGNOSTICSNODE_H_
#define _EMBOT_APP_DIAGNOSTICSNODE_H_

#include "embot_common.h"
#include "embot_utils.h"
#include "embot_eprot_rop.h"

#include "embot_eprot_diagnostics.h"

namespace embot { namespace app {
           
    class DiagnosticsNode 
    {
    public:
        struct Config
        {
            bool concurrentuse {false}; // ffu
            uint16_t singleropstreamcapacity {128};
            uint16_t ropframecapacity {512};
            // todo: 
            // - add any customisation such as: capacityofropframe, maxropsize, capacity of fifo of ropframes, etc.  
            Config() = default;
            constexpr Config(bool cu, uint16_t src, uint16_t rfc) : concurrentuse(cu), singleropstreamcapacity(src), ropframecapacity(rfc) {}
            bool isvalid() const { return (ropframecapacity > (28+32)) && (singleropstreamcapacity > 32); }
        };         
               
        DiagnosticsNode();  
        ~DiagnosticsNode();

        // usage: init(), then add() as many rops one wants, then when one wants to attempt transmit: 
        // if(prepare()) { retrieve(data); <alert the sender>}

        bool init(const Config &config);
        bool initted() const;
        bool add(const embot::utils::Data &ropstream);
        bool add(const embot::eprot::rop::Descriptor &ropdes); // WIP: now it does nothing
        bool add(const embot::eprot::diagnostics::InfoBasic &infobasic); // WIP: now it does nothing
        bool prepare(size_t &sizeofropframe); // returns true if anything to retrieve. in sizeofropframe the size of required mem
        bool retrieve(embot::utils::Data &datainropframe); // it copies the ropframe. 

    private:    
        struct Impl;
        Impl *pImpl;
    };  
    

}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
