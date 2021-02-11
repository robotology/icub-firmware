
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theCANparserMC.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include <new>

#include "embot_app_theCANboardInfo.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserMC::Impl
{   
    class dummyCANagentMC : public CANagentMC 
    {
    public:
        dummyCANagentMC() {}
        virtual ~dummyCANagentMC() {}
            
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) {  return true; }          
    };
    
    dummyCANagentMC dummyagent;
    
    Config config;
        
    bool txframe;
    bool recognised;
    
    embot::prot::can::Clas cls;
    std::uint8_t cmd;    
        
    embot::prot::can::Frame reply;
    

    Impl() 
    {  
        config.agent = &dummyagent;             
        recognised = false;        
        txframe = false;
        cls = embot::prot::can::Clas::none;
        cmd = 0;               
    }
    
   
    bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    
    bool process_get_ems2foc_desired_current(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies); 
};


bool embot::app::application::theCANparserMC::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    txframe = false;
    recognised = false;
    
    if(false == embot::prot::can::frameis4board(frame, embot::app::theCANboardInfo::getInstance().cachedCANaddress()))
    {
        // if we are in here we may have received a periodic message.
        // we want hopwever be able to decode the Clas::periodicMotorControl        
        if(embot::prot::can::Clas::periodicMotorControl != embot::prot::can::frame2clas(frame))
        {
            recognised = false;
            return recognised;
        }
    }
        
    // now get cls and cmd
    cls = embot::prot::can::frame2clas(frame);
    cmd = embot::prot::can::frame2cmd(frame);
    
    // the basic can handle only some messages ...
    
    switch(cls)
    {        
        case embot::prot::can::Clas::periodicMotorControl:
        {
            
            // only embot::prot::can::motor::periodic::CMD::EMSTO2FOC_DESIRED_CURRENT            
            if(static_cast<std::uint8_t>(embot::prot::can::motor::periodic::CMD::EMSTO2FOC_DESIRED_CURRENT) == cmd)
            {
                txframe = process_get_ems2foc_desired_current(frame, replies);
                recognised = true;
            }
 
        } break;

        
        default:
        {
            txframe = false;
            recognised = false;
        } break;
    }    
    
    
    return recognised;
}




bool embot::app::application::theCANparserMC::Impl::process_get_ems2foc_desired_current(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT msg;
    msg.load(frame);
    
    config.agent->get(msg.info);
    
    return false;        
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserMC& embot::app::application::theCANparserMC::getInstance()
{
    static theCANparserMC* p = new theCANparserMC();
    return *p;
}

embot::app::application::theCANparserMC::theCANparserMC()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();

}  

    
embot::app::application::theCANparserMC::~theCANparserMC() { }
   
        
bool embot::app::application::theCANparserMC::initialise(const Config &config)
{
    if(!config.isvalid())
    {
        return false;
    }
    
    pImpl->config = config;
        
    return true;
}
  


bool embot::app::application::theCANparserMC::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


