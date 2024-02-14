

/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEICCMAPPING_H_
#define __EMBOT_APP_ETH_THEICCMAPPING_H_

#include "embot_core.h"
#include "embot_app_eth.h"


#include "embot_app_eth_mc_messaging.h"


namespace embot::app::eth {
      
#if 0
    The singleton `theICCmapping` is responsible to ... 
    
#endif
    
        
    class theICCmapping
    {
    public:
        static theICCmapping& getInstance();
    
        struct Entity
        {
            uint8_t endpoint {0};
            uint8_t value {0};  
            
            Entity() = default;
            Entity(uint8_t e, uint8_t v) : endpoint(e), value(v) {}
                
            bool operator==(const Entity& rhs) const
            {
                return (rhs.endpoint == endpoint) && (rhs.value == value);
            }
            
        };
        
        using Index = uint8_t;
        static constexpr Index invalidindex {255};
        
        struct Item
        {
            embot::msg::Location location {};
            Entity entity {};
            Index index {0};
            constexpr Item() = default;
            constexpr Item(const embot::msg::Location &l, const Entity &e, const Index &i)
                : location(l), entity(e), index(i) {}
            
            bool operator==(const Item& rhs) const
            {
                return (rhs.location == location) && (rhs.entity == entity) && (rhs.index == index);  
            }
            
        };

        
        struct Config
        {
            size_t maxitems {8};            
            constexpr Config() = default;
            bool isvalid() const { return true; } // for now always valid. but we check vs it inside initialise()
        };   
       
        bool initialise(const Config &config);
        
        bool load(const embot::app::eth::theICCmapping::Item &item);
        bool clear(const embot::app::eth::theICCmapping::Item &item);
        
        bool clear();
        
        Index toindex(const embot::msg::Location &loc, const Entity &entity) const;
                
    
    private:
        theICCmapping(); 
        ~theICCmapping(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 


} // namespace embot::app::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
