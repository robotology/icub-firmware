
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

#include "can_decoder.h"

#include "can_rx_raw2struct.h"


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
    
		// Internal Variables For CAN Decoder
    MCControlModes rtb_mode;
    CANErrorTypes rtb_type;
    uint16_T rtb_peak;
    uint16_T rtb_overload;
    int16_T rtb_nominal;
    int16_T rtb_current_o;
    boolean_T rtb_motor;
    boolean_T rtb_motor_e;
    boolean_T rtb_control_mode;
    boolean_T rtb_current_limit;
    boolean_T rtb_desired_current;
    boolean_T rtb_event;
		
		// Input Variables For Packet Formatter
		uint8_T formatter_available;
		uint8_T packet_length;
		uint16_T packet_ID;
		uint8_T packet_payload[8];
		
		// Output Variables For Packet Formatter/Input Variables For Decoder
		uint8_T decoder_available;
		CANClassTypes packet_CLS;
		uint8_T packet_SRC;
		uint8_T packet_DST_TYP;
		uint8_T packet_LEN;
		boolean_T packet_M;
		uint8_T packet_OPC;
		uint8_T packet_ARG[7];	
		
		
		can_messaging::CAN_RX_raw2struct can_packet_formatter;
		can_messaging::CAN_Decoder can_decoder;
		
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
    
    #warning VALENTINA VASCO: add in here initialization code for MBD CAN modem
		
		can_decoder.init(&rtb_motor, &rtb_mode, &rtb_motor_e, &rtb_nominal,
      &rtb_peak, &rtb_overload, &rtb_current_o, &rtb_control_mode,
      &rtb_current_limit, &rtb_desired_current, &rtb_event, &rtb_type);
		
		
    
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
       
    #warning VALENTINA VASCO: add in here MBD CAN decoding. if any reply is required, push it back inside replies 
			
		formatter_available = 1;
		
		packet_length = (uint8_T)rx_size;
		
		packet_ID = (uint16_T)rx_id;
		
		for (int i=0;i<rx_size;i++) {
			packet_payload[i] = (uint8_T)rx_data[i];
		}
		
		can_packet_formatter.step(&formatter_available, &packet_length, &packet_ID, &packet_payload[0],
															&decoder_available, &packet_CLS, &packet_SRC, &packet_DST_TYP, &packet_LEN, &packet_M, &packet_OPC, &packet_ARG[0]);
		
		can_decoder.step(&decoder_available, &packet_CLS, &packet_SRC, &packet_DST_TYP, &packet_LEN, &packet_M, &packet_OPC, &packet_ARG[0],
										&rtb_motor, &rtb_mode, &rtb_motor_e, &rtb_nominal, &rtb_peak, &rtb_overload, &rtb_current_o, &rtb_control_mode, &rtb_current_limit, &rtb_desired_current, &rtb_event, &rtb_type);
    
		#warning VALENTINA VASCO: here you print the variables to see if the decoder worked correctly
    
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


