
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "DiagnosticsHost.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_eprot_ropframe.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::DiagnosticsHost::Impl
{    
    Config config {};    
    bool initted {false}; 

    embot::eprot::ropframe::Parser *_ropframeparser {nullptr};
    embot::eprot::rop::Descriptor ropdes {};
    
    // it should be a map<ipv4, uint64_t>
    uint64_t rxsequencenumber {0};

    
    Impl() = default;   

    ~Impl()
    {
        constexpr bool force = true;
        deinit(force);
    }

    bool deinit(bool force = false)
    {
        if(!force)
        {
            if(!initted)
            {
                return false;
            }
        }

        if(nullptr != _ropframeparser)
        {
            delete _ropframeparser;
            _ropframeparser = nullptr;
        }
        
        initted = false;

        return true;
    }

    bool init(const Config &c)
    {
        if(initted)
        {
            return false;
        }

        config = c;

        _ropframeparser = new embot::eprot::ropframe::Parser; // created, but it is an empty shell which will point to the accepted ropframe
        
        initted = true;
        return true;
    }
    
    
//    bool load(EOrop* rop, embot::eprot::rop::Descriptor &ropdes)
//    {
//        ropdes.reset();
//        eOropdescriptor_t *rd = &rop->ropdes;
//        ropdes.opcode = (rd->ropcode <= eo_ropcode_sig) ? static_cast<embot::eprot::rop::OPC>(rd->ropcode) : embot::eprot::rop::OPC::none;
//        ropdes.id32 = rd->id32;
//        if(0 != rd->size)
//        {
//            ropdes.value.load(rd->data, rd->size);         
//        }
//        
//        return true;
//    }        


    bool accept(const embot::eprot::IPv4 &ipv4, const embot::utils::Data &ropframedata, embot::eprot::rop::fpOnROP onrop = nullptr)
    {
        if(!initted)
        {
            return false;
        }

        if(!ropframedata.isvalid())
        {
            return false;
        }

        // load data into the ropframe
        _ropframeparser->load(ropframedata);        

        // check validity
        if(false == _ropframeparser->isvalid())
        {
            _ropframeparser->unload();
            return false;
        }

        // check sequence number ... from each ip address, so ... maybe a map. but not in here
        
        uint64_t rxsequencenumber = _ropframeparser->getSequenceNumber();
        
        // and parse
        uint16_t numberofprocessed = 0;
        if(nullptr == onrop)
        {
            onrop = config.onrop;
        }
        bool r = _ropframeparser->parse(ipv4, onrop, numberofprocessed);        
        return r;
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::app::DiagnosticsHost::DiagnosticsHost()
: pImpl(new Impl)
{ 
}

embot::app::DiagnosticsHost::~DiagnosticsHost()
{   
    delete pImpl;
}


bool embot::app::DiagnosticsHost::initted() const
{
    return pImpl->initted;
}


bool embot::app::DiagnosticsHost::init(const Config &config)
{       
    if(true == initted())
    {
        return false;
    }

    return pImpl->init(config);        
}


bool embot::app::DiagnosticsHost::accept(const embot::eprot::IPv4 &ipv4, const embot::utils::Data &ropframedata, embot::eprot::rop::fpOnROP onrop)
{
    return pImpl->accept(ipv4, ropframedata, onrop);
}

    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

