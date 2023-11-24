
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_eth_theICCmapping.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::eth::theICCmapping::Impl
{
    bool _initted {false};
    Config _config {};
    // we can use any container. the std::map for instance. but in here i use std::vector
        
    std::vector<Item> items {};
    
    constexpr static const char _objname[] = "theICCmapping";
    
    Impl() = default;
    
    bool initialise(const Config &config);        
    bool load(const embot::app::eth::theICCmapping::Item &item);
    bool clear(const embot::app::eth::theICCmapping::Item &item);        
    bool clear();        
    Index toindex(const embot::app::eth::mc::messaging::Location &loc, const Entity &entity) const;
};


bool embot::app::eth::theICCmapping::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
        
    // embot::core::print("embot::app::eth::theICCmapping::Impl::initialise()");
     
    // ok, do something
    _config = config;
            
    items.resize(_config.maxitems);
    
    
    _initted = true;
    return true;
}

bool embot::app::eth::theICCmapping::Impl::load(const embot::app::eth::theICCmapping::Item &item)
{
    bool r {false};
    
    // at first find it if not found i push ot back
//    auto is_equal = [item](const Item &i) { 
//        return i == item;         
//    };    
    auto it = std::find(items.begin(), items.end(), item);
    if(it == std::end(items))
    {
        items.push_back(item);
        r = true;
    }
    
    return r;
}

bool embot::app::eth::theICCmapping::Impl::clear(const embot::app::eth::theICCmapping::Item &item)
{
    bool r {false};
    
    // at first find it if not found i push ot back
//    auto is_equal = [item](const Item &i) { 
//        return i == item;         
//    };    
    auto it = std::find(items.begin(), items.end(), item);
    if(it != std::end(items))
    {
        items.erase(it);
        r = true;
    }
    
    return r; 
}

bool embot::app::eth::theICCmapping::Impl::clear()
{ 
    items.clear();
    return true;  
}

embot::app::eth::theICCmapping::Index embot::app::eth::theICCmapping::Impl::toindex(const embot::app::eth::mc::messaging::Location &loc, const Entity &entity) const
{
    Index i {invalidindex};
    
    auto is_entity_on_location = [loc, entity](const Item &i) { 
        return((i.location == loc) && (i.entity == entity));    
    }; //return((i.location == loc) && (i.entity == entity)); };    
    auto it = std::find_if(items.begin(), items.end(), is_entity_on_location);
    
    if(it != std::end(items))
    {
        i = it->index;
    }    
    
    return i;
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theICCmapping& embot::app::eth::theICCmapping::getInstance()
{
    static theICCmapping* p = new theICCmapping();
    return *p;
}

embot::app::eth::theICCmapping::theICCmapping()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::theICCmapping::~theICCmapping() { }

bool embot::app::eth::theICCmapping::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::theICCmapping::load(const embot::app::eth::theICCmapping::Item &item)
{
    return pImpl->load(item);
}

bool embot::app::eth::theICCmapping::clear(const embot::app::eth::theICCmapping::Item &item)
{
    return pImpl->clear(item);
}


bool embot::app::eth::theICCmapping::clear()
{
    return pImpl->clear();
}

embot::app::eth::theICCmapping::Index embot::app::eth::theICCmapping::toindex(const embot::app::eth::mc::messaging::Location &loc, const Entity &entity) const
{
    return pImpl->toindex(loc, entity);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


