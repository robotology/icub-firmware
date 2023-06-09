
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_icc_printer.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_icc_ltr.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::hw::icc::printer::theClient::Impl
{
    embot::hw::icc::LTR _l {embot::hw::icc::LTR::one}; 
    bool _initted {false};    
    
    char _str[256] = {0};
    embot::core::Data _dd { &_str, sizeof(_str) };
   
    
    Impl() = default;
            
    bool initialise(const Config &config)
    {
        if(true == _initted)
        {
            return false;;
        } 
        _l = config.l;        
        _initted = (resOK == embot::hw::icc::ltr::init(_l, {DIR::tx})) ? true : false;
        
        return true;
    }
    
    
    bool post(const std::string &str, embot::core::relTime tout)
    {
        if(false == _initted)
        {
            return false;
        }
        std::snprintf(_str, sizeof(_str), "%s", str.c_str());
        embot::hw::icc::ltr::post(_l, _dd, tout);
        return embot::hw::icc::ltr::acked(_l);           
    }
    
};


struct embot::hw::icc::printer::theServer::Impl
{
    embot::hw::icc::LTR _l {embot::hw::icc::LTR::one}; 
    bool _initted {false};    
    
    char _str[256] = {0};
    embot::core::Data _dd { &_str, sizeof(_str) };
    
    static void oninterrupt(void *p)
    {
        Impl *tHIS = reinterpret_cast<Impl*>(p);
        if(nullptr == tHIS)
        {
            return;
        }
                   
        embot::hw::icc::ltr::read(tHIS->_l, tHIS->_dd);
        embot::core::TimeFormatter tf(embot::core::now());
        embot::core::print("PS@" + tf.to_string() + ": " + tHIS->_str);   
    }
    
    
    Impl() = default;
            
    bool initialise(const Config &config)
    {
        if(true == _initted)
        {
            return false;;
        } 
        _l = config.l;        
        _initted = (resOK == embot::hw::icc::ltr::init(_l, {DIR::rx})) ? true : false;
        if(true == _initted)
        {
            embot::hw::icc::ltr::subscribe(_l, {{oninterrupt, this}, embot::hw::Subscription::MODE::permanent});
        }
        
        return true;
    }
    
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------
  

embot::hw::icc::printer::theClient& embot::hw::icc::printer::theClient::getInstance()
{
    static theClient instance;
    return instance;
}

embot::hw::icc::printer::theClient::theClient()
{
    pImpl = std::make_unique<Impl>();
}  
       
bool embot::hw::icc::printer::theClient::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::hw::icc::printer::theClient::post(const std::string &str, embot::core::relTime tout)
{
    return pImpl->post(str, tout);
}


embot::hw::icc::printer::theServer& embot::hw::icc::printer::theServer::getInstance()
{
    static theServer* p = new theServer();
    return *p;
}

embot::hw::icc::printer::theServer::theServer()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::hw::icc::printer::theServer::~theServer() { }
        
bool embot::hw::icc::printer::theServer::initialise(const Config &config)
{
    return pImpl->initialise(config);
}


    
namespace embot::hw::icc::printer::test {

    DIR _dir {DIR::none};
    
    void init(DIR dir)
    {
        _dir = dir;
        
        if(_dir == DIR::tx)
        {
            theClient::getInstance().initialise({embot::hw::icc::LTR::one});
        }
        else if(_dir == DIR::rx)
        {
            theServer::getInstance().initialise({embot::hw::icc::LTR::one});
        }            
    }
    
    void tick(const std::string &str)
    {        
        if(_dir == DIR::tx)
        {
            theClient::getInstance().getInstance().post(str);
        }      
    }
}    


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

