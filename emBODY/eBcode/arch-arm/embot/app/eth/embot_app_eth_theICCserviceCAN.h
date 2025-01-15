

/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEICCSERVICECAN_H_
#define __EMBOT_APP_ETH_THEICCSERVICECAN_H_

#include "embot_core.h"
#include "embot_app_eth.h"
#include "embot_app_icc.h"

#include "embot_app_eth_theICCservice.h"

#include "embot_app_eth_icc_ItemCANframe.h"

#include <array>

namespace embot::app::eth::icc {
    
        
    class theICCserviceCAN
    {
    public:
        
        static theICCserviceCAN& getInstance();

        using ItemCANframeParser = bool (*)(const ItemCANframe &itemcan);
        static bool ItemCANframeParserDummy(const ItemCANframe &itemcan) { return false; }
        
        struct Config
        {
            theICCservice::Pipe pipe {theICCservice::Pipe::one};
            embot::app::eth::icc::theICCservice::modeTX modetx {embot::app::eth::icc::theICCservice::modeTX::onflush};
            embot::core::Callback onrx {};
            ItemCANframeParser itemparser {ItemCANframeParserDummy};
            
            constexpr Config() = default;
            
            constexpr Config(theICCservice::Pipe p, embot::app::eth::icc::theICCservice::modeTX m, const embot::core::Callback &o, ItemCANframeParser i) 
                : pipe(p), modetx(m), onrx(o), itemparser((nullptr == i) ? ItemCANframeParserDummy : i) {}
                                
            bool isvalid() const { return true; } // for now always valid. but we check vs it inside initialise()
        };      
        
        bool initialise(const Config &config);
        
        bool set(embot::app::eth::icc::theICCservice::modeTX modetx);
        bool set(const embot::core::Callback &onrx);  
        bool set(const ItemCANframeParser itemparser);
        
        bool put(const ItemCANframe &item, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        size_t output() const;
        bool flush(embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool flush(const embot::core::Callback &cbk);
        bool flushed() const;
        
        size_t input() const;
        // we can get the Item one by one and use it as we like
        bool get(ItemCANframe &item, size_t &remaining);
        // or we can parse them all as for instance theCANservice does
        bool parse();
    
    private:
        theICCserviceCAN(); 
        ~theICCserviceCAN(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 

} // namespace embot::app::eth:icc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
