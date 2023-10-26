
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theICCservice.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::eth::theICCservice::Impl
{

    bool _initted {false};

    
    constexpr static const char _objname[] = "theICCservice";
    
    Impl() = default;
    
    bool initialise();
    
    bool add(const embot::app::eth::mc::messaging::Location &des, const embot::prot::can::Frame &frame);                            

private:    
    void todo();
};


bool embot::app::eth::theICCservice::Impl::initialise()
{
    if(true == _initted)
    {
        return true;
    }
        
    embot::core::print("embot::app::eth::theICCservice::Impl::initialise()");
     
    // ok, do something
    
    _initted = true;
    return true;
}

bool embot::app::eth::theICCservice::Impl::add(const embot::app::eth::mc::messaging::Location &des, const embot::prot::can::Frame &frame)
{ 
    // add in a buffer. do not trigger any tx yet
    return true;  
}



// private members

void embot::app::eth::theICCservice::Impl::todo()
{

}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theICCservice& embot::app::eth::theICCservice::getInstance()
{
    static theICCservice* p = new theICCservice();
    return *p;
}

embot::app::eth::theICCservice::theICCservice()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::theICCservice::~theICCservice() { }

bool embot::app::eth::theICCservice::initialise()
{
    return pImpl->initialise();
}

bool embot::app::eth::theICCservice::add(const embot::app::eth::mc::messaging::Location &des, const embot::prot::can::Frame &frame)
{
    return pImpl->add(des, frame); 
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


