
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theCANparserMBD.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include <new>

#include "embot_app_theCANboardInfo.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


#define parserMDB_legacyPARSERactivated

#if defined(parserMDB_legacyPARSERactivated)
    #warning parserMDB_legacyPARSERactivated is activated
#endif

struct embot::app::application::theCANparserMBD::Impl
{   
    class dummyCANagentMBD : public CANagentMBD 
    {
    public:
        dummyCANagentMBD() {}
        virtual ~dummyCANagentMBD() {}
            
        virtual bool onrecognisedframe(void *p) { return true; }    
        virtual bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) {  return true; }  
        virtual bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info) { return true; } 
        virtual bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo) { return true; }
        
				
    };
    

		
    dummyCANagentMBD dummyagent;
    
    Config config;
        
    bool recognised {false};
    
    embot::prot::can::Clas cls {embot::prot::can::Clas::none};
    std::uint8_t cmd {0};    
        
    embot::prot::can::Frame reply {};
    
    Impl() 
    {  
        config.agent = &dummyagent;             
        recognised = false;        
        cls = embot::prot::can::Clas::none;
        cmd = 0;               
    }
    
   
    bool initialise(const Config &cfg);
    bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
        
    bool legacy_process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool legacy_process_get_ems2foc_desired_current(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies); 
    bool legacy_process_set_controlmode(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool legacy_process_get_controlmode(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);

};


bool embot::app::application::theCANparserMBD::Impl::initialise(const Config &cfg)
{
    if(!cfg.isvalid())
    {
        return false;
    }
    
    config = cfg;
		
    
    return true;
}

bool embot::app::application::theCANparserMBD::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    recognised = false;
       
    // i suppose that the mbd accepts raw values, so in here the raw values in rx and in tx (if any)
    bool replyisrequired {false};
    uint8_t rx_data[8] {0};
    uint8_t rx_size {0};
    uint32_t rx_id {0};
    
    uint8_t tx_data[8] {0};
    uint8_t tx_size {0};
    uint32_t tx_id {0};
    
    frame.copyto(rx_id, rx_size, rx_data);     
      		
		
    // if the frame is recognised, the function must return true
    recognised = true;
    // and alert its agent
    config.agent->onrecognisedframe(nullptr);
        
    if(replyisrequired)
    {
        reply = {tx_id, tx_size, tx_data};
        replies.push_back(reply);        
    }
    
    
#if defined(parserMDB_legacyPARSERactivated)
    // just for debug ... the decoding will also trigger the agent
    recognised = legacy_process(frame, replies);
#endif    
        
    return recognised;    
}

// for debug only ....
// here is the legacy decoding
bool embot::app::application::theCANparserMBD::Impl::legacy_process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    bool txframe = false;
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
                txframe = legacy_process_get_ems2foc_desired_current(frame, replies);
                recognised = true;
            }
 
        } break;

        case embot::prot::can::Clas::pollingMotorControl:
        {
                    
            if(static_cast<std::uint8_t>(embot::prot::can::motor::polling::CMD::SET_CONTROL_MODE) == cmd)
            {
                txframe = legacy_process_set_controlmode(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::motor::polling::CMD::GET_CONTROL_MODE) == cmd)
            {
                txframe = legacy_process_get_controlmode(frame, replies);
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

bool embot::app::application::theCANparserMBD::Impl::legacy_process_get_ems2foc_desired_current(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT msg;
    msg.load(frame);
    
    config.agent->get(msg.info);
    
    return false;        
}


bool embot::app::application::theCANparserMBD::Impl::legacy_process_set_controlmode(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::motor::polling::Message_SET_CONTROL_MODE msg;
    msg.load(frame);
    
    config.agent->get(msg.info);
    
    return false;        
}

bool embot::app::application::theCANparserMBD::Impl::legacy_process_get_controlmode(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::motor::polling::Message_GET_CONTROL_MODE msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo replyinfo;
      
    config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }  
    
    return false;         
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::application::theCANparserMBD& embot::app::application::theCANparserMBD::getInstance()
{
    static theCANparserMBD* p = new theCANparserMBD();
    return *p;
}

embot::app::application::theCANparserMBD::theCANparserMBD()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  
   
embot::app::application::theCANparserMBD::~theCANparserMBD() { }
          
bool embot::app::application::theCANparserMBD::initialise(const Config &config)
{   
    return pImpl->initialise(config);        
}
  
bool embot::app::application::theCANparserMBD::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


